#ifndef ITOOLPROCESSWRAPPER_H
#define ITOOLPROCESSWRAPPER_H

#include <QFileInfo>
#include <QString>
#include <QObject>
#include "CProcessParameters.h"

struct SOptimizationStatus
{
    SOptimizationStatus() : deckHash(""), chanceWin(0.0f), chanceStall(0.0f), chanceLoss(0.0f), avRaidDmg(0.0f) { }
    QString deckHash;
    float chanceWin;
    float chanceStall;
    float chanceLoss;
    float avRaidDmg;
};

class IProcessWrapper : public QObject
{
    Q_OBJECT
public:

    /**
    * Used to retrieve path and file name of the tool process's executable.
    * @return File object representing the process executable
    */
    virtual QFileInfo getProcessExecutable() = 0;

    /**
    * Used to retrieve the command line parameters passed when calling tool process.
    * @param guiParams Parameter object representing all supported parameters in GUI
    * @param comLineParams The command line parameters to pass to process
    */
    virtual void getCommandLineParameters(const CProcessParameters &guiParams, QStringList &comLineParams) = 0;

    /**
    * Used to handle initialization.
    */
    virtual void processInit() = 0;

    /**
    * Used to handle the command line output while tool process is running.
    * @param output One or more lines of process std/err output
    */
    virtual void processCommandLineOutput(const QStringList &output) = 0;

    /**
    * Used to handle initialization.
    */
    virtual void processFinished() = 0;


signals:

    /**
    * Signal update of status during simulation (optional).
    * @status Current status including deck, win chance, etc.
    */
    void statusUpdated(SOptimizationStatus status);
};

#endif // ITOOLPROCESSWRAPPER_H
