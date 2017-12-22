#include "NetDvrApi.h"
#include "Global.h"
#include "DvsPro16.h"
#include "DeviceStream.h"
#include "AudioStream.h"
#include "LocalFilePlayer.h"

#define MAX_ALLOWVIEW	18
struct DvsUtility
{
	S8  domain[64];//÷≤Ω∑œ∆˙
	UNS32 userId;
	UNS32 accessId;
	Dvs *pDvs;
	S32 nDvsType;
	S32 streamType;
	HANDLE hReal[MAX_ALLOWVIEW];
};

typedef struct 
{
	Dvs	*pDvs;
	S8	sFileName[DVS_FILE_NAME_LEN];
}RemotePlayUtility;

typedef struct 
{
	CLocalFilePlayer *pLocalFilePlayer;
	S8	sFileName[DVS_FILE_NAME_LEN];
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
	UNS32 nHandleType;//”√¿¥ ∂±µ±«∞ « Ù”⁄ƒƒ÷÷¿‡–Õµƒ≤•∑≈æ‰±˙
	StruUtility	uniStruUtility;
}PlayUtility;


UNS32 dwLastErr = 0;
UNS32 g_dwConnectTime = 40000;//40s
fMessCallBack gpcbMessFunc = NULL;
UNS32 gcbMessUserData = 0;
bool  bInit = false;
UNS32 dvrVersion = 0x000000;
/*
uBOOL DVX_SetConfig(HANDLE hDvx,UNS32 cmd,LPVOID lpvoid,UNS32 optSize)
{
    DvsUtility *pDvsUtility = (DvsUtility*)hDvx;
    do {
        if (!bInit) {
            dwLastErr = DVS_NOINIT;
            break;
        }
        
        if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL) {
            dwLastErr = DVS_INVALID_HANDLE;
            break;
        }
        
        return pDvsUtility->pDvs->SetDvrConfig(cmd,lpvoid, optSize);
    } while (0);
    
}

UNS32 DVX_GetConfig(HANDLE hDvx,UNS32 cmd,LPVOID lpvoid,UNS32 bufSize)
{
    DvsUtility *pDvsUtility = (DvsUtility*)hDvx;
    do {
        if (!bInit) {
            dwLastErr = DVS_NOINIT;
            break;
        }
        
        if(pDvsUtility == NULL)
        {
            dwLastErr = DVS_LOGIN_USER_EXIST;
            break;
        }
        
        if(pDvsUtility->pDvs == NULL)
        {
            dwLastErr = DVS_ERROR;
            break;
        }
        
        return pDvsUtility->pDvs->GetDvrConfig(cmd, lpvoid, bufSize);
    } while (0);
}
*/
//SDKø‚µƒ≥ı ºªØ£¨≥…π¶∑µªÿDVS_NOERROR£¨ ß∞‹∑µªÿ∆‰À˚÷µ
 UNS32  DVS_Init()
{
	printf("DVS_Init() succeed!\n");
#ifdef WIN32
	if (!(InitSocket()))
	{
		dwLastErr = DVS_NOINIT;
		bInit = false;
	}
#endif
	bInit = true;
	dwLastErr = DVS_NOERROR;

	return dwLastErr;
}

 void	 DVS_Cleanup()
{
	printf("DVS_Cleanup() succeed!\n");
#ifdef WIN32
	WSACleanup();
#endif
	bInit = false;
	dwLastErr = DVS_NOERROR;

	return;
}

//∑µªÿ¥ÌŒÛ¬Î
 UNS32  DVS_GetLastError()
{
	return dwLastErr;
}

//…Ë÷√¡¨Ω”≥¨ ± ±º‰∫Õ¡¨Ω”¥Œ ˝£¨ƒø«∞µ⁄∂˛∏ˆ≤Œ ˝¡¨Ω”¥Œ ˝‘›≤ª π”√£¨ÃÓ0
 void	 DVS_SetConnectTime(UNS32 dwWaitTime, UNS32 dwTryTimes)
{
	printf("DVS_SetConnectTime() succeed!\n");
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return;
	}
	g_dwConnectTime = dwWaitTime;
}

