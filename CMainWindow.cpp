#include "CMainWindow.h"
#include "ui_MainWindow.h"
#include "CGlobalConfig.h"
#include "CDeckSaveWidget.h"
#include "CCardLabelNexus.h"
#include "process/CProcessPriority.h"
#include "process/CTyrantOptimizeWrapper.h"
#include <QSortFilterProxyModel>
#include <QDesktopWidget>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QThread>
#include <QHelpEvent>
#include <QClipboard>

#ifdef Q_OS_WIN
    #include <windows.h>
#endif

const CVersion CMainWindow::VERSION = CVersion("1.5.2");
const QString CMainWindow::AUTHOR = "warranty_void";
const QString CMainWindow::HOMEPAGE = "<a href=\'http://www.hunterthinks.com/to/gui\'>hunterthinks.com/to/gui</a>";
const QString CMainWindow::FORUM = "<a href=\'http://www.kongregate.com/forums/65-tyrant/topics/257807-automatic-deck-optimization\'>kongregate.com/[..]automatic-deck-optimization</a>";
CVersion CMainWindow::TOOL_VERSION = CVersion::INVALID_VERSION;

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
, mDownloadStatusProgress(new QProgressBar())
, mCards(CCardTable::getCardTable())
, mDecks(CDeckTable::getDeckTable())
, mParameters()
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
        mUi->battleGroundBox->setItemIcon(mUi->battleGroundBox->count() - 1, QIcon(CGlobalConfig::getCfg().getResourcePicturePath() + battlegrounds.at(i).getPicture()));
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

    // Signal for syncing components while loading
    connect(
        mUi->ownedCardsBox, SIGNAL(toggled(bool)),
        this, SLOT(setOwnedCardsWatchingEnabled(bool)));
    connect(
        mUi->ownedCardsFileBox, SIGNAL(currentIndexChanged(const QString&)),
        mFilterWidget, SLOT(setOwnedCardsFile(const QString&)));
    connect(
        &mDecks, SIGNAL(deckBlockageUpdated(const CDeck&, bool)),
        mFilterWidget, SLOT(setDeckBlockage(const CDeck&, bool)));

    mUi->baseDeckWidget->setLockEnabled(true);
    mUi->baseDeckWidget->setVisible(false);
    mUi->enemyDeckWidget->setVisible(false);
    scanForOwnedCards();
    loadDefaultSettings();

    // Base deck view setup  
    mUi->baseDeckWidget->setVisible(mUi->displayBaseButton->isChecked());
    mUi->baseDeckWidget->setWinLabel(QPixmap(":/img/trash.png"));        
    mUi->baseDeckWidget->setDropEnabled(true);

    // Enemy deck view setup   
    mUi->enemyDeckWidget->setVisible(mUi->displayEnemyButton->isChecked());
    mUi->enemyDeckWidget->setWinLabel(QPixmap(":/img/trash.png"));
    mUi->enemyDeckWidget->setDropEnabled(true);
    adjustSize();

    // Status bar setup
    mUi->statusBar->addPermanentWidget(mProcessStatusLabel, 0);
    mUi->statusBar->addPermanentWidget(mDownloadStatusLabel, 1);
    mUi->statusBar->addPermanentWidget(mDownloadStatusProgress, 2);
    mDownloadStatusProgress->setVisible(false);
    mProcessStatusLabel->setFocusPolicy(Qt::ClickFocus);
    mProcessStatusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // Tooltip setup
    mDeckToolTip->setWindowFlags(Qt::ToolTip);
    mDeckToolTip->setStyleSheet("QDialog{background-color: rgb(255, 255, 219);border-color: rgb(0, 0, 0);border-width: 1px;border-style: solid;}");
    QBoxLayout *vLayout = new QBoxLayout(QBoxLayout::TopToBottom, mDeckToolTip);
    vLayout->setMargin(5);
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
    connect(
        mMultiDeckWidget, SIGNAL(deckToolTipTriggered(bool, const QString&)),
        this, SLOT(toggleDeckToolTip(bool, const QString&)));

    //Deck managment setup
    connect(
        mUi->deckManagementWidget, SIGNAL(deckToolTipTriggered(bool, const QString&)),
        this, SLOT(toggleDeckToolTip(bool, const QString&)));
    connect(
        mUi->deckManagementWidget, SIGNAL(setDeck(const QString &,EInputDeckTarget)),
        this, SLOT(setDeckInput(const QString &,EInputDeckTarget)));

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
        mUi->muteSoundAction, SIGNAL(triggered(bool)),
        this, SLOT(toggleMuteSound(bool)));
    connect(
        mUi->combineDeckActionsAction, SIGNAL(triggered(bool)),
        this, SLOT(toggleDeckActionButton(bool)));
    connect(
        mUi->refreshAction, SIGNAL(triggered()),
        this, SLOT(refreshModels()));
    connect(
        mUi->shadeOwnedCardsAction, SIGNAL(triggered(bool)),
        this, SLOT(toggleCardsShading(bool)));
    connect(
        mUi->labelBlackCardsAction, SIGNAL(triggered(bool)),
        this, SLOT(toggleCardLabelling(bool)));
    connect(
        mUi->labelWhiteCardsAction, SIGNAL(triggered(bool)),
        this, SLOT(toggleCardLabelling(bool)));
    connect(
        mUi->cardFilterAction, SIGNAL(triggered()),
        mFilterDialog, SLOT(show()));
    connect(
        mUi->updateOwnedCardsAction, SIGNAL(triggered(bool)),
        this, SLOT(updateOwnedCards()));
    connect(
        mUi->updateXmlAction, SIGNAL(triggered(bool)),
        this, SLOT(updateXmlData()));
    connect(
        mUi->updateXmlBetaAction, SIGNAL(triggered(bool)),
        this, SLOT(updateXmlData()));
    connect(
        mUi->aboutAction, SIGNAL(triggered()),
        this, SLOT(checkToolVersion()));

    // Base deck connections
    connect(
        mUi->baseDeckEdit->lineEdit(), SIGNAL(textChanged(const QString &)),
        mUi->baseDeckWidget, SLOT(setDeck(const QString &)));
    connect(
        mUi->baseDeckWidget, SIGNAL(deckChanged(const QString &)),
        mUi->baseDeckEdit, SLOT(setDeckId(const QString &)));
    connect(
        mUi->displayBaseButton, SIGNAL(toggled(bool)),
        this, SLOT(updateWindowHeight(bool)), Qt::QueuedConnection);
    connect(
        mUi->actionBaseButton, SIGNAL(clicked()),
        this, SLOT(displayDeckMenu()), Qt::QueuedConnection);
    connect(
        mUi->nameBaseDeckAction, SIGNAL(triggered()),
        this, SLOT(copyDeckCards()));
    connect(
        mUi->nameBaseDeckButton, SIGNAL(clicked()),
        this, SLOT(copyDeckCards()));
    connect(
        mUi->hashBaseDeckAction, SIGNAL(triggered()),
        this, SLOT(copyDeckHash()));
    connect(
        mUi->hashBaseDeckButton, SIGNAL(clicked()),
        this, SLOT(copyDeckHash()));
    connect(
        mUi->saveBaseDeckAction, SIGNAL(triggered()),
        this, SLOT(saveCustomDeck()));
    connect(
        mUi->saveBaseDeckButton, SIGNAL(clicked()),
        this, SLOT(saveCustomDeck()));

    // Enemy deck connections
    connect(
        mUi->enemyDeckEdit->lineEdit(), SIGNAL(textChanged(const QString &)),
        mUi->enemyDeckWidget, SLOT(setDeck(const QString &)));
    connect(
        mUi->enemyDeckEdit, SIGNAL(deckDropped(const QString &)),
        this, SLOT(adjustToDeckType(const QString &)));
    connect(
        mUi->enemyDeckWidget, SIGNAL(deckChanged(const QString &)),
        mUi->enemyDeckEdit, SLOT(setDeckId(const QString &)));
    connect(
        mUi->displayEnemyButton, SIGNAL(clicked(bool)),
        this, SLOT(updateWindowHeight(bool)), Qt::QueuedConnection);
    connect(
        mUi->actionEnemyButton, SIGNAL(clicked()),
        this, SLOT(displayDeckMenu()), Qt::QueuedConnection);
    connect(
        mUi->multiEnemyDeckAction, SIGNAL(triggered()),
        mMultiDeckDialog, SLOT(show()));
    connect(
        mUi->multiEnemyDeckButton, SIGNAL(clicked()),
        mMultiDeckDialog, SLOT(show()));
    connect(
        mUi->nameEnemyDeckAction, SIGNAL(triggered()),
        this, SLOT(copyDeckCards()));
    connect(
        mUi->nameEnemyDeckButton, SIGNAL(clicked()),
        this, SLOT(copyDeckCards()));
    connect(
        mUi->hashEnemyDeckAction, SIGNAL(triggered()),
        this, SLOT(copyDeckHash()));
    connect(
        mUi->hashEnemyDeckButton, SIGNAL(clicked()),
        this, SLOT(copyDeckHash()));
    connect(
        mUi->saveEnemyDeckAction, SIGNAL(triggered()),
        this, SLOT(saveCustomDeck()));
    connect(
        mUi->saveEnemyDeckButton, SIGNAL(clicked()),
        this, SLOT(saveCustomDeck()));

    // Optimized deck connections
    connect(
        mUi->displayOptimizedButton, SIGNAL(clicked()),
        this, SLOT(toggleToolResultWidget()));
    connect(
        mUi->actionOptimizedButton, SIGNAL(clicked()),
        this, SLOT(displayDeckMenu()), Qt::QueuedConnection);
    connect(
        mUi->useOptimizedButton, SIGNAL(clicked()),
        this, SLOT(useOptimizedDeck()));
    connect(
        mUi->nameOptimizedDeckAction, SIGNAL(triggered()),
        this, SLOT(copyDeckCards()));
    connect(
        mUi->nameOptimizedDeckButton, SIGNAL(clicked()),
        this, SLOT(copyDeckCards()));
    connect(
        mUi->hashOptimizedDeckAction, SIGNAL(triggered()),
        this, SLOT(copyDeckHash()));
    connect(
        mUi->hashOptimizedDeckButton, SIGNAL(clicked()),
        this, SLOT(copyDeckHash()));
    connect(
        mUi->saveOptimizedDeckAction, SIGNAL(triggered()),
        this, SLOT(saveCustomDeck()));
    connect(
        mUi->saveOptimizedDeckButton, SIGNAL(clicked()),
        this, SLOT(saveCustomDeck()));

    // Shared deck connections
    connect(
        mUi->switchDecksAction, SIGNAL(triggered()),
        this, SLOT(switchDecks()));
    connect(
        mUi->switchDecksButton, SIGNAL(clicked()),
        this, SLOT(switchDecks()));

    // Button connections
    connect(
        mUi->checkBaseButton, SIGNAL(clicked()),
        this, SLOT(checkBaseDeck()));
    connect(
        mUi->reorderBaseButton, SIGNAL(clicked()),
        this, SLOT(reorderBaseDeck()));
    connect(
        mUi->optimizeBaseButton, SIGNAL(clicked()),
        this, SLOT(toggleToolProcess()));

    // Widgets connections
    connect(
        &mCards, SIGNAL(downloadProgress(const CCard&,int,int,bool)),
        this, SLOT(downloadProgress(const CCard&,int,int,bool)));
    connect(
        &mCards, SIGNAL(dataUpdated(const QStringList&)),
        this, SLOT(dataUpdated(const QStringList&)));
    connect(
        &mCards, SIGNAL(cardStatusUpdated(ECardStatusUpdate)),
        this, SLOT(updateView(ECardStatusUpdate)));
    connect(
        mFilterWidget, SIGNAL(ownedCardsUpdated(const QStringList &)),
        this, SLOT(ownedCardsUpdated(const QStringList &)));
    connect(
        mFilterWidget, SIGNAL(filterUpdated(bool)),
        mFilterDialog, SLOT(hide()));    
    connect(
        mMultiDeckWidget, SIGNAL(decksUpdated(const QString&)),
        mMultiDeckDialog, SLOT(hide()));
    connect(
        mUi->cardSearchWidget, SIGNAL(cardSelected(TCardId)),
        this, SLOT(addCard(TCardId)));
    connect(
        mOwnedCardsWatcher, SIGNAL(directoryChanged(const QString &)),
        this, SLOT(scanForOwnedCards()));
    connect(
        &CCardLabelNexus::getCardLabelNexus(), SIGNAL(blackListStatusToggled(const CCard&, bool)),
        mFilterWidget, SLOT(setCardBlackListStatus(const CCard&, bool)));
    connect(
        &CCardLabelNexus::getCardLabelNexus(), SIGNAL(whiteListStatusToggled(const CCard&, bool)),
        mFilterWidget, SLOT(setCardWhiteListStatus(const CCard&, bool)));

    // Process wrapper connections
    connect(
        mProcessWrapper, SIGNAL(versionInfo(const QString&)),
        this, SLOT(setToolVersion(const QString&)),
        Qt::QueuedConnection);
    connect(
        mProcessWrapper, SIGNAL(statusUpdated(const SOptimizationStatus&)),
        this, SLOT(setOptimizationStatus(const SOptimizationStatus&)));
    setOptimizationStatus(SOptimizationStatus());
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

