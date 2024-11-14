#include "rd_meshdata.h"
#include "../rd_queue.h"

typedef struct
{
	u16 op_code_rsp;
	u8 *par;
	u16 par_len;
	u16 addr_dst;
}rd_rsp_mesh_t;

#define NUM_MAX_QUEUE_RSP			20
#define SIZE_QUEUE_RSP				sizeof(rd_rsp_mesh_t)

rd_queue_t rd_queue_rsp;
rd_rsp_mesh_t rd_rsp_mesh[NUM_MAX_QUEUE_RSP];

void rd_init_queue_rsp()
{
	rd_initQueue(rd_queue_rsp,NUM_MAX_QUEUE_RSP,SIZE_QUEUE_RSP,(void *)rd_rsp_mesh);
}

void rd_call_tx(u16 op_code, u8 *par, u16 par_len, u16 addr_dst)
{
	rd_rsp_mesh_t mess_tx;
	mess_tx.op_code_rsp = op_code;
	memcpy((void *)mess_tx.par,(void *)par,par_len);
	mess_tx.par_len = par_len;
	mess_tx.addr_dst = addr_dst;
	rd_enqueue(rd_queue_rsp,(void *)&mess_tx);
}

static void rd_rsp()
{
	rd_rsp_mesh_t mess_tx;
	int err = rd_dequeue(rd_queue_rsp,(void *)&mess_tx);
	if(err != -1)
	{
		mesh_tx_cmd2normal_primary(mess_tx.op_code_rsp, mess_tx.par, 8, mess_tx.addr_dst, RD_MAXRESPONESEND);
	}
}

void rd_handle_tx()
{
	static u32 last_time_rsp = 0;
	if(clock_time_ms() - last_time_rsp > 500)
	{
		rd_rsp();
		last_time_rsp = clock_time_ms();
	}
}



