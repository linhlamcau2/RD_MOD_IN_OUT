#include "rd_in_out.h"

static queue_handle_t array_queue_out[NUM_OF_ELEMENT];

void init_handler_caller_state_output()
{
	queue_handle_t *q = array_queue_out;
	for(uint8_t i=0; i< NUM_OF_ELEMENT; i++)
	{
		(q+i)->front = -1;
		(q+i)->rear = -1;
	}
}

static int is_empty(uint8_t id_ele)
{
	if(id_ele < NUM_OF_ELEMENT)
		return (array_queue_out + id_ele)->front == -1;
	else
		return -1;
}

static int get_num_empty(uint8_t id_ele)
{
	if(is_empty(id_ele) == 1)
		return MAX_STATE_QUEUE_CALLER;
	else if(is_empty(id_ele) == -1)
		return -1;
	queue_handle_t *q = array_queue_out + id_ele;
	return MAX_STATE_QUEUE_CALLER - q->rear;
}

static int is_full(uint8_t id_ele)
{
//	RD_ev_log("array front: %d, rear: %d\n",(array_queue_out + id_ele)->front,(array_queue_out + id_ele)->rear);
	if(id_ele < NUM_OF_ELEMENT)
		return (array_queue_out + id_ele)->rear == MAX_STATE_QUEUE_CALLER -1;
	else
		return -1;
}

static int add_ele_queue(out_stt_t out_state,uint8_t id_ele)
{
	queue_handle_t *q = array_queue_out + id_ele;
	if(is_full(id_ele) == 1)
			return -1;
	if(is_empty(id_ele) == 1)
	{
		q -> front = 0;
	}
	q ->rear ++;
	q->out_status[q ->rear].light = out_state.light;
	q->out_status[q ->rear].time_delay_100ms = out_state.time_delay_100ms;
//	RD_ev_log("add queue caller: %d %d\n",out_state.light,out_state.time_delay_100ms);
	return 0;
}

static out_stt_t get_ele_queue(uint8_t id_ele)
{
	queue_handle_t *q = array_queue_out + id_ele;
	out_stt_t out_stt = {-1,-1};
	if(is_empty(id_ele) == 1)
			return out_stt;
	out_stt = q->out_status[q->front];
	if(q->front == q->rear)
	{
		q->front = (q->rear = -1);
	}
	else
		q->front ++;
//	u32 time = clock_time_100ms();
//	RD_ev_log("delete queue caller: %d %d %d\n",out_stt.light,out_stt.time_delay_100ms,time);
////	sleep_ms(50);
	return out_stt;
}

static int caller_get_time_delay(uint8_t id_ele)
{
	if(id_ele < NUM_OF_ELEMENT)
	{
		queue_handle_t *q = array_queue_out + id_ele;
		if(is_empty(id_ele) == 1)
				return -1;
		return q->out_status[q->front].time_delay_100ms;
	}
	return -1;
}
int RD_mod_io_blink(uint8_t cyclce, uint8_t time_ms,uint8_t id_ele)
{
	if(cyclce > CYCLE_MAX || time_ms >TIME_DELAY_MAX_100MS)
			return -1;
	int num = get_num_empty(id_ele);
	if(num == -1 || num < 2*cyclce)
		return -1;
	for(int i=0; i< 2*cyclce; i++)
	{
		out_stt_t out_state;
		out_state.light = i % 2;
		out_state.time_delay_100ms = time_ms;
		add_ele_queue(out_state,id_ele);
	}
	return 0;
}

int RD_mod_io_onoff(int light,uint8_t id_ele, int rsp)
{
//	if(is_full(id_ele) != 0)
//		return -1;
	RD_ev_log("mode on off \n");
	out_stt_t out_state;
	out_state.light = light;
	out_state.time_delay_100ms = 0;
	if(add_ele_queue(out_state,id_ele) == -1)
		return -1;
//	RD_ev_log("add_ele_queue \n");
	if(rsp == 1)
	{
		RD_SetAndRsp_Switch(id_ele,light,RD_GATEWAYADDRESS);
	}
	return 0;
}

extern int add_ele_queue_callee(uint8_t light_level,uint8_t id_ele);
void RD_handle_caller_state_out()
{
	static u32 lastTick[NUM_OF_ELEMENT] = {0};
	for(int i=0; i<NUM_OF_ELEMENT; i++)
	{
		if(is_empty(i)== 0)
		{
			int time = caller_get_time_delay(i);
			if((clock_time_100ms() - lastTick[i]) > time)
			{
				out_stt_t out_state = get_ele_queue(i);
				if(out_state.light != -1)
				{
					add_ele_queue_callee(out_state.light,i);
					lastTick[i] = clock_time_100ms();
				}
			}
		}
	}
}


