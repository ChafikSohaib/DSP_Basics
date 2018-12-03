#include "dsk6713_aic23.h"

Uint32 input_sample();
void output_sample(int out_data);
void comm_intr();

Uint32 fs = DSK6713_AIC23_FREQ_8KHZ;

short SinTab[64] = {0,401,799,1189,1567,1931,2276,2598,2896,3166,3406,3612,3784,3920,4017,4076,4096,4076,4017,3920,3784,3612,3406,3166,2896,2598,2276,1931,1567,1189,799,401,0,-401,-799,-1189,-1567,-1931,-2276,-2598,-2896,-3166,-3406,-3612,-3784,-3920,-4017,-4076,-4096,-4076,-4017,-3920,-3784,-3612,-3406,-3166,-2896,-2598,-2276,-1931,-1567,-1189,-799,-401};

#define N  64


short Xn;
short XnBuf[N];
Uint16 i = 0, j;

Uint16 GAIN=1, STEP =1;

interrupt void c_int11()
{
	j= (i*STEP) % N ;
	Xn = GAIN*SinTab[j];
	XnBuf[i] = Xn;
	i = (i + 1) % 64;

	output_sample(Xn);
	return;
}

main()
{

	comm_intr();
	i=0;
	while(1);
}
