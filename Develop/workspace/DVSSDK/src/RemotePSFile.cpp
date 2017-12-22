// RemotePSFile.cpp: implementation of the CRemotePSFile class.
//
//////////////////////////////////////////////////////////////////////
#include "DVRSocket.h"
#include "RemotePSFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRemotePSFile::CRemotePSFile()
{
	m_CurPlayChn = -1;
	m_CurIndexItemNum = 0;
	m_psIndexRecord = NULL;    
	m_RecvPESPacket = true;
    m_bRecvFileOffset = false;
	m_isMoveTo = false;
	m_isNormalplay = false;
	m_bTimeNeedReset = false;
    m_bIsEof = false;
}

CRemotePSFile::~CRemotePSFile()
{
	if (m_psIndexRecord != NULL) 
	{
		delete []m_psIndexRecord;
		m_psIndexRecord = NULL;
	}
}

void CRemotePSFile::RecvRemoteFileInfo()
{
	m_Section.Lock();
	if (m_DVRSocket && m_DVRSocket->m_bOnLine)
	{
		memcpy(&m_psFileHead, m_DVRSocket->ReceiveBuffer, sizeof(PS_FILE_HDR));
	}
	else
	{
		memset(&m_psFileHead, 0, sizeof(PS_FILE_HDR));
	}

	for (S32 i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		for (S32 j=0; j<MAX_MIN_IN_HOUR; j++)
		{
			if (m_psFileHead.RecMode[i][j] != REC_OFF)
			{
				//录像通道根据读取的RecordChannel和每分钟各通道的录像模式两项判断
				m_psFileHead.RecordChannel = m_psFileHead.RecordChannel|(0x0001<<i);
				break;
			}
		}
	}

	if (m_psIndexRecord != NULL) 
	{
		delete []m_psIndexRecord;
		m_psIndexRecord = NULL;
	}
	m_psIndexRecord = new PS_INDEX_RECORD[m_psFileHead.TotalIndex];
	m_Section.Unlock();
}

void CRemotePSFile::CloseRemotePSFile()
{
	if (m_psIndexRecord != NULL) 
	{
		delete []m_psIndexRecord;
		m_psIndexRecord = NULL;
	}
	m_pesPackHead.InitPESPacket();	
}

void CRemotePSFile::RecvRemoteFileOffset()
{
	std64_net_t offset;
	if (m_DVRSocket == NULL || !m_DVRSocket->m_bOnLine)
	{
		return;
	}

	memcpy(&offset, m_DVRSocket->ReceiveBuffer, sizeof(offset));
	if (!m_isMoveTo)
	{
		m_CurIndexItemNum = GetIndexItemNumByFileoffset(offset);
	}
	m_bRecvFileOffset = true;
}

//返回false表示还没有接收完,返回true表示接收完
bool CRemotePSFile::RecvRemoteFileIndex()
{
	if (m_DVRSocket == NULL || !m_DVRSocket->m_bOnLine || m_psIndexRecord == NULL)
	{
		return false;
	}

	NET_FILE_INDEX *pNetFileIndex = (NET_FILE_INDEX*)m_DVRSocket->ReceiveBuffer;
	if (pNetFileIndex->StartItemNum > m_psFileHead.TotalIndex)
	{
		pNetFileIndex->StartItemNum = m_psFileHead.TotalIndex;
	}

	if (pNetFileIndex->EndItemNum < 1)
	{
		pNetFileIndex->EndItemNum = 1;
	}

	pNetFileIndex->StartItemNum--;
	pNetFileIndex->EndItemNum--;

	memcpy(&m_psIndexRecord[pNetFileIndex->StartItemNum], pNetFileIndex->Index, 
		sizeof(PS_INDEX_RECORD) * (pNetFileIndex->EndItemNum-pNetFileIndex->StartItemNum + 1));

	if (m_DVRSocket == NULL || !m_DVRSocket->m_bOnLine)
	{
		return false;
	}

	if (0 == pNetFileIndex->StartItemNum) 
	{
		MoveFirstPESPack();
		m_DVRSocket->m_bIsTreated = false;
		return true;

	}else
	{
		m_DVRSocket->m_bIsTreated = true;
		return false;
	}
}

void CRemotePSFile::SetEof(bool bIsEof)
{
	m_bIsEof = bIsEof;
}

