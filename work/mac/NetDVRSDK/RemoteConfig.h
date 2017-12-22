#pragma once
#ifndef _REMOTECONFIG_H_
#define _REMOTECONFIG_H_

#include "DVRSocket.h"

class CRemoteConfig
{
public:
	CRemoteConfig();
	~CRemoteConfig();

	void SetDVRSocket(DVRSocket *pDVRSocket);
	bool GetRecordfileDateInfo(NET_SYS_DB_DATE_INFO *pRecordfileDateInfo);
	bool GetDayRecordfileInfo(NET_SYS_DB_DAY_FILE *pDayRecordfileInfo, UNS32 date);
	UNS32 GetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size);
	UNS32 SetDvrConfig(UNS32 cmdId, LPVOID pCfg, UNS32 size);
	void GetUserRight(LPVOID pCfg, S32 size);//获取用户权限
	void GetLinkNum(LPVOID pCfg, S32 size);//获取网络资源连接数
	UNS32 RequestEmailTest();//请求DVR发送测试邮件
	bool CheckStructSize(UNS32 cmdId, S32 size, UNS32 iVersion);//检测结构体长度是否与版本相符，符合返回false，不符合返回true

private:
	DVRSocket *m_pDVRSocket;
};

#endif
