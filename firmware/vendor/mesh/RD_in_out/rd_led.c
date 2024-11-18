#include "rd_in_out.h"
#include "../rd_queue.h"


typedef struct
{
	u16 light;
	u8 time_delay_100ms;
}led_handle_t;

static u8 led_stt[NUM_LED] = {0};
u32 arr_led[] = ARR_LED;

#define NUM_MAX_QUEUE_HANDLE_LED				10
#define SIZE_QUEUE_HANDLE_LED					sizeof(led_handle_t)

rd_queue_t rd_queue_led[NUM_LED];
led_handle_t led_handle[NUM_LED][NUM_MAX_QUEUE_HANDLE_LED];

#define LED_HANDLE(i)   (&led_handle[i])

void rd_call_queue_led(u16 light,u8 time_delay_100ms,u8 idx_led)
{
	if(idx_led < NUM_LED)
	{
		led_handle_t led_handle = {light,time_delay_100ms};
		rd_enqueue(&rd_queue_led[idx_led],(void *)&led_handle);
	}
}

void rd_on_off_led(u8 idx_led, u8 status)
{
	rd_call_queue_led(status,0,idx_led);
	if(idx_led < NUM_LED)
	{
		led_stt[idx_led] = (status >0);
	}
}

u8 get_stt_led(u8 idx_led)
{
	return led_stt[idx_led];
}

void rd_blink_led(u8 idx_led, u8 num,u8 time_delay_100ms)
{
	if(idx_led < NUM_LED)
	{
		u8 stt = get_stt_led(idx_led);
		for(int i = 0; i< 2*num; i++)
		{
			stt = !stt;
			rd_call_queue_led(stt,time_delay_100ms,idx_led);
		}
	}
}

int get_count_delay_100ms(u8 idx_led)
{
	if(idx_led < NUM_LED)
	{
		int id_front = rd_queue_led[idx_led].front;
		if(id_front >= 0)
			return led_handle[idx_led][id_front].time_delay_100ms;
	}
	return -1;
}

int is_queue_led_handle_empty(u8 idx_led)
{
	if(idx_led < NUM_LED)
		return isQueueEmpty(&(rd_queue_led[idx_led]));
	return 0;
}
void rd_hanlde_led()
{
	static u32 last_tick_100ms[NUM_LED] = {0};
	for(int i =0; i<NUM_LED; i++)
	{
		if(is_queue_led_handle_empty(i) == 0)
		{
			u32 time = get_count_delay_100ms(i);
			if(clock_time_ms() - last_tick_100ms[i] > 100 *time)
			{
				led_handle_t led_handle = {0};
				int err = rd_dequeue(&(rd_queue_led[i]),(void *)&led_handle);
				if(err != -1)
				{
					last_tick_100ms[i] = clock_time_ms();
					gpio_write(arr_led[i],!(led_handle.light));
				}
			}
		}
	}
}

void rd_init_queue_led()
{
	for(int i=0; i<NUM_LED;i++)
	{
		rd_initQueue(&rd_queue_led[i],NUM_MAX_QUEUE_HANDLE_LED,SIZE_QUEUE_HANDLE_LED,(void *)led_handle[i],rd_hanlde_led);
	}
}

