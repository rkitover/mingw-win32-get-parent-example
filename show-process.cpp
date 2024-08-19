#include <cstdlib>
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM

#ifdef __CYGWIN__
#define sprintf_s sprintf
#endif

#include <windows.h>
#include <comutil.h>
#include <stringapiset.h>
#include <wbemidl.h>

int
main(int argc, char **argv)
{
	int			 pid = argv[1] ? std::atoi(argv[1]) : GetCurrentProcessId();
	HRESULT			 hr = E_FAIL;
	IWbemLocator		*wbem_locator  = NULL;
	IWbemServices		*wbem_services = NULL;
	IEnumWbemClassObject	*enum_wbem  = NULL;

	CoInitializeEx(0, COINIT_MULTITHREADED);
	CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &wbem_locator);

	wbem_locator->ConnectServer(L"ROOT\\CIMV2", NULL, NULL, NULL, 0, NULL, NULL, &wbem_services);
	wchar_t *query = new wchar_t[4096];
	swprintf(query, 4096, L"select commandline from win32_process where processid = %d", pid);
	wbem_services->ExecQuery(L"WQL", query, WBEM_FLAG_FORWARD_ONLY, NULL, &enum_wbem);
	delete[] query;

	if (enum_wbem != NULL) {
		IWbemClassObject *result = NULL;
		ULONG returned_count = 0;

		if((hr = enum_wbem->Next(WBEM_INFINITE, 1, &result, &returned_count)) == S_OK) {
			VARIANT process_id;
			VARIANT command_line;

			result->Get(L"CommandLine", 0, &command_line, 0, 0);

			wchar_t		*command_line_utf16 = command_line.bstrVal;
			size_t		 size = WideCharToMultiByte(CP_UTF8, 0, command_line_utf16, -1, NULL, 0, NULL, NULL) + 1;
			char		*command_line_utf8 = new char[size];

			WideCharToMultiByte(CP_UTF8, 0, command_line_utf16, -1, command_line_utf8, size, NULL, NULL);

			SysFreeString(command_line_utf16);

			std::cout << pid << ": " << command_line_utf8 << std::endl;

			delete command_line_utf8;

			result->Release();
		}
	}
}
