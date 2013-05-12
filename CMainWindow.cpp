#include "CMainWindow.h"
#include "ui_MainWindow.h"
#include "CPathManager.h"
#include "process/CTyrantOptimizeWrapper.h"
#include <QSortFilterProxyModel>
#include <QDesktopWidget>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QThread>
#include <QHelpEvent>
#include <QClipboard>

const QString CMainWindow::VERSION = "1.0.3";

CMainWindow::CMainWindow(QWidget *parent)
: QMainWindow(parent)
, mUi(new Ui::MainWindow)
, mOwnedCardsWatcher(new QFileSystemWatcher(this))
, mDeckToolTip(new QDialog(this))
, mFilterDialog(new QDialog(this))
, mMultiDeckDialog(new QDialog(this))
, mDeckToolTipContent(new CDeckWidget())
, mFilterWidget(new CCardFilterWidget())
, mMultiDeckWidget(new CMultiDeckWidget())
, mProcess(0)
, mProcessWrapper(new CTyrantOptimizeWrapper())
, mProcessStatusLabel(new QLabel())
, mDownloadStatusLabel(new QLabel())
, mCards(CCardTable::getCardTable())
, mDecks(CDeckTable::getDeckTable())
, mParameters()
, mLastDir(QApplication::applicationDirPath())
{
    mUi->setupUi(this);
    setWindowTitle(QString("%1 GUI").arg(mProcessWrapper->getProcessExecutable().baseName()));

    // Menu bar setup
    mUi->cardSearchDockWidget->toggleViewAction()->setShortcut(QKeySequence("Ctrl+F"));
    mUi->deckManagementDockWidget->toggleViewAction()->setShortcut(QKeySequence("Ctrl+M"));
    mUi->menuView->addAction(mUi->deckManagementDockWidget->toggleViewAction());
    mUi->menuView->addAction(mUi->cardSearchDockWidget->toggleViewAction());

    // Enemy deck input setup
    mUi->enemyDeckEdit->setAllowMultiDecks(true);

    // Battle ground setup
    mUi->battleGroundBox->addItem("");
    const QList<CBattleground> &battlegrounds = mCards.getBattlegrounds();
    for (int i = 0; i < battlegrounds.size(); ++i)
    {
        mUi->battleGroundBox->addItem(battlegrounds.at(i).getName());
        mUi->battleGroundBox->setItemData(mUi->battleGroundBox->count() - 1, battlegrounds.at(i).getDescription(), Qt::ToolTipRole);
        mUi->battleGroundBox->setItemIcon(mUi->battleGroundBox->count() - 1, QIcon(CPathManager::getPathManager().getResourcePicturePath() + battlegrounds.at(i).getPicture()));
    }
    QSortFilterProxyModel* battlegroundProxy = new QSortFilterProxyModel(mUi->battleGroundBox);
    battlegroundProxy->setSourceModel(mUi->battleGroundBox->model());
    mUi->battleGroundBox->model()->setParent(battlegroundProxy);
    mUi->battleGroundBox->setModel(battlegroundProxy);
    mUi->battleGroundBox->model()->sort(0);
    connect(
        mUi->battleGroundBox, SIGNAL(activated(int)),
        this, SLOT(updateParameterBoxToolTip(int)));

    // Achievement setup
    mUi->achievementBox->addItem("");
    const QList<CAchievement> &achievements = mCards.getAchievements();
    for (int i = 0; i < achievements.size(); ++i)
    {
        mUi->achievementBox->addItem(achievements.at(i).getName(), achievements.at(i).getId());
        mUi->achievementBox->setItemData(mUi->achievementBox->count() - 1, achievements.at(i).getDescription(), Qt::ToolTipRole);
    }
    QSortFilterProxyModel* achievementProxy = new QSortFilterProxyModel(mUi->achievementBox);
    achievementProxy->setSourceModel(mUi->achievementBox->model());
    mUi->achievementBox->model()->setParent(achievementProxy);
    mUi->achievementBox->setModel(achievementProxy);
    mUi->achievementBox->model()->sort(0);
    connect(
        mUi->achievementBox, SIGNAL(activated(int)),
        this, SLOT(updateParameterBoxToolTip(int)));

    // Parameter setup
    connect(
        mUi->ownedCardsBox, SIGNAL(toggled(bool)),
        this, SLOT(setOwnedCardsWatchingEnabled(bool)));
    loadDefaultSettings();

    // Base deck view setup
    CDeck baseDeck;    
    getInputDeck(mUi->baseDeckEdit, baseDeck);    
    mUi->baseDeckWidget->setVisible(mUi->displayBaseButton->isChecked());
    mUi->baseDeckWidget->setDeck(baseDeck);
    mUi->baseDeckWidget->setWinLabel(QPixmap(":/img/trash.png"));
    mUi->baseDeckWidget->setDropEnabled(true);

    // Enemy deck view setup
    CDeck enemyDeck;    
    getInputDeck(mUi->enemyDeckEdit, enemyDeck);    
    mUi->enemyDeckWidget->setVisible(mUi->displayEnemyButton->isChecked());
    mUi->enemyDeckWidget->setDeck(enemyDeck);
    mUi->enemyDeckWidget->setWinLabel(QPixmap(":/img/trash.png"));
    mUi->enemyDeckWidget->setDropEnabled(true);
    adjustSize();


    // Status bar setup
    mUi->statusBar->addWidget(mProcessStatusLabel);
    mUi->statusBar->addWidget(mDownloadStatusLabel);
    mProcessStatusLabel->setFocusPolicy(Qt::ClickFocus);
    mProcessStatusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // Tooltip setup
    mDeckToolTip->setWindowFlags(Qt::ToolTip);
    mDeckToolTip->setStyleSheet("QDialog{background-color: rgb(255, 255, 219);border-color: rgb(0, 0, 0);border-width: 1px;border-style: solid;}");
    QBoxLayout *vLayout = new QBoxLayout(QBoxLayout::TopToBottom, mDeckToolTip);
    vLayout->addWidget(mDeckToolTipContent);
    mUi->baseDeckEdit->installEventFilter(this);
    mUi->enemyDeckEdit->installEventFilter(this);

    // Filter configuration setup
    QBoxLayout *vLayout2 = new QBoxLayout(QBoxLayout::TopToBottom, mFilterDialog);
    vLayout2->addWidget(mFilterWidget);
    mFilterDialog->setWindowTitle("Card Filter Configuration");
    mFilterDialog->resize(QSize(300, 500));
    connect(
        mFilterDialog, SIGNAL(rejected()),
        mFilterWidget, SLOT(declineFilter()));

    // Multi deck setup
    QBoxLayout *vLayout3 = new QBoxLayout(QBoxLayout::TopToBottom, mMultiDeckDialog);
    vLayout3->addWidget(mMultiDeckWidget);
    mMultiDeckWidget->setDeckInputWidget(mUi->enemyDeckEdit);
    mMultiDeckDialog->setWindowTitle("Multi Deck Creation");
    connect(
        mMultiDeckDialog, SIGNAL(rejected()),
        mMultiDeckWidget, SLOT(declineDecks()));

    // Menu connections
    connect(
        mUi->saveAsDefaultAction, SIGNAL(triggered()),
        this, SLOT(saveDefaultParameterSettings()));
    connect(
        mUi->loadDefaultAction, SIGNAL(triggered()),
        this, SLOT(loadDefaultParameterSettings()));
    connect(
        mUi->saveAsAction, SIGNAL(triggered()),
        this, SLOT(saveParameterSettings()));
    connect(
        mUi->loadAction, SIGNAL(triggered()),
        this, SLOT(loadParameterSettings()));
    connect(
        mUi->alwaysOnTopAction, SIGNAL(triggered(bool)),
        this, SLOT(toggleAlwaysOnTop(bool)));
    connect(
        mUi->cardFilterAction, SIGNAL(triggered()),
        mFilterDialog, SLOT(show()));
    connect(
        mUi->updateOwnedCardsAction, SIGNAL(triggered(bool)),
        mFilterWidget, SLOT(updateOwnedCards()));
    connect(
        mUi->updateXmlAction, SIGNAL(triggered(bool)),
        this, SLOT(updateXmlData()));
    connect(
        mUi->aboutAction, SIGNAL(triggered()),
        this, SLOT(displayAboutMessage()));

    // Base deck connections
    connect(
        mUi->baseDeckEdit->lineEdit(), SIGNAL(textChanged(const QString &)),
        mUi->baseDeckWidget, SLOT(setDeck(const QString &)));
    connect(
        mUi->baseDeckWidget, SIGNAL(deckChanged(const QString &)),
        mUi->baseDeckEdit, SLOT(setDeckId(const QString &)));
    connect(
        mUi->displayBaseButton, SIGNAL(clicked(bool)),
        this, SLOT(updateWindowHeight(bool)), Qt::QueuedConnection);
    connect(
        mUi->switchDecksButton, SIGNAL(clicked()),
        this, SLOT(switchDecks()));
    connect(
        mUi->deckManagementWidget, SIGNAL(setBaseDeck(const QString &)),
        mUi->baseDeckEdit, SLOT(setDeckId(const QString &)));

    // Enemy deck connections
    connect(
        mUi->enemyDeckEdit->lineEdit(), SIGNAL(textChanged(const QString &)),
        mUi->enemyDeckWidget, SLOT(setDeck(const QString &)));
    connect(
        mUi->enemyDeckWidget, SIGNAL(deckChanged(const QString &)),
        mUi->enemyDeckEdit, SLOT(setDeckId(const QString &)));
    connect(
        mUi->displayEnemyButton, SIGNAL(clicked(bool)),
        this, SLOT(updateWindowHeight(bool)), Qt::QueuedConnection);
    connect(
        mUi->multiEnemyButton, SIGNAL(clicked()),
        mMultiDeckDialog, SLOT(show()));
    connect(
        mUi->deckManagementWidget, SIGNAL(setEnemyDeck(const QString &)),
        mUi->enemyDeckEdit, SLOT(setDeckId(const QString &)));

    // Button connections
    connect(
        mUi->checkBaseButton, SIGNAL(clicked()),
        this, SLOT(checkBaseDeck()));
    connect(
        mUi->optimizeBaseButton, SIGNAL(clicked()),
        this, SLOT(toggleToolProcess()));
    connect(
        mUi->useOptimizedButton, SIGNAL(clicked()),
        this, SLOT(useOptimizedDeck()));
    connect(
        mUi->saveBaseButton, SIGNAL(clicked()),
        this, SLOT(saveCustomDeck()));
    connect(
        mUi->saveEnemyButton, SIGNAL(clicked()),
        this, SLOT(saveCustomDeck()));
    connect(
        mUi->saveOptimizedButton, SIGNAL(clicked()),
        this, SLOT(saveCustomDeck()));
    connect(
        mUi->hashBaseButton, SIGNAL(clicked()),
        this, SLOT(copyDeckHash()));
    connect(
        mUi->hashEnemyButton, SIGNAL(clicked()),
        this, SLOT(copyDeckHash()));
    connect(
        mUi->hashOptimizedButton, SIGNAL(clicked()),
        this, SLOT(copyDeckHash()));

    // Widgets connections
    connect(
        &mCards, SIGNAL(downloadProgress(int,int)),
        this, SLOT(downloadProgress(int,int)));
    connect(
        &mCards, SIGNAL(dataUpdated(const QStringList&)),
        this, SLOT(dataUpdated(const QStringList&)));
    connect(
        mFilterWidget, SIGNAL(ownedCardsUpdated(int)),
        this, SLOT(ownedCardsUpdated(int)));
    connect(
        mFilterWidget, SIGNAL(filterUpdated(bool)),
        mFilterDialog, SLOT(hide()));    
    connect(
        mMultiDeckWidget, SIGNAL(decksUpdated(const QString&)),
        mMultiDeckDialog, SLOT(hide()));
    connect(
        mUi->cardSearchWidget, SIGNAL(cardSelected(unsigned int)),
        this, SLOT(addCard(unsigned int)));
    connect(
        mOwnedCardsWatcher, SIGNAL(directoryChanged(const QString &)),
        this, SLOT(scanForOwnedCards()));
    connect(
        mUi->deckManagementWidget, SIGNAL(setBaseOrEnemyDeck(const QString &)),
        this, SLOT(setActiveEditingDeck(const QString &)));

    // Process wrapper connections
    connect(
        mProcessWrapper, SIGNAL(winChanceUpdated(float)),
        this, SLOT(setWinChance(float)));
    connect(
        mProcessWrapper, SIGNAL(anpUpdated(float)),
        this, SLOT(setAnp(float)));
    connect(
        mProcessWrapper, SIGNAL(deckUpdated(const QString&)),
        this, SLOT(setResultDeckButtonAvailability(const QString&)));
}

