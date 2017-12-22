#include "RemoteConfig.h"

CRemoteConfig::CRemoteConfig()
{
	m_pDVRSocket = NULL;
}

CRemoteConfig::~CRemoteConfig()
{
}

void CRemoteConfig::SetDVRSocket(DVRSocket *pDVRSocket)
{
	m_pDVRSocket = pDVRSocket;
}

bool CRemoteConfig::GetRecordfileDateInfo(NET_SYS_DB_DATE_INFO *pRecordfileDateInfo)
{
    if (pRecordfileDateInfo == NULL)
	{
		return false;
	}

	//发送获取录像日期消息
	if (m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		S32 iRet = m_pDVRSocket->RequestOperation(S_WANT_DB_DATE, 0);
		if (iRet == 0)
		{
			return false;
		}
	}

    //等待接收参数直至超时
	S32 nWaitTime = 0;
	while(m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		if (m_pDVRSocket->GetLastRecvMsgType() == NM_SYS_DB_FILE_DATE)
		{
			memcpy(pRecordfileDateInfo, m_pDVRSocket->ReceiveBuffer, sizeof(NET_SYS_DB_DATE_INFO));
			return true;
		}
		if (nWaitTime >= m_pDVRSocket->GetOverTime() * 3)
		{
			break;
		}
		nWaitTime++;
		Sleep(1);
	}
	
	return false;
}

bool CRemoteConfig::GetDayRecordfileInfo(NET_SYS_DB_DAY_FILE *pDayRecordfileInfo, UNS32 date)
{
	if (pDayRecordfileInfo == NULL)
	{
		return false;
	}
	
	//发送获取当日录像文件信息
	if (m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		memcpy(m_pDVRSocket->SendBuffer, &date, sizeof(date));
		S32 iRet = m_pDVRSocket->RequestOperation(S_WANT_DB_DAY_FILE, &date);
		if (iRet == 0)
		{
			return false;
		}
	}

	//等待接收参数直至超时
	S32 nWaitTime = 0;
	while(m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		if (m_pDVRSocket->GetLastRecvMsgType() == NM_SYS_DB_DAY_FILE)
		{
			memcpy(pDayRecordfileInfo, m_pDVRSocket->ReceiveBuffer, sizeof(NET_SYS_DB_DATE_INFO));
			return true;
		}
		if (nWaitTime >= m_pDVRSocket->GetOverTime())
		{
			break;
		}
		nWaitTime++;
		Sleep(1);
	}
	
	return false;
}

UNS32 CRemoteConfig::GetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size)
{
	bool bFailed = false;
	S32 iVersion = 0;//dvr net config struct version
	S8 netSysConfig[sizeof(NET_SYS_CONFIG_CHANGE)];//dvr config union
	UNS32 theDVRVersion = DVS_GetDvrVersion();//dvr soft version
	if (theDVRVersion >= ADD_CDMA_VERSION)
	{
		iVersion = 4;
	}
	else if (theDVRVersion >= MODIFY_DDNS_VERSION)
	{
		iVersion = 3;
	}
	else if (theDVRVersion >= ADD_PPPOE_VERSION)
	{
		iVersion = 2;
	}
	else
	{
		iVersion = 1;
	}
	bFailed = CheckStructSize(cmdId, size, iVersion);

	if (bFailed == true)
	{
		return DVS_ERROR;
	}

	//发送获取系统配置信息请求
	if (m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		if (m_pDVRSocket->RequestOperation(S_WANT_SYS_CONFIG, &cmdId, INVALID_SOCKET, iVersion) == FAILURE)
		{
			return DVS_ERROR;
		}
	}

	S32 getStructLen = sizeof(DVS_SETUP) + size;//length of net config struct
	if (iVersion >= 2 && iVersion <= 4)
		getStructLen += sizeof(UNS32);

	//等待接收参数直至超时
	S32 nWaitTime = 0;
	while(m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		if (m_pDVRSocket->GetLastRecvMsgType() == NM_SYS_CONFIG)
		{
			memcpy(netSysConfig, m_pDVRSocket->ReceiveBuffer, getStructLen);
			bFailed = false;
			break;
		}
		if (nWaitTime >= m_pDVRSocket->GetOverTime())
		{
			bFailed = true;
			break;
		}
		nWaitTime++;
		Sleep(1);
	}

	if (!bFailed)
	{
		if (iVersion >= 2 && iVersion <= 4)
		{
			UNS32 iStructLen = 0;
			memcpy(&iStructLen, netSysConfig + sizeof(DVS_SETUP), sizeof(UNS32));
			if (iStructLen != size)
			{
				return DVS_VERSION_NOT_MATCH;
			}
		}
		memcpy(pCfg, netSysConfig + getStructLen - size, size);
		return DVS_NOERROR;
	}

	return DVS_ERROR;
}

