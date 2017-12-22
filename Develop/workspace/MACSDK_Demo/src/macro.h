#ifdef WIN32
#if _MSC_VER <1400        //vc2003
#define _DO_NOT_HAVE_S   //这个宏表示没有_s版，就是vc2003或者linux
#else
#endif    //VC2003
#else    //LINUX
#define _DO_NOT_HAVE_S
#endif    //WIN32

#ifdef _DO_NOT_HAVE_S    //如果没有_s版的函数，则启动如下的替换宏
//#define sprintf_s(buffer,size,format, ...) sprintf(buffer,format,__VA_ARGS__)
#include <stdio.h>
#include <stdarg.h>

int sprintf_s(char *buffer,size_t sizeOfBuffer=0,const char *format=NULL ,...); //这个比较特殊，需要真的去实现

#define memcpy_s(dest,num,src,count) memcpy(dest,src,count)
#define fprintf_s fprintf
#define _strdate_s(buf,num) _strdate(buf)
#define strcat_s(dest,num,src) strcat(dest,src)
#define fopen_s(pf,name,mode) *pf=fopen(name,mode)
#define strncpy_s(dest,num,src,count) strncpy(dest,src,count)
#define localtime_s(tm,time) *tm=*localtime(time)
#define _strdup strdup

#else
#define mkdir(pathname,mode) _mkdir(pathname)

#endif
