#include <sys/time.h>
#include "up_sys_config.h"
#include "up_types.h"
#include "up_sys_log.h"
#include "ucm.h"

#include "up_sys_util.h"

static UPSint8* m_up_uniqueIDStr = UP_NULL;
static unsigned int m_up_sys_currTick_baseline = 0;

static UPUint32 m_up_iphone_countTick(void)
{
	struct timeval time_val;
	struct timezone junk;

	unsigned short al,bl,ah;
	unsigned int tmp,totalMillisecondsHi,totalMillisecondsLo;

	gettimeofday(&time_val, &junk);

    al = (unsigned short)time_val.tv_sec;
    bl = (unsigned short)1000;
    ah = (unsigned short)(time_val.tv_sec >> 16);    
   
	totalMillisecondsLo = al * bl;	
   
    tmp = ah * bl;				
    totalMillisecondsHi = tmp >> 16; 
    tmp <<= 16;
    totalMillisecondsLo += tmp;
    if (totalMillisecondsLo < tmp)
        ++totalMillisecondsHi; 
    totalMillisecondsLo += time_val.tv_usec / 1000;
    totalMillisecondsHi += (totalMillisecondsLo < time_val.tv_usec / 1000);
	
  	return (UPUint32)(totalMillisecondsLo&0xffffffff);
}

void up_util_init(void* appInstance)
{
	m_up_sys_currTick_baseline = m_up_iphone_countTick();
}

void up_util_fini(void)
{
	if(m_up_uniqueIDStr) ucm_free(UP_NULL, m_up_uniqueIDStr);
	m_up_uniqueIDStr = UP_NULL;
}

UPUint up_util_getCurrentTick(void)
{
	return (m_up_iphone_countTick() - m_up_sys_currTick_baseline);
}

UPSint up_util_s2i(UPSint8* str)
{
	return (UPSint)atoi((const char*)str);
}

int util_str_isnumber(char c)
{
	if(c >= '0' && c <= '9')
		return 1;
	
	return 0;
}

UPResult util_str_isIPAddress(UPUint8* src, UPSint length)
{
	UPUint8* strPos = NULL;
	UPUint8 ipToken[5] = {0};
	UPSint i = 0, j = 0;
	UPResult ret = 0;
	UPSint counter = 0;
	
	if(!src || !length)
	{
		return 0;
	}
	
	strPos = src;
	for(i = 0, j = 0; i < length; i++)
	{
		if(strPos[i] == '.')
		{
			j = 0;
			memset(ipToken, 0, sizeof(ipToken));
			counter++;
		}
		else
		{
			if(util_str_isnumber(strPos[i]))
			{
				if(j > 4)
				{
					counter = 0;
					break;
				}
				ipToken[j] = strPos[i];
				j++;
			}
			else
			{
				counter = 0;
				break;
			}
		}
	}
	
	if(counter == 3)
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	
	return ret;
}