void CRemotePSFile::WantRemoteFilePESPacket()
{
	m_Section.Lock();
	if (m_RecvPESPacket) 
	{
		if (m_psIndexRecord == NULL || m_CurIndexItemNum >= m_psFileHead.TotalIndex)
		{
			m_Section.Unlock();
			return;
		}

		std64_net_t offset;
		offset.H = m_psIndexRecord[m_CurIndexItemNum].H_offset;
		offset.L = m_psIndexRecord[m_CurIndexItemNum].L_offset;
		m_RecvPESPacket = false;
		if (m_DVRSocket && m_DVRSocket->m_bOnLine)
		{
			m_DVRSocket->RequestOperation(S_WANT_PLAY_FILE_PES_PACKET, &offset);
		}
		SetEof(0);
	}
	m_Section.Unlock();
}

bool CRemotePSFile::GetRemoteFileInfo(S8 *pchFileName)
{
	if (m_DVRSocket && m_DVRSocket->m_bOnLine)
	{
		S32 iRet = m_DVRSocket->RequestOperation(S_PLAY_FILE_NAME, pchFileName, m_DVRSocket->m_CmdSocket);	
		iRet = m_DVRSocket->RequestOperation(S_WANT_PLAY_FILE_INFO, pchFileName, m_DVRSocket->m_CmdSocket);
		if (iRet == 0)
		{
			return false;
		}
	}

	//等待接收参数直至超时
	S32 nWaitTime = 0;
	while(m_DVRSocket && m_DVRSocket->m_bOnLine)
	{
		if (m_DVRSocket->GetLastRecvMsgType() == NM_PLAY_FILE_INFO)
		{
			RecvRemoteFileInfo();
			return true;
		}
		if (nWaitTime >= m_DVRSocket->GetOverTime())
		{
			break;
		}
		nWaitTime++;
		Sleep(1);
	}

	return false;
}

bool CRemotePSFile::GetRemoteFileIndex()
{
	if (m_DVRSocket == NULL || !m_DVRSocket->m_bOnLine)
	{
		return false;
	}

	m_DVRSocket->m_bIsTreated = false;
    S32 iRet = m_DVRSocket->RequestOperation(S_WANT_PLAY_FILE_INDEX, 0);
	if (iRet == 0) 
	{
		return false;
	}
    
	//等待接收参数直至超时
	S32 nWaitTime = 0;
	while(m_DVRSocket && m_DVRSocket->m_bOnLine)
	{
		if (m_DVRSocket->GetLastRecvMsgType() == NM_PLAY_FILE_INDEX)
		{
            return true;
		}
		Sleep(1);

		nWaitTime++;
		if (nWaitTime > m_DVRSocket->GetOverTime())
		{
			if (m_DVRSocket->m_bIsTreated)
			{
				Sleep(10);
				continue;
			}else
			{
				Sleep(10);
				if (m_DVRSocket == NULL || !m_DVRSocket->m_bOnLine || 
					m_DVRSocket->GetLastRecvMsgType() != NM_PLAY_FILE_INDEX)
				{
					break;
				}
			}
		}
	}
	
	return false;
}

void CRemotePSFile::RecvRemoteFilePESPacket()
{
	if (m_DVRSocket == NULL || !m_DVRSocket->m_bOnLine)
	{
		return;
	}

    m_DVRSocket->m_DVRSocketSection.Lock();
	NET_FILE_PES_PACKET *pNetFilePESPacket = (NET_FILE_PES_PACKET*)m_DVRSocket->ReceiveBuffer;
	NET_FILE_PES_PACKET tmpPack;
	memcpy(&tmpPack, pNetFilePESPacket, sizeof(NET_FILE_PES_PACKET));
	m_hRecvPESPacketMutex.Lock();
	memcpy(m_PESBuf, tmpPack.PESPacket, tmpPack.PESPacketLength);
	m_hRecvPESPacketMutex.Unlock();
	m_pesPackHead.SetPESBuf(m_PESBuf);
	m_RecvPESPacket = true;

	static S32 jmpPackNum=0;//由于各个线程之间同步不好 导致在定点播放的时候有时候会暂停
	if (m_isMoveTo) 
	{
		jmpPackNum++;
		m_bTimeNeedReset = true;
		if (3 == jmpPackNum) 
		{
			m_isMoveTo = false;
			jmpPackNum = 0;
		}
	}
	m_DVRSocket->m_DVRSocketSection.Unlock();
}

