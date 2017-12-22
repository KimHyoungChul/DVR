#include "StdAfx.h"
#include "DvsSDKAPI.h"
#include "DvsPro16.h"
#include "DVRSocket.h"
#include "CommonLib.h"
#include "LocalFilePlayer.h"
#include "DeviceStream.h"
#include "AudioStream.h"


typedef struct 
{
	Dvs *pDvs;
	char sFileName[DVS_FILE_NAME_LEN];
}RemotePlayUtility;

typedef struct 
{
	CLocalFilePlayer *pLocalFilePlayer;
	char sFileName[DVS_FILE_NAME_LEN];
}LocalPlayUtility;

typedef enum
{
	REAL_VIEW_HANDLE = 0,
    REMOTE_PLAY_HANDLE,
	LOCAL_PLAY_HANDLE
}HANDLE_TYPE;

typedef union
{
	RemotePlayUtility	struRemotePlayUtility;
	LocalPlayUtility	struLocalPlayUtility;
}StruUtility;

typedef struct
{
	 //用来识别当前是属于哪种类型的播放句柄
     int nHandleType;   
	 StruUtility uniStruUtility;
}PlayUtility;

DWORD dwLastErr = 0;
DWORD g_dwConnectTime = 40000;
fMessCallBack gpcbMessFunc = NULL;
DWORD gcbMessUserData = NULL;
BOOL  bInit = FALSE;
DWORD dvrVersion = 0x000000;

//返回函数失败的原因
DVS_API DWORD __stdcall DVS_GetLastError(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return dwLastErr;
}

//函数库初始化
DVS_API BOOL  __stdcall DVS_Init(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//初始化SOCKET库
	BOOL bRet = InitSocket();
	if(bRet)
	{
		dwLastErr = 0;
		bInit = TRUE;
		return TRUE;
	}
	else
	{
		dwLastErr = DVS_SOCKETINIT_ERROR;
		bInit = FALSE;
		return FALSE;
	}
}

DVS_API void  __stdcall DVS_Cleanup(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    WSACleanup();
	bInit = FALSE;
	dwLastErr = DVS_NOERROR;
}

DVS_API DWORD __stdcall DVS_GetSDKVersion(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(!bInit)
	{
       dwLastErr = DVS_NOINIT;
	   return 0;
	}
    
    return 2<<16|0x00000001;
}

DVS_API BOOL  __stdcall DVX_SetConfig(HANDLE hDvx, int cmd, LPVOID lpvoid, int optSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DvsUtility *pDvsUtility = (DvsUtility*)hDvx;
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		return pDvsUtility->pDvs->SetDvrConfig(cmd, lpvoid, optSize);;
	}while(0);
	
	return FALSE;
}

DVS_API int  __stdcall DVX_GetConfig(HANDLE hDvx, int cmd, LPVOID lpvoid, int bufSize, int *pRetLen)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DvsUtility *pDvsUtility = (DvsUtility*)hDvx;
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		if (pDvsUtility == NULL)
		{
			dwLastErr = DVS_LOGIN_USER_EXIST;
			break;
		}

		if (pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_ERROR;
			break;
		}

		return pDvsUtility->pDvs->GetDvrConfig(cmd, lpvoid, bufSize, pRetLen);
	}while(0);

	return 0;
}

DVS_API void   __stdcall DVS_SetConnectTime(DWORD dwWaitTime, DWORD dwTryTimes)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (!bInit)
	{
       dwLastErr = DVS_NOINIT;
	   return;
	}
    g_dwConnectTime = dwWaitTime;
}

