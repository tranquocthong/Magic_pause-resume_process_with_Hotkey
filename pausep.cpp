// pausep.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "pausep.h"
#include "winuser.h"
# include <stdio.h>
# include <conio.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

BOOL PauseResumeThreadList(DWORD dwOwnerPID, bool bResumeThread) 
{ 
    HANDLE        hThreadSnap = NULL; 
    BOOL          bRet        = FALSE; 
    THREADENTRY32 te32        = {0}; 
 
    // Take a snapshot of all threads currently in the system. 

    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); 
    if (hThreadSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    // Fill in the size of the structure before using it. 

    te32.dwSize = sizeof(THREADENTRY32); 
 
    // Walk the thread snapshot to find all threads of the process. 
    // If the thread belongs to the process, add its information 
    // to the display list.
 
    if (Thread32First(hThreadSnap, &te32)) 
    { 
        do 
        { 
            if (te32.th32OwnerProcessID == dwOwnerPID) 
            {
				HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
				if (bResumeThread)
				{
					cout << _T("Resuming Thread 0x") << cout.setf( ios_base::hex ) << te32.th32ThreadID << '\n';
					ResumeThread(hThread);
				}
				else
				{
					cout << _T("Suspending Thread 0x") << cout.setf( ios_base::hex ) << te32.th32ThreadID << '\n';
					SuspendThread(hThread);
				}
				CloseHandle(hThread);
            } 
        }
        while (Thread32Next(hThreadSnap, &te32)); 
        bRet = TRUE; 
    } 
    else 
        bRet = FALSE;          // could not walk the list of threads 
 
    // Do not forget to clean up the snapshot object. 
    CloseHandle (hThreadSnap); 
 
    return (bRet); 
} 

BOOL ProcessList() 
{
    HANDLE         hProcessSnap = NULL; 
    BOOL           bRet      = FALSE; 
    PROCESSENTRY32 pe32      = {0}; 
 
    //  Take a snapshot of all processes in the system. 
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    //  Fill in the size of the structure before using it. 
    pe32.dwSize = sizeof(PROCESSENTRY32); 
 
    //  Walk the snapshot of the processes, and for each process, 
    //  display information. 

    if (Process32First(hProcessSnap, &pe32)) 
    { 
        do 
        { 
			cout << _T("PID\t") << pe32.th32ProcessID << '\t' << pe32.szExeFile << '\n';
        } 
        while (Process32Next(hProcessSnap, &pe32)); 
        bRet = TRUE; 
    } 
    else 
        bRet = FALSE;    // could not walk the list of processes 
 
    // Do not forget to clean up the snapshot object. 

    CloseHandle (hProcessSnap); 
    return (bRet); 
} 
int hotKeyRegister(int MOD, int type) {
	if (RegisterHotKey(NULL, 1, MOD, type))
	{
		std::cout << "Hotkey registed successfully............." << endl;
	}
	else
	{
		std::cout << "Error code " << GetLastError();
	}
	return 1;
}
int hotKeyCatch(MSG &msg) {
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		std::cout << "\nMessage:" << msg.message;
		if (msg.message == WM_HOTKEY)
		{
			return 1;
		}
	}
}
int _tmain(int argc, TCHAR* argv[], TCHAR* /* envp[] */)
{
	int iPid;
	MSG msg = { 0 };
	ProcessList();
	std::cout << "Enter process id (PID) : "; 
	cin >> iPid;
	DWORD pid = iPid;  //_ttoi(argv[1]);
	if (pid == 0)
	{
		cerr << _T("Invalid PID number: ") << pid << '\n';
		return 1;
	}
	else {
		while (1) {
			hotKeyRegister(MOD_SHIFT, 0x50);
			if (hotKeyCatch(msg))
				PauseResumeThreadList(pid, 0);
			hotKeyRegister(MOD_ALT, 0x50);
			if (hotKeyCatch(msg))
				PauseResumeThreadList(pid, 1);
		}
	}
	return 0;
}