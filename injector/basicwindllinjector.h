/*
  basicwindllinjector.h - self-contained copy for standalone build
*/
#ifndef BASICWINDLLINJECTOR_H
#define BASICWINDLLINJECTOR_H

#include <windows.h>

class BasicWinDllInjector
{
public:
    static BOOL addDllDirectory(HANDLE destProcess, wchar_t *dllDirPath);
    static BOOL inject(HANDLE destProcess, wchar_t *dllPath);
    static BOOL injectProcess(int pid, wchar_t *path, wchar_t *probePath);

private:
    static DWORD remoteKernel32Call(HANDLE destProcess, const char *funcName, const wchar_t *argument);
    static void logError(DWORD error = GetLastError());
    static void log(const wchar_t *msg);
};

#endif // BASICWINDLLINJECTOR_H
