#ifndef ITOOLPROCESSWRAPPER_H
#define ITOOLPROCESSWRAPPER_H

#include <QFileInfo>
#include <QString>
#include <QObject>
#include "CProcessParameters.h"

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
    * Used to handle the command line output while tool process is running.
    * @param output One or more lines of process std/err output
    */
    virtual void processCommandLineOutput(const QStringList &output) = 0;

signals:

    /**
    * Signal update of current win chance.
    * @winChance Win chance in percent as float
    */
    void winChanceUpdated(float winChance);

    /**
    * Signal update of current average net points.
    * @anp Net points as float
    */
    void anpUpdated(float anp);

    /**
    * Signal update of current optimum deck.
    * @deckHash Hash of the current optimum deck
    */
    void deckUpdated(const QString &deckHash);
};

#endif // ITOOLPROCESSWRAPPER_H
