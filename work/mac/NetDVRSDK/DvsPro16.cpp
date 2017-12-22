#include "RemoteFilePlayer.h"
#include "DVRSocket.h"
#include "DvsPro16.h"
#include "macro.h"
#include "../PlayerDVR/kt_util.h"

DWORD DumpFileThread(LPVOID lpvoid);
DWORD DumpFileThread(LPVOID lpvoid)
{
	DvsPro16 *pDvs = (DvsPro16*)lpvoid;
	if (pDvs != NULL)
	{
		pDvs->DumpTask();
	}
	
	return 0;
}

DvsPro16::DvsPro16()
{
	m_pWriter		= NULL;
	m_bExitDump		= false;
	m_pSpeech		= NULL;
	m_pDVRSocket	= NULL;
}

DvsPro16::DvsPro16(DevInitUtility devInitUtility)
{
	//解析设备类型   
	m_pDVRSocket = new DVRSocket;
	if (m_pDVRSocket)
	{
		m_pDVRSocket->m_CmdSocket  = devInitUtility.sCmdSocket;
		m_pDVRSocket->m_AccessCode = devInitUtility.userAccess;
		m_pDVRSocket->m_nIDCode    = devInitUtility.nIDCode;
		m_pDVRSocket->CreateNetThread();
		m_pDVRSocket->m_IPAddr     = devInitUtility.nRemoteViewIP;
		m_pDVRSocket->m_wCmdPort   = devInitUtility.nCmdPort;

		m_RemoteConfig.SetDVRSocket(m_pDVRSocket);
		m_RemotePlayer.m_RemotePSFile.SetDVRSocket(m_pDVRSocket);
		m_pDVRSocket->m_pRemotePlayer = (void*)&m_RemotePlayer;
	}
	m_nOldRemotePlayMode = -1;

	m_pWriter		= NULL;
	m_bExitDump		= false;
	m_pSpeech		= NULL;
}

DvsPro16::~DvsPro16()
{
	SpeechStop();
	RemoteDumpClose();
	if (m_pDVRSocket)
	{
		m_RemoteConfig.SetDVRSocket(0);
		delete m_pDVRSocket;
		m_pDVRSocket = NULL;
	}
	
	if (m_pWriter != NULL)
	{
		delete m_pWriter;
		m_pWriter = NULL;
	}
}

void DvsPro16::Logout()
{
	if (this->m_pDVRSocket)
	{
		this->m_pDVRSocket->DeleteCMDSocket();
	}
	this->m_RemotePlayer.ClosePlayer();
}

void DvsPro16::StartDVRMessCallBack(fMessCallBack cbMessFunc, HANDLE hDvs, UNS32 dwUser)
{
	if (this->m_pDVRSocket)
	{
		this->m_pDVRSocket->StartDVRMessCallBack(cbMessFunc, hDvs, dwUser);
	}
}

void DvsPro16::StopDVRMessCallBack()
{
	if (this->m_pDVRSocket)
	{
		this->m_pDVRSocket->StopDVRMessCallBack();
	}
}

