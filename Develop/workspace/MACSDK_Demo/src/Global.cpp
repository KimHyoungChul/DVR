#include "Global.h"

bool InitSocket()
{
/*	UNS16 wVersionRequested = MAKEWORD(2,0);
	WSADATA wsaData;
	S32 nErr = WSAStartup(wVersionRequested,&wsaData);
	if(nErr != 0)
	{
		return false;
	}

	if(LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 0)
	{
		return false;
	}
*/
	return true;
}

UNS32 PackCommand(void *pOpts, UNS32 optlen, S32 type, S8 *cmd, UNS32 cmdlen, UNS32 codeID)
{
	KTCMDHDR *pHdr		= (KTCMDHDR*)cmd;
	S8 *pData			= cmd + sizeof(KTCMDHDR);
	UNS32 *pEndMagic	= (UNS32*)(cmd + sizeof(KTCMDHDR) + optlen);

	do
	{
		if (cmd == 0 || cmdlen < sizeof(KTCMDHDR))
		{
			break;
		}

		pHdr->totalSize = optlen + sizeof(KTCMDHDR) + sizeof(UNS32);
		if (pHdr->totalSize > cmdlen)
		{
			break;
		}

		pHdr->startCode = MAGIC_START;
		pHdr->idCode    = codeID;
		pHdr->infoType  = type;

		memcpy(pData, pOpts, optlen);
		*pEndMagic = MAGIC_END;

		return pHdr->totalSize;
	} while (0);

	return 0;
}

unsigned int GetTickCount()
{
	struct timeval tv;
	if(gettimeofday(&tv,NULL) != 0)
		return 0;
		
	return (tv.tv_sec*1000) + (tv.tv_usec/1000);
}

char *strupr(char *str)
{
      char *ptr = str;
      while(*ptr != '\0')   
      {
          if(islower(*ptr))  
              *ptr = toupper(*ptr);
              ptr++;
      }

      return   str;
}