DVS_API HANDLE __stdcall DVS_Login(char *pchDeviceIP, WORD wDevicePort, char *pchUserName, char *pchPwd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
       dwLastErr = DVS_NOINIT;
	   return 0;
	}
	
	char cmd[MAX_CMD_LEN + 1] = {0, };
	int cmdlen = 0;	
	SOCKET ConnectSocket = INVALID_SOCKET;
	// normal function body here
	do
	{
		//以下进行控制登陆		
		ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);//创建连接SOCKET
		if (ConnectSocket == INVALID_SOCKET)
		{
			dwLastErr = DVS_NETWORK_ERROR;
			break;
		}
		//设置最大接收缓冲区
		int nRecvbufSize = 0;
		//for(int i = 1;i < 10;i++)
		{
			nRecvbufSize = SOCKET_RECV_BUFFER;//16*1024*i;
			setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nRecvbufSize, sizeof(int));
		}
		//连接指定设备
		sockaddr_in clientService;
		memset(&clientService, 0, sizeof(clientService));
		clientService.sin_family      = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(pchDeviceIP);
		clientService.sin_port        = htons(wDevicePort);
		int nRet = connect(ConnectSocket, (sockaddr*)&clientService, sizeof(sockaddr_in));
		if (SOCKET_ERROR == nRet)
		{
			TRACE("IP %s, Port %d, User %s, Password %s, Error code = %d\n", pchDeviceIP, wDevicePort, pchUserName, pchPwd, WSAGetLastError());
			dwLastErr = DVS_NETWORK_ERROR;
			break;
		}

		//设置为非阻塞式
		u_long iMode = 1;
		ioctlsocket(ConnectSocket, FIONBIO, &iMode);

		//发送登陆认证需要的信息
		NET_REMOTE_ACCESS remoteAccess;
		memset(&remoteAccess, 0, sizeof(remoteAccess));
		remoteAccess.AccessType = NET_CLIENT_CONFIG;
		memcpy(remoteAccess.UserName, pchUserName, sizeof(remoteAccess.UserName));
		memcpy(remoteAccess.PassWord, pchPwd, sizeof(remoteAccess.PassWord));
		cmdlen = PackCommand(&remoteAccess, sizeof(remoteAccess), USER_ACCESS, cmd, sizeof(cmd), 0);		
		if (send(ConnectSocket, cmd, cmdlen, 0) != cmdlen)
		{
			dwLastErr = DVS_SEND_PARAM_FAILED;
			break;
		}

		//等待直到收到登录确认
		fd_set r;
		FD_ZERO(&r);
		FD_SET(ConnectSocket, &r);
		struct timeval timeout;
		timeout.tv_sec = g_dwConnectTime/1000;
		timeout.tv_usec = g_dwConnectTime%1000;
		int iError = select(ConnectSocket + 1, &r, 0, 0, &timeout);
		if (SOCKET_ERROR == iError)
		{
			TRACE("IP %s, Port %d, User %s, Password %s, Error code = %d\n", pchDeviceIP, wDevicePort, pchUserName, pchPwd, WSAGetLastError());
			dwLastErr = DVS_NETWORK_ERROR;
			break;
		}
		else if (0 == iError)
		{
			dwLastErr = DVS_LOGIN_TIMEOUT;
			break;
		}
		
		KTCMDHDR hdr = {0, };
		int temp = recv(ConnectSocket, (char*)&hdr, sizeof(hdr), 0);
		if (temp != sizeof(hdr))
		{
			dwLastErr = DVS_RECV_PARAM_FAILED;
			break;
		}
		
		if (recv(ConnectSocket, cmd, hdr.totalSize - sizeof(hdr), 0) != hdr.totalSize - sizeof(hdr))
		{
			dwLastErr = DVS_RECV_PARAM_FAILED;
			break;
		}

		int nRecvMsgType = ((int)hdr.infoType);
		int nDvsType = 0;
		DevInitUtility accessUtility;
		memset((void*)&accessUtility, 0, sizeof(accessUtility));
		//已达最大用户数
		if (CLIENT_IS_FULL == nRecvMsgType)
		{
			dwLastErr = DVS_ADD_USER_FULL;
			break;
		}
		//密码错误
		if (USER_PASSWORD_ERROR == nRecvMsgType)
		{
			dwLastErr = DVS_LOGIN_PWD_ERROR;
			break;
		}
		//用户名无效
		else if (USER_NAME_INVALID == nRecvMsgType)
		{			
			dwLastErr = DVS_LOGIN_USER_INVALID;
			break;
		}
		//该用户已经存在
		else if (USER_HAS_ACCESSED == nRecvMsgType)
		{			
			dwLastErr = DVS_LOGIN_USER_EXIST;
			break;
		}
		else if (ACCESS_ACCEPT == nRecvMsgType)
		{
			accessUtility.sCmdSocket = ConnectSocket;
			accessUtility.nCmdPort	 = wDevicePort;
			accessUtility.nIDCode	 = ((int)hdr.idCode);
			accessUtility.nRemoteViewIP = inet_addr(pchDeviceIP);
			//实时监视校验使用			
			NET_USER_ACCESS *pAccess = (NET_USER_ACCESS*)cmd;      
			accessUtility.userAccess.AccessCode = (pAccess->AccessCode);
			accessUtility.userAccess.DeviceType = (pAccess->DeviceType);
			accessUtility.userAccess.SysVersion = (pAccess->SysVersion);
		}
		else
		{			
			dwLastErr = DVS_ERROR;
			break;
		}

		DvsUtility *pDvsUtility = new DvsUtility;
		Dvs *pDvs = NULL;
		if (NULL == pDvsUtility)
		{
			dwLastErr = DVS_ERROR;
			break;
		}

		pDvsUtility->streamType = 0;
		pDvs = new DvsPro16(accessUtility);
		//初始化设备信息
		if (pDvs->InitDvs())
		{
			pDvsUtility->pDvs		= pDvs;
			pDvsUtility->nDvsType	= nDvsType;
			pDvsUtility->userId		= accessUtility.nIDCode;
			pDvsUtility->accessId	= accessUtility.userAccess.AccessCode;
			//pDvs->SetDvrVersion(accessUtility.userAccess.SysVersion);
			dvrVersion				= accessUtility.userAccess.SysVersion;//DVR版本

			memset(pDvsUtility->domain, 0, sizeof(pDvsUtility->domain));
			sprintf(pDvsUtility->domain, "%s", pchDeviceIP);
			dwLastErr = DVS_NOERROR;
			
			pDvsUtility->pDvs->SetDvrAddr(pchDeviceIP);
			pDvsUtility->pDvs->SetCmdPort(wDevicePort);

			return (HANDLE)pDvsUtility;
		}
		else
		{
			if (pDvs) 
			{
				delete pDvs;
				pDvs = NULL;
			}
			if (pDvsUtility)
			{
				delete pDvsUtility;
				pDvsUtility = NULL;
			}
			dwLastErr = DVS_NETWORK_ERROR;
			break;
		}
	}while(0);
	
	if (ConnectSocket != INVALID_SOCKET)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	return NULL;
}

DVS_API BOOL __stdcall DVS_Logout(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
	// normal function body here
	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL) 
	{
		pDvsUtility->pDvs->Logout();
		delete pDvsUtility->pDvs;
		pDvsUtility->pDvs = NULL;
		delete pDvsUtility;
		pDvsUtility = NULL;
	}

	dwLastErr = DVS_NOERROR; 
	return TRUE;
}