//////////////////////////////////////////////////////////////
////////
//函数名: PTZControl
//作者:   czg
//日期:   2009-4-27
//功能:   云台控制
//参数:   channel     通道号
//        dwPTZCommand  云台控制指令
//        dwParam       调用预置点或者存储预置点的组号,其它指令此参数不用
//返回值; 0表示成功
//        1当前设备不存在该通道
//        2参数非法
/////////
//////////////////////////////////////////////////////////////
UNS32 DvsPro16::PTZControl(UNS8 channel, UNS32 dwPTZCommand, UNS32 dwParam)
{
    //判断该设备是否存在指定通道
	if (channel > m_DeviceInfo.VideoInNum)
	{
		return 4;
	}
    
	NET_PTZ_CTL_INFO NetPTZCtlInfo;
	NetPTZCtlInfo.Channel = channel;
	switch(dwPTZCommand)
	{
	case DVS_PTZ_CTRL_STOP:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_STOP;
		break;
	case DVS_PTZ_CTRL_UP:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_UP;
		break;
	case DVS_PTZ_CTRL_DOWN:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_DOWN;
		break;
	case DVS_PTZ_CTRL_LEFT:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_LEFT;
		break;
	case DVS_PTZ_CTRL_RIGHT:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_RIGHT;
		break;
	case DVS_PTZ_CTRL_ZOOM_IN:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_ZOOM_IN;
		break;
	case DVS_PTZ_CTRL_ZOOM_OUT:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_ZOOM_OUT;
		break;
	case DVS_PTZ_CTRL_FOCUS_NEAR:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_FOCUS_NEAR;
		break;
	case DVS_PTZ_CTRL_FOCUS_FAR:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_FOCUS_FAR;
		break;
	case DVS_PTZ_CTRL_PRESET_SET:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_PRESET_SET;
		NetPTZCtlInfo.ControlInfo = dwParam;
		break;
	case DVS_PTZ_CTRL_PRESET_GOTO:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_PRESET_GOTO;
		NetPTZCtlInfo.ControlInfo = dwParam;
		break;
	case DVS_PTZ_CTRL_CLEARPRESET:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_CLEARPRESET;
		break;
	case DVS_PTZ_CTRL_IRIS_OPEN:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_IRIS_OPEN;
		break;
	case DVS_PTZ_CTRL_IRIS_CLOSE:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_IRIS_CLOSE;
		break;
	case DVS_PTZ_CTRL_RESETCAMERA:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_RESETCAMERA;
		break;
	case DVS_PTZ_CTRL_AUTOPAN:
        NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_AUTOPAN;
		NetPTZCtlInfo.ControlInfo = dwParam;
		break;
	case DVS_PTZ_CTRL_UPLEFT:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_UPLEFT;
		break;
	case DVS_PTZ_CTRL_UPRIGHT:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_UPRIGHT;
		break;
	case DVS_PTZ_CTRL_DOWNLEFT:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_DOWNLEFT;
		break;
	case DVS_PTZ_CTRL_DOWNRIGHT:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_DOWNRIGHT;
		break;
	case DVS_PTZ_CTRL_SETZOOMSPEED:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_SETZOOMSPEED;
		break;
	case DVS_PTZ_CTRL_SETFOCUSSPEED:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_SETFOCUSSPEED;
		break;
	case DVS_PTZ_CTRL_OSDMENU:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_OSDMENU;
		NetPTZCtlInfo.ControlInfo = dwParam;
		break;
	case DVS_PTZ_CTRL_MENUENTER:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_MENUENTER;
		break;
	case DVS_PTZ_CTRL_SETPATTERNSTART:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_SETPATTERNSTART;
		break;
	case DVS_PTZ_CTRL_SETPATTERNSTOP:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_SETPATTERNSTOP;
		break;
	case DVS_PTZ_CTRL_RUNPATTERN:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_RUNPATTERN;
		break;
	case DVS_PTZ_CTRL_STOPPATTERN:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_STOPPATTERN;
		break;
	case DVS_PTZ_CTRL_MAX_PTZ_CMD:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_MAX_PTZ_CMD;
		break;
	case DVS_PTZ_CTRL_WRITE_CHAR:
		NetPTZCtlInfo.ControlMode = DVS_PTZ_CTRL_WRITE_CHAR;
		break;
	default:
		return 2;
	}
	if (IsOnline())
	{
		m_pDVRSocket->RequestOperation(S_PTZ_CONTROL, &NetPTZCtlInfo);
	}

	return 0;
}

void DvsPro16::GetBiAudio(UNS8 chnnl, UNS8 &biAudio)
{
	if (chnnl < 1 || chnnl > DVS_MAX_SOLO_CHANNEL_16 || m_pDVRSocket == NULL)
	{
		return;
	}

	biAudio = m_pDVRSocket->m_LastViewInfo.AudioChannel[chnnl - 1];
}

void DvsPro16::GetVideoSignalType(UNS8 &videoType)
{
	if (!m_pDVRSocket || !m_pDVRSocket->m_bGetViewInfo)
	{
		return;
	}

	videoType = m_pDVRSocket->m_LastViewInfo.VideoType;
}

void DvsPro16::GetChnnlName(UNS8 chnnl, S8 *chnnlName)
{
	if (chnnl < 1 || chnnl > DVS_MAX_SOLO_CHANNEL_16 || m_pDVRSocket == NULL)
	{
		return;
	}

	memcpy(chnnlName, m_pDVRSocket->m_LastViewInfo.PTZName[chnnl-1], DVS_MAX_PTZ_NAME_LEN);
}

void DvsPro16::GetVideoResolution(UNS8 chnnl, bool bSub, UNS8 &videoRes)
{
	if (chnnl < 1 || chnnl > DVS_MAX_SOLO_CHANNEL_16 || m_pDVRSocket == NULL)
	{
		return;
	}
	
	if (bSub)
	{
		videoRes = m_pDVRSocket->m_LastViewInfo.RecResolution[chnnl - 1]; //D1=0;CIF=1
	}else
	{
		videoRes = m_pDVRSocket->m_LastViewInfo.NetResolution[chnnl - 1]; //D1=0;CIF=1
	}
}

