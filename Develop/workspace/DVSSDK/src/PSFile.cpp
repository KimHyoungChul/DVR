// PSFile.cpp: implementation of the CPSFile class.
//
//////////////////////////////////////////////////////////////////////

#include "PSFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPSFile::CPSFile()
{
	m_psFileHead = new PS_FILE_HDR;
	memset(m_psFileHead, 0, sizeof(PS_FILE_HDR));//初始化
	for (S32 i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		FindIFrame[i] = false;			
	}
	m_pLocalFile = NULL;
	m_PSFileStartOffset = 0;
	m_CurIndexItemNum = 0;
	m_psIndexRecord = NULL;
	memset(m_biAudio, 0xff, sizeof(m_biAudio));
	memset(m_PSFileName, 0, sizeof(m_PSFileName));
}

CPSFile::~CPSFile()
{
	if (m_psFileHead != NULL)
	{
		delete m_psFileHead;
		m_psFileHead = NULL;
	}
}

UNS8 CPSFile::GetBiAudio(UNS8 chnnl)
{
	if (chnnl >= MAX_SOLO_CHANNEL_16)
	{
		return 0xff;
	}

	return m_biAudio[chnnl];
}

#define READ_EXEFILE_INDEX_LEN	(2 * 1024 * 1024)
bool CPSFile::OpenPSFile(S8 *filename)
{
	if (strlen(filename) > MAX_PATH)
	{
		return false;
	}

	strncpy(m_PSFileName, filename, strlen(filename));
	//判断是什么类型的文件
	//此处主要是判断一下该文件前是否加了EXE程序
	m_PSFileStartOffset = 0;
	m_pLocalFile = fopen(filename, "rb");
	if (NULL == m_pLocalFile)
	{
		return false;
	}
	
	if (strlen(filename) > 3) 
	{
		if (strcmp(strupr(filename + strlen(filename) - 3), "EXE") == 0)
		{
			S8* tempbuf = (S8*)malloc(READ_EXEFILE_INDEX_LEN);
			if (tempbuf == NULL)
			{
				return false;
			}

			S32 curpos = 0;
			fseek(m_pLocalFile, 0, SEEK_SET);
			//读取一个索引区的内容
			fread(tempbuf, READ_EXEFILE_INDEX_LEN, 1, m_pLocalFile);
			
			for (curpos = 0; curpos < READ_EXEFILE_INDEX_LEN - 16; curpos++)
			{
				if (strncmp(tempbuf + curpos, EXE_FILE_MAGIC, 16) == 0)
				{
					m_PSFileStartOffset = curpos + 16;
					break;
				}
			}

			free(tempbuf);
			tempbuf = NULL;

			if (curpos == READ_EXEFILE_INDEX_LEN - 16)
			{
				return false;
			}
		}
		else 
		{
			m_PSFileStartOffset = 0;
		}
	}
	
	//从文件中读取文件头，同时对文件头中要用到的参数进行字节顺序的处理，
	//同时包括对文件时间的处理
	ReadPSFileHead(m_psFileHead);

	//判断是不是PS文件
	if (PSFILE_KEY != m_psFileHead->FileID)
	{
		return false;
	}
    
	m_psIndexRecord = new PS_INDEX_RECORD[m_psFileHead->TotalIndex];
	//获得文件的索引表，并对整个索引表的起始时间在出现意外时进行处理
	if (m_psIndexRecord == NULL || ParsePSFile() != true) 
	{
		return false;
	}

    //将当前索引号指向第一个包，并读取其包头信息
	if (MoveFirstPESPack() != true) 
	{
		return false;
	}
	
	return true;
}

void CPSFile::ClosePSFile()
{
	if (m_psIndexRecord != NULL) 
	{
		delete []m_psIndexRecord;
		m_psIndexRecord = NULL;
	}

	if (m_pLocalFile != NULL)
	{
		fclose(m_pLocalFile);
		m_pLocalFile = NULL;
	}

	memset(m_PSFileName, 0, sizeof(m_PSFileName));
}