bool CRemotePSFile::MoveFirstPESPack()
{
	m_Section.Lock();
	m_CurIndexItemNum = 0;
	m_Section.Unlock();
	return true;
}

bool CRemotePSFile::MoveFirstIPESPack()
{
	m_Section.Lock();
	m_CurIndexItemNum = 0;
	while(1)
	{
		if (GetChnID() >= DVS_MAX_SOLO_CHANNEL_16)
		{
			break;
		}
		if ((1<<GetChnID() & m_CurPlayChn) && (I_PICTURE == GetVopType())) 
		{
			WantRemoteFilePESPacket();
			m_Section.Unlock();
			return true;
		}
		m_CurIndexItemNum++;
	}	
	m_Section.Unlock();

	return false; 
}

bool CRemotePSFile::MoveNextPESPack()
{
	m_Section.Lock();
	if (IsEof()) 
	{
		//调用界面的暂停功能
	}else
	{
		WantRemoteFilePESPacket();
	}
	m_Section.Unlock();
	
	return true;
}

bool CRemotePSFile::MoveNextIPESPack()
{
	m_Section.Lock();
	while (!IsEof()) 
	{
		m_CurIndexItemNum++;
		if (IsEof()) 
		{
			//调用界面的暂停功能
		}
		if (GetChnID() >= DVS_MAX_SOLO_CHANNEL_16)
		{
			break;
		}
		if ((1<<GetChnID() & m_CurPlayChn) && (I_PICTURE == GetVopType())) 
		{
			WantRemoteFilePESPacket();
			m_Section.Unlock();
			return true;
		}
	}
	m_Section.Unlock();

	return false;
}

bool CRemotePSFile::MovePrePESPack()
{
	m_Section.Lock();
	if (0 != m_CurIndexItemNum) 
	{
		m_CurIndexItemNum--;
	}	
	m_Section.Unlock();

	return true;
}

bool CRemotePSFile::MovePreIPESPack()
{
	m_Section.Lock();
	while (!IsFirstPESPack()) 
	{
		MovePrePESPack();
		if (GetChnID() >= DVS_MAX_SOLO_CHANNEL_16)
		{
			break;
		}
		if ((1<<GetChnID() & m_CurPlayChn) && (I_PICTURE == GetVopType())) 
		{
			WantRemoteFilePESPacket();
			m_Section.Unlock();
			return true;
		}
	}
	m_Section.Unlock();

	return false;
}

bool CRemotePSFile::MoveTo(UNS64 Time)
{
	m_Section.Lock();
	m_isMoveTo = true;
	m_CurIndexItemNum = GetIndexItemNumByTime(Time);
	
	if (IsEof())
	{
		if (m_CurIndexItemNum < 100)
		{
			m_CurIndexItemNum = 100;
		}
		m_CurIndexItemNum = m_CurIndexItemNum - 100;		
	}

	WantRemoteFilePESPacket();
	m_Section.Unlock();

	return true;
}

void CRemotePSFile::SetCurPlayChn(UNS32 chn)
{
	if (m_CurPlayChn != chn) 
	{
		m_CurPlayChn = chn;
		NET_PLAY_STATUS_CHANGE StatusChange;
		memset(&StatusChange, 0, sizeof(NET_PLAY_STATUS_CHANGE));
		StatusChange.PlayStatus = VIDEO_CHANGE;
		StatusChange.SecondData = chn;
		if (m_DVRSocket && m_DVRSocket->m_bOnLine)
		{
			m_DVRSocket->RequestOperation(S_PLAY_STATUS_CHANGE, &StatusChange, m_DVRSocket->m_CmdSocket);
		}
		m_RecvPESPacket = true;
	}		
}

bool CRemotePSFile::IsEof()
{
	if (m_bIsEof == true)
	{
		return true;
	}else
	{
		return m_CurIndexItemNum >= m_psFileHead.TotalIndex - 1;
	}
}

UNS32 CRemotePSFile::GetPESLength()
{
	return m_pesPackHead.GetPESLength();
}

UNS32 CRemotePSFile::GetChnID()
{
	if (true == m_isNormalplay)
	{
		return m_pesPackHead.GetChnID();
	}else
	{
		if (m_psIndexRecord == NULL || m_CurIndexItemNum >= m_psFileHead.TotalIndex)
		{
			return MAX_SOLO_CHANNEL_16;
		}
		return m_psIndexRecord[m_CurIndexItemNum].ch_type & 0x0F;
	}
}

