#include <cwchar>
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM

#ifdef __CYGWIN__
#define sprintf_s sprintf
#endif

#include <windows.h>
#include <comutil.h>
#include <stringapiset.h>
#include <tlhelp32.h>
#include <wbemidl.h>

// The code for retrieving the parent process id on Windows using the Tool Help
// library is based on the example from:
// 
// https://stackoverflow.com/a/558251/262458
// 
// , and the code for the WMI query to retrieve the commandline is from:
// 
// https://stackoverflow.com/a/20082113/262458
// .

int
main(int argc, char** argv)
{
	unsigned pid = argv[1] ? std::atoi(argv[1]) : GetCurrentProcessId();
	int ppid = -1;
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { 0 };
	IWbemLocator* wbem_locator       = nullptr;
	IWbemServices* wbem_services     = nullptr;
	IEnumWbemClassObject* enum_wbem  = nullptr;

	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(h, &pe)) {
		do {
			if (pe.th32ProcessID == pid)
				ppid = pe.th32ParentProcessID;
		} while (Process32Next(h, &pe));
	}

	CloseHandle(h);

	CoInitializeEx(0, COINIT_MULTITHREADED);
	CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
	CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&wbem_locator);

	wbem_locator->ConnectServer(L"ROOT\\CIMV2", nullptr, nullptr, nullptr, 0, nullptr, nullptr, &wbem_services);
	wchar_t* query = new wchar_t[4096];
	swprintf(query, 4096, L"select commandline from win32_process where processid = %d", ppid);
	wbem_services->ExecQuery(L"WQL", query, WBEM_FLAG_FORWARD_ONLY, nullptr, &enum_wbem);
	delete[] query;

	if (enum_wbem) {
		IWbemClassObject *result = nullptr;
		ULONG returned_count = 0;

		if(enum_wbem->Next(WBEM_INFINITE, 1, &result, &returned_count) == S_OK) {
			VARIANT process_id;
			VARIANT command_line;

			result->Get(L"CommandLine", 0, &command_line, 0, 0);

			wchar_t* command_line_utf16 = command_line.bstrVal;
			size_t size = WideCharToMultiByte(CP_UTF8, 0, command_line_utf16, -1, nullptr, 0, nullptr, nullptr) + 1;
			char* command_line_utf8 = new char[size];

			WideCharToMultiByte(CP_UTF8, 0, command_line_utf16, -1, command_line_utf8, size, nullptr, nullptr);

			SysFreeString(command_line_utf16);

			std::cout << ppid << ": " << pid << ": " << command_line_utf8 << std::endl;

			delete[] command_line_utf8;

			result->Release();
		}
	}
}