//设置消息回调函数
DVS_API void   __stdcall DVS_SetDVRMessCallBack(fMessCallBack cbMessFunc, DWORD dwUser)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
	}
    gpcbMessFunc = cbMessFunc;
	gcbMessUserData = dwUser;
}

DVS_API BOOL   __stdcall DVS_StartListen(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
	
	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL) 
	{
		pDvsUtility->pDvs->StartDVRMessCallBack(gpcbMessFunc, hDvs, gcbMessUserData);
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}

	return FALSE;
}

DVS_API BOOL   __stdcall DVS_StopListen(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		pDvsUtility->pDvs->StopDVRMessCallBack();
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}

	return FALSE;
}

//获取设备信息
DVS_API BOOL __stdcall DVS_GetDeviceInfo(HANDLE hDvs, LPDVS_DEV_INFO lpDeviceInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
    
	if (NULL == lpDeviceInfo)
	{
		dwLastErr = DVS_PARAM_ERROR;
		return FALSE;
	}
	
	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->GetDeviceInfo(lpDeviceInfo);
	}

	return FALSE;
}

//对讲相关函数
DVS_API BOOL   __stdcall DVS_SpeechStart(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (NULL == hDvs)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		char cIP[DVX_MAX_DOMAINLEN + 1] = {0};
		strcpy(cIP, pDvsUtility->pDvs->GetDvrAddr());
		if (pDvsUtility->pDvs != NULL)
		{
			char netSetup[sizeof(DVX_NETSETUP_VER4)];
			memset(netSetup, 0, sizeof(DVX_NETSETUP_VER4));
			int getStructLen = sizeof(DVX_NETSETUP_VER1);
			if (dvrVersion >= ADD_EMAILINTERVAL)
				getStructLen = sizeof(DVX_NETSETUP_VER4);
			else if (dvrVersion >= ADD_CDMA_VERSION)
				getStructLen = sizeof(DVX_NETSETUP_VER4) - sizeof(UNS16) * 2;
			else if (dvrVersion >= MODIFY_DDNS_VERSION)
				getStructLen = sizeof(DVX_NETSETUP_VER3);
			else if (dvrVersion >= ADD_PPPOE_VERSION)
				getStructLen = sizeof(DVX_NETSETUP_VER2);
			int iRet = DVX_GetConfig(hDvs, SETUP_NET, netSetup, getStructLen, NULL);
			if (!iRet || iRet == DVS_VERSION_NOT_MATCH)
			{
				break;
			}

			UNS16 intercomPort = 0;
			DVRSETUP_UNION netSetupUnion;
			memset(&netSetupUnion, 0, sizeof(DVRSETUP_UNION));
			memcpy((S8*)&netSetupUnion, netSetup, getStructLen);
			if (dvrVersion >= ADD_CDMA_VERSION)
				intercomPort = netSetupUnion.NetSetupVer4.RemoteAudioPort;
			else if (dvrVersion >= MODIFY_DDNS_VERSION)
				intercomPort = netSetupUnion.NetSetupVer3.RemoteAudioPort;
			else if (dvrVersion >= ADD_PPPOE_VERSION)
				intercomPort = netSetupUnion.NetSetupVer2.RemoteAudioPort;
			else
				intercomPort = netSetupUnion.NetSetupVer1.RemoteAudioPort;
			return pDvsUtility->pDvs->SpeechStart(cIP, intercomPort);
		}
	}while(false);

	return FALSE;
}

DVS_API void   __stdcall DVS_SpeechStop(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return ;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return ;
	}

	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		pDvsUtility->pDvs->SpeechStop();
	}
}

DVS_API BOOL   __stdcall DVS_IsOnSpeech(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->IsSpeech();
	}

	return FALSE;
}

DVS_API BOOL   __stdcall DVS_SetStreamType(HANDLE hDvs, int type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	if (type != 0 && type != 1)
	{
		return FALSE;
	}

	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		pDvsUtility->streamType = type;
		return TRUE;
	}

	return FALSE;
}