S8* CRemotePSFile::GetBuf()
{
	return m_PESBuf;
}

UNS32 CRemotePSFile::GetPTS()
{
	return m_pesPackHead.GetPTS();
}

UNS32 CRemotePSFile::GetAVType()
{
	S32 frametype = 0;
	if (true == m_isNormalplay)
	{
		return m_pesPackHead.GetAVType();
	}else
	{
		if (m_psIndexRecord != NULL && m_CurIndexItemNum < m_psFileHead.TotalIndex)
		{
			frametype = (m_psIndexRecord[m_CurIndexItemNum].ch_type & 0x30)>>4;
		}else
		{
			return 0;
		}
		
		if ((I_PICTURE == frametype) || P_PICTURE == frametype)
		{
			return VIDEO_FRAME;
		}

		return AUDIO_FRAME;
	}
}

UNS32 CRemotePSFile::GetVopType()
{
	if (true == m_isNormalplay)
	{
		return m_pesPackHead.GetVopType();
	}else
	{
		if (m_psIndexRecord == NULL || m_CurIndexItemNum >= m_psFileHead.TotalIndex)
		{
			return VIDEO_FRAME;
		}
		return (m_psIndexRecord[m_CurIndexItemNum].ch_type & 0x30)>>4;
	}
}

UNS64 CRemotePSFile::GetStartTime()
{
	return m_psFileHead.TimeStart;
}

UNS32 CRemotePSFile::GetCurLTime()
{
	return (UNS32)m_pesPackHead.GetCurLtime();
}

UNS32 CRemotePSFile::GetCurDate()
{
	return m_pesPackHead.GetCurDate();
}

UNS32 CRemotePSFile::GetIndexItemNumByFileoffset(std64_net_t offset)
{
	UNS32 ret = 0;
	m_Section.Lock();
/*	do
	{
		if (!m_isNormalplay)
		{
			ret = m_CurIndexItemNum;
			break;
		}

		if (m_psIndexRecord == NULL)
		{
			break;
		}

		UNS32 headcount = 0;
		UNS32 endcount  = m_psFileHead.TotalIndex - 1;
		UNS32 compcount = (headcount + endcount) / 2;
		UNS64 tempoffset = 0;
		tempoffset = ((UNS64)m_psIndexRecord[headcount].H_offset<<32)|m_psIndexRecord[headcount].L_offset;
		if ((((UNS64)offset.H<<32)|offset.L) <= tempoffset)
		{
			ret = headcount;
			break;
		}

		tempoffset = ((UNS64)m_psIndexRecord[endcount].H_offset<<32)|m_psIndexRecord[endcount].L_offset;
		if ((((UNS64)offset.H<<32)|offset.L) >= tempoffset)
		{
			ret = endcount;
			break;
		}

		tempoffset = ((UNS64)m_psIndexRecord[compcount].H_offset<<32)|m_psIndexRecord[compcount].L_offset;
		while(!(((((UNS64)offset.H<<32)|offset.L) == tempoffset) || ((endcount-headcount)==1)))
		{
			if (tempoffset < (((UNS64)offset.H<<32)|offset.L))
			{
				headcount = compcount;		
			}else
			{
				endcount = compcount;
			}
			compcount = (headcount+endcount)/2;
			tempoffset = ((UNS64)m_psIndexRecord[compcount].H_offset<<32)|m_psIndexRecord[compcount].L_offset;
		}
		if (compcount > (m_psFileHead.TotalIndex-3))
		{
			S32 i = 0;
		}
		ret = compcount;

	} while (0);
	m_Section.Unlock();
*/
	return ret;
}

UNS32 CRemotePSFile::GetIndexItemNumByTime(UNS64 Time)
{
	if (m_psFileHead.TotalIndex < 2)
	{
		return 0;
	}

	for (UNS32 i=0; i<m_psFileHead.TotalIndex - 1; i++)
	{
		if (m_psIndexRecord[i].time >= Time)
		{
			return i;
		}
	}

	return 0;
}

//如接收到数据包返回true,否则返回false
bool CRemotePSFile::GetRecvPESPacket() 
{
	return m_RecvPESPacket;
}