//µ«¬º
 HANDLE  DVS_Login(S8 *pchDeviceIP, UNS16 wDevicePort, S8 *pchUserName, S8 *pchPwd)
{
	
    printf("DVS_Login() IP = %s,Port = %d,Name = %s,Pwd = %s\n",pchDeviceIP,wDevicePort,pchUserName,pchPwd);
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return 0;
	}

	S8 cmd[MAX_CMD_LEN + 1] = {0, };
	UNS32 cmdlen = 0;
	SOCKET ConnectSocket = INVALID_SOCKET;
    wDevicePort += 2;
	// normal function body here
	do
	{
		ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);//¥¥Ω®¡¨Ω”SOCKET
		if (ConnectSocket == INVALID_SOCKET)
		{
			dwLastErr = DVS_NETWORK_ERROR;
			break;
		}
		//…Ë÷√◊Ó¥ÛΩ” ’ª∫≥Â«¯
		S32 nRecvbufSize = SOCKET_RECV_BUFFER;
        setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDBUF,(S8*)&nRecvbufSize, sizeof(S32));
        setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVBUF, (S8*)&nRecvbufSize, sizeof(S32));
		//¡¨Ω”÷∏∂®…Ë±∏
		sockaddr_in clientService;
		memset(&clientService, 0, sizeof(clientService));
		clientService.sin_family      = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(pchDeviceIP);
		clientService.sin_port        = htons(wDevicePort);
		S32 nRet = connect(ConnectSocket, (sockaddr*)&clientService, sizeof(sockaddr_in));
		if (SOCKET_ERROR == nRet)
		{
			printf("Error code = %d\n", WSAGetLastError());
			dwLastErr = DVS_NETWORK_ERROR;
			break;
		}

		//…Ë÷√Œ™∑«◊Ë»˚ Ω
		UNSL32 iMode = 1;
		ioctlsocket(ConnectSocket, FIONBIO, &iMode);

		//∑¢ÀÕµ«¬Ω»œ÷§–Ë“™µƒ–≈œ¢
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

		//µ»¥˝÷±µΩ ’µΩµ«¬º»∑»œ
		fd_set r;
		FD_ZERO(&r);
		FD_SET(ConnectSocket, &r);
		struct timeval timeout;
		timeout.tv_sec = g_dwConnectTime/1000;
		timeout.tv_usec = g_dwConnectTime%1000;
		S32 iError = select(ConnectSocket + 1, &r, 0, 0, &timeout);
		if (SOCKET_ERROR == iError)
		{
			printf("Error code = %d\n", WSAGetLastError());
			dwLastErr = DVS_NETWORK_ERROR;
			break;
		}
		else if (0 == iError)
		{
			dwLastErr = DVS_LOGIN_TIMEOUT;
			break;
		}

		KTCMDHDR hdr = {0, };		
		S32 temp = recv(ConnectSocket, (S8*)&hdr, sizeof(hdr), 0);
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

		UNS32 nRecvMsgType = hdr.infoType;
		S32 nDvsType = 0;
		DevInitUtility accessUtility;
		memset((void*)&accessUtility, 0, sizeof(accessUtility));
		//“—¥Ô◊Ó¥Û”√ªß ˝
		if (CLIENT_IS_FULL == nRecvMsgType)
		{
			dwLastErr = DVS_ADD_USER_FULL;
			break;
		}
		//√‹¬Î¥ÌŒÛ
		if (USER_PASSWORD_ERROR == nRecvMsgType)
		{
			dwLastErr = DVS_LOGIN_PWD_ERROR;
			break;
		}
		//”√ªß√˚Œﬁ–ß
		else if (USER_NAME_INVALID == nRecvMsgType)
		{			
			dwLastErr = DVS_LOGIN_USER_INVALID;
			break;
		}
		//∏√”√ªß“—æ≠¥Ê‘⁄
		else if (USER_HAS_ACCESSED == nRecvMsgType)
		{			
			dwLastErr = DVS_LOGIN_USER_EXIST;
			break;
		}
		else if (ACCESS_ACCEPT == nRecvMsgType)
		{
			accessUtility.sCmdSocket = ConnectSocket;
			accessUtility.nCmdPort	 = wDevicePort;
			accessUtility.nIDCode	 = (hdr.idCode);
			accessUtility.nRemoteViewIP = inet_addr(pchDeviceIP);
			// µ ±º‡ ”–£—È π”√			
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
		if (NULL == pDvsUtility)
		{
			dwLastErr = DVS_ERROR;
			break;
		}

		pDvsUtility->streamType = 0;
		Dvs *pDvs = new DvsPro16(accessUtility);
 		//≥ı ºªØ…Ë±∏–≈œ¢
		if (pDvs->InitDvs())
		{
			pDvsUtility->pDvs		= pDvs;
			pDvsUtility->nDvsType	= nDvsType;			
			pDvsUtility->userId		= accessUtility.nIDCode;
			pDvsUtility->accessId	= accessUtility.userAccess.AccessCode;
			dvrVersion				= accessUtility.userAccess.SysVersion;//DVR∞Ê±æ

			memset(pDvsUtility->domain, 0, sizeof(pDvsUtility->domain));
			memcpy(pDvsUtility->domain, pchDeviceIP,DVS_MAX_DOMAINLEN);
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
	} while (0);

	if (ConnectSocket != INVALID_SOCKET)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	return 0;
}

 UNS32  DVS_Logout(HANDLE hDvs)
{
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

		// normal function body here
		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility->pDvs != NULL) 
		{
			pDvsUtility->pDvs->Logout();
			delete pDvsUtility->pDvs;
			pDvsUtility->pDvs = NULL;
			delete pDvsUtility;
			pDvsUtility = NULL;
			dwLastErr = DVS_NOERROR;
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);

	return dwLastErr;
}

