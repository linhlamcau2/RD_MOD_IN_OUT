#include "rd_meshdata.h"
#include "../rd_queue.h"

typedef struct
{
	u16 op_code_rsp;
	u8 par[8];
	u16 par_len;
	u16 addr_dst;
	u16 addr_src;
}rd_rsp_mesh_t;

#define NUM_MAX_QUEUE_RSP			20
#define SIZE_QUEUE_RSP				sizeof(rd_rsp_mesh_t)
#define TIME_WAITING_MS				200

rd_queue_t rd_queue_rsp;
rd_rsp_mesh_t rd_rsp_mesh[NUM_MAX_QUEUE_RSP];

void rd_init_queue_rsp()
{
	rd_initQueue(&rd_queue_rsp,NUM_MAX_QUEUE_RSP,SIZE_QUEUE_RSP,(void *)rd_rsp_mesh);
}

void rd_call_tx(u16 op_code, u8 *par, u16 par_len, u16 addr_dst)
{
	rd_rsp_mesh_t mess_tx;
	mess_tx.op_code_rsp = op_code;
	if(par_len <=8)
	{
		memset((void *)mess_tx.par,0,sizeof(mess_tx.par));
		memcpy((void *)mess_tx.par,(void *)par,par_len);
		mess_tx.par_len = par_len;
		mess_tx.addr_dst = addr_dst;
		rd_enqueue(&rd_queue_rsp,(void *)&mess_tx);
		RD_ev_log("rd_call_tx 0\n");
	}
//	memcpy((void *)mess_tx.par,(void *)par,par_len);
}

void rd_call_tx2(u16 op_code, u8 *par, u16 par_len, u16 addr_dst, u16 addr_src)
{
	rd_rsp_mesh_t mess_tx;
	mess_tx.op_code_rsp = op_code;
	memcpy((void *)mess_tx.par,(void *)par,par_len);
	mess_tx.par_len = par_len;
	mess_tx.addr_dst = addr_dst;
	mess_tx.addr_src = addr_src;
	rd_enqueue(&rd_queue_rsp,(void *)&mess_tx);
}

static void rd_rsp()
{
	rd_rsp_mesh_t mess_tx;
	int err = rd_dequeue(&rd_queue_rsp,(void *)&mess_tx);
	if(err != -1)
	{
		RD_ev_log("rd_rsp\n");
		int rsp_max = (mess_tx.op_code_rsp == SCENE_RECALL_NOACK) ? 0: RD_MAXRESPONESEND;
		if(mess_tx.op_code_rsp == G_ONOFF_STATUS)
		{
			mesh_tx_cmd2normal(mess_tx.op_code_rsp, mess_tx.par, 8, mess_tx.addr_src, mess_tx.addr_dst, rsp_max);
		}
		else
		{
			mesh_tx_cmd2normal_primary(mess_tx.op_code_rsp, mess_tx.par, 8, mess_tx.addr_dst, rsp_max);
		}
	}
}

void rd_handle_tx()
{
	static u32 last_time_rsp = 0;
	if(clock_time_ms() - last_time_rsp > TIME_WAITING_MS)
	{
		rd_rsp();
		last_time_rsp = clock_time_ms();
	}
}