void CMainWindow::startToolProcess(EProcessMode processMode)
{
    if (mProcessWrapper && mProcess == 0)
    {
        mProcess = new QProcess();
        if (mProcess)
        {
            // Fetch parameters from gui
            mParameters.setProcessMode(processMode);
            mParameters.fetchFromUi(*mUi);
            if (mParameters.ownedCardsOnly())
            {
                if (mFilterWidget->hasOwnedCardsChanged(mParameters.ownedCardsFile()))
                {
                    mFilterWidget->setOwnedCardsFile(mParameters.ownedCardsFile());
                    mParameters.fetchFromUi(*mUi);
                }
            }

            QStringList toolParameters;
            mProcessWrapper->getCommandLineParameters(mParameters, toolParameters);

            // Connect signals
            connect(
                mProcess, SIGNAL(stateChanged(QProcess::ProcessState)),
                this, SLOT(processStateChanged(QProcess::ProcessState)));
            connect(
                mProcess, SIGNAL(error(QProcess::ProcessError)),
                this, SLOT(processError(QProcess::ProcessError)));
            connect(
                mProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(processFinished(int, QProcess::ExitStatus)));
            connect(
                mProcess, SIGNAL(readyReadStandardOutput()),
                this, SLOT(processReadyReadStandardOutput()));

            // Start tool
            QFileInfo toolExe = mProcessWrapper->getProcessExecutable();
            addConsoleLine(toolExe.baseName() + " " + toolParameters.join(" "), true);
            mProcessWrapper->processInit();
            mProcess->setWorkingDirectory(toolExe.absolutePath());
            mProcess->setProcessChannelMode(QProcess::MergedChannels);
            mProcess->start(toolExe.absoluteFilePath(), toolParameters);
        }
    }
}