//…Ë÷√…Ë±∏œ˚œ¢ªÿµ˜∫Ø ˝
 void	 DVS_SetDVRMessCallBack(fMessCallBack cbMessFunc, UNS32 dwUser)
{
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
	}
	gpcbMessFunc = cbMessFunc;
	gcbMessUserData = dwUser;
}

 UNS32  DVS_StartListen(HANDLE hDvs)
{
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
			pDvsUtility->pDvs->StartDVRMessCallBack(gpcbMessFunc, hDvs, gcbMessUserData);
			dwLastErr = DVS_NOERROR;
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);
	

	return dwLastErr;
}

 UNS32  DVS_StopListen(HANDLE hDvs)
{
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
			pDvsUtility->pDvs->StopDVRMessCallBack();
			dwLastErr = DVS_NOERROR;
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);

	return dwLastErr;
}

//ªÒ»°…Ë±∏–≈œ¢
 UNS32  DVS_GetDeviceInfo(HANDLE hDvs, LPDVS_DEV_INFO lpDeviceInfo)
{
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
		if (NULL == lpDeviceInfo)
		{
			dwLastErr = DVS_PARAM_ERROR;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility->pDvs != NULL)
		{
			if (pDvsUtility->pDvs->GetDeviceInfo(lpDeviceInfo))
				dwLastErr = DVS_NOERROR;
			else
				dwLastErr = DVS_NETWORK_ERROR;
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);

	return dwLastErr;
}

//∂‘Ω≤
 UNS32  DVS_SpeechStart(HANDLE hDvs)
{
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
		S8 cIP[DVS_MAX_DOMAINLEN + 1] = {0};
		strncpy(cIP, pDvsUtility->pDvs->GetDvrAddr(), DVS_MAX_DOMAINLEN);
		if (pDvsUtility->pDvs != NULL)
		{
			S8 netSetup[sizeof(DVS_NETSETUP_VER4)];
			memset(netSetup, 0, sizeof(DVS_NETSETUP_VER4));
			S32 getStructLen = sizeof(DVS_NETSETUP_VER1);
			if (dvrVersion >= ADD_EMAILINTERVAL)
				getStructLen = sizeof(DVS_NETSETUP_VER4);
			else if (dvrVersion >= ADD_CDMA_VERSION)
				getStructLen = sizeof(DVS_NETSETUP_VER4) - sizeof(UNS16) * 2;
			else if (dvrVersion >= MODIFY_DDNS_VERSION)
				getStructLen = sizeof(DVS_NETSETUP_VER3);
			else if (dvrVersion >= ADD_PPPOE_VERSION)
				getStructLen = sizeof(DVS_NETSETUP_VER2);
			S32 iRet = DVS_GetConfig(hDvs, SETUP_NET, netSetup, getStructLen);
			if (!iRet || iRet == DVS_VERSION_NOT_MATCH)
			{
				dwLastErr = iRet;
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
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);

	return dwLastErr;
}

 void	 DVS_SpeechStop(HANDLE hDvs)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
		}
		if (NULL == hDvs)
		{
			dwLastErr = DVS_INVALID_HANDLE;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility->pDvs != NULL)
		{
			pDvsUtility->pDvs->SpeechStop();
			dwLastErr = DVS_NOERROR;
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);
}

 UNS32  DVS_IsOnSpeech(HANDLE hDvs)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		if (pDvsUtility->pDvs->IsSpeech())
			dwLastErr = DVS_NOERROR;
		else
			dwLastErr = DVS_ERROR;
	} while (0);
	
	return (dwLastErr == DVS_NOERROR ? 1 : 0);
}

