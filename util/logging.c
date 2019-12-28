#include <types.h>
#include <kio.h>
#include "logging.h"

void logging(const char * fmt,...)
{
	char log_buf[1024];
	va_list args;

	va_start(args, fmt);
	vsprintf(log_buf, fmt, args);
	va_end(args);

	show_string(log_buf);
}