DVS_API BOOL   __stdcall DVS_OpenRealStream(HANDLE hDvs, char *pchHostIP, WORD wHostPort, DWORD dwChannel, HWND hWnd, HANDLE &hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hDvs)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		DvsUtility *pDvs = (DvsUtility*)hDvs;
		CDeviceStream *pReal = new CDeviceStream();
		if (pReal == NULL)
		{
			dwLastErr = DVS_ERROR;
			break;
		}
		
		unsigned char videoSignal = 0;
		//unsigned char videoRes    = 0;
		unsigned char biAudio     = 0;
		CString strName;
		
		if (pDvs->pDvs != NULL)
		{
			//pDvs->pDvs->GetVideoResolution(dwChannel, pDvs->streamType, videoRes);
			pDvs->pDvs->GetChnnlName(dwChannel, strName);
			pDvs->pDvs->GetBiAudio(dwChannel, biAudio);
			pDvs->pDvs->GetVideoSignalType(videoSignal);  // 0---NTSC, 1---PAL
			pReal->SetSignalType(videoSignal);
			pReal->SetChnnlName(strName);
			pReal->SetUserInfo(pDvs->userId, pDvs->accessId);

			if (!pReal->OpenRealStream(hWnd, pDvs->domain, pDvs->pDvs->GetCmdPort() - 2, dwChannel, biAudio, pDvs->streamType, pDvs->pDvs->m_bFluenceFirst))
			{
				dwLastErr = DVS_CUR_STREAMCHANNEL_OPENED;
				delete pReal;
				hRealHandle = NULL;
				break;
			}

			hRealHandle = (HANDLE)pReal;
			return TRUE;
		}
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_CloseRealStream(HANDLE hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
		pReal->CloseRealStream();
		delete pReal;
		dwLastErr = DVS_NOERROR;
		
		return TRUE;
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_PlayRealStream(HANDLE hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;		
		return pReal->PlayRealStream(NULL);
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_StopRealStream(HANDLE hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
		pReal->StopRealStream();
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_PlayRealAudio(HANDLE hDvs, HANDLE hRealHandle, DWORD dwChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (NULL == hRealHandle || NULL == hDvs)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		DvsUtility *pDvs = (DvsUtility*)hDvs;
		unsigned char biAudio = 0;
		if(pDvs->pDvs != NULL)
		{
			pDvs->pDvs->GetBiAudio(dwChannel, biAudio);
		}
		
		//此处仍需完善
		CAudioStream *pReal = (CAudioStream*)hRealHandle;
		pReal->PlayRealAudio(biAudio);
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_StopRealAudio(HANDLE hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CAudioStream *pReal = (CAudioStream*)hRealHandle;
		pReal->StopRealAudio();
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_GetRealBiAudio(HANDLE hRealHandle, unsigned char &biAudio)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
		biAudio = pReal->GetBiAudio();
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}while(false);

	return FALSE;
}

DVS_API BOOL   __stdcall DVS_OpenAudioStream(HANDLE hDvs, char *pchHostIP, WORD wHostPort, HANDLE &hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hDvs)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		DvsUtility *pDvs = (DvsUtility*)hDvs;
		if (hRealHandle != NULL)
		{
			dwLastErr = DVS_NOERROR;
			return TRUE;
		}
		
		CAudioStream *pRealAudio = new CAudioStream();		
		if (pRealAudio == NULL)
		{
			dwLastErr = DVS_ERROR;
			break;
		}
		
		pRealAudio->SetUserInfo(pDvs->userId, pDvs->accessId);		
		if (!pRealAudio->OpenAudioStream(pDvs->domain, pDvs->pDvs->GetCmdPort() - 2))
		{
			dwLastErr = DVS_CUR_STREAMCHANNEL_OPENED;
			delete pRealAudio;
			hRealHandle = NULL;
			break;
		}

		hRealHandle = (HANDLE)pRealAudio;
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_CloseAudioStream(HANDLE hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CAudioStream *pReal = (CAudioStream*)hRealHandle;
		pReal->CloseAudioStream();
		delete pReal;
		
		dwLastErr = DVS_NOERROR;		
		return TRUE;
	}while(false);
	
	return FALSE;
}

DVS_API int    __stdcall DVS_GetRealChnnl(HANDLE hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;		
		return pReal->GetChnnlNum();
	}while(false);
	
	return 0;
}

//////////////////////////////////////////////////////////////
////////
//函数名: DVS_SaveRealData
//作者:   czg
//日期:   2009-5-22
//功能:   云台控制函数
//参数:   hRealHandle           实时流句柄，由DVS_OpenRealStream函数获取
//        sFileName             保存文件名
//返回值; FALSE表示失败，TRUE表示成功
/////////
//////////////////////////////////////////////////////////////
DVS_API BOOL   __stdcall DVS_SaveRealDataStart(HANDLE hRealHandle, char *sFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
		int nRet = pReal->SaveRealData(sFileName);
		if (nRet == 0)
		{
			dwLastErr = DVS_NOERROR;
			return TRUE;
		}
		
		if (nRet == 1)
		{
			dwLastErr = DVS_CUR_STREAMCHANNEL_NOTEXIST;
			break;
		}
		
		if (nRet == 2)
		{
			dwLastErr = DVS_OPEN_FILE_ERROR;
			break;
		}
		
		if (nRet == 3)
		{
			dwLastErr = DVS_REAL_ALREADY_SAVING;
			break;
		}		
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_SaveRealDataStop(HANDLE hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
		pReal->StopRealData();
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_IsRecording(HANDLE hRealHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
		return pReal->IsRecording();		
	}while(false);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_RealCapBmp(HANDLE hRealHandle, const char* pathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
		return pReal->CapBmp(pathname);
	}while(false);

	return FALSE;
}

//////////////////////////////////////////////////////////////
////////
//函数名: DVS_PTZControl
//作者:   czg
//日期:   2009-4-27
//功能:   云台控制函数
//参数:   hDvs                  函数登陆句柄，DVS_Login的返回值
//        dwChannel             通道号,从1开始
//        dwPTZCommand          云台控制指令,具体见DVS_PTZ_CTRL_MODE宏定义注释
//        dwParam               调用预置或者设置预置点的组号,其它指令此参数不用，默认为0
//返回值; FALSE表示失败，TRUE表示成功
/////////
//////////////////////////////////////////////////////////////
DVS_API BOOL   __stdcall DVS_PTZControl(HANDLE hDvs, DWORD dwChannel, DWORD dwPTZCommand, DWORD dwParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

    DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		int nRet = pDvsUtility->pDvs->PTZControl(dwChannel, dwPTZCommand, dwParam);
		if (nRet == 0)
		{
			dwLastErr = DVS_NOERROR;
			return TRUE;
		}
		else if (nRet == 1)
		{
			dwLastErr = DVS_CUR_STREAMCHANNEL_NOTEXIST;
			return FALSE;
		}
		else if (nRet == 2)
		{
			dwLastErr = DVS_PARAM_ERROR;
			return FALSE;
		}
		else
		{
			dwLastErr = DVS_ERROR;
			return FALSE;
		}
	}

	return FALSE;
}

DVS_API BOOL   __stdcall DVS_QueryRecordFile(HANDLE hDvs, DVS_TIME dtStartTime, DVS_TIME dtEndTime, int type, LPDVS_RECORDFILE_LIST lpRecordfileList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
	if (NULL == lpRecordfileList)
	{
		dwLastErr = DVS_PARAM_ERROR;
		return FALSE;
	}
    DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		int nRet = pDvsUtility->pDvs->QueryRecordFile(dtStartTime, dtEndTime, type, lpRecordfileList);
		if (nRet == 0)
		{
			dwLastErr = DVS_NOERROR;
			return TRUE;
		}
		else if (nRet == 1)
		{
			dwLastErr = DVS_ERROR;
			return FALSE;
		}
		else if (nRet == 2)
		{
			dwLastErr = DVS_NETWORK_ERROR;
			return FALSE;
		}
		else
		{
			dwLastErr = DVS_NOERROR;
			return TRUE;
		}
	}

	return FALSE;
}

