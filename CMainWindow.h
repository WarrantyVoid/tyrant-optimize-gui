#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProcess>
#include <QComboBox>
#include <QFileSystemWatcher>
#include "process/IProcessWrapper.h"
#include "process/CProcessParameters.h"
#include "model/CCardTable.h"
#include "model/CDeckTable.h"
#include "CDeckWidget.h"
#include "CDeckInput.h"
#include "CCardFilterWidget.h"
#include "CMultiDeckWidget.h"
#include "CDeckManagementWidget.h"

namespace Ui
{
class MainWindow;
}

class CMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

protected:
    void getInputDeck(const CDeckInput *input, CDeck &deck) const;
    void startToolProcess(EProcessMode processMode);
    void killToolProcess();
    void setProcessActivityChanged(bool isActive);
    void loadDefaultSettings();

protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual bool eventFilter(QObject *obj, QEvent *e);

protected slots:

    // Menu slots
	void loadDefaultParameterSettings();
	void saveDefaultParameterSettings();
	void loadParameterSettings();
	void saveParameterSettings();
    void toggleAlwaysOnTop(bool checked);
    void toggleMuteSound(bool checked);
    void toggleCardsShading(bool checked);
    void toggleCardLabelling(bool checked);
    void updateXmlData();
    void updateOwnedCards();
    void displayAboutMessage();

    // Button slots
    void toggleToolProcess();
    void toggleToolResultWidget();
    void checkToolVersion();
    void checkBaseDeck();
    void reorderBaseDeck();
    void saveCustomDeck();
    void copyDeckHash();
    void copyDeckCards();
    void useOptimizedDeck();
    void switchDecks();
    void updateView(ECardStatusUpdate status);
    void updateWindowHeight(bool grow);
    void updateParameterBoxToolTip(int boxIndex);
    void adjustToDeckType(const QString &deckStr);

    // Widget slots
    void processStateChanged(QProcess::ProcessState newState);
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processReadyReadStandardOutput();
    void downloadProgress(int numDone, int numDownloads);
    void dataUpdated(const QStringList &result);
    void ownedCardsUpdated(const QStringList &result);
    void setOwnedCardsWatchingEnabled(bool enabled);
    void scanForOwnedCards();
    void addCard(unsigned int cardId);

    // Hotkey slots
    void refreshModels();

    // Process slots
    void setToolVersion(const QString &toolVersion);
    void setDeckInput(const QString &deckStr, EInputDeckTarget target);
    void setOptimizationStatus(const SOptimizationStatus &status);

private:
    void addConsoleLine(const QString &line, bool truncate = false);
    void getActiveDeckInput(CDeckInput *&deckInput) const;

private:
    Ui::MainWindow *mUi;
    QFileSystemWatcher *mOwnedCardsWatcher;
    QDialog *mDeckToolTip;
    QDialog *mFilterDialog;
    QDialog *mMultiDeckDialog;
    CDeckWidget *mDeckToolTipContent;    
    CCardFilterWidget *mFilterWidget;
    CMultiDeckWidget *mMultiDeckWidget;
    QProcess *mProcess;
    IProcessWrapper *mProcessWrapper;
    QLabel *mProcessStatusLabel;
    QLabel *mDownloadStatusLabel;
    CCardTable &mCards;
    CDeckTable &mDecks;
    CProcessParameters mParameters;

private:
    static const QString VERSION;
    static const QString AUTHOR;
    static const QString HOMEPAGE;
    static const QString FORUM;
    static QString TOOL_VERSION;
};

#endif // MAINWINDOW_H