// µ ±¡˜‘§¿¿∫Ø ˝----0=÷˜¬Î¡˜, 1= ◊”¬Î¡˜, 2=“Ù∆µ¡˜
 UNS32  DVS_SetStreamType(HANDLE hDvs, S32 type)
{
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

		if (type != 0 && type != 1)
		{
			dwLastErr = DVS_PARAM_ERROR;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility->pDvs != NULL)
		{
			pDvsUtility->streamType = type;
			dwLastErr = DVS_NOERROR;
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);

	return dwLastErr;
}

#include "../PlayerDVR/shared/kt_stream_playback.h"
 HANDLE  DVS_OpenRealStream(HANDLE hDvs, S8 *pchHostIP, UNS16 wHostPort, UNS8 channel, HANDLE hRealHandle,void* instance)
{
    Stream_Playback *inst = (Stream_Playback*)instance;

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

		DvsUtility *pDvs = (DvsUtility *)hDvs;
		CDeviceStream *pReal = new CDeviceStream();
		if (pReal == NULL || pDvs->pDvs == NULL)
		{
			dwLastErr = DVS_ERROR;
			break;
		}

		UNS8 videoSignal = 0;
		UNS8 biAudio     = 0;
		S8 chnnlName[DVS_MAX_PTZ_NAME_LEN + 1] = {0};
		pDvs->pDvs->GetChnnlName(channel, chnnlName);
		pDvs->pDvs->GetBiAudio(channel, biAudio);
		pDvs->pDvs->GetVideoSignalType(videoSignal);
		pReal->SetSignalType(videoSignal);
		pReal->SetChnnlName(chnnlName);
		pReal->SetUserInfo(pDvs->userId, pDvs->accessId);

		if (!pReal->OpenRealStream(pDvs->domain, pDvs->pDvs->GetCmdPort() - 2, channel, biAudio))
		{
			dwLastErr = DVS_CUR_STREAMCHANNEL_OPENED;
			delete pReal;
			hRealHandle = NULL;
			break;
		}
        
        pReal->SetRealVideoCallBack(inst->cbRealVideoFunc, inst);
        
		hRealHandle = (HANDLE)pReal;
		dwLastErr = DVS_NOERROR;
	} while (0);

	return hRealHandle;
}

 UNS32  DVS_CloseRealStream(HANDLE hRealHandle)
{
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
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_PlayRealStream(HANDLE hRealHandle)
{
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
		if (pReal->PlayRealStream())
        {
       		dwLastErr = DVS_NOERROR;
        }
		else
			dwLastErr = DVS_ERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_StopRealStream(HANDLE hRealHandle)
{
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
        pReal->CloseRealStream();
 		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_OpenAudioStream(HANDLE hDvs, S8 *pchHostIP, UNS16 wHostPort, HANDLE &hRealHandle)
{
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
			dwLastErr = DVS_CUR_STREAMCHANNEL_OPENED;
			break;
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
			dwLastErr = DVS_ERROR;
			delete pRealAudio;
			hRealHandle = NULL;
			break;
		}

		hRealHandle = (HANDLE)pRealAudio;
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_PlayRealAudio(HANDLE hDvs, HANDLE hRealHandle, UNS8 channel)
{
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
		UNS8 biAudio = 0;
		if(pDvs->pDvs != NULL)
		{
			pDvs->pDvs->GetBiAudio(channel, biAudio);
		}

		//¥À¥¶»‘–ËÕÍ…∆
		CAudioStream *pReal = (CAudioStream*)hRealHandle;
		if (pReal->PlayRealAudio(biAudio))
			dwLastErr = DVS_NOERROR;
		else
			dwLastErr = DVS_PARAM_ERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_StopRealAudio(HANDLE hRealHandle)
{
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
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_GetRealBiAudio(HANDLE hRealHandle, UNS8 &biAudio)
{
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
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_CloseAudioStream(HANDLE hRealHandle)
{
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
	} while (0);

	return dwLastErr;
}

 UNS8	 DVS_GetRealChnnl(HANDLE hRealHandle)
{
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
	} while (0);

	return 0;
}

 void	 DVS_SetRealVideoCallBack(process_cb_ex cbRealVideoFunc, HANDLE hRealHandle, void* dwUser)
{
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return;
	}

	CDeviceStream *pReal = (CDeviceStream*)hRealHandle;
	pReal->SetRealVideoCallBack(cbRealVideoFunc, dwUser);
	dwLastErr = DVS_NOERROR;
}

 void	 DVS_SetRealAudioCallBack(fDataCallBack cbRealAudioFunc, HANDLE hRealHandle, UNS32 dwUser)
{
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return;
	}

	CAudioStream *pRealAudio = (CAudioStream*)hRealHandle;
	pRealAudio->SetRealAudioCallBack(cbRealAudioFunc, dwUser);
	dwLastErr = DVS_NOERROR;
}

//ø™ ºªÚÕ£÷π±£¥Ê µ ±¡˜ ˝æ›Œƒº˛
 UNS32  DVS_SaveRealDataStart(HANDLE hRealHandle, S8 *sFileName)
{
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
		S32 nRet = pReal->SaveRealData(sFileName);
		if (nRet == 0)
		{
			dwLastErr = DVS_NOERROR;
			break;
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
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_SaveRealDataStop(HANDLE hRealHandle)
{
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
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_IsRecording(HANDLE hRealHandle)
{
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
		if (pReal->IsRecording())
			dwLastErr = DVS_NOERROR;
		else
			dwLastErr = DVS_ERROR;
	} while (0);

	return (dwLastErr == DVS_NOERROR ? 1 : 0);
}

//‘∆Ã®øÿ÷∆∫Ø ˝
 UNS32  DVS_PTZControl(HANDLE hDvs, UNS8 channel, UNS32 dwPTZCommand, UNS32 dwParam)
{
	do
	{
        dwParam = 0;
        
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
			S32 nRet = pDvsUtility->pDvs->PTZControl(channel, dwPTZCommand, dwParam);
			if (nRet == 0)
			{
				dwLastErr = DVS_NOERROR;
				break;
			}
			else if (nRet == 1)
			{
				dwLastErr = DVS_CUR_STREAMCHANNEL_NOTEXIST;
				break;
			}
			else if (nRet == 2)
			{
				dwLastErr = DVS_PARAM_ERROR;
				break;
			}
			else
			{
				dwLastErr = DVS_ERROR;
				break;
			}
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);

	return dwLastErr;
}

//‘∂≥ÃŒƒº˛ªÿ∑≈
 UNS32  DVS_QueryRecordFile(HANDLE hDvs, DVS_TIME dtStartTime, DVS_TIME dtEndTime, S32 type, LPDVS_RECORDFILE_LIST lpRecordfileList)
{
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
		if (NULL == lpRecordfileList)
		{
			dwLastErr = DVS_PARAM_ERROR;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility->pDvs != NULL)
		{
			S32 nRet = pDvsUtility->pDvs->QueryRecordFile(dtStartTime, dtEndTime, type, lpRecordfileList);
			if (nRet == 0)
			{
				dwLastErr = DVS_NOERROR;
				break;
			}
			else if (nRet == 2)
			{
				dwLastErr = DVS_NETWORK_ERROR;
				break;
			}
			else
			{
				dwLastErr = DVS_ERROR;
				break;
			}
		}else
			dwLastErr = DVS_INVALID_HANDLE;
	} while (0);
	
	return dwLastErr;
}

 UNS32  DVS_OpenRemotePlay(HANDLE hDvs, S8 *pchRemoteFileName, UNS8 playCounts, HANDLE* hPlayHandle)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		
		S32 nRet = pDvsUtility->pDvs->OpenRemotePlay(pchRemoteFileName, playCounts);
		if (nRet == 0)
		{
			PlayUtility *pPlayUtility = new PlayUtility;
			pPlayUtility->nHandleType = REMOTE_PLAY_HANDLE;
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs = pDvsUtility->pDvs;
			memcpy(pPlayUtility->uniStruUtility.struRemotePlayUtility.sFileName, pchRemoteFileName, DVS_FILE_NAME_LEN);
			*hPlayHandle = (HANDLE)pPlayUtility;
			dwLastErr = DVS_NOERROR;
			break;
		}
		else if (nRet == 2)
		{
			dwLastErr = DVS_NETWORK_ERROR;
			break;
		}
		else if (nRet == 3)
		{
			dwLastErr = DVS_GET_FILEINDEX_ERROR;
			break;
		}
		else
		{
			dwLastErr = DVS_NOERROR;
			break;
		}
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_CloseRemotePlay(HANDLE hPlayHandle)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		if (NULL == pPlayUtility || 
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->CloseRemotePlay();
		delete pPlayUtility;
		pPlayUtility = NULL;
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_SetRemoteChannelMask(HANDLE hPlayHandle, UNS32 dwBitMaskChn)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		
		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		if (NULL == pPlayUtility || 
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->SetRemoteChannelMask(dwBitMaskChn);
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_RemotePlayControl(HANDLE hPlayHandle, UNS8 dwControlCode)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		if (NULL == pPlayUtility || 
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->RemotePlayControl(dwControlCode);
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_GetRemotePlayTime(HANDLE hPlayHandle, LPDVS_TIME lpCurPlayTime)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		if (NULL == pPlayUtility || pPlayUtility->nHandleType != REMOTE_PLAY_HANDLE || 
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		bool bRet = pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->GetRemotePlayTime(lpCurPlayTime);
		if (!bRet)
			dwLastErr = DVS_ERROR;
		else
			dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_StartRemotePlayAudio(HANDLE hPlayHandle, UNS8 channel)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		if (NULL == pPlayUtility || 
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->StartRemotePlayAudio(channel);
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_StopRemotePlayAudio(HANDLE hPlayHandle, UNS8 channel)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		if (NULL == pPlayUtility || 
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->StopRemotePlayAudio();
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

//type£∫1∞¥’’ ±º‰∂®Œª£¨2∞¥’’◊÷Ω⁄∂®Œª£¨‘› ±÷ª÷ß≥÷1
 UNS32  DVS_SeekRemotePlay(HANDLE hPlayHandle, UNS8 type, UNS64 dwArg)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}
		if (type != 1)
		{
			dwLastErr = DVS_PARAM_ERROR;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		if (NULL == pPlayUtility || 
			pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->SeekRemotePlayByTime((UNS64)dwArg);
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 void  DVS_SetRemoteDataCallBack(process_cb_ex cbRemoteDataFunc, HANDLE hPlayHandle, void* dwUser)
{ 
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return;
	}

	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	if (NULL == pPlayUtility || 
		pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs == NULL)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return;
	}
	pPlayUtility->uniStruUtility.struRemotePlayUtility.pDvs->SetRemoteDataCallBack(cbRemoteDataFunc, dwUser);
	dwLastErr = DVS_NOERROR;
}

//‘∂≥ÃŒƒº˛œ¬‘ÿ
 UNS32  DVS_DumpOpen(HANDLE hDvs, S8 *pDumpFile, S8 *pSaveAs, UNS32 startTime, UNS32 endTime, UNS32 chnnlMask)
{
	do 
	{
        chnnlMask = 0xFFFF;
        
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (NULL == hDvs || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		return pDvsUtility->pDvs->RemoteDumpOpen(pDumpFile, pSaveAs, startTime, endTime, chnnlMask);
	} while (0);

	return dwLastErr;
}

 void	 DVS_DumpClose(HANDLE hDvs)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (NULL == hDvs || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}
		pDvsUtility->pDvs->RemoteDumpClose();
	} while (0);
}

 S32  DVS_DumpProgress(HANDLE hDvs)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (NULL == hDvs || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		return pDvsUtility->pDvs->RemoteDumpProgress();
	} while (0);

	return dwLastErr;
}

//±æµÿŒƒº˛ªÿ∑≈
UNS32  DVS_OpenLocalPlay(S8 *pchLocalFileName, UNS8 playCounts, HANDLE* hPlayHandle)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		PlayUtility *pPlayUtility = new PlayUtility;
		if (pPlayUtility == NULL)
		{
			dwLastErr = DVS_ERROR;
			break;
		}

		pPlayUtility->nHandleType = LOCAL_PLAY_HANDLE;
		pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer = new CLocalFilePlayer;
		if (pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer == NULL)
		{
			dwLastErr = DVS_ERROR;
			break;
		}

		bool bRet = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer->OpenLocalPlay(pchLocalFileName, playCounts);
		if (bRet)
		{
			memcpy(pPlayUtility->uniStruUtility.struLocalPlayUtility.sFileName, pchLocalFileName, DVS_FILE_NAME_LEN);
			*hPlayHandle = (HANDLE)pPlayUtility;
			dwLastErr = DVS_NOERROR;
			break;
		}
		else
		{
			delete pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
			pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer = NULL;
			delete pPlayUtility;
			dwLastErr = DVS_ERROR;
			break;
		}
	} while (0);
	
	return dwLastErr;
}

 UNS32  DVS_GetDuration(HANDLE hPlay, UNS32 &startTime, UNS32 &endTime)
{
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
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		if (pLocalFilePlayer->GetDuration(startTime, endTime))
		{
			dwLastErr = DVS_NOERROR;
			break;
		}else
			dwLastErr = DVS_ERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_GetLocalPlayCurTime(HANDLE hPlayHandle, UNS32 &curTime)
{
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
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		curTime = pLocalFilePlayer->GetCurPlayTime();
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_CloseLocalPlay(HANDLE hPlayHandle)
{
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
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		pLocalFilePlayer->ClosePlayer();
		delete pLocalFilePlayer;
		pLocalFilePlayer = NULL;
		delete pPlayUtility;
		pPlayUtility = NULL;
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_SetLocalChannelMask(HANDLE hPlayHandle, UNS32 dwBitMaskChn)
{
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
		if (pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer->SetLocalChannelMask(dwBitMaskChn);
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_LocalPlayControl(HANDLE hPlayHandle, UNS8 dwControlCode)
{
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
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		switch(dwControlCode)
		{
		case DVS_PLAYCTRL_BACKWARD:
			{
				S32 rate = pLocalFilePlayer->m_RateScale;
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
				pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
			}
			break;
		case DVS_PLAYCTRL_BACKPLAY:
			if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
			{
				pLocalFilePlayer->SetPlayRate(1);
				pLocalFilePlayer->Backward();
				pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
			}
			break;
		case DVS_PLAYCTRL_PLAY:
			if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
			{
				pLocalFilePlayer->SetPlayRate(1);
				pLocalFilePlayer->SetPlayIFrame(0);
				pLocalFilePlayer->PlayAllVideo();
				pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
			}
			break;
		case DVS_PLAYCTRL_FORWARD:
			{
				S32 rate = pLocalFilePlayer->m_RateScale;
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
				pLocalFilePlayer->SetPlayIFrame(true);
				pLocalFilePlayer->PlayAllVideo();
				pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
			}
			break;
		case DVS_PLAYCTRL_BACKSHIFT:
			if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
			{
				pLocalFilePlayer->SetPlayRate(1);
			}
			pLocalFilePlayer->StepBack();
			pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
			break;
		case DVS_PLAYCTRL_SHIFT:
			pLocalFilePlayer->StepNext();
			pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
			break;
		case DVS_PLAYCTRL_PAUSE:
			if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
			{
				pLocalFilePlayer->Pause();
				pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
			}
			break;
		case DVS_PLAYCTRL_STOP:
			if (dwControlCode != pLocalFilePlayer->m_nOldLocalPlayMode)
			{
				pLocalFilePlayer->Stop();
				pLocalFilePlayer->Pause();
				pLocalFilePlayer->m_nOldLocalPlayMode = dwControlCode;
			}
			break;
		default:
			dwLastErr = DVS_PARAM_ERROR;
			return dwLastErr;
		}

		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_StartLocalPlayAudio(HANDLE hPlayHandle, UNS8 channel)
{
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
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		pLocalFilePlayer->PlayAudio(channel - 1);
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_StopLocalPlayAudio(HANDLE hPlayHandle, UNS8 channel)
{
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
		if (NULL == pLocalFilePlayer)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		pLocalFilePlayer->StopAudio();
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_SeekLocalPlay(HANDLE hPlayHandle, UNS8 type, UNS64 dwArg)
{
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
		if (type != 1)
		{
			dwLastErr = DVS_PARAM_ERROR;
			break;
		}

		PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
		CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
		if (pLocalFilePlayer == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		pLocalFilePlayer->Seek((UNS64)dwArg);
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

void  DVS_SetLocalDataCallBack(process_cb_ex cbLocalDataFunc, HANDLE hPlayHandle, void* dwUser)
{
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return;
	}
	if (NULL == hPlayHandle)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return;
	}
    
	PlayUtility *pPlayUtility = (PlayUtility*)hPlayHandle;
	CLocalFilePlayer *pLocalFilePlayer = pPlayUtility->uniStruUtility.struLocalPlayUtility.pLocalFilePlayer;
	if (pLocalFilePlayer == NULL)
	{
		dwLastErr = DVS_INVALID_HANDLE;
		return;
	}
    
	pLocalFilePlayer->SetLocalDataCallBack(cbLocalDataFunc, dwUser);
	dwLastErr = DVS_NOERROR;
}

//≈–∂œŒƒº˛ «∑Ò≤•∑≈Ω· ¯
 UNS32  DVS_PlayIsEnd(HANDLE hPlayHandle)
{
	bool bEnd = true;
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
	} while (0);

	return bEnd;
	return 0;
}

//DVS‘⁄œﬂ◊¥Ã¨
 UNS32  DVS_IsOnline(HANDLE hDvs)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		if (pDvsUtility->pDvs->IsOnline())
			dwLastErr = DVS_NOERROR;
		else
			dwLastErr = DVS_ERROR;
	} while (0);

	return (dwLastErr == DVS_NOERROR ? 1 : 0);
}

//ªÒ»°”√ªß»®œﬁ
 UNS32  DVS_GetUserNetRight(HANDLE hDvs, USER_NETRIGHT *pNetAccess)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		pDvsUtility->pDvs->GetUserRight(pNetAccess, sizeof(USER_NETRIGHT));
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

//ªÒ»°DVR∞Ê±æ
 UNS32  DVS_GetDvrVersion()
{
	printf("DVS_GetDvrVersion() succeed!\n");
	if (!bInit)
	{
		dwLastErr = DVS_NOINIT;
		return dwLastErr;
	}

	return dvrVersion;
}

//…Ë÷√Õ®µ¿√˚≥∆
 UNS32  DVS_SetChName(HANDLE hRealHandle, S8 *cChName)
{
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
		pReal->SetChnnlName(cChName);
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

//ªÒ»°Õ¯¬Á◊ ‘¥¡¨Ω” ˝
 UNS32  DVS_GetLinkNum(HANDLE hDvs, NET_LINK_INFO *pNetLinkInfo)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		pDvsUtility->pDvs->GetLinkNum(pNetLinkInfo, sizeof(NET_LINK_INFO));
		dwLastErr = DVS_NOERROR;
	} while (0);

	return dwLastErr;
}

//Õ®µ¿√˚≥∆ «∑Ò∏ƒ±‰
 UNS32  DVS_ChnnlNameChanged(HANDLE hDvs)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (NULL == pDvsUtility || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		if (pDvsUtility->pDvs->ChnnlNameChanged())
			dwLastErr = DVS_NOERROR;
		else
			dwLastErr = DVS_ERROR;
	} while (0);

	return (dwLastErr == DVS_NOERROR ? 1 : 0);
}

//«Î«ÛDVR∑¢ÀÕ≤‚ ‘” º˛
 UNS32  DVS_RequestEmailTest(HANDLE hDvs)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility *)hDvs;
		if (NULL == pDvsUtility || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		return pDvsUtility->pDvs->RequestEmailTest();
	} while (0);

	return dwLastErr;
}

//Õ¯¬Á≤Œ ˝…Ë÷√”ÎªÒ»°
 UNS32  DVS_SetConfig(HANDLE hDvs, UNS32 cmd, LPVOID lpvoid, UNS32 optSize)
{
	do 
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		return pDvsUtility->pDvs->SetDvrConfig(cmd, lpvoid, optSize);
	} while (0);

	return dwLastErr;
}

 UNS32  DVS_GetConfig(HANDLE hDvs, UNS32 cmd, LPVOID lpvoid, UNS32 bufSize)
{
	do
	{
		if (!bInit)
		{
			dwLastErr = DVS_NOINIT;
			break;
		}

		DvsUtility *pDvsUtility = (DvsUtility*)hDvs;
		if (pDvsUtility == NULL || pDvsUtility->pDvs == NULL)
		{
			dwLastErr = DVS_INVALID_HANDLE;
			break;
		}

		return pDvsUtility->pDvs->GetDvrConfig(cmd, lpvoid, bufSize);
	} while (0);

	return dwLastErr;
}

UNS32 DVS_GetChnNumber(HANDLE hDvs)
{
	sleep(1);
	DVS_DEV_INFO DeviceInfo={0};
	DVS_GetDeviceInfo(hDvs,&DeviceInfo);
	return DeviceInfo.VideoInNum;
}