CMainWindow::~CMainWindow()
{
    killToolProcess();
    delete mUi;
    mUi = 0;

    if (mDeckToolTip)
    {
        delete mDeckToolTip;
        mDeckToolTip = 0;
    }


    if (mFilterDialog)
    {
        delete mFilterDialog;
        mFilterDialog = 0;
    }
}

void CMainWindow::getInputDeck(const QComboBox *input, CDeck &deck) const
{
    if (input)
    {
        QString deckId = input->currentText();
        if (!deckId.isEmpty())
        {
            deck = mDecks.getDeckForName(deckId);
            if (deck.isValid())
            {
                // Custom Deck
            }
            else if (mDecks.hashToDeck(deckId, deck))
            {
                // Deck Hash
            }
            else if (mDecks.strToDeck(deckId, deck))
            {
                // Direct Card List
            }
        }
    }
}

void CMainWindow::startToolProcess(bool isOptimizationEnabled)
{
    if (mProcessWrapper && mProcess == 0)
    {
        mProcess = new QProcess();
        if (mProcess)
        {
            // Update history of all deck input widgets
            mUi->baseDeckEdit->updateHistory();
            mUi->enemyDeckEdit->updateHistory();
            mMultiDeckWidget->initDecks();
            mMultiDeckWidget->updateHistory();

            // Fetch parameters from gui
            mParameters.setOptimizationEnabled(isOptimizationEnabled);
			mParameters.fetchFromUi(*mUi);
            if (mParameters.ownedCardsOnly())
            {
                if (mFilterWidget->hasOwnedCardsChanged(mParameters.ownedCardsFile()))
                {
                    mFilterWidget->setOwnedCardsFile(mParameters.ownedCardsFile());
                }
            }

            QStringList toolParameters;
            mProcessWrapper->getCommandLineParameters(mParameters, toolParameters);
            //debug output only
            //mProcessStatusLabel->setText(toolParameters.join(" "));
            //return;

            // Connect signals
            connect(
                mProcess, SIGNAL(error(QProcess::ProcessError)),
                this, SLOT(processError(QProcess::ProcessError)));
            connect(
                mProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(processFinished(int, QProcess::ExitStatus)));
            connect(
                mProcess, SIGNAL(readyReadStandardOutput()),
                this, SLOT(processReadyReadStandardOutput()));
            connect(
                mProcess, SIGNAL(started()),
                this, SLOT(processStarted()));

            // Start tool
            QFileInfo toolExe = mProcessWrapper->getProcessExecutable();
            mProcess->setWorkingDirectory(toolExe.absolutePath());
            mProcess->setProcessChannelMode(QProcess::MergedChannels);
            mProcess->start(toolExe.absoluteFilePath(), toolParameters);
            setProcessActivityChanged(true);
        }
    }
}

