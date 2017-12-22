#include "PSWriter.h"
#include "macro.h"

#define MAX_INDEX		(30 * 1024)
#define EXE_FILE_MAGIC	"LianYouShiChuang"

PSWriter::PSWriter()
{
	m_pRWFile = NULL;
	m_blockStartTime = 0;
	m_pIndex = NULL;
	m_indexNum = 0;
	m_totalIndex = 0;
	m_offset.QuadPart = 0;
	m_bFirstPacket = true;
	m_bFirstWrite = false;
	m_bRealStream = false;
	m_bEXEFileBk = false;
	m_startTime = 0;
	m_lastTime  = 0;
	m_exeFileLen = 0;
	m_iRecordChnnel = 0;
	memset(m_cFileName, 0, MAX_PATH + 1);
}

PSWriter::~PSWriter()
{
	Destroy();
}

bool PSWriter::Create(const S8 *pathname, void *pInfo, S32 infoSize, bool bReal, UNS32 chnnlMask)
{
	bool bFailed = true;
	PS_FILE_HDR fileInfo;
	memset(&fileInfo, 0, sizeof(PS_FILE_HDR));
	m_bRealStream = bReal;
	if (m_bRealStream)
	{
		m_bFirstWrite = true;
	}else
	{
		m_bEXEFileBk = false;
		m_iRecordChnnel = chnnlMask;
	}

	do
	{
		m_pIndex = new PS_INDEX_RECORD[MAX_INDEX];
		if (m_pIndex == NULL)
		{
			break;
		}

		memset(&fileInfo, 0, sizeof(fileInfo));
		m_pRWFile = fopen(pathname, "w+b");
		if (m_pRWFile == NULL)
		{
			break;
		}
		snprintf(m_cFileName, MAX_PATH, "%s", pathname);

		/*if (m_bEXEFileBk)
		{
			CString strPath;
			if (isOcx)
			{
				GetSystemDirectory(strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
				strPath.ReleaseBuffer();
			}else
			{
				GetModuleFileName(0, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
				strPath.ReleaseBuffer();
				S32 nPos = strPath.ReverseFind('\\');
				strPath = strPath.Left(nPos);
			}
			strPath += _T("\\player.exe");
			S8 chPath[MAX_PATH + 1] = {0};
			UnicodeToANSI(strPath.GetBuffer(0), chPath);
			FILE *exeFile = NULL;
			fopen_s(&exeFile, chPath, "rb");
			if (exeFile == NULL)
			{
				m_bEXEFileBk = false;
				break;
			}

			fseek(exeFile, 0, SEEK_END);
			m_exeFileLen = ftell(exeFile);
			S8 *exeInput = new S8[m_exeFileLen + 1];
			memset(exeInput, 0, m_exeFileLen + 1);
			fseek(exeFile, 0, SEEK_SET);
			fread_s(exeInput, m_exeFileLen, m_exeFileLen, 1, exeFile);
			S32 writeCount = fwrite(exeInput, m_exeFileLen, 1, m_pRWFile);
			writeCount = fwrite(EXE_FILE_MAGIC, 16, 1, m_pRWFile);
			m_exeFileLen += 16;
			delete []exeInput;
			exeInput = NULL;
			fclose(exeFile);
			exeFile = NULL;
			if (writeCount != 1)
			{
				m_bEXEFileBk = false;
				m_exeFileLen = 0;
				break;
			}
		}*/

		if (infoSize != sizeof(fileInfo) || pInfo == NULL)
		{
			pInfo = &fileInfo;
			infoSize = sizeof(fileInfo);
		}
		S32 writeSize = infoSize;
		if (fwrite(pInfo, writeSize, 1, m_pRWFile) != 1)
			break;

		m_offset.QuadPart += writeSize;
		if (m_bEXEFileBk)
		{
			m_offset.QuadPart += m_exeFileLen;
		}
		bFailed = false;
	} while (0);

	if (bFailed && m_pIndex != NULL)
	{
		delete[] m_pIndex;
		m_pIndex = NULL;
	}
	return !bFailed;
}

bool PSWriter::PushPacket(S8 *packet, S32 len)
{
	PES_PACKET_HEADER *pHeader = (PES_PACKET_HEADER*)packet;
	do
	{
		if (pHeader == NULL || len <= sizeof(PES_PACKET_HEADER))
		{
			break;
		}

		if (pHeader->channel_id >= DVS_MAX_SOLO_CHANNEL_16)
		{
			break;
		}

		if (m_bFirstWrite)
		{
			m_startTime   = pHeader->ltime;
			m_bFirstWrite = false;
		}
		m_lastTime = pHeader->ltime;
		if (m_bFirstPacket)
		{
			m_blockStartTime = pHeader->ltime;
			m_bFirstPacket   = false;
			m_indexNum       = 0;
			memset(&m_blockInfo, 0, sizeof(m_blockInfo));
		}
		m_blockInfo.BlockSize                    += len;
		m_blockInfo.recSize[pHeader->channel_id] += len;

		if (pHeader->av_type == DT_VIDEO && pHeader->avInfo.VideoInfo.vop_type == 0)
		{
			if (m_pIndex != NULL && m_indexNum < MAX_INDEX)
			{
				//保存索引号
				m_pIndex[m_indexNum].ch_type = 0;
				m_pIndex[m_indexNum].L_offset = (UNS32)(m_offset.QuadPart & 0xffffffff);
				m_pIndex[m_indexNum].H_offset = (UNS8)((m_offset.QuadPart >> 32) & 0xff);
				m_pIndex[m_indexNum].time = pHeader->ltime;
				m_indexNum++;
			}
		}

		//保存数据包
		m_indexSection.Lock();
		if (fwrite(packet, len, 1, m_pRWFile) != 1)
			break;
		m_indexSection.Unlock();

		m_offset.QuadPart += len;
		if (pHeader->ltime >= m_blockStartTime + 60)
		{
			//保存块信息
			SaveIndex();
			m_bFirstPacket = true;
			m_indexNum = 0;
		}

		return true;
	} while (0);

	return false;
}

