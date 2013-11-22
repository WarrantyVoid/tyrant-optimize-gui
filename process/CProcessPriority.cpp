#include "CProcessPriority.h"

#ifdef Q_OS_WIN
    #include <Windows.h>
#elif Q_OS_LINUX
    #include <sys/resource.h>
#else
    #error OS not supported
#endif

CProcessPriority::CProcessPriority()
{

}

void CProcessPriority::setProcessPriority(QProcess &process, EProcessPriority priority)
{
    #ifdef Q_OS_WIN
        _PROCESS_INFORMATION *info = static_cast<_PROCESS_INFORMATION*>(process.pid());
        switch(priority)
        {
        case ENormalPriority:
            SetPriorityClass(info->hProcess, NORMAL_PRIORITY_CLASS);
            break;
        case ELowerPriority:
            SetPriorityClass(info->hProcess, BELOW_NORMAL_PRIORITY_CLASS);
            break;
        case ELowestPriority:
            SetPriorityClass(info->hProcess, IDLE_PRIORITY_CLASS);
            break;
        }
    #elif Q_OS_LINUX
        int id = static_cast<int>(mProcess->pid());
        int prio = getpriority(PRIO_PROCESS, id);
        switch(priority)
        {
        case ENormalPriority:
            setpriority(PRIO_PROCESS, id, 0);
            break;
        case ELowerPriority:
            setpriority(PRIO_PROCESS, id, 10);
            break;
        case ELowestPriority:
            setpriority(PRIO_PROCESS, id, 20);
            break;
        }
    #else
        #error OS not supported
    #endif
}