void CMainWindow::killToolProcess()
{
    if (mProcess)
    {
        // Disconnect signals
        disconnect(
            mProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(processError(QProcess::ProcessError)));
        disconnect(
            mProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(processFinished(int, QProcess::ExitStatus)));
        disconnect(
            mProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(processReadyReadStandardOutput()));
        disconnect(
            mProcess, SIGNAL(started()),
            this, SLOT(processStarted()));

        mProcess->kill();
        mProcess->waitForFinished();
        delete mProcess;
        mProcess = 0;

        if (mUi)
        {
            mProcessStatusLabel->setText(mProcessWrapper->getProcessExecutable().fileName() + "\' killed");
        }
        setProcessActivityChanged(false);
    }
}

void CMainWindow::setProcessActivityChanged(bool isActive)
{
    if (mUi)
    {
        if (isActive)
        {
            mUi->checkBaseButton->setEnabled(false);
            mUi->optimizeBaseButton->setText("Stop Optimization");
        }
        else
        {
            mUi->checkBaseButton->setEnabled(true);
            mUi->optimizeBaseButton->setText("Optimize Base Deck");
        }
    }
}

void CMainWindow::loadDefaultSettings()
{
    QSettings settings("default.ini", QSettings::IniFormat); 
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/state").toByteArray());

    bool alwaysOnTop = settings.value("window/alwaysOnTop").toBool();
    mUi->alwaysOnTopAction->setChecked(alwaysOnTop);    
    toggleAlwaysOnTop(alwaysOnTop);

    bool baseDisplayed = settings.value("window/baseDeckDisplayed", false).toBool();
    bool enemyDisplayed = settings.value("window/enemyDeckDisplayed", false).toBool();
    mUi->displayBaseButton->setChecked(baseDisplayed);
    mUi->displayEnemyButton->setChecked(enemyDisplayed);

    mLastDir = settings.value("paths/lastDir", mLastDir).toString();

    mFilterWidget->loadParameterSettings(settings);
    mUi->cardSearchWidget->loadParameterSettings(settings);

    loadDefaultParameterSettings();
}