void CMainWindow::killToolProcess()
{
    if (mProcess)
    {
        // Disconnect signals
        disconnect(
            mProcess, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(processStateChanged(QProcess::ProcessState)));
        disconnect(
            mProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(processError(QProcess::ProcessError)));
        disconnect(
            mProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(processFinished(int, QProcess::ExitStatus)));
        disconnect(
            mProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(processReadyReadStandardOutput()));

        mProcessWrapper->processFinished();
        mProcess->kill();
        mProcess->waitForFinished();        
        delete mProcess;
        mProcess = 0;

        if (mUi)
        {
            mProcessStatusLabel->setText(mProcessWrapper->getProcessExecutable().fileName() + "\' killed");
            mUi->optimizerStatusWidget->setStatus(EStatusUnknown);
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
            mUi->reorderBaseButton->setEnabled(false);
            mUi->optimizeBaseButton->setText("Cancel");
        }
        else
        {
            mUi->checkBaseButton->setEnabled(true);
            mUi->reorderBaseButton->setEnabled(true);
            mUi->optimizeBaseButton->setText("Optimize");
        }
    }
}

void CMainWindow::loadDefaultSettings()
{
    QSettings settings("default.ini", QSettings::IniFormat);
    CVersion cfgVersion = CVersion(settings.value("general/version").toString());
    if (cfgVersion.isValid())
    {
        if (cfgVersion > CVersion(1,4,5))
        {
            restoreGeometry(settings.value("window/geometry").toByteArray());
            restoreState(settings.value("window/state").toByteArray());
        }
    }

    bool alwaysOnTop = settings.value("window/alwaysOnTop").toBool();
    mUi->alwaysOnTopAction->setChecked(alwaysOnTop);    
    toggleAlwaysOnTop(alwaysOnTop);

    bool baseDisplayed = settings.value("window/baseDeckDisplayed", false).toBool();
    bool enemyDisplayed = settings.value("window/enemyDeckDisplayed", false).toBool();
    mUi->displayBaseButton->setChecked(baseDisplayed);
    mUi->displayEnemyButton->setChecked(enemyDisplayed);

    CGlobalConfig::getCfg().load(settings);
    bool combinedDeckActions = CGlobalConfig::getCfg().isDeckActionButtonEnabled();
    mUi->combineDeckActionsAction->setChecked(combinedDeckActions);
    toggleDeckActionButton(combinedDeckActions);

    mUi->muteSoundAction->setChecked(CGlobalConfig::getCfg().isSoundMuted());
    mUi->shadeOwnedCardsAction->setChecked(CGlobalConfig::getCfg().isCardShadingEnabled());
    mUi->labelBlackCardsAction->setChecked(CGlobalConfig::getCfg().isBlackLabellingEnabled());
    mUi->labelWhiteCardsAction->setChecked(CGlobalConfig::getCfg().isWhiteLabellingEnabled());
    mFilterWidget->loadParameterSettings(settings);

    loadDefaultParameterSettings();

    mUi->cardSearchWidget->loadParameterSettings(settings);
    mUi->deckManagementWidget->loadParameterSettings(settings);
}