//解析ps文件
//构造索引
bool CPSFile::ParsePSFile()
{
	/*索引区有关的变量*/
	UNS32 StartMagicalword = 0;					//索引区起始标志 "RUI"
	UNS32 ChRecSize[MAX_SOLO_CHANNEL_16] = {0,};	//每个通道已经保存的数据尺寸,单位是KB
	UNS32 IndexCount = 0;							//本块索引条数
	UNS32 BlockSize = 0;							//本块大小，数据区和索引区的总大小
	UNS32 EndMagicalword = 0;						//索引区结束标志 "XUN"
	//获取最后一个索引区的最后一个位置
	UNS64 blockOffset = m_psFileHead->EffectiveOffset;
	UNS32 indexBlockTailSize = (sizeof(ChRecSize) + sizeof(IndexCount) + sizeof(BlockSize) + sizeof(EndMagicalword));
	//用于标记当前正在抽取的索引号
	UNS32 curIndexItemNum = m_psFileHead->TotalIndex;	
	if (blockOffset < indexBlockTailSize || m_psFileHead->TotalIndex <= 0) 
	{
		return false;
	}

	//开始抽取pes包的索引，注意是反向抽取，从最后一个索引开始提取
	while (blockOffset > (sizeof(PS_FILE_HDR) + m_PSFileStartOffset)) 
	{
		//读索引块尾部固定大小
		//将偏移位置指向指向索引区第三部分起始位置
		//这里指的是ChRecSize[0]的位置，对照文档
		UNS64 tmpOffset = blockOffset - indexBlockTailSize;
		fseek(m_pLocalFile, tmpOffset, SEEK_SET);
		//读取一个索引区的内容
		fread(ChRecSize, sizeof(ChRecSize), 1, m_pLocalFile);
		fread(&IndexCount,sizeof(IndexCount), 1, m_pLocalFile);
		fread(&BlockSize,sizeof(BlockSize), 1, m_pLocalFile);
		fread(&EndMagicalword,sizeof(EndMagicalword), 1, m_pLocalFile);
		if (EndMagicalword != 0x0058554E)
		{// XUN
			return false;
		}
		//读首魔术字和索引块,根据新的文件架构
		tmpOffset = blockOffset - (sizeof(PES_PACKET_HEADER) + IndexCount * sizeof(PS_INDEX_RECORD) + indexBlockTailSize);
		fseek(m_pLocalFile, tmpOffset, SEEK_SET);
		fread(&StartMagicalword,sizeof(StartMagicalword), 1, m_pLocalFile);
		if (StartMagicalword != 0x00525549) 
		{// RUI
			return false;
		}
		//获取每次需要读取索引区的起始索引号
		curIndexItemNum -= IndexCount;
		tmpOffset = tmpOffset + sizeof(PES_PACKET_HEADER);

		fseek(m_pLocalFile, tmpOffset, SEEK_SET);
		fread(m_psIndexRecord + curIndexItemNum,sizeof(PS_INDEX_RECORD) * IndexCount, 1, m_pLocalFile);

        //转移到上一个索引区抽取索引
		blockOffset -= BlockSize;
	}
  
	return true;
}

void CPSFile::ReadPSFileHead(PS_FILE_HDR* psFileHead)
{
	fseek(m_pLocalFile, m_PSFileStartOffset, SEEK_SET); 
	fread((PS_FILE_HDR*)psFileHead,sizeof(PS_FILE_HDR), 1, m_pLocalFile);
	
	//用来统计有录像的通道
	for (S32 i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		for (S32 j=0; j<MAX_MIN_IN_HOUR; j++)
		{
			if (psFileHead->RecMode[i][j] != REC_OFF)
			{
				//录像通道根据读取的RecordChannel和每分钟各通道的录像模式两项判断
				psFileHead->RecordChannel = psFileHead->RecordChannel|(0x0001<<i);
				break;
			}
		}
	}
}

bool CPSFile::ReadPESPack()
{
	fseek(m_pLocalFile, m_PsFileOffset, SEEK_SET);
	//读取包头
	fread(m_pBuf,sizeof(PES_PACKET_HEADER), 1, m_pLocalFile);
	m_pesPackHead.SetPESBuf(m_pBuf);
	if (m_pesPackHead.GetPESLength() > MAX_PLAYBACK_BUF_SIZE)
	{
		return false;
	}
	
	fread(m_pBuf, m_pesPackHead.GetPESLength(),1, m_pLocalFile);
	m_PsFileOffset = m_PsFileOffset + m_pesPackHead.GetPESLength() + sizeof(PES_PACKET_HEADER);
	if (m_pesPackHead.GetAVType() == VIDEO_FRAME)//可能的数据类型：音频、视频、索引
	{
		UNS32 chnnl = m_pesPackHead.GetChnID();
		if (chnnl >= MAX_SOLO_CHANNEL_16)
		{
			return false;
		}

		m_biAudio[chnnl] = m_pesPackHead.GetBiAudio();
		if (m_biAudio[chnnl] == 0x00)
		{
			m_biAudio[chnnl] = 0xff;
		}else
		{
			m_biAudio[chnnl] -= 1;
		}
	}

	return true;
}

bool CPSFile::MoveFirstPESPack()
{
/*	m_Section.Lock();
	m_CurIndexItemNum = 0;
    m_PsFileOffset = (((UNS64)m_psIndexRecord[m_CurIndexItemNum].H_offset<<32) | m_psIndexRecord[m_CurIndexItemNum].L_offset);
	//读取其包头信息
	if (ReadPESPack() == false) 
	{
		m_Section.Unlock();
		return false;
	}

	for (S32 i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		FindIFrame[i] = false;	
	}
	m_Section.Unlock();
*/
	return true;
}

bool CPSFile::MoveFirstIPESPack()
{
	if (!MoveFirstPESPack()) 
	{
		return false;
	}
	while (GetVopType() != I_PICTURE)
	{
		if (!MoveNextPESPack()) 
		{
			return false;
		}
	}
    return true;
}