void CMainWindow::closeEvent(QCloseEvent *e)
{
	QSettings settings("default.ini", QSettings::IniFormat);
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
    settings.setValue("window/alwaysOnTop",  mUi->alwaysOnTopAction->isChecked());
    settings.setValue("window/baseDeckDisplayed", mUi->displayBaseButton->isChecked());
    settings.setValue("window/enemyDeckDisplayed", mUi->displayEnemyButton->isChecked());
	settings.setValue("paths/lastDir", mLastDir);

    mFilterWidget->saveParameterSettings(settings);
    mUi->cardSearchWidget->saveParameterSettings(settings);
    QMainWindow::closeEvent(e);
}

bool CMainWindow::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type())
    {
        case QEvent::ToolTip:
        {
            QHelpEvent *he = static_cast<QHelpEvent *>(e);
            QString deckName = "";
            QString deckId = "";
            QComboBox *deckEdit = 0;
            if (obj == mUi->baseDeckEdit && !mUi->baseDeckWidget->isVisible())
            {
                deckName = mUi->baseLabel->text().trimmed();
                deckId = mUi->baseDeckEdit->currentText().trimmed();
                deckEdit = mUi->baseDeckEdit;
            }
            else if(obj == mUi->enemyDeckEdit && !mUi->enemyDeckWidget->isVisible())
            {
                deckName = mUi->enemyLabel->text().trimmed();
                deckId = mUi->enemyDeckEdit->currentText().trimmed();
                deckEdit = mUi->enemyDeckEdit;
            }
            
            CDeck deck;
            if (!mDeckToolTip->isVisible())
            {
                getInputDeck(deckEdit, deck);
            }
            
            if (deck.isValid())
            {
                //mProcessStatusLabel->setText(QString("(%1, %2)").arg(deckHash).arg(cards.join("|")));
                mDeckToolTipContent->setDeck(deck);

                int reqW = mDeckToolTip->width() + 5;
                int reqH = mDeckToolTip->height() + 20;
                int xRel = (QApplication::desktop()->width() - he->globalX() < reqW)
                    ? -reqW
                    : 5;
                int yRel = (QApplication::desktop()->height() - he->globalY() < reqH)
                    ? -reqH
                    : 20;
                mDeckToolTip->move(he->globalX() + xRel, he->globalY() + yRel);
                mDeckToolTip->setVisible(true);
                he->accept();
            }
            return true;
        } 
        //case QEvent::FocusOut:
        case QEvent::Leave:
            mDeckToolTip->setVisible(false);
            return QObject::eventFilter(obj, e);
        default:
        {
            // standard event processing
            return QObject::eventFilter(obj, e);
        }
    }
}

