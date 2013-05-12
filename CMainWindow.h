#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProcess.h>
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
    void getInputDeck(const QComboBox *input, CDeck &deck) const;
    void startToolProcess(bool isOptimizationEnabled);
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
    void updateXmlData();
    void displayAboutMessage();

    // Button slots
    void toggleToolProcess();
    void checkBaseDeck();
    void saveCustomDeck();
    void copyDeckHash();
    void useOptimizedDeck();
    void switchDecks();
    void updateWindowHeight(bool grow);
    void updateParameterBoxToolTip(int boxIndex);

    // Widget slots
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processReadyReadStandardOutput();
    void processStarted();
    void downloadProgress(int numDone, int numDownloads);
    void dataUpdated(const QStringList &result);
    void ownedCardsUpdated(int numCards);
    void setOwnedCardsWatchingEnabled(bool enabled);
    void scanForOwnedCards();
    void addCard(unsigned int cardId);

    // Process slots
    void setWinChance(float winChance);
    void setAnp(float anp);
    void setActiveEditingDeck(const QString &deckStr);
    void setResultDeckButtonAvailability(const QString &deckHash);

private:
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
	QString mLastDir;

private:
    static const QString VERSION;
};

#endif // MAINWINDOW_H