UNS32 CRemoteConfig::SetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size)
{
	bool bFailed = false;
	UNS32 iVersion = 0;//dvr net config struct version
	S8 netSysConfig[sizeof(NET_SYS_CONFIG_CHANGE)];//dvr config union
	UNS32 theDVRVersion = DVS_GetDvrVersion();//dvr soft version
	if (theDVRVersion >= ADD_CDMA_VERSION)
	{
		iVersion = 4;
	}
	else if (theDVRVersion >= MODIFY_DDNS_VERSION)
	{
		iVersion = 3;
	}
	else if (theDVRVersion >= ADD_PPPOE_VERSION)
	{
		iVersion = 2;
	}
	else
	{
		iVersion = 1;
	}
	bFailed = CheckStructSize(cmdId, size, iVersion);

	if (!bFailed)
	{
		S32 setStructLen = sizeof(DVS_SETUP) + size;
		memset(netSysConfig, 0, setStructLen);
		memcpy(netSysConfig, &cmdId, sizeof(DVS_SETUP));
		if (iVersion >= 2 && iVersion <= 4)
		{
			memcpy(netSysConfig + sizeof(DVS_SETUP), &size, sizeof(UNS32));
			memcpy(netSysConfig + sizeof(DVS_SETUP) + sizeof(UNS32), pCfg, size);
			setStructLen += sizeof(UNS32);
		}else if (iVersion == 1)
		{
			memcpy(netSysConfig + sizeof(DVS_SETUP), pCfg, size);
		}
		if (m_pDVRSocket && m_pDVRSocket->m_bOnLine && 
			(m_pDVRSocket->SetConfig(netSysConfig, setStructLen, iVersion) == SUCCESS))
		{
			if (cmdId == SETUP_CAMERA)
			{
				if (m_pDVRSocket && m_pDVRSocket->m_bOnLine)
				{
					m_pDVRSocket->RequestOperation(S_WANT_VIEW_INFO, 0);
				}
			}
			return DVS_NOERROR;
		}
	}

	return DVS_ERROR;
}

//获取用户权限
void CRemoteConfig::GetUserRight(LPVOID pCfg, S32 size)
{
	bool bFailed = true;
	USER_NETRIGHT userNetAccess;
	memset(&userNetAccess, 0, sizeof(userNetAccess));

	//发送获取系统配置信息请求
	if (m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		S32 iRet = m_pDVRSocket->RequestOperation(S_WANT_GET_USER_RIGHT, 0);
		if (iRet == 0)
		{
			return;
		}
	}

	//等待接收参数直至超时
	S32 nWaitTime = 0;
	while(m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		if(m_pDVRSocket->GetLastRecvMsgType() == NM_GET_USER_RIGHT)
		{
			memcpy(&userNetAccess, m_pDVRSocket->ReceiveBuffer, sizeof(userNetAccess));
			bFailed = false;
			break;
		}
		if (nWaitTime >= m_pDVRSocket->GetOverTime())
		{
			break;
		}
		nWaitTime++;
		Sleep(1);
	}

	if (bFailed == true)
	{
		return;
	}

	if (size != sizeof(USER_NETRIGHT))
	{
		bFailed = true;
	}
	memcpy(pCfg, &userNetAccess, size);

	return;
}

