#include "up_sys_config.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "up_types.h"
#include "up_sys_log.h"

#define UP_LOG_BUFFER_SIZE  8192

static char m_log_buffer[UP_LOG_BUFFER_SIZE];

void uplog_printout(char* filename, int line, char* func)
{
    int     i;
    int     length          = strlen(filename);
    char*   newFileName     = NULL;
	
    memset(m_log_buffer, 0, UP_LOG_BUFFER_SIZE);

    for ( i = length - 1 ; i >= 0 ; i-- )
    {
        if ( filename[i] == '\\' || filename[i] == '/' ) break;
    }
    newFileName = i < (length - 1) ?
                    ((char *) (filename + i + 1)) :
                    (char *) filename;
	
	sprintf(m_log_buffer, "[%s#%d][%s]", newFileName,line,func);

	{
		printf("%s",m_log_buffer);
	}

}

void uplog_printtext(char* text,...)
{
	va_list ap;

	if(text == UP_NULL) text = "(up_log_warning) the text is NULL";

	if(strlen((const char*)text) >= UP_LOG_BUFFER_SIZE)
	{
		m_log_buffer[0] = '\0';
		strcpy(m_log_buffer, "warning!!! print buffer size is only 8 KB");
	}
	else
	{
	    va_start(ap, text);

	    vsprintf(m_log_buffer, text, ap);

	    va_end(ap);
	}
	
	{
		printf("%s\n",m_log_buffer);
	}

    memset((void*)m_log_buffer, 0, UP_LOG_BUFFER_SIZE);
}

