#include "CMainWindow.h"
#include "ui_MainWindow.h"
#include "CPathManager.h"
#include <QSortFilterProxyModel>
#include <QDesktopWidget>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QThread>
#include <QHelpEvent>
#include <QClipboard>

const QString CMainWindow::PROCESS_NAME = "tyrant_optimize";
const QString CMainWindow::VERSION = "1.0.1";

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
, mProcessStatusLabel(new QLabel())
, mDownloadStatusLabel(new QLabel())
, mCards(&CCardTable::getCardTable())
, mParameters()
, mLastDir(QApplication::applicationDirPath())
{
    mUi->setupUi(this);
    setWindowTitle(QString("%1 GUI").arg(PROCESS_NAME));

    mUi->baseDeckEdit->addItem("");
    mUi->baseDeckEdit->addItems(mCards->getCustomDecks());
    QSortFilterProxyModel* baseProxy = new QSortFilterProxyModel(mUi->baseDeckEdit);
    baseProxy->setSourceModel(mUi->baseDeckEdit->model());
    mUi->baseDeckEdit->model()->setParent(baseProxy);
    mUi->baseDeckEdit->setModel(baseProxy);
    mUi->baseDeckEdit->model()->sort(0);

    mUi->enemyDeckEdit->addItem("");
    mUi->enemyDeckEdit->addItems(mCards->getRaidDecks());
    mUi->enemyDeckEdit->addItems(mCards->getCustomDecks());
    QSortFilterProxyModel* enemyProxy = new QSortFilterProxyModel(mUi->enemyDeckEdit);
    enemyProxy->setSourceModel(mUi->enemyDeckEdit->model());
    mUi->enemyDeckEdit->model()->setParent(enemyProxy);
    mUi->enemyDeckEdit->setModel(enemyProxy);
    mUi->enemyDeckEdit->model()->sort(0);

    mUi->battleGroundBox->addItem("");
    const QList<CBattleground> &battlegrounds = mCards->getBattlegrounds();
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

    mUi->achievementBox->addItem("");
    const QList<TAchievement> &achievements = mCards->getAchievements();
    for (int i = 0; i < achievements.size(); ++i)
    {
        mUi->achievementBox->addItem(achievements.at(i).first);
        mUi->achievementBox->setItemData(mUi->achievementBox->count() - 1, achievements.at(i).second, Qt::ToolTipRole);
    }
    QSortFilterProxyModel* achievementProxy = new QSortFilterProxyModel(mUi->achievementBox);
    achievementProxy->setSourceModel(mUi->achievementBox->model());
    mUi->achievementBox->model()->setParent(achievementProxy);
    mUi->achievementBox->setModel(achievementProxy);
    mUi->achievementBox->model()->sort(0);

    // Parameters
    connect(
        mUi->ownedCardsBox, SIGNAL(toggled(bool)),
        this, SLOT(setOwnedCardsWatchingEnabled(bool)));
    loadDefaultSettings();

    CDeck baseDeck;    
    getInputDeck(mUi->baseDeckEdit, baseDeck);    
    mUi->baseDeckWidget->setVisible(false);
    mUi->baseDeckLabel->setVisible(false);
    mUi->baseDeckWidget->setDeck(baseDeck);
    mUi->baseDeckWidget->setWinLabel(QPixmap(":/img/trash.png"));
    mUi->baseDeckWidget->setDropEnabled(true);

    CDeck enemyDeck;    
    getInputDeck(mUi->enemyDeckEdit, enemyDeck);    
    mUi->enemyDeckWidget->setVisible(false);
    mUi->enemyDeckLabel->setVisible(false);
    mUi->enemyDeckWidget->setDeck(enemyDeck);
    mUi->enemyDeckWidget->setWinLabel(QPixmap(":/img/trash.png"));
    mUi->enemyDeckWidget->setDropEnabled(true);
    adjustSize();
    
    //setup status bar
    mUi->statusBar->addWidget(mProcessStatusLabel);
    mUi->statusBar->addWidget(mDownloadStatusLabel);
    mProcessStatusLabel->setFocusPolicy(Qt::ClickFocus);
    mProcessStatusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    //setup tooltip popup
    mDeckToolTip->setWindowFlags(Qt::ToolTip);
    mDeckToolTip->setStyleSheet("QDialog{background-color: rgb(255, 255, 219);border-color: rgb(0, 0, 0);border-width: 1px;border-style: solid;}");
    QBoxLayout *vLayout = new QBoxLayout(QBoxLayout::TopToBottom, mDeckToolTip);
    vLayout->addWidget(mDeckToolTipContent);
    mUi->baseDeckEdit->installEventFilter(this);
    mUi->enemyDeckEdit->installEventFilter(this);

    //setup filter configuration popup
    QBoxLayout *vLayout2 = new QBoxLayout(QBoxLayout::TopToBottom, mFilterDialog);
    vLayout2->addWidget(mFilterWidget);
    mFilterDialog->setWindowTitle("Card Filter Configuration");
    mFilterDialog->resize(QSize(300, 500));
    connect(
        mFilterDialog, SIGNAL(rejected()),
        mFilterWidget, SLOT(declineFilter()));

    //setup multi deck popup    
    QBoxLayout *vLayout3 = new QBoxLayout(QBoxLayout::TopToBottom, mMultiDeckDialog);
    vLayout3->addWidget(mMultiDeckWidget);
    mMultiDeckWidget->setDeckWidget(mUi->enemyDeckEdit->lineEdit());
    mMultiDeckDialog->setWindowTitle("Multi Deck Creation");
    connect(
        mMultiDeckDialog, SIGNAL(rejected()),
        mMultiDeckWidget, SLOT(declineDecks()));

    // Menu
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

    // Base deck
    connect(
        mUi->baseDeckEdit, SIGNAL(currentIndexChanged(const QString &)),
        mUi->baseDeckWidget, SLOT(setDeck(const QString &)));    
    connect(
        mUi->baseDeckEdit->lineEdit(), SIGNAL(textEdited(const QString &)),
        mUi->baseDeckWidget, SLOT(setDeck(const QString &)));
    connect(
        mUi->baseDeckEdit, SIGNAL(currentIndexChanged(const QString &)),
        mUi->baseDeckLabel, SLOT(setText(const QString &)));
    connect(
        mUi->baseDeckEdit->lineEdit(), SIGNAL(textEdited(const QString &)),
        mUi->baseDeckLabel, SLOT(clear()));
    connect(
        mUi->baseDeckWidget, SIGNAL(deckChanged(const QString &)),
        mUi->baseDeckEdit->lineEdit(), SLOT(setText(const QString &)));
    connect(
        mUi->displayBaseButton, SIGNAL(toggled(bool)),
        this, SLOT(updateWindowHeight(bool)), Qt::QueuedConnection);
    connect(
        mUi->switchDecksButton, SIGNAL(clicked()),
        this, SLOT(switchDecks()));

    // Enemy deck
    connect(
        mUi->enemyDeckEdit, SIGNAL(currentIndexChanged(const QString &)),
        mUi->enemyDeckWidget, SLOT(setDeck(const QString &)));
    connect(
        mUi->enemyDeckEdit->lineEdit(), SIGNAL(textEdited(const QString &)),
        mUi->enemyDeckWidget, SLOT(setDeck(const QString &)));
    connect(
        mUi->enemyDeckEdit, SIGNAL(currentIndexChanged(const QString &)),
        mUi->enemyDeckLabel, SLOT(setText(const QString &)));
    connect(
        mUi->enemyDeckEdit->lineEdit(), SIGNAL(textEdited(const QString &)),
        mUi->enemyDeckLabel, SLOT(clear()));
    connect(
        mUi->enemyDeckWidget, SIGNAL(deckChanged(const QString &)),
        mUi->enemyDeckEdit->lineEdit(), SLOT(setText(const QString &)));
    connect(
        mUi->displayEnemyButton, SIGNAL(toggled(bool)),
        this, SLOT(updateWindowHeight(bool)), Qt::QueuedConnection);
    connect(
        mUi->multiEnemyButton, SIGNAL(clicked()),
        mMultiDeckDialog, SLOT(show()));

    // Buttons
    connect(
        mUi->checkBaseButton, SIGNAL(clicked()),
        this, SLOT(checkBaseDeck()));
    connect(
        mUi->optimizeBaseButton, SIGNAL(clicked()),
        this, SLOT(toggleToolProcess()));
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

    // Widgets
    connect(
        mCards, SIGNAL(downloadProgress(int,int)),
        this, SLOT(downloadProgress(int,int)));
    connect(
        mCards, SIGNAL(dataUpdated(const QStringList&)),
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
        mCards, SIGNAL(debugOutputSignal(const QString &)),
        mProcessStatusLabel, SLOT(setText(const QString &)));
    connect(
        mOwnedCardsWatcher, SIGNAL(directoryChanged(const QString &)),
        this, SLOT(scanForOwnedCards()));
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
            if (mCards->nameToDeck(deckId, deck))
            {
                // Custom Deck
            }
            else if (mCards->hashToDeck(deckId, deck))
            {
                // Deck Hash
            }
            else if (mCards->strToDeck(deckId, deck))
            {
                // Direct Card List
            }
        }
    }
}

