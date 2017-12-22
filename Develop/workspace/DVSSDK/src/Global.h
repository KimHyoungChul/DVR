#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>

//�����̲���
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
//#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
//�û�Ȩ�޼�������֤����
//#include <shadow.h>
//#include <crypt.h>
//#include <pwd.h> 
//�ļ���ʱ�����
#include <sys/time.h>
#include <utime.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>
//����̲���
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
//���̲߳���
#include <pthread.h>
#include <sys/poll.h>
#include <semaphore.h>

#define SOCKET								unsigned int
#define INVALID_SOCKET						(SOCKET)(~0)
#define SOCKET_ERROR						(-1)
#define WAIT_OBJECT_0						((UNSL32)0x00000000)
#define STACK_SIZE_PARAM_IS_A_RESERVATION	0x00010000

#define WSAGetLastError()					errno
#define closesocket(a)						close(a)
#define ioctlsocket							ioctl
#define Sleep(a)							usleep(a*1000)


#pragma warning(disable: 4996)

#include "VersionDefine.h"
#include "DvssNet.h"

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)					\
	((UNS32)(UNS8)(ch0) | ((UNS32)(UNS8)(ch1) << 8) |   \
	((UNS32)(UNS8)(ch2) << 16) | ((UNS32)(UNS8)(ch3) << 24))
#endif //defined(MAKEFOURCC)

#define MAGIC_START MAKEFOURCC('K', 'T', 'S', 'Y')
#define MAGIC_END   MAKEFOURCC('k', 't', 's', 'y')
#define MAX_CMD_LEN		128
#define MAX_PATH        260

typedef UNSL32 (__attribute__((stdcall)) *LPThreadStartAddr)(LPVOID lpThreadParameter);

struct KTCMDHDR
{
	UNSL32 startCode;
	UNS32 totalSize;//�����ݰ��Ĵ�С ��λ��Byte
	UNS32 idCode;	//����ϵͳ���������û��ı�ʶ��Ҳ������������
	UNS32 infoType;
};

bool InitSocket();
UNS32 PackCommand(void *pOpts, UNS32 optlen, S32 type, S8 *cmd, UNS32 cmdlen, UNS32 codeID);

/*added by wuzy 12.4.18*/
unsigned int GetTickCount();
char *strupr(char *str);
/*
#if defined(__linux__)
#define _itoa   itoa

char* itoa(int value, char*  str, int radix)
{
    int  rem = 0;
    int  pos = 0;
    char ch  = ''!'' ;
    do
    {
        rem    = value % radix ;
        value /= radix;
        if ( 16 == radix )
        {
            if( rem >= 10 && rem <= 15 )
            {
                switch( rem )
                {
                    case 10:
                        ch = ''a'' ;
                        break;
                    case 11:
                        ch =''b'' ;
                        break;
                    case 12:
                        ch = ''c'' ;
                        break;
                    case 13:
                        ch =''d'' ;
                        break;
                    case 14:
                        ch = ''e'' ;
                        break;
                    case 15:
                        ch =''f'' ;
                        break;
                }
            }
        }
        if( ''!'' == ch )
        {
            str[pos++] = (char) ( rem + 0x30 );
        }
        else
        {
            str[pos++] = ch ;
        }
    }while( value != 0 );
    str[pos] = ''\0'' ;
    return strrev(str);
}

#endif


//
// strrev ��׼��
//

#if !defined(__linux__)
#define __strrev strrev
#endif

char* strrev(char* szT)
{
    if ( !szT )                 // ������Ŀմ�.
        return "";
    int i = strlen(szT);
    int t = !(i%2)? 1 : 0;      // ��鴮����.
    for(int j = i-1 , k = 0 ; j > (i/2 -t) ; j-- )
    {
        char ch  = szT[j];
        szT[j]   = szT[k];
        szT[k++] = ch;
    }
    return szT;
}

void Sleep(unsigned int useconds )
{
    // 1 ���루milisecond�� = 1000 ΢�� ��microsecond��.
    // Windows �� Sleep ʹ�ú��루miliseconds��
    // Linux �� usleep ʹ��΢�루microsecond��
    // ����ԭ���Ĵ������� Windows ��ʹ�õģ����Բ���Ҫ��һ�����뵽΢���ת����
    usleep( useconds * 1000 );
}
*/

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;



typedef   union   _LARGE_INTEGER   {    
          struct   { 
                  DWORD   LowPart;    
                  long     HighPart;    
          }; 
          long long   QuadPart; 
}  	LARGE_INTEGER; 

#endif
