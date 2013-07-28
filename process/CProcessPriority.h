#ifndef CPROCESSPRIORITY_H
#define CPROCESSPRIORITY_H

#include <QProcess>

enum EProcessPriority
{
    ENormalPriority = 0,
    ELowerPriority = 1,
    ELowestPriority = 2
};

class CProcessPriority
{
public:
    CProcessPriority();

 public:
    static void setProcessPriority(QProcess &process, EProcessPriority priority);
};

#endif // CPROCESSPRIORITY_H