void CMainWindow::startToolProcess(bool isOptimizationEnabled)
{
    if (mProcess == 0)
    {
        mProcess = new QProcess();
        if (mProcess)
        {
            const CPathManager &pm = CPathManager::getPathManager();

            // Fetch parameters from gui
            mParameters.setOptimizationEnabled(isOptimizationEnabled);
			mParameters.fetchFromUi(*mUi);

            QStringList toolParameters;            

            // Base deck
            CDeck deck;
            QString deckId = mParameters.baseDeck();
            if (!mCards->nameToDeck(deckId, deck) && !mCards->hashToDeck(deckId, deck) && mCards->strToDeck(deckId, deck))
            {
                QString deckHash;
                if (mCards->deckToHash(deck, deckHash))
                {
                    deckId = deckHash;
                }
            }
            toolParameters << deckId;


            // Enemy deck
            deckId = mParameters.enemyDeck();
            if (!mCards->nameToDeck(deckId, deck) && !mCards->hashToDeck(deckId, deck) && mCards->strToDeck(deckId, deck))
            {
                QString deckHash;
                if (mCards->deckToHash(deck, deckHash))
                {
                    deckId = deckHash;
                }
            }
            toolParameters << deckId;

            // Switches
			if (mParameters.anpOnly())
            {
                toolParameters << "-a";
            }
			if (mParameters.lockCommander())
            {
                toolParameters << "-c";
            }
            if (mParameters.lockCardCount())
            {
                toolParameters << "-fixedlen";
            }
			if (mParameters.ownedCardsOnly())
            {
                if (mFilterWidget->hasOwnedCardsChanged(mParameters.ownedCardsFile()))
                {                                        
                    mFilterWidget->setOwnedCardsFile(mParameters.ownedCardsFile());
                }
                if (QFileInfo(pm.getToolPath() + "ownedcards_f.txt").exists())
                {
                    toolParameters << "-o=ownedcards_f.txt";
                }
                else
                {
                    toolParameters << "-o";
                }
            }
            if (mParameters.orderedBase())
            {
                toolParameters << "-r";
            }
            if (mParameters.orderedEnemy())
            {
                toolParameters << "defender:ordered";
            }
            if (mParameters.surge())
            {
                toolParameters << "-s";
            }
            if (mParameters.winTie())
            {
                toolParameters << "-wintie";
            }
            if (mParameters.tournament())
            {
                toolParameters << "tournament";
            }
            if (!mParameters.battleGround().isEmpty())
            {
                toolParameters << "-e" << mParameters.battleGround();
            }
            if (!mParameters.achievement().isEmpty())
            {
                toolParameters << "-A" << mParameters.achievement();
            }

            if (mParameters.isOptimizationEnabled())
            {
                toolParameters << "climb" << QString("%1").arg(mParameters.numBattles());
            }
            else
            {
                toolParameters << "sim" << QString("%1").arg(mParameters.numBattles());
            }
            toolParameters << "-t" << QString("%1").arg(mParameters.numThreads());

            if (mParameters.numTurns() != 50)
            {
                toolParameters << "-turnlimit" << QString("%1").arg(mParameters.numTurns());
            }

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
            mProcess->setWorkingDirectory(pm.getToolPath());
            mProcess->setProcessChannelMode(QProcess::MergedChannels);
            mProcess->start(pm.getToolPath() + PROCESS_NAME + ".exe", toolParameters);
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
            mProcessStatusLabel->setText("Process \'" + PROCESS_NAME + "\' killed");
        }
        setProcessActivityChanged(false);
    }
}

