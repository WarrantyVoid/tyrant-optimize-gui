#ifndef CTYRANTOPTIMIZEWRAPPER_H
#define CTYRANTOPTIMIZEWRAPPER_H

#include "IProcessWrapper.h"

class CTyrantOptimizeWrapper : public IProcessWrapper
{
public:
    CTyrantOptimizeWrapper();
    virtual ~CTyrantOptimizeWrapper();

protected:
    QFileInfo getProcessExecutable();
    void getCommandLineParameters(const CProcessParameters &guiParams, QStringList &comLineParams);
    void processCommandLineOutput(const QStringList &output);

    void processInit();
    QString processDeckString(const QString &deckStr);
    void processFinished();

private:
    static const QString PROCESS_NAME;
    SOptimizationStatus mStatus;
};

#endif // CTYRANTOPTIMIZEWRAPPER_H
