#include "RD_log.h"
#include "tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

static char out[512];

unsigned char uart_CSend(char* data){		//RD_EDIT: uart_Csend
	#if (HCI_ACCESS == HCI_USE_UART)
	while(*data != '\0')
	{
		uart_ndma_send_byte(*(data++));
	}
	#endif
	return 0;
}

void RD_ev_log(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(out, 1024, format, args);
	va_end(args);
//	print(&out, format, args);
	uart_CSend(out);
//	free(out);
}

