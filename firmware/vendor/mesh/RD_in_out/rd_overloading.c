#include "rd_in_out.h"

#define myFunction(...) GET_MACRO(__VA_ARGS__, myFunction2, myFunction1)(__VA_ARGS__)
#define GET_MACRO(_1, _2, NAME, ...) NAME

void handleInt(int x) {
	RD_ev_log("handleInt: %d\n",x);
}


void handleString(const char *x) {
	RD_ev_log("Handling string: %s\n", x);
}


#define handle(x) _Generic((x), \
    int: handleInt, \
    const char* : handleString)(x)



void myFunction1(int a)
{
	RD_ev_log("myFunction1: %d\n",a);
}

void myFunction2(int a, int b)
{
	RD_ev_log("myFunction2: %d %d\n",a,b);
}

void rd_my_func()
{
	static u32 time = 0;
	if(clock_time_ms() - time > 3000)
	{
		myFunction(1,2);
		myFunction(3);
		time = clock_time_ms();
	}
}
