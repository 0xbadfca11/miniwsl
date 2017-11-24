#define WIN32_LEAN_AND_MEAN
#define STRICT_GS_ENABLED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <windows.h>
#include <pathcch.h>
#include <wslapi.h>
#include <atlbase.h>
#include <cstdio>
#include <cstdlib>
#pragma comment(lib, "pathcch")

const WCHAR DistributionName[] = L"miniwsl";

int main()
{
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);
	HMODULE wslapi = LoadLibraryW(L"wslapi");
	if (!wslapi)
	{
		puts("Windows Subsystem for Linux isn't installed");
		ExitProcess(EXIT_FAILURE);
	}
	auto WslIsDistributionRegisteredPtr = AtlGetProcAddressFn(wslapi, WslIsDistributionRegistered);
	auto WslLaunchInteractivePtr = AtlGetProcAddressFn(wslapi, WslLaunchInteractive);
	auto WslRegisterDistributionPtr = AtlGetProcAddressFn(wslapi, WslRegisterDistribution);

	if (!WslIsDistributionRegisteredPtr(DistributionName))
	{
		WCHAR tar_ball[MAX_PATH];
		GetModuleFileNameW(nullptr, tar_ball, ARRAYSIZE(tar_ball));
		PathCchRemoveFileSpec(tar_ball, ARRAYSIZE(tar_ball));
		PathCchCombine(tar_ball, ARRAYSIZE(tar_ball), tar_ball, L"rootfs.tgz");
		HRESULT last_error = WslRegisterDistributionPtr(DistributionName, tar_ball);
		if (FAILED(last_error))
		{
			printf("Installation failed: 0x%08lx", last_error);
			ExitProcess(EXIT_FAILURE);
		}
	}
	ULONG exit_code;
	HRESULT last_error = WslLaunchInteractivePtr(DistributionName, nullptr, TRUE, &exit_code);
	if (FAILED(last_error))
	{
		printf("Launch shell failed: 0x%08lx", last_error);
		ExitProcess(EXIT_FAILURE);
	}
	ExitProcess(exit_code);
}