#define DEVICEINFO_TIMEVAL		(5 * 1000)
bool DvsPro16::InitDvs()
{
	memset(&m_DeviceInfo, 0, sizeof(DVS_DEV_INFO));
	if (m_pDVRSocket && !m_pDVRSocket->m_bGetViewInfo)
	{
		//if (m_pDVRSocket->m_hWaitViewInfo.WaitForMilliSeconds(DEVICEINFO_TIMEVAL) != WAIT_OBJECT_0)
       	//{
			//没有获取到设备信息，返回;
		//	return false;
		//}
        m_pDVRSocket->m_hWaitViewInfo.WaitSemaphore();
	}

	if (m_pDVRSocket)
	{
		memcpy(&m_DeviceInfo, &m_pDVRSocket->m_LastViewInfo, sizeof(DVS_DEV_INFO));
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////
////////
//函数名: CompareDvsTime
//作者:   czg
//日期:   2009-4-10
//功能:   判断两个自定义的时间的先后
//参数:   dtFirstTime           第一个自定义的时间
//        dtSecondTime          第二个自定义的时间
//返回值; 0表示两时间值相等
//        >0表示第一个自定义的时间在后
//        <0表示第一个自定义的时间在先
/////////
//////////////////////////////////////////////////////////////
S32 DvsPro16::CompareDvsTime(DVS_TIME dtFirstTime, DVS_TIME dtSecondTime)
{
	if (dtFirstTime.dwYear > dtSecondTime.dwYear)   { return 1; }
	else if (dtFirstTime.dwYear < dtSecondTime.dwYear) { return -1; }
	else if (dtFirstTime.dwMonth > dtSecondTime.dwMonth) { return 1; }
	else if (dtFirstTime.dwMonth < dtSecondTime.dwMonth) { return -1; }
	else if (dtFirstTime.dwDay > dtSecondTime.dwDay) { return 1; }
    else if (dtFirstTime.dwDay < dtSecondTime.dwDay) { return -1; }
	else if (dtFirstTime.dwHour > dtSecondTime.dwHour) { return 1; }
    else if (dtFirstTime.dwHour < dtSecondTime.dwHour) { return -1; }
	else if (dtFirstTime.dwMinute > dtSecondTime.dwMinute) { return 1; }
    else if (dtFirstTime.dwMinute < dtSecondTime.dwMinute) { return -1; }
	else if (dtFirstTime.dwSecond > dtSecondTime.dwSecond) { return 1; }
    else if (dtFirstTime.dwSecond < dtSecondTime.dwSecond) { return 1; }
	else { return 0; }
}

//////////////////////////////////////////////////////////////
////////
//函数名: QueryRecordFile
//作者:   czg
//日期:   2009-4-10
//功能:   返回指定时间的录像文件
//参数:   dtStartTime           指定起始时间
//        dtEndTime             指定结束时间
//        lpRecordfileList      保存返回录像文件信息的链表
//返回值; 0表示成功
//        1表示未知错误
//        2表示网络错误
/////////
//////////////////////////////////////////////////////////////
UNS32 DvsPro16::QueryRecordFile(DVS_TIME dtStartTime, DVS_TIME dtEndTime, S32 type, LPDVS_RECORDFILE_LIST lpRecordfileList)
{
	NET_SYS_DB_DATE_INFO RecordfileDateInfo = {0,};
    NET_SYS_DB_DAY_FILE DayRecordfileInfo = {0,};
	DVS_TIME dtTempTime1,dtTempTime2;
	struct tm *pTime = NULL;
	bool bRet = false;	

	//获取录像日期
	bRet = m_RemoteConfig.GetRecordfileDateInfo(&RecordfileDateInfo);
	if (!bRet)
	{
		return 2;
	}
    
	lpRecordfileList->dwRecordfileCount = 0;
    //对日期进行判断然后获取日期信息
	for (UNS32 i=0; i<RecordfileDateInfo.ItemNum; i++)
	{
		dtTempTime1.dwYear = RecordfileDateInfo.Date[i]>>16;
		dtTempTime1.dwMonth = RecordfileDateInfo.Date[i]>>8&0xff;
		dtTempTime1.dwDay = RecordfileDateInfo.Date[i]&0xff;
		dtTempTime1.dwHour = 0;
		dtTempTime1.dwMinute = 0;
		dtTempTime1.dwSecond = 0;

		dtTempTime2.dwYear = RecordfileDateInfo.Date[i]>>16;
		dtTempTime2.dwMonth = RecordfileDateInfo.Date[i]>>8&0xff;
		dtTempTime2.dwDay = RecordfileDateInfo.Date[i]&0xff;
		dtTempTime2.dwHour = 23;
		dtTempTime2.dwMinute = 59;
		dtTempTime2.dwSecond = 59;

		if (CompareDvsTime(dtStartTime, dtTempTime1) >= 0 &&
			CompareDvsTime(dtEndTime, dtTempTime2) <= 0)
		{
			bRet = m_RemoteConfig.GetDayRecordfileInfo(&DayRecordfileInfo,RecordfileDateInfo.Date[i]);
			if (!bRet)
			{
				return 2;
			}

			//对接收到的日录像文件进行分析
			for (UNS32 j=0; j<DayRecordfileInfo.TotalNum; j++)
			{ 
                time_t tz = (time_t)DayRecordfileInfo.File[j].start_time;
                pTime = localtime(&tz);
               
                
				if (pTime != NULL)
				{
					dtTempTime1.dwYear = pTime->tm_year+1900;
					dtTempTime1.dwMonth = pTime->tm_mon+1;
					dtTempTime1.dwDay = pTime->tm_mday;
					dtTempTime1.dwHour = pTime->tm_hour;
					dtTempTime1.dwMinute = pTime->tm_min;
					dtTempTime1.dwSecond = pTime->tm_sec;
				}
				
                tz = (time_t)DayRecordfileInfo.File[j].end_time;
                pTime = localtime(&tz);
				if (pTime != NULL)
				{
					dtTempTime2.dwYear = pTime->tm_year+1900;
					dtTempTime2.dwMonth = pTime->tm_mon+1;
					dtTempTime2.dwDay = pTime->tm_mday;
					dtTempTime2.dwHour = pTime->tm_hour;
					dtTempTime2.dwMinute = pTime->tm_min;
					dtTempTime2.dwSecond = pTime->tm_sec;
				}

				if (type != 0)
				{
					if (type == 1)
					{
						//手动录像
						if (DayRecordfileInfo.File[j].manual == 0)
						{
							continue;
						}
					}else if (type == 2)
					{
						if (DayRecordfileInfo.File[j].schedule == 0)
						{
							continue;
						}
					}else if (type == 3)
					{
						if (DayRecordfileInfo.File[j].motion == 0 && DayRecordfileInfo.File[j].sensor == 0)
						{
							continue;
						}
					}else if (type == 4)
					{
						if (DayRecordfileInfo.File[j].schedule_event == 0)
						{
							continue;
						}
					}
				}
				
				//if(!(CompareDvsTime(dtTempTime2,dtStartTime) < 0|| CompareDvsTime(dtTempTime1,dtEndTime) > 0))
				{
					//判断类型是否匹配
					lpRecordfileList->pRecordfileList[lpRecordfileList->dwRecordfileCount].dtStartTime = dtTempTime1;
					lpRecordfileList->pRecordfileList[lpRecordfileList->dwRecordfileCount].dtEndTime = dtTempTime2;
					lpRecordfileList->pRecordfileList[lpRecordfileList->dwRecordfileCount].dwFileSize = DayRecordfileInfo.File[j].file_size;
					memcpy(lpRecordfileList->pRecordfileList[lpRecordfileList->dwRecordfileCount].sFileName,
						DayRecordfileInfo.File[j].filename,
						MAX_FILE_NAME_LEN);
					lpRecordfileList->dwRecordfileCount++;
				}
			}
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////
////////
//函数名: OpenRemotePlay
//作者:   czg
//日期:   2009-4-14
//功能:   打开远程播放文件
//参数:   pchRemoteFileName     指定播放文件名
//        playCounts           回放路数
//返回值; 0表示成功
//        1表示未知错误
//        2表示网络错误
//        3表示抽取远程文件索引失败
/////////
//////////////////////////////////////////////////////////////
UNS32 DvsPro16::OpenRemotePlay(S8 *pchRemoteFileName, UNS8 playCounts)
{
	bool bRet = false;
	bRet = this->m_RemotePlayer.m_RemotePSFile.GetRemoteFileInfo(pchRemoteFileName);
	if (!bRet)
	{
		return 2;
	}
	
	bRet = m_RemotePlayer.m_RemotePSFile.GetRemoteFileIndex();
	if (!bRet)
	{
		return 3;//抽取远程文件索引失败
	}

	m_RemotePlayer.m_RemotePSFile.SetEof(0);
	bRet = m_RemotePlayer.InitPlayer(playCounts);
    if (!bRet)
	{
		return 1;
	}
 
	m_RemotePlayer.SetNeedRenderChn(0xffff);
	m_RemotePlayer.StopAudio();
	if (m_RemotePlayer.m_NeedRenderChn == 0)
	{
		m_RemotePlayer.ClosePlayer();
	}

	return 0;
}

void DvsPro16::CloseRemotePlay()
{
	m_RemotePlayer.ClosePlayer();
	m_nOldRemotePlayMode = -1;
}

void DvsPro16::SetRemoteChannelMask(UNS32 chnnlMask)
{
	m_RemotePlayer.SetNeedRenderChn(chnnlMask);
}

void DvsPro16::RemotePlayControl(UNS8 dwControlCode)
{
	switch(dwControlCode)
	{
	case DVS_PLAYCTRL_BACKWARD://快退
		{
			m_RemotePlayer.SendPlayStatus(PAUSE);
			S32 rate = m_RemotePlayer.m_RateScale;
			switch (rate)
			{
			case 1:
				m_RemotePlayer.SetPlayRate(2);
				break;
			case 2:
				m_RemotePlayer.SetPlayRate(4);
				break;
			case 4:
				m_RemotePlayer.SetPlayRate(8);
				break;
			case 8:
				m_RemotePlayer.SetPlayRate(16);
				break;
			case 16:
				m_RemotePlayer.SetPlayRate(32);
				break;
			case 32:
				m_RemotePlayer.SetPlayRate(2);
				break;
			default:
				m_RemotePlayer.SetPlayRate(1);
				break;
			}
			m_RemotePlayer.FastBackward();
			m_nOldRemotePlayMode = dwControlCode;
		}
		break;
	case DVS_PLAYCTRL_BACKPLAY://倒放
		if (dwControlCode != m_nOldRemotePlayMode)
		{
			m_RemotePlayer.SendPlayStatus(PAUSE);
			m_RemotePlayer.SetPlayRate(1);
			m_RemotePlayer.Backward();
			m_nOldRemotePlayMode = dwControlCode;
		}
        break;
	case DVS_PLAYCTRL_PLAY://播放
		if (dwControlCode != m_nOldRemotePlayMode)
		{
			m_RemotePlayer.SendPlayStatus(PAUSE);
			m_RemotePlayer.SetPlayRate(1);
			m_RemotePlayer.SetPlayIFrame(0);
			m_RemotePlayer.PlayAllVideo();
			m_RemotePlayer.SendPlayStatus(PLAY_PLUS_0);
			m_nOldRemotePlayMode = dwControlCode;
		}
        break;
	case DVS_PLAYCTRL_FORWARD://快进
		{
			S32 rate = m_RemotePlayer.m_RateScale;
			switch (rate)
			{
			case 1:
				m_RemotePlayer.SetPlayRate(2);
				break;
			case 2:
				m_RemotePlayer.SetPlayRate(4);
				break;
			case 4:
				m_RemotePlayer.SetPlayRate(8);
				break;
			case 8:
				m_RemotePlayer.SetPlayRate(16);
				break;
			case 16:
				m_RemotePlayer.SetPlayRate(32);
				break;
			case 32:
				m_RemotePlayer.SetPlayRate(2);
				break;
			default:
				m_RemotePlayer.SetPlayRate(1);
				break;
			}
			m_RemotePlayer.SendPlayStatus(PAUSE);
			m_RemotePlayer.SetPlayIFrame(true);
			m_RemotePlayer.PlayAllVideo();
			m_nOldRemotePlayMode = dwControlCode;
		}
        break;
	case DVS_PLAYCTRL_BACKSHIFT://单帧后退
		if (dwControlCode != m_nOldRemotePlayMode)
		{
			m_RemotePlayer.SendPlayStatus(PAUSE);
			m_RemotePlayer.SetPlayRate(1);
		}
		m_RemotePlayer.StepBack();
		m_nOldRemotePlayMode = dwControlCode;
        break;
	case DVS_PLAYCTRL_SHIFT://单帧播放
		if ((S32)dwControlCode != m_nOldRemotePlayMode)
		{
            m_RemotePlayer.SendPlayStatus(PLAY_PLUS_0);
        }
		m_RemotePlayer.StepNext();
		m_nOldRemotePlayMode = dwControlCode;
		break;
	case DVS_PLAYCTRL_PAUSE://暂停
		if (dwControlCode != m_nOldRemotePlayMode)
		{
			m_RemotePlayer.Pause();
			m_nOldRemotePlayMode = dwControlCode;
		}
		break;
	case DVS_PLAYCTRL_STOP://停止
		if (dwControlCode != m_nOldRemotePlayMode)
		{
			m_RemotePlayer.Stop();
			m_nOldRemotePlayMode = dwControlCode;
		}
        break;
	default:
		break;
	}
}

bool DvsPro16::GetRemotePlayTime(LPDVS_TIME lpCurPlayTime)
{
	struct tm *timeTemp = NULL;
	UNS32 curtime = m_RemotePlayer.m_RemotePSFile.GetCurLTime();
	if (curtime == 0)
	{
		return false;
	}

	//localtime_s(timeTemp, (const time_tx*)&curtime);
	if (timeTemp != NULL)
	{
		lpCurPlayTime->dwYear = timeTemp->tm_year + 1900;
		lpCurPlayTime->dwMonth = timeTemp->tm_mon + 1;
		lpCurPlayTime->dwDay = timeTemp->tm_mday;
		lpCurPlayTime->dwHour = timeTemp->tm_hour;
		lpCurPlayTime->dwMinute = timeTemp->tm_min;
		lpCurPlayTime->dwSecond = timeTemp->tm_sec;
		return true;
	}

	return false;
}

void DvsPro16::StartRemotePlayAudio(UNS8 channel)
{
	if (channel <= m_DeviceInfo.VideoInNum)
	{
		m_RemotePlayer.PlayAudio(channel - 1);
	}
}

void DvsPro16::StopRemotePlayAudio()
{
	m_RemotePlayer.StopAudio();
}

void DvsPro16::SeekRemotePlayByTime(UNS64 dwOffsetTime)
{
	//算出相对当前小时0分0秒的相对偏移
	UNS64 unStartTime     = m_RemotePlayer.m_RemotePSFile.GetStartTime();
	UNS64 unNewOffsetTime = unStartTime + dwOffsetTime;

	m_RemotePlayer.SendPlayStatus(PAUSE);
	m_RemotePlayer.Seek(unNewOffsetTime);

	if ((m_nOldRemotePlayMode == DVS_PLAYCTRL_PLAY) ||
		(m_nOldRemotePlayMode == DVS_PLAYCTRL_SHIFT))
	{
		m_RemotePlayer.SendPlayStatus(PLAY_PLUS_0);
	}
	
	if (m_nOldRemotePlayMode == DVS_PLAYCTRL_FORWARD)
	{
		m_RemotePlayer.SendPlayStatus(PAUSE);
	}
}

bool DvsPro16::GetDeviceInfo(LPDVS_DEV_INFO lpDeviceInfo)
{
	if (lpDeviceInfo == NULL)
	{
		return false;
	}

	memset(lpDeviceInfo, 0, sizeof(DVS_DEV_INFO));
	if (m_pDVRSocket && m_pDVRSocket->m_bOnLine && m_pDVRSocket->m_bGetViewInfo)
	{
		memcpy(lpDeviceInfo, &m_pDVRSocket->m_LastViewInfo, sizeof(DVS_DEV_INFO));
		m_pDVRSocket->m_bChnnlNameChanged = false;
		return true;
	}

	return false;
}

UNS32 DvsPro16::GetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size)
{
	return m_RemoteConfig.GetDvrConfig(cmdId, pCfg, size);
}

UNS32 DvsPro16::SetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size)
{
	return m_RemoteConfig.SetDvrConfig(cmdId, pCfg, size);
}

bool DvsPro16::RemotePlayIsEnd()
{
	return m_RemotePlayer.IsEnd();
}

UNS32 DvsPro16::RemoteDumpOpen(S8 *filename, S8 *pSaveAs, UNS32 startTime, UNS32 endTime, UNS32 chnnlMask)
{
	UNS32 ret = 0;
	do
	{
		m_dumpFileSize.QuadPart = 0;
		m_dumpedSize.QuadPart   = 0;
		bool bGetFileInfo = false;
		
		if (m_pWriter != NULL || m_pDVRSocket == NULL)
		{
			ret = 1;//下载任务已经存在
			break;
		}
		
		//获取文件信息
		UNS32 iRet = 0;
		if (IsOnline())
		{
			iRet = m_pDVRSocket->RequestOperation(S_PLAY_FILE_NAME, filename, m_pDVRSocket->m_CmdSocket);
			iRet = m_pDVRSocket->RequestOperation(S_WANT_PLAY_FILE_INFO, filename, m_pDVRSocket->m_CmdSocket);
		}
		if (iRet == 0)
		{
			ret = 2;
			break;
		}
		
		//等待接收参数直至超时
		S32 nWaitTime = 0;
		while(IsOnline())
		{
			if (m_pDVRSocket->GetLastRecvMsgType() == NM_PLAY_FILE_INFO)
			{
				memcpy(&m_dumpFileInfo, m_pDVRSocket->ReceiveBuffer, sizeof(PS_FILE_HDR));
				bGetFileInfo = true;
				break;
			}
			if (nWaitTime >= m_pDVRSocket->GetOverTime())
			{
				break;
			}
			nWaitTime++;
			Sleep(1);
		}
		
		if (!bGetFileInfo)
		{
			//获取文件信息失败
			ret = 3;
			break;
		}
		
		if (!IsOnline() || !m_pDVRSocket->CreateDownTCPSocket())
		{
			ret = 4;
			break;
		}
		
		NET_TRANS_PACKET packet;
		if (!IsOnline() || !m_pDVRSocket->Net_GetTcpPacket(&packet, m_pDVRSocket->m_DownTcpSocket, 2))
		{
			ret = 5;
			break;
		}
		
		if (packet.InfoType == BACKUP_ACCESS_REFUSED)
		{
			ret = 6;
			break;
		}
		
		if (IsOnline())
		{
			m_pDVRSocket->RequestOperation(S_PLAY_FILE_NAME, filename, m_pDVRSocket->m_DownTcpSocket);
		}		
		if (!IsOnline() || !m_pDVRSocket->Net_GetTcpPacket(&packet, m_pDVRSocket->m_DownTcpSocket, 2))
		{
			ret = 7;
			break;
		}
		
		if (packet.InfoType == FILE_OPEN_FAILED)
		{
			ret = 8;
			break;
		}
		
		if (IsOnline())
		{
			m_pDVRSocket->RequestOperation(S_WANT_FILE_RIGHT_CHANNEL, 0);
		}
		if (!IsOnline() || !m_pDVRSocket->Net_GetTcpPacket(&packet, m_pDVRSocket->m_DownTcpSocket, 2))
		{
			ret = 9;
			break;
		}
		
		NET_FILE_BACKUP_SIZE netFileBackupSize;
		netFileBackupSize.VideoChannel  = chnnlMask;
		netFileBackupSize.AudioChannel  = chnnlMask;
		netFileBackupSize.FileStartTime = startTime;
		netFileBackupSize.FileEndTime   = endTime;
		netFileBackupSize.FileSize_L    = 0;//0 表示想获得文件尺寸
		netFileBackupSize.FileSize_H    = 0;
		netFileBackupSize.FileFormat    = BACKUP_FORMAT_PS;		
		if (IsOnline())
		{
			m_pDVRSocket->RequestOperation(S_BACKUP_SIZE, &netFileBackupSize, m_pDVRSocket->m_DownTcpSocket);
		}
		
		UNS32 headersize = MAX_NET_STARTCODE_LEN + sizeof(UNS32) * 2 + sizeof(NET_TRANS_TYPE);
		if (!IsOnline() || !m_pDVRSocket->Net_GetTcpPacket(&packet, m_pDVRSocket->m_DownTcpSocket, 3))
		{
			ret = 10;
			break;
		}
		
		memcpy(&netFileBackupSize, ((S8*)&packet) + headersize, sizeof(netFileBackupSize));
		m_dumpFileSize.LowPart  = netFileBackupSize.FileSize_L;
		m_dumpFileSize.HighPart = netFileBackupSize.FileSize_H;		
		if (IsOnline())
		{
			m_pDVRSocket->SendStartDownload(m_pDVRSocket->m_DownTcpSocket, &netFileBackupSize, sizeof(netFileBackupSize));
		}
		
		m_pWriter = new PSWriter();
		if (m_pWriter == NULL)
		{
			ret = 11;
			break;
		}
		
		if (!m_pWriter->Create(pSaveAs, &m_dumpFileInfo, sizeof(m_dumpFileInfo), false, chnnlMask))
		{
			ret = 12;
			break;
		}
		
		m_bExitDump = false;
		if (!m_hDump.CreateAThread(DumpFileThread, this))
		{
			ret = 13;
			break;
		}

		return ret;
	} while (0);
	
	if (m_pDVRSocket && m_pDVRSocket->m_DownTcpSocket != INVALID_SOCKET)
	{
		m_pDVRSocket->DeleteDownTCPSocket();
	}
	
	if (m_pWriter != NULL)
	{
		delete m_pWriter;
		m_pWriter = NULL;
	}
	
	return ret;
}

void DvsPro16::RemoteDumpClose()
{
	if (m_pDVRSocket)
	{
		m_pDVRSocket->DeleteDownTCPSocket();
	}

	m_bExitDump = true;
	m_hDump.ExitAThread();

	if (m_pWriter != NULL)
	{
		delete m_pWriter;
		m_pWriter = NULL;
	}
}

S32  DvsPro16::RemoteDumpProgress()
{
	if (m_hDump.ThreadIsNULL())
	{		
		return -1;//返回-1当前没有下载任务
	}
	
	if (m_bExitDump)
	{
		return 100;
	}
	
	S64 temp = 0;
	temp = (m_dumpedSize.QuadPart * 100 / 1024) / m_dumpFileSize.QuadPart;
	if (temp > 100)
	{
		temp = 100;
	}

	return (S32)temp;
}

void DvsPro16::DumpTask()
{
	S8 *buf = NULL;
	NET_TRANS_PACKET packet;
	
	UNS32 headersize = MAX_NET_STARTCODE_LEN + sizeof(UNS32) * 2 + sizeof(NET_TRANS_TYPE);
	do
	{
		if (!IsOnline() || !m_pDVRSocket->Net_GetTcpPacket(&packet, m_pDVRSocket->m_DownTcpSocket, 10))
		{
			break;
		}
		
		switch(packet.InfoType)
		{
		case PLAY_FILE_IS_END://文件备份结束回复消息
			m_dumpedSize.QuadPart = m_dumpFileSize.QuadPart;
			m_dumpedSize.QuadPart *= 1024;
			break;
		case PLAY_FILE_PES_PACKET://获取备份文件的PES包回复
			{
				buf = (S8*)&packet;
				buf = buf + headersize;
				NET_FILE_PES_PACKET *filepacket = (NET_FILE_PES_PACKET*)buf;
				printf("pes length = %d\n", filepacket->PESPacketLength);
				m_dumpedSize.QuadPart += filepacket->PESPacketLength;
				if (m_pWriter != NULL)
				{
					if (!m_pWriter->PushPacket((S8*)filepacket->PESPacket, filepacket->PESPacketLength))
					{
						//写文件出错
						break;
					}
				}
			}
			break;
		case RIGHT_IS_INVALID://BACKUP_ACCESS的回复消息，没有下载权限
			m_pWriter->Destroy(true);
			m_dumpedSize.QuadPart = m_dumpFileSize.QuadPart;
			m_dumpedSize.QuadPart *= 1024;
			break;
		default:
			break;
		}
	} while (!m_bExitDump);
	
	m_bExitDump = true;
}

UNS32 DvsPro16::SpeechStart(const S8* ip, UNS16 port)
{
	UNS32 unRet = 0;
	S8 cmd[MAX_CMD_LEN + 1] = {0, };
	do
	{
		if (m_pSpeech != NULL)
		{
			unRet = 1;
			break;
		}

		m_pSpeech = new DvrSpeech();
		if (m_pSpeech == NULL)
		{
			unRet = 2;
			break;
		}

		WAVEFORMATEX format;
		memset(&format, 0, sizeof(format));
		format.wFormatTag = AUDIO_CODEC_ADPCM;
		format.nChannels = 1;
		format.nSamplesPerSec = 8000;
		format.wBitsPerSample = 16;

		if (!IsOnline())
		{
			unRet = 3;
			break;
		}

		NET_MEDIA_ACCESS access;
		access.UserID      = m_pDVRSocket->m_nIDCode;
		access.AccessCode  = m_pDVRSocket->m_AccessCode.AccessCode;
		access.StreamType  = MAIN_STREAM;
		UNS32 cmdlen = PackCommand(&access, sizeof(access), MIDEA_ACCESS, cmd, sizeof(cmd), m_pDVRSocket->m_nIDCode);
		if (0 == m_pSpeech->Create(ip, port, format, cmd, cmdlen))
		{
			return unRet;
		}else
		{
			unRet = 4;
		}
	} while (0);

	if (m_pSpeech != NULL)
	{
		m_pSpeech->Destroy();
		delete m_pSpeech;
		m_pSpeech = NULL;
	}

	return unRet;
}

void DvsPro16::SpeechStop()
{
	if (m_pSpeech == NULL)
	{
		return;
	}

	m_pSpeech->Destroy();
	delete m_pSpeech;
	m_pSpeech = NULL;
}

bool DvsPro16::IsSpeech()
{
	return (m_pSpeech != NULL);
}

bool DvsPro16::IsOnline()
{
	if (m_pDVRSocket == NULL)
		return false;
	else
		return m_pDVRSocket->m_bOnLine;
}

//获取用户权限
void DvsPro16::GetUserRight(LPVOID pCfg, S32 size)
{
	m_RemoteConfig.GetUserRight(pCfg, size);
}

//获取网络资源连接数
void DvsPro16::GetLinkNum(LPVOID pCfg, S32 size)
{
	m_RemoteConfig.GetLinkNum(pCfg, size);
}

//通道名称改变
bool DvsPro16::ChnnlNameChanged()
{
	if (m_pDVRSocket == NULL)
		return false;
	else
		return m_pDVRSocket->m_bChnnlNameChanged;
}

//请求DVR发送测试邮件
UNS32 DvsPro16::RequestEmailTest()
{
	return m_RemoteConfig.RequestEmailTest();
}

void DvsPro16::SetRemoteDataCallBack(process_cb_ex cbRemoteFunc, void* dwUser)
{
	if (cbRemoteFunc)
	{
		m_RemotePlayer.SetRemoteDataCallBack(cbRemoteFunc, dwUser);
	}
}
