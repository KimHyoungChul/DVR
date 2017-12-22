#include "Dvs.h"
#include "macro.h"

Dvs::Dvs()
{
	memset(m_cAddr, 0, DVS_MAX_DOMAINLEN + 1);
}

Dvs::~Dvs()
{
}

void Dvs::Logout()
{
	return;
}

void Dvs::SetCmdPort(UNS16 cmdport)
{
	m_cmdPort = cmdport;
}

void Dvs::SetDvrAddr(const S8 *pAddr)
{
	snprintf(m_cAddr, DVS_MAX_DOMAINLEN, "%s", pAddr);
}

const S8* Dvs::GetDvrAddr(bool bFromIP)
{	
	return m_cAddr;
}

UNS16 Dvs::GetCmdPort()
{
	return m_cmdPort;
}
