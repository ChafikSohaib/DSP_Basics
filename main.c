#include "dsk6713_aic23.h"

Uint32 input_sample();
void output_sample(int out_data);
void comm_intr();

Uint32 fs = DSK6713_AIC23_FREQ_8KHZ;
short Xn;
short XnBuf[128];
Uint16 i = 0;

interrupt void c_int11()
{
	Xn = input_sample();
	XnBuf[i] = Xn;
	i = (i + 1) % 128;

	output_sample(Xn);
	return;
}

main()
{
	comm_intr();
	while(1);
}