DVS_API UNS32 __stdcall DVS_GetRemoteFileInfo(HANDLE hDvs, char *pchRemoteFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->GetRemoteFileInfo_Ex(pchRemoteFileName);
	}

	return FALSE;
}

DVS_API BOOL   __stdcall DVS_OpenRemotePlay(HANDLE hDvs, char *pchRemoteFileName, HWND *pHwndList, DWORD dwHwndCount, HANDLE &hPlayHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
	if (NULL == pHwndList)
	{
		dwLastErr = DVS_PARAM_ERROR;
        return FALSE;
	}
    DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		int nRet = pDvsUtility->pDvs->OpenRemotePlay(pchRemoteFileName, pHwndList, dwHwndCount);
		if (nRet == 0)
		{
			PlayUtility *pPlayUtility = new PlayUtility;
			pPlayUtility->nHandleType = REMOTE_PLAY_HANDLE;
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs = pDvsUtility->pDvs;
			memcpy(pPlayUtility->uniStruUtility.struRemotePlayUtility.sFileName, pchRemoteFileName, DVS_FILE_NAME_LEN);
			hPlayHandle = (HANDLE*)pPlayUtility;
			dwLastErr = DVS_NOERROR;
			return TRUE;
		}
		else if (nRet == 1)
		{
			dwLastErr = DVS_ERROR;
			return FALSE;
		}
		else if (nRet == 2)
		{
			dwLastErr = DVS_NETWORK_ERROR;
			return FALSE;
		}
		else if (nRet == 3)
		{
			dwLastErr = DVS_GET_FILEINDEX_ERROR;
			return FALSE;
		}
		else
		{
			dwLastErr = DVS_NOERROR;
			return TRUE;
		}
	}

	return FALSE;
}

DVS_API BOOL   __stdcall DVS_CloseRemotePlay(HANDLE hPlayHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->CloseRemotePlay();
	delete pPlayUtility;
	pPlayUtility = NULL;
	dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_SetRemotePlayChannel(HANDLE hPlayHandle, DWORD dwBitMaskChn)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
    
	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->SetRemotePlayChannel(dwBitMaskChn);

	dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_RemotePlayControl(HANDLE hPlayHandle, DWORD dwControlCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

    PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->RemotePlayControl(dwControlCode);

	dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_GetRemotePlayTime(HANDLE hPlayHandle, LPDVS_TIME lpCurPlayTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
    
	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	if (pPlayUtility->nHandleType == REMOTE_PLAY_HANDLE)
	{
		BOOL bRet = pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->GetRemotePlayTime(lpCurPlayTime);
		if (!bRet)
		{
			dwLastErr = DVS_ERROR;
			return FALSE;
		}
	}
	else
	{
        dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_StartRemotePlayAudio(HANDLE hPlayHandle, DWORD dwChannel)
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	
	if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
    
	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->StartRemotePlayAudio(dwChannel);

	dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_StopRemotePlayAudio(HANDLE hPlayHandle, DWORD dwChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
    
	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->StopRemotePlayAudio();

	dwLastErr = DVS_NOERROR;
	return TRUE;
}
//////////////////////////////////////////////////////////////
////////
//函数名: DVS_SeekRemotePlay
//作者:   czg
//日期:   2009-4-22
//功能:   按照指定的参数值设置远程播放文件的的播放位置
//参数:   hPlayHandle           播放句柄，DVS_OpenRemotePlay的返回值
//        dwType                设置类型
//                              如1按照时间定位，相对当前文件开始时间偏移量，单位为秒
//                              如2按照字节定位，相对当前文件开始处偏移大小，单位为byte
//        dwArg                 按照参数2指定类型设置参数值，目前暂不支持第二种定为类型
//返回值; FALSE表示失败，TRUE表示成功
/////////
//////////////////////////////////////////////////////////////
DVS_API BOOL   __stdcall DVS_SeekRemotePlay(HANDLE hPlayHandle, DWORD dwType, DWORD64 dwArg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
    
	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	//按照时间定位
	if (dwType == 1)
	{
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->SeekRemotePlayByTime((DWORD)dwArg);
	}
	
	dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL __stdcall DVS_DumpOpen(HANDLE hDvs, char *pDumpFile, char *pSaveAs, UNS32 startTime, UNS32 endTime, UNS32 chnnlMask, int dumpType, BOOL isOcx)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}

	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->RemoteDumpOpen(pDumpFile, pSaveAs, startTime, endTime, chnnlMask, dumpType, isOcx);
	}

	return FALSE;
}

DVS_API void   __stdcall DVS_DumpClose(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (NULL == hDvs)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility->pDvs != NULL)
		{
			pDvsUtility->pDvs->RemoteDumpClose();
		}
	}while(0);
}

