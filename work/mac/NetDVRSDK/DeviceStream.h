// DeviceStream.h: interface for the CDeviceStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICESTREAM_H__9C93A77B_AE42_4CCF_87BF_4B6B5C6FF902__INCLUDED_)
#define AFX_DEVICESTREAM_H__9C93A77B_AE42_4CCF_87BF_4B6B5C6FF902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DVRSocket.h"
#include "PSWriter.h"
#include "StreamSocket.h"
#include "PESPacketHead.h"
typedef void (*process_cb_ex)(void* param, void* data, int len, int dropFrame, int fps);

typedef struct
{
	process_cb_ex	cbRealVideoFunc;
    void*			dwUser;
}CbRealVideoStruct;


class CDeviceStream
{
public:
	CDeviceStream();
	virtual ~CDeviceStream();

	//现场相关开启、关闭视频相关
	bool OpenRealStream(S8 *destIP, UNS16 port, UNS8 chnnl, UNS8 biAudio, bool bSubStream = true);
	bool PlayRealStream();
	void StopRealStream();
	void CloseRealStream();
	
	//文件存储相关
	S32  SaveRealData(S8 *sFileName);
	void StopRealData();
	
	//图片信息相关
	void SaveRealData(S8 *ps, UNS32 len);
	bool IsRecording();
	
	void SetUserInfo(UNS32 userId, UNS32 accessId);
	bool GetDomainIP(S8 *pDomain, S8 *ip, bool bFromIP = true);
	UNS8 GetChnnlNum() { return m_chnnl; }
	UNS8 GetBiAudio() { return m_biAudio; }
	void SetSignalType(UNS8 signalType) { m_videoSignalType = signalType; }
	void SetChnnlName(S8 *chnnlName);
	void SetVideoResolusion(UNS8 videoRes) { m_videoRes = videoRes; }
	void RenderTask();//显示任务相关
	void SetRealVideoCallBack(process_cb_ex cbRealVideoFunc, void* dwUser);

private:
	//创建音视频数据接收相关
	bool CreateStreamer(UNSL32 ip, UNS16 port);
	void DestroyStreamer();
	bool ChangeStream(UNS8 chnnl, MODE_PLAY playMode);
	void ReConnect();
	void GenerateFileInfo(S32 chnnl, PS_FILE_HDR &fileHdr);

private:
	S8	m_chnnlName[DVS_MAX_PTZ_NAME_LEN + 1];
	S8	m_cDvrAddr[DVS_MAX_DOMAINLEN + 1];
	
	PSWriter		*m_pRealWriter;	
	StreamSocket	*m_pStreamer;
	StreamFifo		*m_pFifo;
	CMyCriticalSection		m_writeSection;
	MyThread		m_hVideoThread;

	bool	m_bExit;
	bool	m_bFindIFrame;
	bool    m_bSaveRealData;
	bool	m_bSubStream;
	
	UNS8 m_chnnl;
	UNS8 m_biAudio;
	UNS8 m_videoRes;
	UNS8 m_videoSignalType; //视频输入信号类型，N制或P制		
	UNS16 m_port;
	UNS32 m_userId;
	UNS32 m_accessId;

	CbRealVideoStruct m_cbRealVideoData;
};

#endif // !defined(AFX_DEVICESTREAM_H__9C93A77B_AE42_4CCF_87BF_4B6B5C6FF902__INCLUDED_)