void PSWriter::Destroy(bool bDelFile)
{
	if (m_pRWFile != NULL)
	{
		SaveIndex();
		fclose(m_pRWFile);
		m_pRWFile = NULL;
		m_indexNum = 0;
		m_bFirstPacket = true;
	}

	if (m_pIndex != NULL)
	{
		delete[] m_pIndex;
		m_pIndex = NULL;
	}
	
	m_pRWFile = NULL;
	m_bFirstPacket = true;
	m_blockStartTime = 0;
	m_pIndex = NULL;
	m_indexNum = 0;
	m_totalIndex = 0;
	m_offset.QuadPart = 0;
	m_bEXEFileBk = false;
	m_exeFileLen = 0;
	m_iRecordChnnel = 0;
	
	if (bDelFile)
	{
		unlink(m_cFileName);
	}
}

bool PSWriter::SaveIndex()
{
	S32 writeSize = 0;
	do
	{
		if (m_pRWFile == NULL)
		{
			break;
		}

		if (m_indexNum < 1)
		{
			break;
		}

		PES_PACKET_HEADER header;
		memset(&header, 0, sizeof(header));
		header.channel_id	= 0x00525549;
		header.av_type		= 0xff;
		header.len			= m_indexNum * sizeof(PS_INDEX_RECORD) + sizeof(BlockInfo);
		writeSize			= sizeof(PES_PACKET_HEADER);
		m_indexSection.Lock();
		if (fwrite(&header, writeSize, 1, m_pRWFile) != 1)//写包头
			break;
		
		writeSize = m_indexNum * sizeof(PS_INDEX_RECORD);
		if (fwrite(m_pIndex, writeSize, 1, m_pRWFile) != 1)//写索引信息
			break;
		
		for (S32 i=0; i<DVS_MAX_SOLO_CHANNEL_16; i++)
		{
			m_blockInfo.recSize[i] = m_blockInfo.recSize[i]/1024;
		}
		m_blockInfo.BlockSize += m_indexNum * sizeof(PS_INDEX_RECORD) + sizeof(PES_PACKET_HEADER) + sizeof(BlockInfo);
		m_blockInfo.endMagic   = 0x0058554E;
		m_blockInfo.indexCount = m_indexNum;
		writeSize = sizeof(m_blockInfo);
		if (fwrite(&m_blockInfo, writeSize, 1, m_pRWFile) != 1)//写数据块信息
			break;
		
		m_offset.QuadPart += m_indexNum * sizeof(PS_INDEX_RECORD) + sizeof(PES_PACKET_HEADER) + sizeof(BlockInfo);
		m_totalIndex += m_indexNum;
		SL32 moveTo = 0;
		if (m_bEXEFileBk)
		{
			moveTo = m_exeFileLen;
		}
		if (fseek(m_pRWFile, moveTo, SEEK_SET) != 0)
			break;

		PS_FILE_HDR fileInfo;
		S32 readSize = sizeof(PS_FILE_HDR);
		fread(&fileInfo, readSize, 1, m_pRWFile);

		fileInfo.TotalIndex = m_totalIndex;
		fileInfo.EffectiveOffset = m_offset.QuadPart;
		if (fseek(m_pRWFile, moveTo, SEEK_SET) != 0)
			break;

		writeSize = sizeof(fileInfo);
		if (m_bRealStream)
		{
			fileInfo.TimeStart = m_startTime;
		}else
		{
			if (m_iRecordChnnel != 0)
			{
				UNS32 chnnlMask = 0;
				for (S32 i=0; i<MAX_SOLO_CHANNEL_16; i++)
				{
					if (!(m_iRecordChnnel & (1<<i) & fileInfo.RecordChannel))
					{
						for (S32 j=0; j<MAX_MIN_IN_HOUR; j++)
						{
							fileInfo.RecMode[i][j] = REC_OFF;
						}
					}else
					{
						chnnlMask |= (1<<i);
					}
				}
				fileInfo.RecordChannel = chnnlMask;
			}
		}
		fileInfo.TimeEnd = m_lastTime;
		if (fwrite(&fileInfo, writeSize, 1, m_pRWFile) != 1)//更新索引信息
			break;

		moveTo = m_offset.QuadPart;
		if (fseek(m_pRWFile, moveTo, SEEK_SET) != 0)
			break;

		m_indexSection.Unlock();
		return true;
	} while (0);

	m_indexSection.Unlock();
	return false;
}