void CMainWindow::loadDefaultParameterSettings()
{
    QSettings settings("default.ini", QSettings::IniFormat);
    mParameters.fetchFromSettings(settings);
    mParameters.updateUi(*mUi);
    mProcessStatusLabel->setText("Default parameters loaded");
}

void CMainWindow::saveDefaultParameterSettings()
{
    QSettings settings("default.ini", QSettings::IniFormat);
    mParameters.fetchFromUi(*mUi);
    mParameters.updateSettings(settings);
    mProcessStatusLabel->setText("Default parameters saved");
}

void CMainWindow::loadParameterSettings()
{
    QFileDialog fileDialog(this, "Load Parameters", mLastDir, "*.ini");
    fileDialog.setDefaultSuffix("ini");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    if (fileDialog.exec())
    {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (!selectedFiles.isEmpty())
        {
            QSettings settings(selectedFiles.at(0), QSettings::IniFormat);
            mParameters.fetchFromSettings(settings);
            mParameters.updateUi(*mUi);
            mProcessStatusLabel->setText("Parameters loaded: " + selectedFiles.at(0));
            mLastDir = fileDialog.directory().absolutePath();
        }
    }
}

void CMainWindow::saveParameterSettings()
{
    QFileDialog fileDialog(this, "Load Parameters", mLastDir, "*.ini");
    fileDialog.setDefaultSuffix("ini");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList	selectedFiles;
    if (fileDialog.exec())
    {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (!selectedFiles.isEmpty())
        {
            QSettings settings(selectedFiles.at(0), QSettings::IniFormat);
            mParameters.fetchFromUi(*mUi);
            mParameters.updateSettings(settings);
            mProcessStatusLabel->setText("Parameters saved: " + selectedFiles.at(0));
            mLastDir = fileDialog.directory().absolutePath();
        }
    }
}