DVS_API int   __stdcall DVS_DumpProgress(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (NULL == hDvs)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility->pDvs != NULL)
		{
			return pDvsUtility->pDvs->RemoteDumpProgress();
		}
	}while(0);
	
	return -1;
}

DVS_API BOOL   __stdcall DVS_OpenLocalPlay(char *pchLocalFileName, HWND *pHwndList, DWORD dwHwndCount, HANDLE &hPlayHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == pHwndList)
	{
		dwLastErr = DVS_PARAM_ERROR;
        return FALSE;
	}

	BOOL bRet = FALSE;
    PlayUtility *pPlayUtility = new PlayUtility;
	pPlayUtility->nHandleType = LOCAL_PLAY_HANDLE;
	pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer = new CLocalFilePlayer;
	bRet = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer->OpenLocalPlay(pchLocalFileName, pHwndList, dwHwndCount);
	if (bRet)
	{
		memcpy(pPlayUtility->uniStruUtility.struLocalPlayUtility.sFileName, pchLocalFileName, DVS_FILE_NAME_LEN);
		hPlayHandle = (HANDLE*)pPlayUtility;
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}
	else
	{
		delete pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
		delete pPlayUtility;
		dwLastErr = DVS_ERROR;
		return FALSE;
	}
}

DVS_API BOOL   __stdcall DVS_GetDuration(HANDLE hPlay, UINT32 &startTime, UINT32 &endTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		if (NULL == hPlay)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlay;
		CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
		if (pLocalFilePlayer == NULL)
		{
			break;
		}

		return pLocalFilePlayer->GetDuration(startTime, endTime);	
	}while(0);
	
	return FALSE;
}

