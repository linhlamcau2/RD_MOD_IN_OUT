#include "rd_in_out.h"

static queue_write_out_t queue_write_out[NUM_OF_ELEMENT];

void init_handler_callee_state_output()
{
	queue_write_out_t *q = queue_write_out;
	for(uint8_t i=0; i< NUM_OF_ELEMENT; i++)
	{
		(q+i)->front = -1;
		(q+i)->rear = -1;
	}
}

static int is_empty_callee(uint8_t id_ele)
{
	if(id_ele < NUM_OF_ELEMENT)
		return (queue_write_out + id_ele)->front == -1;
	else
		return -1;
}

static int callee_check_queue_out()
{
	for(int i=0; i<NUM_OF_ELEMENT; i++)
	{
		if(is_empty_callee(i) == 0)
				return 0;
	}
	return -1;
}

static int is_full_callee(uint8_t id_ele)
{
	if(id_ele < NUM_OF_ELEMENT)
		return (queue_write_out + id_ele)->rear == MAX_STATE_QUEUE_CALLEE -1;
	else
		return -1;
}

int add_ele_queue_callee(uint8_t light_level,uint8_t id_ele)
{
	if(id_ele < NUM_OF_ELEMENT)
	{
		queue_write_out_t *q = queue_write_out + id_ele;
		if(is_full_callee(id_ele) == 1)
				return -1;
		if(is_empty_callee(id_ele) == 1)
		{
			q -> front = 0;
		}
		q ->rear ++;
		q->light[q ->rear] = light_level;
//		RD_ev_log("add queue caller out: %d\n",light_level);
		return 0;
	}
	return -1;
}

static int callee_get_ele(uint8_t id_ele)
{
	queue_write_out_t *q = queue_write_out + id_ele;
	if(is_empty_callee(id_ele) == 1)
			return -1;
	int light_level = (q->light)[q->front];
	if(q->front == q->rear)
	{
		q->front = (q->rear = -1);
	}
	else
		q->front ++;
//	RD_ev_log("delete queue callee: %d\n",light_level);
	return light_level;
}

extern uint32_t Output_Array[NUM_OF_ELEMENT];
void RD_handle_callee_state_out()
{
	if(callee_check_queue_out() == 0)
	{
//		RD_ev_log("callee_check_queue_out: %d\n");
		for(int i=0; i<NUM_OF_ELEMENT; i++)
		{
//			RD_ev_log("num queue %d: %d\n",i,(queue_write_out+i)->front);
			int light_level = callee_get_ele(i);
			if(light_level != -1)
			{
				u8 state = (light_level > 0)? 0: 1;
//				RD_ev_log("state callee: %d",state);
				OUTPUT_WRITE(i,state);
//				if(get_provision_state() != STATE_DEV_PROVED)  RD_Flash_SavePowerOff();
			}
		}
	}
}
