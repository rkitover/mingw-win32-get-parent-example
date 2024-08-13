#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define sprintf_s sprintf

#include <windows.h>
#include <comutil.h>
#include <tlhelp32.h>
#include <wbemidl.h>

// From: https://stackoverflow.com/a/20082113/262458

int
main(int argc, char **argv)
{
	int			 pid = argv[1] ? atoi(argv[1]) : GetCurrentProcessId();
	int			 ppid = -1;
	HANDLE			 h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	HRESULT			 hr = E_FAIL;
	PROCESSENTRY32		 pe = { 0 };
	IWbemLocator		*wbem_locator  = NULL;
	IWbemServices		*wbem_services = NULL;
	IEnumWbemClassObject	*enum_wbem  = NULL;

	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(h, &pe)) {
		do {
			if (pe.th32ProcessID == pid)
				ppid = pe.th32ParentProcessID;
		} while (Process32Next(h, &pe));
	}

	CloseHandle(h);

	CoInitializeEx(0, COINIT_MULTITHREADED);
	CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &wbem_locator);

	wbem_locator->ConnectServer(L"ROOT\\CIMV2", NULL, NULL, NULL, 0, NULL, NULL, &wbem_services);
	wbem_services->ExecQuery(L"WQL", L"SELECT ProcessId,CommandLine FROM Win32_Process", WBEM_FLAG_FORWARD_ONLY, NULL, &enum_wbem);

	if (enum_wbem != NULL) {
		IWbemClassObject *result = NULL;
		ULONG returned_count = 0;

		while((hr = enum_wbem->Next(WBEM_INFINITE, 1, &result, &returned_count)) == S_OK) {
			VARIANT process_id;
			VARIANT command_line;

			result->Get(L"ProcessId", 0, &process_id, 0, 0);
			result->Get(L"CommandLine", 0, &command_line, 0, 0);

			if (process_id.uintVal == ppid)
			{
				wchar_t		*command_line_utf16 = command_line.bstrVal;
				size_t		 size = wcslen(command_line_utf16) + 1;
				char		*command_line_utf8 = new char[size];

				wcstombs(command_line_utf8, command_line_utf16, size);

				SysFreeString(command_line_utf16);

				printf("%d: %d: %s\n", pid, ppid, command_line_utf8);

				delete command_line_utf8;
				break;
			}

			result->Release();
		}
	}
}