void CMainWindow::toggleAlwaysOnTop(bool checked)
{
    Qt::WindowFlags flags = this->windowFlags();
    if (checked)
    {
        setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        setVisible(true);
    }
    else
    {
        setWindowFlags(flags & ~(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        setVisible(true);
    }
}

void CMainWindow::updateXmlData()
{
    mUi->updateXmlAction->setEnabled(false);
    mCards.updateData();
}

void CMainWindow::displayAboutMessage()
{
    QMessageBox::about(this, "About..", QString("%1\nVersion: %2\nAuthor: warranty_void").arg(windowTitle()).arg(CMainWindow::VERSION));
}

void CMainWindow::toggleToolProcess()
{
    // Termination or creation of process
    if (mProcess)
    {
        killToolProcess();
    }
    else
    {
        startToolProcess(true);
    }
}

void CMainWindow::checkBaseDeck()
{
    startToolProcess(false);
}

void CMainWindow::saveCustomDeck()
{    
    CDeck customDeck;
    QString customDeckName;    
    if(QObject::sender() == mUi->saveBaseButton)
    {
        getInputDeck(mUi->baseDeckEdit, customDeck);
        if (!customDeck.isEmpty())
        {          
            QStringList comTokens = customDeck.getCards().first().getName().split(QRegExp("\\s|\\,|\\*"), QString::SkipEmptyParts);
            if (!comTokens.empty())
            {
                QString comName = comTokens.at(0);
                if (comTokens.size() > 1)
                {
                    comName = "";
                    for (int i = 0; i < comTokens.size(); ++i)
                    {
                        comName += comTokens.at(i).left(1);
                    }

                }
                customDeckName = comName.toLower() + "-";
            }
        }
    }
    else if(QObject::sender() == mUi->saveEnemyButton)
    {
        getInputDeck(mUi->enemyDeckEdit, customDeck);
        if (!customDeck.isEmpty())
        {         
            QStringList comTokens = customDeck.getCards().first().getName().split(QRegExp("\\s|\\,|\\*"), QString::SkipEmptyParts);
            if (!comTokens.empty())
            {
                QString comName = comTokens.at(0);
                if (comTokens.size() > 1)
                {
                    comName = "";
                    for (int i = 0; i < comTokens.size(); ++i)
                    {
                        comName += comTokens.at(i).left(1);
                    }

                }
                customDeckName = comName.toLower() + "-";
            }
        }
    }
    else if(QObject::sender() == mUi->saveOptimizedButton)
    {
        customDeck = mUi->resultDeckWidget->getDeck();
        if (!customDeck.isEmpty())
        {
            QString enemyDeckName = mParameters.enemyDeck().toLower().replace(" ", "-");
            customDeckName = QString("anti-%1").arg(enemyDeckName);
            if(!enemyDeckName.isEmpty() && !enemyDeckName[enemyDeckName.length() - 1].isDigit()
                && customDeck.getCards().size() < 10)
            {
                customDeckName += QString("-%1").arg((customDeck.getCards().size() - 1));
            }
        }
    }

    if (!customDeck.isEmpty() && !customDeckName.isEmpty())
    {
        QMessageBox::StandardButton ok = QMessageBox::NoButton;
        do
        {
            customDeckName = QInputDialog::getText(
                this,
                "Add custom deck",
                "Enter custom deck name:",
                QLineEdit::Normal,
                customDeckName);

            customDeck.setName(customDeckName);
            customDeck.setType(ECustomDeckType);
            if (customDeck.isValid())
            {
                const CDeck &existingDeck = mDecks.getDeckForName(customDeckName);
                if (existingDeck.isValid())
                {
                    ok = QMessageBox::question(
                        this,
                        "Overwrite existing Deck",
                        "A deck with given name does already exist, overwrite?",
                        QMessageBox::Yes | QMessageBox::No);
                }
                else
                {
                    ok = QMessageBox::Yes;
                }
            }
            else
            {
                ok = QMessageBox::No;
            }
        }
        while(customDeck.isValid() && ok != QMessageBox::Yes);


        if (customDeck.isValid() && ok == QMessageBox::Yes)
        {
            mDecks.addCustomDeck(customDeck);
        }
   }
}

void CMainWindow::useOptimizedDeck()
{
    CDeck deck = mUi->resultDeckWidget->getDeck();
    if (deck.isValid())
    {
        QString hash;
        mDecks.deckToHash(deck, hash);
        mUi->baseDeckEdit->setDeckId(hash);
    }
}

void CMainWindow::copyDeckHash()
{
    CDeck deck;
    if(QObject::sender() == mUi->hashBaseButton)
    {
        getInputDeck(mUi->baseDeckEdit, deck);
    }
    else if(QObject::sender() == mUi->hashEnemyButton)
    {
        getInputDeck(mUi->enemyDeckEdit, deck);
    }
    else if(QObject::sender() == mUi->hashOptimizedButton)
    {
        deck = mUi->resultDeckWidget->getDeck();
    }
    if (deck.isValid())
    {
        QString hash;
        mDecks.deckToHash(deck, hash);
        QApplication::clipboard()->setText(hash);
    }
}

void CMainWindow::switchDecks()
{
    QString baseDeckStr = mUi->baseDeckEdit->currentText();
    QString enemyDeckStr = mUi->enemyDeckEdit->currentText();
    QStringList enemyParts = enemyDeckStr.split(QRegExp("\\;|\\:"), QString::SkipEmptyParts);
    if (enemyParts.size() > 1)
    {
        QString firstEnemyDeck = enemyParts.first();
        baseDeckStr.append(enemyDeckStr.right(enemyDeckStr.length() - firstEnemyDeck.length()));
        enemyDeckStr = firstEnemyDeck;
    }
    mUi->baseDeckEdit->setDeckId(enemyDeckStr);
    mUi->enemyDeckEdit->setDeckId(baseDeckStr);

    bool orderedBase = mUi->orderedBaseBox->isChecked();
    mUi->orderedBaseBox->setChecked(mUi->orderedEnemyBox->isChecked());
    mUi->orderedEnemyBox->setChecked(orderedBase);
}

void CMainWindow::updateWindowHeight(bool grow)
{
    if (!grow)
    {
        mUi->centralWidget->layout()->activate();
        adjustSize();
    }
}

void CMainWindow::updateParameterBoxToolTip(int boxIndex)
{
    QComboBox* parameterBox = dynamic_cast<QComboBox*>(QWidget::sender());
    if (parameterBox)
    {
        parameterBox->setToolTip((boxIndex > -1)
            ? parameterBox->itemData(boxIndex, Qt::ToolTipRole).toString()
            : "");
    }
}

void CMainWindow::processError(QProcess::ProcessError error)
{
    QString processName = mProcessWrapper->getProcessExecutable().baseName();
    switch(error)
    {
    case QProcess::FailedToStart:
        mProcessStatusLabel->setText(QString("Process \'%1\' not started").arg(processName));
        break;
    case QProcess::Crashed:
        mProcessStatusLabel->setText(QString("Process \'%1\' crashed").arg(processName));
        break;
    case QProcess::Timedout:
        mProcessStatusLabel->setText(QString("Process \'%1\' timeout").arg(processName));
        break;
    case QProcess::ReadError:
    case QProcess::WriteError:
        mProcessStatusLabel->setText(QString("Process \'%1\' io error").arg(processName));
        break;
    default:
        mProcessStatusLabel->setText(QString("Process \'%1\' unknown error").arg(processName));
        break;
    }
}

void CMainWindow::processFinished(int /*exitCode*/, QProcess::ExitStatus exitStatus)
{
    QString processName = mProcessWrapper->getProcessExecutable().baseName();
    switch(exitStatus)
    {
    case QProcess::NormalExit:
        //mProcessStatusLabel->setText(QString("\'%1\' finished with code %2").arg(PROCESS_NAME).arg(exitCode));
        break;
    case QProcess::CrashExit:
        mProcessStatusLabel->setText(QString("Process \'%1\' crashed").arg(processName));
        break;
    }
    if (mProcess)
    {
        delete mProcess;
        mProcess = 0;
        setProcessActivityChanged(false);
    }
}

void CMainWindow::processReadyReadStandardOutput()
{
    QString toolOutput = QString(mProcess->readAllStandardOutput());
    QStringList outputLines = toolOutput.split(QRegExp("\\n|\\r"), QString::SkipEmptyParts);    
    for (int iLine = 0; iLine < outputLines.size(); ++iLine)
    {
        QString curLine = outputLines.at(iLine);
        if (!curLine.isEmpty())
        {
            mProcessStatusLabel->setText(curLine);
        }
    }
    if (mProcessWrapper)
    {
        mProcessWrapper->processCommandLineOutput(outputLines);
    }
}

void CMainWindow::processStarted()
{
    QString processName = mProcessWrapper->getProcessExecutable().baseName();
    mProcessStatusLabel->setText(QString("Process \'%1\' started").arg(processName));
}

void  CMainWindow::downloadProgress(int numDone, int numDownloads)
{
    if (numDone == numDownloads)
    {
        mDownloadStatusLabel->setText("Downloading card pictures..done");
    }
    else
    {
        mDownloadStatusLabel->setText(QString("Downloading card pictures..(%1/%2)").arg(numDone).arg(numDownloads));
    }
}

void CMainWindow::dataUpdated(const QStringList &result)
{
    mUi->updateXmlAction->setEnabled(true);
    QMessageBox::information(this, "xml update result", result.join("\n"));
}

void CMainWindow::ownedCardsUpdated(int numCards)
{
    QString msg;
    if (numCards > 0)
    {
        msg = QString("Updated 'ownedcards.txt' from clipboard, new #cards=%1").arg(numCards);
    }
    else
    {
        msg = "Could not update 'ownedcards.txt' from clipboard. Please copy card export string from fansite into clipboard first.";
    }
    QMessageBox::information(this, "owned cards update result", msg);
}

void CMainWindow::setOwnedCardsWatchingEnabled(bool enabled)
{
    const CPathManager &pm = CPathManager::getPathManager();
    QStringList watchedDirs = mOwnedCardsWatcher->directories();
    if (enabled)
    {
        if (watchedDirs.isEmpty())
        {
            scanForOwnedCards();
            mOwnedCardsWatcher->addPath(pm.getToolPath());
        }
    }
    else
    {
        for(QStringList::const_iterator i = watchedDirs.begin(); i != watchedDirs.end(); ++i)
        {
            mOwnedCardsWatcher->removePath(*i);
        }
    }
}

void CMainWindow::scanForOwnedCards()
{
    QString lastText =  mUi->ownedCardsFileBox->currentText();
    QStringList mOwnedCardsFiles;
    mFilterWidget->scanOwnedCards(mOwnedCardsFiles);
    mUi->ownedCardsFileBox->clear();
    mUi->ownedCardsFileBox->addItems(mOwnedCardsFiles);
    mUi->ownedCardsFileBox->setCurrentIndex(mUi->ownedCardsFileBox->findText(lastText));
}

void CMainWindow::addCard(unsigned int cardId)
{
    CDeckInput *deckInput = 0;
    getActiveDeckInput(deckInput);
    if (deckInput)
    {
        CDeck deck;
        getInputDeck(deckInput, deck);
        const CCard &newCard = mCards.getCardForId(cardId);
        if (deck.getNumCards() == 0)
        {
            const CCard &whisper = mCards.getCardForId(1003);
            deck.addCard(whisper);
        }
        if (newCard.getType() == ECommanderType)
        {
            deck.replaceCard(0, newCard);
        }
        else if (deck.getNumCards() > 10)
        {
            deck.replaceCard(10, newCard);
        }
        else
        {
            deck.addCard(newCard);
        }
        QString hash;
        mDecks.deckToHash(deck, hash);
        deckInput->setDeckId(hash);
    }
}

void CMainWindow::setWinChance(float winChance)
{
    if (!mParameters.anpOnly())
    {
        mUi->resultDeckWidget->setWinLabel(QString("Win=\n%1%").arg(winChance, 3));
    }
}

void CMainWindow::setAnp(float anp)
{
    if (mParameters.anpOnly())
    {
        mUi->resultDeckWidget->setWinLabel(QString("ANP=\n%1").arg(anp, 3));
    }
}

void CMainWindow::setActiveEditingDeck(const QString &deckStr)
{
    CDeckInput *deckInput = 0;
    getActiveDeckInput(deckInput);
    if (deckInput)
    {
        deckInput->setDeckId(deckStr);
    }
}

void CMainWindow::setResultDeckButtonAvailability(const QString &deckHash)
{
    CDeck deck;
    if (mDecks.hashToDeck(deckHash, deck))
    {
        // We have at least one valid card -> enable saving
        mUi->useOptimizedButton->setEnabled((true));
        mUi->saveOptimizedButton->setEnabled(true);
        mUi->hashOptimizedButton->setEnabled(true);
        mUi->resultDeckWidget->setDeck(deck);
    }
    else
    {
        mUi->useOptimizedButton->setEnabled(false);
        mUi->saveOptimizedButton->setEnabled(false);
        mUi->hashOptimizedButton->setEnabled(false);
        mUi->resultDeckWidget->setDefaultUnits();
    }
}

void CMainWindow::getActiveDeckInput(CDeckInput *&deckInput) const
{
    if (mUi->baseDeckWidget->isVisible())
    {
        deckInput = mUi->baseDeckEdit;
    }
    else if (mUi->enemyDeckWidget->isVisible())
    {
        deckInput = mUi->enemyDeckEdit;
    }
    else
    {
        deckInput = mUi->baseDeckEdit;
    }
}