//获取网络资源连接数
void CRemoteConfig::GetLinkNum(LPVOID pCfg, S32 size)
{
	UNS32 nRtn = DVS_ERROR;
	NET_LINK_INFO netLingInfo;
	memset(&netLingInfo, 0, sizeof(netLingInfo));

	//发送获取系统配置信息请求
	if (m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		if (m_pDVRSocket->RequestOperation(S_WANT_GET_LINK_NUM, 0) == 0)
		{
			nRtn = DVS_NETWORK_ERROR;
			return;
		}
	}

	//等待接收参数直至超时
	S32 nWaitTime = 0;
	while(m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		if(m_pDVRSocket->GetLastRecvMsgType() == NM_GET_LINK_NUM)
		{
			memcpy(&netLingInfo, m_pDVRSocket->ReceiveBuffer, sizeof(netLingInfo));
			nRtn = DVS_NOERROR;
			break;
		}
		if (nWaitTime >= m_pDVRSocket->GetOverTime())
		{
			break;
		}
		nWaitTime++;
		Sleep(1);
	}

	if (nRtn != DVS_NOERROR)
	{
		return;
	}

	if (size != sizeof(NET_LINK_INFO))
	{
		nRtn = DVS_NETWORK_ERROR;
	}
	memcpy(pCfg, &netLingInfo, size);

	return;
}

//请求DVR发送测试邮件
UNS32 CRemoteConfig::RequestEmailTest()
{
	UNS32 nRtn = DVS_ERROR;
	//发送获取系统配置信息请求
	if (m_pDVRSocket && m_pDVRSocket->m_bOnLine)
	{
		if (m_pDVRSocket->RequestOperation(S_WANT_REQUEST_EMAIL_TESTING, 0) == 0)
		{
			nRtn = DVS_NETWORK_ERROR;
		}else
		{
			nRtn = DVS_NOERROR;
		}
	}

	return nRtn;
}

bool CRemoteConfig::CheckStructSize(UNS32 cmdId, S32 size, UNS32 iVersion)
{
	bool bFailed = false;
	UNS32 theDVRVersion = DVS_GetDvrVersion();
	switch(cmdId)
	{
	case SETUP_RECORD:
		if (size != sizeof(DVS_RECSETUP))
		{
			bFailed = true;
		}
		break;
	case SETUP_CAMERA:
		if (size != sizeof(DVS_CAMERASETUP))
		{
			bFailed = true;
		}
		break;
	case SETUP_EVENT:
		if (size != sizeof(DVS_EVENTSETUP))
		{
			bFailed = true;
		}
		break;
	case SETUP_DISPLAY:
		if (size != sizeof(DVS_VGASETUP))
		{
			bFailed = true;
		}
		break;
	case SETUP_NET:
		switch(iVersion)
		{
		case 1:
			if (size != sizeof(DVS_NETSETUP_VER1))
			{
				bFailed = true;
				break;
			}
			break;
		case 2:
			if (size != sizeof(DVS_NETSETUP_VER2))
			{
				bFailed = true;
				break;
			}
			break;
		case 3:
			if (size != sizeof(DVS_NETSETUP_VER3))
			{
				bFailed = true;
				break;
			}
			break;
		case 4:
			if (theDVRVersion >= ADD_EMAILINTERVAL)
			{
				if (size != sizeof(DVS_NETSETUP_VER4))
				{
					bFailed = true;
				}
			}else
			{
				if (size != sizeof(DVS_NETSETUP_VER4) - sizeof(UNS16) * 2)
				{
					bFailed = true;
				}
			}
			break;
		}
		break;
	case SETUP_SYS:
		if (size != sizeof(DVS_SYSSETUP))
		{
			bFailed = true;
		}
		break;
	case SETUP_USER:
		if (size != sizeof(DVS_USERSETUP))
		{
			bFailed = true;
		}
		break;
	case SETUP_PTZ:
		switch(iVersion)
		{
		case 1:
			if (size != sizeof(UNS32))
			{
				bFailed = true;
			}
			break;
		case 2:
		case 3:
			if (size != sizeof(DVS_PTZSETUP))
			{
				bFailed = true;
			}
			break;
		case 4:
			if (size != sizeof(DVS_PTZCRUISE))
			{
				bFailed = true;
			}
			break;
		}
		break;
	case SETUP_SYSTIME:
		if (size != sizeof(UNS32))
		{
			bFailed = true;
		}
		break;
	}

	return bFailed;
}