DVS_API BOOL   __stdcall DVS_CloseLocalPlay(HANDLE hPlayHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
	pLocalFilePlayer->ClosePlayer();
    
	delete pLocalFilePlayer;
	pLocalFilePlayer = NULL;
	delete pPlayUtility;
	pPlayUtility = NULL;

    dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_SetLocalPlayChannel(HANDLE hPlayHandle, DWORD dwChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer->SetLocalPlayChannel(dwChannel);

	dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_LocalPlayControl(HANDLE hPlayHandle, DWORD dwControlCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
	switch(dwControlCode)
	{
	case DVS_PLAYCTRL_BACKWARD:
		{
			int rate = pLocalFilePlayer->m_RateScale;
			switch (rate)
			{
			case 1:
				pLocalFilePlayer->SetPlayRate(2);
				break;
			case 2:
				pLocalFilePlayer->SetPlayRate(4);
				break;
			case 4:
				pLocalFilePlayer->SetPlayRate(8);
				break;
			case 8:
				pLocalFilePlayer->SetPlayRate(16);
				break;
			case 16:
				pLocalFilePlayer->SetPlayRate(32);
				break;
			case 32:
				pLocalFilePlayer->SetPlayRate(2);
				break;
			default:
				pLocalFilePlayer->SetPlayRate(1);
				break;
			}
			pLocalFilePlayer->FastBackward();
			pLocalFilePlayer->ShowPlayRate(TRUE);
			pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
		}
		break;
	case DVS_PLAYCTRL_BACKPLAY:
		if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
		{
			pLocalFilePlayer->ShowPlayRate(FALSE);
			pLocalFilePlayer->SetPlayRate(1);
			pLocalFilePlayer->Backward();
			pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
		}
		break;
	case DVS_PLAYCTRL_PLAY:
		if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
		{
			pLocalFilePlayer->ShowPlayRate(FALSE);
			pLocalFilePlayer->SetPlayRate(1);
			pLocalFilePlayer->SetPlayIFrame(FALSE);
			pLocalFilePlayer->PlayAllVideo();
			pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
		}
		break;
	case DVS_PLAYCTRL_FORWARD:
		{
			int rate = pLocalFilePlayer->m_RateScale;
			switch (rate)
			{
			case 1:
				pLocalFilePlayer->SetPlayRate(2);
				break;
			case 2:
				pLocalFilePlayer->SetPlayRate(4);
				break;
			case 4:
				pLocalFilePlayer->SetPlayRate(8);
				break;
			case 8:
				pLocalFilePlayer->SetPlayRate(16);
				break;
			case 16:
				pLocalFilePlayer->SetPlayRate(32);
				break;
			case 32:
				pLocalFilePlayer->SetPlayRate(2);
				break;
			default:
				pLocalFilePlayer->SetPlayRate(1);
				break;
			}
			pLocalFilePlayer->SetPlayIFrame(TRUE);
			pLocalFilePlayer->PlayAllVideo();
			pLocalFilePlayer->ShowPlayRate(TRUE);
			pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
		}
		break;
	case DVS_PLAYCTRL_BACKSHIFT:
		if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
		{
			pLocalFilePlayer->ShowPlayRate(FALSE);
			pLocalFilePlayer->SetPlayRate(1);
		}
		pLocalFilePlayer->StepBack();
		pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
		break;
	case DVS_PLAYCTRL_SHIFT:
		pLocalFilePlayer->ShowPlayRate(FALSE);
		pLocalFilePlayer->StepNext();
		pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
		break;
	case DVS_PLAYCTRL_PAUSE:
		if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
		{
			pLocalFilePlayer->ShowPlayRate(FALSE);
			pLocalFilePlayer->Pause();
			pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
		}
		break;
	case DVS_PLAYCTRL_STOP:
		if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
		{
			pLocalFilePlayer->ShowPlayRate(FALSE);
			pLocalFilePlayer->Stop();
			pLocalFilePlayer->Pause();
			pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
		}
		break;
	default:
		pLocalFilePlayer->ShowPlayRate(FALSE);
        dwLastErr = DVS_PARAM_ERROR;
		return FALSE;
	}

	dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_GetLocalPlayCurTime(HANDLE hPlayHandle, UINT32 &curTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (NULL == hPlayHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
		if (pLocalFilePlayer == NULL)
		{
			break;
		}

		curTime = pLocalFilePlayer->GetCurPlayTime();
		dwLastErr = DVS_NOERROR;
		return TRUE;
	}while(0);

	return FALSE;
}

DVS_API BOOL   __stdcall DVS_StartLocalPlayAudio(HANDLE hPlayHandle, DWORD dwChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

    PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
	
	pLocalFilePlayer->PlayAudio(dwChannel - 1);
    dwLastErr = DVS_NOERROR;
	return TRUE;
}

DVS_API BOOL   __stdcall DVS_StopLocalPlayAudio(HANDLE hPlayHandle, DWORD dwChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

    PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;

	pLocalFilePlayer->StopAudio();
	dwLastErr = DVS_NOERROR;
	return TRUE;
}
//////////////////////////////////////////////////////////////
////////
//函数名: DVS_SeekLocalPlay
//作者:   czg
//日期:   2009-4-23
//功能:   按照指定的参数值设置本地播放文件的的播放位置
//参数:   hPlayHandle           播放句柄，DVS_OpenRemotePlay的返回值
//        dwType                设置类型
//                              如1按照时间定位，相对当前文件开始时间偏移量，单位为秒
//                              如2按照字节定位，相对当前文件开始处偏移大小，单位为byte
//        dwArg                 按照参数2指定类型设置参数值，目前暂不支持第二种定位类型
//返回值; FALSE表示失败，TRUE表示成功
/////////
//////////////////////////////////////////////////////////////
DVS_API BOOL   __stdcall DVS_SeekLocalPlay(HANDLE hPlayHandle, DWORD dwType, DWORD64 dwArg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

    PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;

    if (dwType == 1)
	{
		pLocalFilePlayer->SeekRemotePlayByTime((DWORD)dwArg);
	}
	else
	{
        dwLastErr = DVS_PARAM_ERROR;
		return FALSE;
	}

	dwLastErr = DVS_NOERROR;
	return TRUE;
}

//判断文件是否播放到结尾
DVS_API BOOL   __stdcall DVS_PlayIsEnd(HANDLE hPlayHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BOOL bEnd = TRUE;
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		if (NULL == hPlayHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;		
		if (pPlayUtility->nHandleType == REMOTE_PLAY_HANDLE)
		{
			bEnd = pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->RemotePlayIsEnd();
		}

		if (pPlayUtility->nHandleType == LOCAL_PLAY_HANDLE)
		{
			CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
			bEnd = pLocalFilePlayer->IsEnd();
		}
	}while(0);

	return bEnd;
}

//////////////////////////////////////////////////////////////
////////
//函数名: DVS_CapturePicture
//作者:   czg
//日期:   2009-6-9
//功能:   保存图片，对显示图片进行瞬间截图
//参数:   hPlayHandle           播放句柄，DVS_OpenRealStream，DVS_OpenRemotePlay，DVS_OpenLocalPlay的返回值
//        pchPicFileName        位图文件名，当前只支持BMP文件
//返回值; FALSE表示失败，TRUE表示成功
/////////
//////////////////////////////////////////////////////////////
DVS_API BOOL   __stdcall DVS_CapturePicture(HANDLE hPlayHandle, DWORD dwChannel, char *pchPicFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
    if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

    PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	if (pPlayUtility->nHandleType == REMOTE_PLAY_HANDLE)
	{
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->CaptureRemotePlayPicture(dwChannel, pchPicFileName);
	}

	if (pPlayUtility->nHandleType == LOCAL_PLAY_HANDLE)
	{
		CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
		return pLocalFilePlayer->CaptureLocalPlayPicture(dwChannel, pchPicFileName);
	}
    return TRUE;
}
//////////////////////////////////////////////////////////////
////////
//函数名: DVS_SetImageInfo
//作者:   czg
//日期:   2009-7-6
//功能:   设置显示画面的各种信息属性
//参数:   hPlayHandle           播放句柄，DVS_OpenRealStream，DVS_OpenRemotePlay，DVS_OpenLocalPlay的返回值
//        LPDVS_ImageInfo       表示画面的各种信息属性结构指针，参考该结构定义说明
//返回值; FALSE表示失败，TRUE表示成功
/////////
//////////////////////////////////////////////////////////////
DVS_API BOOL   __stdcall DVS_SetImageInfo(HANDLE hPlayHandle, DWORD dwChannel, LPDVS_ImageInfo lpImageInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}

	if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	if (pPlayUtility->nHandleType == REMOTE_PLAY_HANDLE)
	{
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->SetRemotePlayImageInfo(dwChannel - 1, lpImageInfo);
	}
	else if (pPlayUtility->nHandleType == LOCAL_PLAY_HANDLE)
	{
		ImageInfo struImageInfo;
		struImageInfo.nDateFormat = lpImageInfo->byDateFormat;
		struImageInfo.nDateCaption = lpImageInfo->byDateCaption;
		struImageInfo.nTimeCaption = lpImageInfo->byTimeCaption;
		struImageInfo.nChannelNameCaption = lpImageInfo->byChannelNameCaption;
		struImageInfo.nSysNameCaption = lpImageInfo->bySysNameCaption;
		
		CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
		pLocalFilePlayer->SetImageInfo(dwChannel - 1, &struImageInfo);
	}
	else
	{
		ImageInfo struImageInfo;
		struImageInfo.nDateFormat = lpImageInfo->byDateFormat;
		struImageInfo.nDateCaption = lpImageInfo->byDateCaption;
		struImageInfo.nTimeCaption = lpImageInfo->byTimeCaption;
		struImageInfo.nChannelNameCaption = lpImageInfo->byChannelNameCaption;
		struImageInfo.nSysNameCaption = lpImageInfo->bySysNameCaption;

		CDeviceStream *pReal = (CDeviceStream*)hPlayHandle;
		pReal->SetImageInfo(&struImageInfo);
	}

	return TRUE;
}

DVS_API BOOL __stdcall DVS_IsOnline(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
	// normal function body here
	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL) 
	{
		return pDvsUtility->pDvs->IsOnline();
	}

	return FALSE;
}

//获取用户权限
DVS_API BOOL   __stdcall DVS_GetUserNetRight(HANDLE hDvs, USER_NETRIGHT *pNetAccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		if (pDvsUtility == NULL)
		{
			dwLastErr = DVS_LOGIN_USER_EXIST;
			break;
		}

		if (pDvsUtility->pDvs != NULL && pDvsUtility->pDvs->GetUserRight(pNetAccess, sizeof(USER_NETRIGHT), NULL))
		{
			dwLastErr = DVS_NOERROR;
			return TRUE;
		}
	}while(0);

	return FALSE;
}

