#include "rd_in_out.h"
#include "../rd_queue.h"


typedef struct
{
	u8 stt;
}relay_handle_t;

#define NUM_MAX_QUEUE_RELAY			10
#define SIZE_QUEUE_HANDLE_LED		sizeof(relay_handle_t)
#define TIME_MIN_HANDLE_RELAY_MS	200

rd_queue_t rd_queue_relay[NUM_OF_RELAY];
relay_handle_t relay_handle[NUM_OF_RELAY][NUM_MAX_QUEUE_RELAY];

uint32_t arr_relay[] = ARR_RELAY;

void rd_call_queue_relay(u8 stt, u8 id_relay)
{
	if(id_relay < NUM_OF_RELAY)
	{
		relay_handle_t relay_handle = {stt};
		rd_enqueue(&(rd_queue_relay[id_relay]),(void *)&relay_handle);
	}
}


static int rd_onoff_output(u8 stt,u8 id_relay, int rsp)
{
	rd_call_queue_relay(stt,id_relay);
	if(rsp == 1)
	{
		rd_rsp_stt_relay(id_relay,stt,RD_GATEWAYADDRESS);
	}
	return 0;
}

int rd_init_onoff_relay(u8 stt,u8 id_relay)
{
	return rd_onoff_output(stt,id_relay,0);
}

int rd_onoff_relay(u8 stt,u8 id_relay, int rsp)
{
	u8 state = RD_get_on_off(id_relay,0);
	if(stt != state)
	{
		if(rsp == 0 && is_output_linked(id_relay))    //RD_EDIT: check output linked
			return 0;
		return rd_onoff_output(stt,id_relay,rsp);
	}
	return 0;
}

void rd_toggle_relay(uint8_t id_ele, int rsp)
{
	u8 state = RD_get_on_off(id_ele,0);
	u8 toggle = (state > 0) ? 0:1;
	rd_onoff_output(toggle,id_ele,rsp);
}

void rd_handle_relay()
{
	static u32 last_time_relay_ms[NUM_OF_RELAY] = {0};
	for(u8 i =0; i<NUM_OF_RELAY; i++)
	{
		if(clock_time_ms() - last_time_relay_ms[i] >TIME_MIN_HANDLE_RELAY_MS)
		{
			last_time_relay_ms[i] = clock_time_ms();
			relay_handle_t relay_handle = {0};
			int err = rd_dequeue(&(rd_queue_relay[i]),(void *)&relay_handle);
			if(err != -1)
			{
				RD_ev_log("relay queue\n");
				gpio_write(arr_relay[i],relay_handle.stt);
				rd_on_off_led(LED_OUT +i,relay_handle.stt);
			}
		}
	}
}
void rd_init_queue_relay()
{
	for(int i =0; i<NUM_OF_RELAY;i++)
	{
		rd_initQueue(&(rd_queue_relay[i]),NUM_MAX_QUEUE_RELAY,SIZE_QUEUE_HANDLE_LED,(void *)relay_handle[i]);
	}
	rd_register_func_loop(rd_handle_relay);
}

