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

    QString processDeckString(const QString &deckStr);

private:
    static const QString PROCESS_NAME;
};

#endif // CTYRANTOPTIMIZEWRAPPER_H