void CMainWindow::closeEvent(QCloseEvent *e)
{
	QSettings settings("default.ini", QSettings::IniFormat);
    settings.setValue("general/version", CMainWindow::VERSION.toString());
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
    settings.setValue("window/alwaysOnTop",  mUi->alwaysOnTopAction->isChecked());
    settings.setValue("window/baseDeckDisplayed", mUi->displayBaseButton->isChecked());
    settings.setValue("window/enemyDeckDisplayed", mUi->displayEnemyButton->isChecked());

    saveDefaultParameterSettings();

    CGlobalConfig::getCfg().save(settings);
    mFilterWidget->saveParameterSettings(settings);
    mUi->cardSearchWidget->saveParameterSettings(settings);
    mUi->deckManagementWidget->saveParameterSettings(settings);
    QMainWindow::closeEvent(e);
}

bool CMainWindow::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type())
    {
        case QEvent::ToolTip:
        {
            CDeckInput *deckInput = dynamic_cast<CDeckInput*>(obj);
            if (deckInput)
            {
                toggleDeckToolTip(true, deckInput->getDeckId());
                e->accept();
                return true;
            }
            return QObject::eventFilter(obj, e);
        } 
        //case QEvent::FocusOut:
        case QEvent::Leave:
            toggleDeckToolTip(false);
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
    CGlobalConfig &cfg = CGlobalConfig::getCfg();
    QFileDialog fileDialog(this, "Load Parameters", cfg.getLastDir(), "*.ini");
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
            cfg.setLastDir(fileDialog.directory().absolutePath());
        }
    }
}

void CMainWindow::saveParameterSettings()
{
    CGlobalConfig &cfg = CGlobalConfig::getCfg();
    QFileDialog fileDialog(this, "Load Parameters", cfg.getLastDir(), "*.ini");
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
            cfg.setLastDir(fileDialog.directory().absolutePath());
        }
    }
}

