#include "rd_in_out.h"
#include "../rd_queue.h"


typedef struct
{
	u16 light;
	u8 time_delay_100ms;
}led_handle_t;

#define NUM_MAX_QUEUE_HANDLE_LED				10
#define SIZE_QUEUE_HANDLE_LED					sizeof(led_handle_t)

rd_queue_t rd_queue_led[NUM_LED];
led_handle_t led_handle[NUM_LED][NUM_MAX_QUEUE_HANDLE_LED];

#define LED_HANDLE(i)   (&led_handle[i])

void rd_init_queue_led()
{
	for(int i=0; i<NUM_LED;i++)
	{
		rd_initQueue(&rd_queue_led[i],NUM_MAX_QUEUE_HANDLE_LED,SIZE_QUEUE_HANDLE_LED,(void *)led_handle[i]);
	}
}

void rd_call_queue_led(u16 light, u8 time_delay_100ms,u8 idx_led)
{
	if(idx_led < NUM_LED)
	{
		led_handle_t led_handle = {light,time_delay_100ms};
		rd_enqueue(&rd_queue_led[i],(void *)&led_handle);
	}
}

void rd_on_off_led(u8 idx_led, u8 status)
{
	rd_call_queue_led(status,0,idx_led);
}

void rd_blink_led(u8 idx_led, u8 num,u8 time_delay_ms)
{

}
