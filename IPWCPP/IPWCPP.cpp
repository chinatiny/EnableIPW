// IPWCPP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <netcon.h>
#include <windows.h>
#include <winnt.h>
#include "wlanapi.h"

// http://www.gershnik.com/faq/manage.asp#enable
// wszName is the name of the connection as appears in Network Connections folder
// set bEnable to true to enable and to false to disable
bool EnableInternet(LPCWSTR wszName, bool bEnable)
{
	bool result = false;
	typedef void(__stdcall * LPNcFreeNetconProperties)(NETCON_PROPERTIES* pProps);
	HMODULE hmod = LoadLibrary(L"netshell.dll");
	if(!hmod)
		return false;
	LPNcFreeNetconProperties NcFreeNetconProperties =
		(LPNcFreeNetconProperties)GetProcAddress(hmod, "NcFreeNetconProperties");
	if(!NcFreeNetconProperties)
		return false;

	INetConnectionManager * pMan = 0;

	HRESULT hres = CoCreateInstance(CLSID_ConnectionManager,
		0,
		CLSCTX_ALL,
		__uuidof(INetConnectionManager),
		(void**)&pMan);
	if(SUCCEEDED(hres))
	{
		IEnumNetConnection * pEnum = 0;
		hres = pMan->EnumConnections(NCME_DEFAULT, &pEnum);
		if(SUCCEEDED(hres))
		{
			INetConnection * pCon = 0;
			ULONG count;
			bool done = false;
			while(pEnum->Next(1, &pCon, &count) == S_OK && !done)
			{
				NETCON_PROPERTIES * pProps = 0;
				hres = pCon->GetProperties(&pProps);
				if(SUCCEEDED(hres))
				{
					if(wcscmp(pProps->pszwName, wszName) == 0)
					{
						if(bEnable)
							result = (pCon->Connect() == S_OK);
						else
							result = (pCon->Disconnect() == S_OK);
						done = true;
					}
					NcFreeNetconProperties(pProps);
				}
				pCon->Release();
			}
			pEnum->Release();
		}
		pMan->Release();
	}

	FreeLibrary(hmod);
	return result;
}

BOOL EnableProxy(bool bEnable)
{
    LONG status;
	DWORD dwData = bEnable ? 1 : 0;

	status = RegSetKeyValue( HKEY_CURRENT_USER,
                             L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
                             L"ProxyEnable",
							 REG_DWORD,
							 &dwData,
							 sizeof(DWORD));
	return status == ERROR_SUCCESS;
}

// http://kent-7.blogspot.jp/2009/10/native-wifi-api-wlanapi.html
//-------------------------------------------------------------------------
//  ワイヤレスネットワークに接続する
//
BOOL EnableWifi(LPCWSTR wszName, LPCWSTR wszPassword, bool bEnable)
{
	HANDLE      hWLAN = NULL;
	PWLAN_INTERFACE_INFO_LIST pInterfaceList = NULL;

	DWORD dwNicCount;
	DWORD dwVersion;
	BOOL retCode = FALSE;

	// WLAN ハンドル取得
	if(WlanOpenHandle(WLAN_API_MAKE_VERSION(1, 0), NULL, &dwVersion, &hWLAN) != ERROR_SUCCESS){
		return FALSE;
	}

	// NIC リスト取得
	if(WlanEnumInterfaces(hWLAN, NULL, &pInterfaceList) != ERROR_SUCCESS){
		WlanCloseHandle(hWLAN, NULL);
		return FALSE;
	}

	// NIC 列挙
	for(dwNicCount = 0; dwNicCount < pInterfaceList->dwNumberOfItems; dwNicCount++){
		PWLAN_AVAILABLE_NETWORK_LIST pAvailableNetworkList;
		GUID*       pGuid;

		pGuid = &pInterfaceList->InterfaceInfo[dwNicCount].InterfaceGuid;

		//使用可能なネットワークリストの取得
		if(WlanGetAvailableNetworkList(hWLAN, pGuid, 0, NULL, &pAvailableNetworkList) == ERROR_SUCCESS){
			DWORD dwAvailableCount;

			//見つけたネットワークに対して順に接続を試みる
			for(dwAvailableCount = 0; dwAvailableCount < pAvailableNetworkList->dwNumberOfItems; dwAvailableCount++){
				PWLAN_AVAILABLE_NETWORK  pAvailableNetwork = &pAvailableNetworkList->Network[dwAvailableCount];
				WLAN_CONNECTION_PARAMETERS wlanConnectionParams;

				//接続パラメータ
				ZeroMemory(&wlanConnectionParams, sizeof(WLAN_CONNECTION_PARAMETERS));
				wlanConnectionParams.wlanConnectionMode = wlan_connection_mode_profile;
				wlanConnectionParams.strProfile = pAvailableNetwork->strProfileName;
				wlanConnectionParams.pDot11Ssid = &pAvailableNetwork->dot11Ssid;
				wlanConnectionParams.pDesiredBssidList = NULL;
				wlanConnectionParams.dot11BssType = pAvailableNetwork->dot11BssType;
				wlanConnectionParams.dwFlags = 0;

				//接続
				if(WlanConnect(hWLAN, pGuid, &wlanConnectionParams, NULL) == ERROR_SUCCESS){
					retCode = TRUE;
					break;
				}
			}

			WlanFreeMemory(pAvailableNetworkList);
		}
	}

	WlanFreeMemory(pInterfaceList);
	WlanCloseHandle(hWLAN, NULL);

	return retCode;
}

// Note: Run as Administrator 
int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 2) return 0;

	bool bEnable = wcscmp(argv[1], L"/e") == 0;
	CoInitialize(0);
	EnableInternet(L"ローカル エリア接続", bEnable);
	EnableProxy(bEnable);
	//EnableWifi(L"", L"", bEnable);
	CoUninitialize();
}