void CMainWindow::toggleAlwaysOnTop(bool checked)
{
    // Required because Qt version is buggy on windows ~.~
#ifdef Q_OS_WIN
    if (checked)
    {
        SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    else
    {
        SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
#else
    Qt::WindowFlags flags = this->windowFlags();
    if (checked)
    {
        setWindowFlags(flags | Qt::WindowStaysOnTopHint);
        show();
    }
    else
    {
        setWindowFlags(flags & (~Qt::WindowStaysOnTopHint));
        show();
    }
#endif
}

void CMainWindow::toggleMuteSound(bool checked)
{
     CGlobalConfig::getCfg().setSoundMuted(checked);
}

void CMainWindow::toggleCardsShading(bool checked)
{
    CGlobalConfig::getCfg().setCardShadingEnabled(checked);
    updateView(EOwnedStatusUpdate);
}

void CMainWindow::toggleCardLabelling(bool checked)
{
    if (QObject::sender() == mUi->labelBlackCardsAction)
    {
        CGlobalConfig::getCfg().setBlackLabellingEnabled(checked);
    }
    else if (QObject::sender() == mUi->labelWhiteCardsAction)
    {
        CGlobalConfig::getCfg().setWhiteLabellingEnabled(checked);
    }
    updateView(EListStatusUpdate);
}

void CMainWindow::toggleDeckActionButton(bool checked)
{
    CGlobalConfig::getCfg().setDeckActionButtonEnabled(checked);
    mUi->actionBaseButton->setVisible(checked);
    mUi->expandedBaseActions->setVisible(!checked);
    mUi->actionEnemyButton->setVisible(checked);
    mUi->expandedEnemyActions->setVisible(!checked);
    mUi->actionOptimizedButton->setVisible(checked);
    mUi->expandedOptimizedActions->setVisible(!checked);
}

void CMainWindow::updateXmlData()
{
    mUi->updateXmlAction->setEnabled(false);
    mUi->updateXmlBetaAction->setEnabled(false);
    mCards.updateData(QWidget::sender() == mUi->updateXmlBetaAction);
}

void CMainWindow::updateOwnedCards()
{
    mFilterWidget->updateOwnedCardsFile(mUi->ownedCardsFileBox->currentText());
}

void CMainWindow::displayAboutMessage()
{
    QStringList aboutMsg;
    aboutMsg << QString("<h2>%1</h2>").arg(windowTitle());
    aboutMsg << "<table>";
    aboutMsg << QString("<tr><td>GUI Version:</td><td>%1</td></tr>").arg(CMainWindow::VERSION.toString());
    aboutMsg << QString("<tr><td>Sim Version:</td><td>%1</td></tr>").arg(CMainWindow::TOOL_VERSION.toString());
    aboutMsg << QString("<tr><td>Author:</td><td>%1</td></tr>").arg(CMainWindow::AUTHOR);
    aboutMsg << QString("<tr><td>Home:</td><td>%1</td></tr>").arg(CMainWindow::HOMEPAGE);
    aboutMsg << QString("<tr><td>Forum:</td><td>%1</td></tr>").arg(CMainWindow::FORUM);
    aboutMsg << "</table>";
    QMessageBox::about(this, "About..", aboutMsg.join(QString("")));
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
        startToolProcess(EProcessOptimize);
    }
}

void CMainWindow::toggleToolResultWidget()
{
    int curOut = mUi->resultStackedWidget->currentIndex();
    if (curOut == 0)
    {
       mUi->displayOptimizedButton->setIcon(QPixmap(":/img/view-cards.png"));
       mUi->displayOptimizedButton->setToolTip("Display optimized deck");
    }
    else
    {
        mUi->displayOptimizedButton->setIcon(QPixmap(":/img/view-console.png"));
        mUi->displayOptimizedButton->setToolTip("Display optimizer console");
    }
    mUi->resultStackedWidget->setCurrentIndex(1 - curOut);
}

void CMainWindow::checkToolVersion()
{
    if (mProcess)
    {
        displayAboutMessage();
    }
    else
    {
        startToolProcess(EProcessVersion);
    }
}

void CMainWindow::checkBaseDeck()
{
    startToolProcess(EProcessSimulate);
}

void CMainWindow::reorderBaseDeck()
{
    startToolProcess(EProcessReorder);
}

void CMainWindow::saveCustomDeck()
{
    QObject* trigger = QObject::sender();
    CDeck customDeck = getTriggeredDeck(trigger);
    customDeck.setName("CustomDeck");
    customDeck.setType(ECustomDeckType);

    if (customDeck.isValid())
    {
        const CDeck &enemyDeck = mDecks.getDeckForName(mUi->enemyDeckEdit->getDeckId());
        EOptimizationMode mode = static_cast<EOptimizationMode>(mUi->optimizationModeBox->currentIndex());
        bool isPvE = false;
        if (trigger == mUi->saveEnemyDeckAction || trigger == mUi->saveEnemyDeckButton)
        {
            switch (mode)
            {
            case EOptimizeWin: mode = EOptimizeDefense; break;
            case EOptimizeDefense: mode = EOptimizeWin; break;
            default: break;
            }
        }
        else
        {
            if (enemyDeck.isValid())
            {
                switch (enemyDeck.getType())
                {
                case EMissionDeckType:
                case ERaidDeckType:
                case EQuestDeckType:
                    isPvE = true;
                    break;
                default:
                    break;
                }
            }
        }

        QDialog saveDialog(this);
        CDeckSaveWidget* saveWidget = new CDeckSaveWidget(isPvE ? enemyDeck : customDeck, mode, &saveDialog);
        QBoxLayout *vLayout = new QBoxLayout(QBoxLayout::TopToBottom, &saveDialog);
        vLayout->addWidget(saveWidget);
        saveDialog.setWindowTitle("Add custom deck");
        saveDialog.setFixedHeight(saveDialog.sizeHint().height());
        saveDialog.exec();
        customDeck.setName(saveWidget->deckName());
        if (customDeck.isValid())
        {
            mUi->deckManagementWidget->addCustomDeck(customDeck);
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
    CDeck deck = getTriggeredDeck(QObject::sender());
    QString hash;
    if (deck.isValid() && mDecks.deckToHash(deck, hash))
    {
        QApplication::clipboard()->setText(hash);
    }
}

void CMainWindow::copyDeckCards()
{
    CDeck deck = getTriggeredDeck(QObject::sender());
    QString str;
    if (deck.isValid() && mDecks.deckToStr(deck, str))
    {
        QApplication::clipboard()->setText(str);
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

    if (mUi->optimizationModeBox->currentIndex() == 0)
    {
        mUi->optimizationModeBox->setCurrentIndex(1);
    }
    else if (mUi->optimizationModeBox->currentIndex() == 1)
    {
        mUi->optimizationModeBox->setCurrentIndex(0);
    }
}

void CMainWindow::updateView(ECardStatusUpdate status)
{
    mUi->baseDeckWidget->setDeck(mUi->baseDeckEdit->currentText());
    mUi->enemyDeckWidget->setDeck(mUi->enemyDeckEdit->currentText());
    mUi->resultDeckWidget->updateView();
    mUi->cardSearchWidget->updateView(status);
    mUi->deckManagementWidget->updateView();
    mDeckToolTipContent->updateView();
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

void CMainWindow::adjustToDeckType(const QString &deckStr)
{
    const CDeck& deck = mDecks.getDeckForName(deckStr);

    // Update battle ground
    if (deck.isValid())
    {
        const CBattleground &battleground = mCards.getBattlegroundForId(deck.getBattlegroundId());
        if (battleground.isValid())
        {
            mUi->battleGroundBox->setCurrentIndex(mUi->battleGroundBox->findText(battleground.getName()));
        }
        else
        {
            mUi->battleGroundBox->setCurrentIndex(0);
        }
    }
    else
    {
        mUi->battleGroundBox->setCurrentIndex(0);
    }

    // Update mode
    if (deck.isValid() && deck.getType() == ERaidDeckType)
    {
        mUi->optimizationModeBox->setCurrentIndex(2);
        mUi->numTurnsSpinBox->setValue(30);
    }
    else
    {
        mUi->optimizationModeBox->setCurrentIndex(0);
        mUi->numTurnsSpinBox->setValue(50);
    }
}

void CMainWindow::displayDeckMenu()
{
    QMenu menu;
    if (QWidget::sender() == mUi->actionBaseButton)
    {
        menu.addAction(mUi->nameBaseDeckAction);
        menu.addAction(mUi->hashBaseDeckAction);
        menu.addAction(mUi->saveBaseDeckAction);
        menu.addSeparator();
        menu.addAction(mUi->switchDecksAction);

    }
    else if(QWidget::sender() == mUi->actionEnemyButton)
    {
        menu.addAction(mUi->nameEnemyDeckAction);
        menu.addAction(mUi->hashEnemyDeckAction);
        menu.addAction(mUi->saveEnemyDeckAction);
        menu.addSeparator();
        menu.addAction(mUi->switchDecksAction);
        menu.addAction(mUi->multiEnemyDeckAction);
    }
    else if(QWidget::sender() == mUi->actionOptimizedButton)
    {
        //menu.addAction(mUi->useOptimizedDeckAction);
        //menu.addSeparator();
        menu.addAction(mUi->nameOptimizedDeckAction);
        menu.addAction(mUi->hashOptimizedDeckAction);
        menu.addAction(mUi->saveOptimizedDeckAction);
    }
    else
    {
        return;
    }
    QPoint p(QCursor::pos());
    p.setX(qMax(0, p.x() - 5));
    p.setY(qMax(0, p.y() - 5));
    menu.exec(p);
}

void CMainWindow::processStateChanged(QProcess::ProcessState newState)
{
    QString processName = mProcessWrapper->getProcessExecutable().baseName();
    switch(newState)
    {
    case QProcess::Starting:
        mProcessStatusLabel->setText(QString("Process \'%1\' started").arg(processName));
        mUi->optimizerStatusWidget->setStatus(EStatusBusy);
        setProcessActivityChanged(true);
        break;
    case QProcess::Running:
        if (mProcess)
        {
            CProcessPriority::setProcessPriority(*mProcess, mParameters.priority());
        }
        break;
    case QProcess::NotRunning:
        if (mProcess)
        {
            mProcessWrapper->processFinished();
            mProcess->deleteLater();
            mProcess = 0;
            setProcessActivityChanged(false);
        }
    default:
        break;
    }
}

void CMainWindow::processError(QProcess::ProcessError error)
{
    QString processName = mProcessWrapper->getProcessExecutable().baseName();
    if (error != QProcess::UnknownError)
    {
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
            break;
        }
        addConsoleLine(mProcess->errorString());
        mUi->optimizerStatusWidget->setStatus(EStatusResultFailure);
    }
}

void CMainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QString processName = mProcessWrapper->getProcessExecutable().baseName();
    switch(exitStatus)
    {
    case QProcess::CrashExit:
        mProcessStatusLabel->setText(QString("Process \'%1\' crashed").arg(processName));
        mUi->optimizerStatusWidget->setStatus(EStatusResultFailure);
        break;
    default:
        mProcessStatusLabel->setText(QString("\'%1\' finished with code %2").arg(processName).arg(exitCode));
        if (exitCode == 0)
        {
            // Success
            mUi->optimizerStatusWidget->setStatus(EStatusResultSuccess);

            // Update history of all deck input widgets
            mUi->baseDeckEdit->updateHistory();
            mUi->enemyDeckEdit->updateHistory();
            mMultiDeckWidget->initDecks();
            mMultiDeckWidget->updateHistory();
        }
        else
        {
            mUi->optimizerStatusWidget->setStatus(EStatusResultFailure);
        }
        break;
    }
}

void CMainWindow::processReadyReadStandardOutput()
{
    QString toolOutput = QString(mProcess->readAllStandardOutput());
    QStringList outputLines = toolOutput.split(QRegExp("\\n|\\r"), QString::SkipEmptyParts);    
    for (QStringList::const_iterator iLine = outputLines.begin(); iLine != outputLines.end(); ++iLine)
    {
        addConsoleLine(*iLine);
    }
    if (mProcessWrapper)
    {
        mProcessWrapper->processCommandLineOutput(outputLines);
    }
}

void  CMainWindow::downloadProgress(const CCard &card, int numDone, int numDownloads, bool success)
{
    if (numDone == numDownloads)
    {

        mDownloadStatusLabel->setText(QString("Downloading of card pictures..%1").arg(success ? "done" : "failed"));
        mDownloadStatusProgress->setVisible(false);
        CCardLabelNexus::getCardLabelNexus().updateCardPictures(card);
    }
    else
    {
        mDownloadStatusLabel->setText(QString("Downloading of card pictures..(%1/%2)").arg(numDone).arg(numDownloads));
        mDownloadStatusProgress->setMaximum(numDownloads);
        mDownloadStatusProgress->setValue(numDone);
        mDownloadStatusProgress->repaint();
        mDownloadStatusProgress->setVisible(true);
        //if (numDone % 10 == 0)
        {
            CCardLabelNexus::getCardLabelNexus().updateCardPictures(card);
        }
    }
}

void CMainWindow::dataUpdated(const QStringList &result)
{
    mFilterWidget->createOwnedAllCardsFile();
    mUi->updateXmlAction->setEnabled(true);
    mUi->updateXmlBetaAction->setEnabled(true);
    QMessageBox::information(this, "xml update result", result.join("\n"));
}

void CMainWindow::ownedCardsUpdated(const QStringList &result)
{
    QMessageBox::information(this, "owned cards update result", result.join("\n"));
}

void CMainWindow::setOwnedCardsWatchingEnabled(bool enabled)
{
    const CGlobalConfig &cfg = CGlobalConfig::getCfg();
    QStringList watchedDirs = mOwnedCardsWatcher->directories();
    if (enabled)
    {
        if (watchedDirs.isEmpty())
        {
            scanForOwnedCards();
            mOwnedCardsWatcher->addPath(cfg.getToolPath());
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

void CMainWindow::addCard(TCardId cardId)
{
    CDeckInput *deckInput = 0;
    CDeckWidget *deckWidget = 0;
    getActiveDeck(deckInput, deckWidget);
    if (deckInput && deckWidget)
    {
        CDeck deck =deckWidget->getDeck();
        const CCard &newCard = mCards.getCardForId(cardId);
        if (deck.getNumCards() == 0)
        {
            const CCard &whisper = mCards.getCardForId(1003);
            deck.addCard(whisper);
        }
        if (newCard.getType() == ECommanderType)
        {
            if (deck.getNumCards() > 20)
            {
                deck.replaceCard(0, newCard);
            }
            else
            {
                deck.setCommander(newCard);
            }
        }
        else
        {
            if (deck.getNumCards() > 20)
            {
                deck.replaceCard(20, newCard);
            }
            else
            {
                deck.addCard(newCard);
            }
        }
        QString hash;
        mDecks.deckToHash(deck, hash);
        deckInput->setDeckId(hash);
    }
}

void CMainWindow::toggleDeckToolTip(bool isVisible, const QString &deckId)
{
    const CDeck &oldDeck = mDeckToolTipContent->getDeck();
    bool deckChanged = deckId.compare(oldDeck.getName()) != 0;
    if (isVisible && (deckChanged || !mDeckToolTip->isVisible()))
    {
        mDeckToolTipContent->setDeck(deckId);

        QPoint p(QCursor::pos());
        int reqW = mDeckToolTip->width() + 5;
        int reqH = mDeckToolTip->height() + 20;
        int xRel = (QApplication::desktop()->width() - p.x() < reqW)
            ? -reqW
            : 5;
        int yRel = (QApplication::desktop()->height() - p.y() < reqH)
            ? -reqH
            : 20;
        mDeckToolTip->setGeometry(p.x() + xRel, p.y() + yRel, 440, 220);
    }
    if (isVisible != mDeckToolTip->isVisible())
    {
        mDeckToolTip->setVisible(isVisible);
    }

}

void CMainWindow::refreshModels()
{
    mDecks.refresh();
    mCards.refresh();
    mFilterWidget->setOwnedCardsFile(mParameters.ownedCardsFile());
    mParameters.fetchFromUi(*mUi);
    updateView(EOwnedStatusUpdate);
}

void CMainWindow::setToolVersion(const QString &toolVersion)
{
    TOOL_VERSION = CVersion(toolVersion);
    displayAboutMessage();
}

void CMainWindow::setDeckInput(const QString &deckStr, EInputDeckTarget target)
{
    CDeckInput *deckInput = 0;
    CDeckWidget *deckWidget = 0;
    switch(target)
    {
    case BaseDeckInputTarget: deckInput = mUi->baseDeckEdit; break;
    case EnemyDeckInputTarget: deckInput = mUi->enemyDeckEdit; break;
    default:
    case ActiveDeckInputTarget: getActiveDeck(deckInput, deckWidget); break;
    }
    if (deckInput)
    {
        deckInput->setDeckId(deckStr);
        if (deckInput == mUi->enemyDeckEdit)
        {
            adjustToDeckType(deckStr);
        }
    }
}

void CMainWindow::setOptimizationStatus(const SOptimizationStatus &status)
{
    if (mParameters.processMode() != EProcessVersion)
    {
        CDeck deck;
        if (mDecks.hashToDeck(status.deckHash, deck))
        {
            mUi->resultDeckWidget->setWinLabel(status, mParameters.optimizationMode());

            // We have at least one valid card -> enable saving
            mUi->useOptimizedButton->setEnabled(true);
            mUi->useOptimizedDeckAction->setEnabled((true));
            mUi->saveOptimizedDeckAction->setEnabled(true);
            mUi->hashOptimizedDeckAction->setEnabled(true);
            mUi->nameOptimizedDeckAction->setEnabled(true);
            mUi->resultDeckWidget->setDeck(deck);
        }
        else
        {
            mUi->resultDeckWidget->setWinLabel(QPixmap());
            mUi->useOptimizedButton->setEnabled(false);
            mUi->useOptimizedDeckAction->setEnabled(false);
            mUi->saveOptimizedDeckAction->setEnabled(false);
            mUi->hashOptimizedDeckAction->setEnabled(false);
            mUi->nameOptimizedDeckAction->setEnabled(false);
            mUi->resultDeckWidget->setDefaultUnits();
        }
    }
}

void CMainWindow::addConsoleLine(const QString &line, bool truncate)
{
    if (truncate)
    {
        mUi->resultConsoleEdit->clear();
    }
    mUi->resultConsoleEdit->appendPlainText(QString("%1> %2")
                                            .arg(mUi->optimizerStatusWidget->getStatusTime())
                                            .arg(line));
    if (truncate)
    {
        mUi->resultConsoleEdit->moveCursor(QTextCursor::StartOfLine);
    }
}

void CMainWindow::getActiveDeck(CDeckInput *&deckInput, CDeckWidget *&deckWidget) const
{
    if (mUi->baseDeckWidget->isVisible())
    {
        deckInput = mUi->baseDeckEdit;
        deckWidget = mUi->baseDeckWidget;
    }
    else if (mUi->enemyDeckWidget->isVisible())
    {
        deckInput = mUi->enemyDeckEdit;
        deckWidget = mUi->enemyDeckWidget;
    }
    else
    {
        deckInput = mUi->baseDeckEdit;
        deckWidget = mUi->baseDeckWidget;
    }
}

const CDeck& CMainWindow::getTriggeredDeck(const QObject *trigger) const
{
    if (trigger == mUi->nameBaseDeckAction || trigger == mUi->hashBaseDeckAction || trigger == mUi->saveBaseDeckAction)
    {
        return mUi->baseDeckWidget->getDeck();
    }
    else if (trigger == mUi->nameEnemyDeckAction || trigger == mUi->hashEnemyDeckAction || trigger == mUi->saveEnemyDeckAction)
    {
        return mUi->enemyDeckWidget->getDeck();
    }
    else if (trigger == mUi->nameOptimizedDeckAction || trigger == mUi->hashOptimizedDeckAction || trigger == mUi->saveOptimizedDeckAction)
    {
        return mUi->resultDeckWidget->getDeck();
    }
    else if (trigger == mUi->nameBaseDeckButton || trigger == mUi->hashBaseDeckButton || trigger == mUi->saveBaseDeckButton)
    {
        return mUi->baseDeckWidget->getDeck();
    }
    else if (trigger == mUi->nameEnemyDeckButton || trigger == mUi->hashEnemyDeckButton || trigger == mUi->saveEnemyDeckButton)
    {
        return mUi->enemyDeckWidget->getDeck();
    }
    else if (trigger == mUi->nameOptimizedDeckButton || trigger == mUi->hashOptimizedDeckButton || trigger == mUi->saveOptimizedDeckButton)
    {
        return mUi->resultDeckWidget->getDeck();
    }
    return CDeck::INVALID_DECK;
}