//获取DVR版本
DVS_API DWORD  __stdcall DVS_GetDvrVersion()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return dwLastErr;
	}

    return dvrVersion;
}

//设置通道名称
DVS_API DWORD  __stdcall DVS_SetChName(HANDLE hRealHandle, char *cChName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		if (NULL == hRealHandle)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
		CString strName;
		ANSIToUnicode(cChName, strName.GetBuffer((DVS_MAX_PTZ_NAME_LEN + 1) * 2));
		strName.ReleaseBuffer();
		pReal->SetChnnlName(strName);

		return DVS_NOERROR;
	}while(false);

	return dwLastErr;
}

//设置流畅度优先
DVS_API DWORD  __stdcall DVS_SetFluenceFirst(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return dwLastErr;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return dwLastErr;
	}

    DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		pDvsUtility->pDvs->m_bFluenceFirst = TRUE;
		return DVS_NOERROR;
	}

	return DVS_ERROR;
}

//获取网络资源连接数
DVS_API DWORD  __stdcall DVS_GetLinkNum(HANDLE hDvs, NET_LINK_INFO *pNetLinkInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return dwLastErr;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return dwLastErr;
	}

    DvsUtility *pDvsUtility = (DvsUtility *)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->GetLinkNum(pNetLinkInfo, sizeof(NET_LINK_INFO), NULL);
	}

	return DVS_ERROR;
}

//通道名称是否改变
DVS_API BOOL __stdcall DVS_ChnnlNameChanged(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}
	// normal function body here
	DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
	if (pDvsUtility->pDvs != NULL) 
	{
		return pDvsUtility->pDvs->ChnnlNameChanged();
	}

	return FALSE;
}

//请求DVR发送测试邮件
DVS_API DWORD  __stdcall DVS_RequestEmailTest(HANDLE hDvs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return dwLastErr;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return dwLastErr;
	}

	DvsUtility *pDvsUtility = (DvsUtility *)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->RequestEmailTest();
	}

	return DVS_ERROR;
}

DVS_API DWORD  __stdcall DVS_SetAlarmIn(HANDLE hDvs, DWORD dwChannel, BOOL bSet) ////2012.03.15万盛平交接添加
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return dwLastErr;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return dwLastErr;
	}

	DvsUtility *pDvsUtility = (DvsUtility *)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->SetAlarmIn(dwChannel, bSet);
	}

	return DVS_ERROR;
}

DVS_API DWORD  __stdcall DVS_SetAlarmOut(HANDLE hDvs, DWORD dwChannel, BOOL bSet) ////2012.03.15万盛平交接添加
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return dwLastErr;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return dwLastErr;
	}

	DvsUtility *pDvsUtility = (DvsUtility *)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->SetAlarmOut(dwChannel, bSet);
	}

	return DVS_ERROR;
}

DVS_API BOOL __stdcall DVS_DVRControl(HANDLE hDvs, DWORD dwPTZCommand, DWORD dwParam /* = NULL */)   //2012.04.06
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return FALSE;
	}
	if (NULL == hDvs)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return FALSE;
	}

	DvsUtility *pDvsUtility = (DvsUtility *)hDvs;
	if (pDvsUtility->pDvs != NULL)
	{
		return pDvsUtility->pDvs->DVRControl(dwPTZCommand,dwParam);
	}
	return TRUE;
}