void CMainWindow::setWinChance(float percentage)
{
    if (mParameters.anpOnly())
    {
        mUi->resultDeckWidget->setWinLabel(QString("ANP=\n %1").arg(percentage, 3));
    }
    else
    {
        mUi->resultDeckWidget->setWinLabel(QString("%1%").arg(percentage, 3));
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
    mLastDir = settings.value("paths/lastDir", mLastDir).toString();
    mFilterWidget->loadDefaultParameterSettings(settings);
    loadDefaultParameterSettings();
}

void CMainWindow::closeEvent(QCloseEvent *e)
{
	QSettings settings("default.ini", QSettings::IniFormat);
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
    settings.setValue("window/alwaysOnTop",  mUi->alwaysOnTopAction->isChecked());
	settings.setValue("paths/lastDir", mLastDir);
    mFilterWidget->saveDefaultParameterSettings(settings);
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
    mCards->updateData();
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
        customDeck = mUi->baseDeckWidget->getDeck();        
        if (!customDeck.isEmpty())
        {
            bool deckLabelNameValid = !mUi->baseDeckLabel->text().isEmpty();
            if (deckLabelNameValid)
            {
                CDeck checkDeck;
                if (mCards->nameToDeck(mUi->baseDeckLabel->text(), checkDeck))
                {
                    deckLabelNameValid = (checkDeck.getType() == ECustomDeckType);
                }
            }
            if (deckLabelNameValid)
            {
                customDeckName = mUi->baseDeckLabel->text();
            }
            else
            {
                QStringList comTokens = customDeck.getCards().first().getName().split(QRegExp("\\s|\\,"));
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
    }
    else if(QObject::sender() == mUi->saveEnemyButton)
    {
        customDeck = mUi->enemyDeckWidget->getDeck();
        if (!customDeck.isEmpty())
        {
            bool deckLabelNameValid = !mUi->enemyDeckLabel->text().isEmpty();
            if (deckLabelNameValid)
            {
                CDeck checkDeck;
                if (mCards->nameToDeck(mUi->enemyDeckLabel->text(), checkDeck))
                {
                    deckLabelNameValid = (checkDeck.getType() == ECustomDeckType);
                }
            }
            if (deckLabelNameValid)
            {
                customDeckName = mUi->enemyDeckLabel->text();
            }
            else
            {
                QStringList comTokens = customDeck.getCards().first().getName().split(QRegExp("\\s|\\,"));
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
                CDeck deckDummy;
                if (mCards->nameToDeck(customDeckName, deckDummy))
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
            if (mCards->addCustomDeck(customDeck))
            {
                mUi->baseDeckEdit->addItem(customDeckName);
                mUi->baseDeckEdit->model()->sort(0);

                mUi->enemyDeckEdit->addItem(customDeckName);
                mUi->enemyDeckEdit->model()->sort(0);
            }
        }
   }
}

void CMainWindow::copyDeckHash()
{
    CDeck deck;
    if(QObject::sender() == mUi->hashBaseButton)
    {
        deck = mUi->baseDeckWidget->getDeck();
    }
    else if(QObject::sender() == mUi->hashEnemyButton)
    {
        deck = mUi->enemyDeckWidget->getDeck();
    }
    else if(QObject::sender() == mUi->hashOptimizedButton)
    {
        deck = mUi->resultDeckWidget->getDeck();
    }
    if (deck.isValid())
    {
        QString hash;
        mCards->deckToHash(deck, hash);
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
    mUi->baseDeckEdit->lineEdit()->setText(enemyDeckStr);
    mUi->enemyDeckEdit->lineEdit()->setText(baseDeckStr);
    mUi->baseDeckWidget->setDeck(enemyDeckStr);
    mUi->enemyDeckWidget->setDeck(baseDeckStr);

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

void CMainWindow::processError(QProcess::ProcessError error)
{
    switch(error)
    {
    case QProcess::FailedToStart:
        mProcessStatusLabel->setText(QString("Process \'%1\' not started").arg(PROCESS_NAME));
        break;
    case QProcess::Crashed:
        mProcessStatusLabel->setText(QString("Process \'%1\' crashed").arg(PROCESS_NAME));
        break;
    case QProcess::Timedout:
        mProcessStatusLabel->setText(QString("Process \'%1\' timeout").arg(PROCESS_NAME));
        break;
    case QProcess::ReadError:
    case QProcess::WriteError:
        mProcessStatusLabel->setText(QString("Process \'%1\' io error").arg(PROCESS_NAME));
        break;
    default:
        mProcessStatusLabel->setText(QString("Process \'%1\' unknown error").arg(PROCESS_NAME));
        break;
    }
}

void CMainWindow::processFinished(int /*exitCode*/, QProcess::ExitStatus exitStatus)
{
    switch(exitStatus)
    {
    case QProcess::NormalExit:
        //mProcessStatusLabel->setText(QString("\'%1\' finished with code %2").arg(PROCESS_NAME).arg(exitCode));
        break;
    case QProcess::CrashExit:
        mProcessStatusLabel->setText(QString("Process \'%1\' crashed").arg(PROCESS_NAME));
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
        if (curLine.startsWith("Attacker:"))
        {
            mUi->resultDeckWidget->setDefaultUnits();
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            if (!curTokens.isEmpty())
            {
                CDeck outputDeck;
                if (mCards->hashToDeck(curTokens.last(), outputDeck))
                {
                    // We have at least one valid card -> enable saving
                    mUi->saveOptimizedButton->setEnabled(!outputDeck.isEmpty());
                    mUi->hashOptimizedButton->setEnabled(!outputDeck.isEmpty());
                    mUi->resultDeckWidget->setDeck(outputDeck);
                }
            }
        }
        else if (curLine.startsWith("win%") || (mParameters.anpOnly() && curLine.startsWith("ANP")))
        {
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
            bool ok(true);
            float winPercent = curTokens.at(1).toFloat(&ok);
            if (ok)
            {
                setWinChance(winPercent);
            }
        }
        else if (curLine.startsWith("Deck improved"))
        {            
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:"), QString::SkipEmptyParts);
           
            //Debug output, enable only for testing
            //QString debugOut = "";
            //for (int iToken = 0; iToken < curTokens.size(); ++iToken)
            //{
            //    debugOut += "[" + curTokens.at(iToken) + "]";
            //}
            //mProcessStatusLabel->setText(debugOut);
            //return;
            
            if (curTokens.size() > (mParameters.anpOnly() ? 6 : 10))
            {  
                int fromSlot(-0xf);
                int toSlot(-0xf);
                int unitBeginIdx(-0xf);
                CDeck outputDeck;
                if (mCards->hashToDeck(curTokens.at(2), outputDeck))
                {
                    mUi->resultDeckWidget->setDeck(outputDeck);
                }
                else if (curTokens.at(2).compare("commander") == 0)
                {
                    // Commander toggled
                    toSlot = -1;
                    unitBeginIdx = 4;
                }
                else if (curTokens.at(2).compare("slot") == 0)
                {
                    // Slot toggled
                    bool ok(true);
                    int parsedSlot = curTokens.at(3).toInt(&ok);
                    if (ok)
                    {
                        toSlot = parsedSlot;
                        unitBeginIdx = 5;
                    }
                }
                else
                {
                    // Slot moved
                    bool ok(true);
                    int parsedSlot = curTokens.at(2).toInt(&ok);
                    if (ok)
                    {
                        fromSlot = parsedSlot;
                        for (int slotIdx = 4; slotIdx < curTokens.size() - 1; ++slotIdx)
                        {
                            if (curTokens.at(slotIdx).compare("->") == 0)
                            {
                                int parsedSlot = curTokens.at(slotIdx + 1).toInt(&ok);
                                if (ok)
                                {
                                    toSlot = parsedSlot;
                                    unitBeginIdx = slotIdx + 2;
                                }
                                break;
                            }
                        }
                    }                    
                }

                QString unitName("");
                if (toSlot != -0xf && unitBeginIdx != -0xf)
                {
                    // Retrieve full unit name and percentage
                    int unitEndIdx = unitBeginIdx;
                    for (; unitEndIdx < curTokens.size(); ++unitEndIdx)
                    {
                        if (curTokens.at(unitEndIdx).compare("win%") == 0 || curTokens.at(unitEndIdx).compare("ANP") == 0)
                        {
                            break;
                        }
                        else
                        {
                            if (!unitName.isEmpty())
                            {
                                unitName += " ";
                            }
                            unitName += curTokens.at(unitEndIdx);
                        }
                    }
                    if (curTokens.size() - unitEndIdx >  (mParameters.anpOnly() ? 0 : 4))
                    {
                        bool ok(true);
                        float winPercent = curTokens.at(unitEndIdx + 1).toFloat(&ok);
                        if (ok)
                        {
                            setWinChance(winPercent);
                        }
                    }
                }

                if (!unitName.isEmpty())
                {
                    if (fromSlot != -0xf)
                    {                       
                        for (int iSlot = fromSlot; iSlot != toSlot; (fromSlot < toSlot ? ++iSlot : --iSlot))
                        {
                            const CCard &exCard = mUi->resultDeckWidget->getUnit(fromSlot < toSlot ? iSlot + 1 : iSlot - 1);
                            mUi->resultDeckWidget->setUnit(iSlot, exCard);
                        }
                    }
                    //mProcessStatusLabel->setText(QString("%1 -> %2").arg(fromSlot).arg(toSlot));
                    const CCard &card = mCards->getCardForName(unitName);                    
                    mUi->resultDeckWidget->setUnit(toSlot, card);
                }
            }
        }
        else
        {
            // Extra output starting with win chance
            QStringList curTokens = curLine.split(QRegExp("\\s|\\(|\\)|:|%"), QString::SkipEmptyParts);
            bool ok(true);
            float winPercent = curTokens.at(0).toFloat(&ok);
            if (ok)
            {
                setWinChance(winPercent);
            }
        }
    }
}

void CMainWindow::processStarted()
{
    mProcessStatusLabel->setText(QString("Process \'%1\' started").arg(PROCESS_NAME));
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