bool CPSFile::MoveNextPESPack()
{
	m_Section.Lock();
	if (IsEof()) 
	{
		m_CurIndexItemNum = m_psFileHead->TotalIndex - 1;
	}

	if (ReadPESPack() == false) 
	{
		m_Section.Unlock();
		return false;
	}

	if (m_pesPackHead.GetAVType() == VIDEO_FRAME) 
	{
		if (m_pesPackHead.GetVopType() == 0)//I帧
		{
			m_CurIndexItemNum++;
		}
	}
	m_Section.Unlock();

	return true;
}

bool CPSFile::MoveNextIPESPack()
{
/*	while(!IsEof())
	{
		m_Section.Lock();
        m_CurIndexItemNum++;
		if (m_CurIndexItemNum < 0 || m_CurIndexItemNum > m_psFileHead->TotalIndex)
		{
			break;
		}
        if (I_PICTURE == ((m_psIndexRecord[m_CurIndexItemNum].ch_type)&0x30)>>4)
		{
            m_PsFileOffset = (((UNS64)m_psIndexRecord[m_CurIndexItemNum].H_offset<<32) | m_psIndexRecord[m_CurIndexItemNum].L_offset);						
			if (ReadPESPack() == false) 
			{
				break;
			}
			m_Section.Unlock();
			return true;
        }
		m_Section.Unlock();
	}

	m_Section.Unlock();*/
	return false;
}

bool CPSFile::MovePreIPESPack()
{
/*	while(!IsFirstPESPack())
	{
		m_Section.Lock();
        m_CurIndexItemNum--;
		if (m_CurIndexItemNum < 0 || m_CurIndexItemNum > m_psFileHead->TotalIndex)
		{
			break;
		}
        if (I_PICTURE == ((m_psIndexRecord[m_CurIndexItemNum].ch_type)&0x30)>>4) 
		{
            m_PsFileOffset = (((UNS64)m_psIndexRecord[m_CurIndexItemNum].H_offset<<32) | m_psIndexRecord[m_CurIndexItemNum].L_offset);						
			if (ReadPESPack() == false) 
			{
				break;
			}
			m_Section.Unlock();
			return true;
        }
		m_Section.Unlock();
	}

	m_Section.Unlock();*/
	return false;
}

void CPSFile::MovePsfileOffset(UNS32 index)
{
	if (index >= m_psFileHead->TotalIndex)
	{
		return;
	}

	m_Section.Lock();
	//m_PsFileOffset = (((UNS64)m_psIndexRecord[index].H_offset<<32) | m_psIndexRecord[index].L_offset);
	m_Section.Unlock();
}

bool CPSFile::MoveToEx(UNS64 Time)
{
	if (m_psFileHead->TotalIndex < 2)
	{
		return false;
	}
	
	m_Section.Lock();
	m_CurIndexItemNum = GetIndexItemNumByTimeEx(Time);
	if (m_CurIndexItemNum >= m_psFileHead->TotalIndex)
	{
		m_Section.Unlock();
		return false;
	}
	//m_PsFileOffset = (((UNS64)m_psIndexRecord[m_CurIndexItemNum].H_offset<<32) | m_psIndexRecord[m_CurIndexItemNum].L_offset);

	for (S32 i=0; i<MAX_SOLO_CHANNEL_16; i++)
	{
		FindIFrame[i] = false;			
	}

	m_Section.Unlock();
	return true;
}

UNS32 CPSFile::GetPESLength()
{
	return m_pesPackHead.GetPESLength();
}

UNS32 CPSFile::GetChnID()
{
	return m_pesPackHead.GetChnID();
}

S8* CPSFile::GetBuf()
{
	return m_pBuf;
}

UNS32 CPSFile::GetPTS()
{
	return m_pesPackHead.GetPTS();
}

UNS32 CPSFile::GetAVType()
{
	return m_pesPackHead.GetAVType();
}

UNS32 CPSFile::GetVopType()
{
	return m_pesPackHead.GetVopType();
}

//获取时分秒，按顺序存在四个字节的低三字节
UNS32 CPSFile::GetCurTime()
{
	return m_pesPackHead.GetCurTime();
}

UNS32 CPSFile::GetCurDate()
{
	return m_pesPackHead.GetCurDate();
}

UNS32 CPSFile::GetIndexItemNumByTimeEx(UNS64 Time)
{
	UNS32 index     = 0;
	UNS32 headcount = 0;
	UNS32 endcount  = m_psFileHead->TotalIndex - 1;

	do
	{
		if (m_psFileHead->TotalIndex < 2)
		{
			break;
		}

		if (Time <= m_psIndexRecord[0].time)
		{
			break;

		}else if (Time >= m_psIndexRecord[endcount].time)
		{
			index = endcount;
			break;
		}
		
		for (UNS32 i=0; i<m_psFileHead->TotalIndex-1; i++)
		{
			if ((Time >= m_psIndexRecord[i].time) && (Time <= m_psIndexRecord[i+1].time))
			{
				index = i;
				break;
			}
		}
	} while (0);

	return index;
}
