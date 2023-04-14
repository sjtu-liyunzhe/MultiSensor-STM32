#include <stdint.h>
#include "EMGProcessing.h"
#include "usart.h"
const int16_t IIR_B[6] = {
		26735, -32768,  32767, -32768,  32767, -26735
	};
const int16_t IIR_A[6] = {
		32767, -32768,  32767, -32768,  32767, -21812
	};
const float comb_rho = 0.4601747394;	// 带宽20
// const float comb_rho = 0.1762159765;	// 带宽30
float comb_b;

int initial_PLF(PLN_FILER *x)
{
	int i=0;
	for( i=0; i< PLF_L; i++)
	{
		x->data[i]=0;
	}
	
	x->flag = 0;
	x->index = 0;
	x->length = 0;
	return x->flag;
}

int add_PLF(PLN_FILER *x, int y, int *z)
{
	//*z = y; //for the test without powerline noise filter
		
		if(x->length < PLF_L) {
		x->length ++;
		x->data[x->index] = y * 0.5;//0.25;//0.2
		x->index = (++(x->index))%PLF_L;
		*z = y;
		return 0;
	} else {
		// *z = y - 
		// 	(x->data[x->index] // it is the last element
		// 	+ x->data[(x->index+20)%PLF_L] 
		// 	); //without use double calculation, save a lot time
		*z = y - 
			(x->data[x->index] // it is the last element
			+ x->data[(x->index+20)%PLF_L] 
			);
		x->data[x->index] = y*0.5;//0.25;//0.5
		x->index = (++(x->index))%PLF_L;
		x->length++;
		return 1;
	}
}
unsigned char initial_IIR_Filter(IIR_FILER * iir_filter)
{
	for(int i = 0; i < IIR_ORDER + 1; ++i)
	{
		iir_filter->data_x[i] = 0;
		iir_filter->data_y[i] = 0;
	}
	iir_filter->flag = 0;
	iir_filter->index = 0;
	iir_filter->length = 0;
	return iir_filter->flag;
}
// void IIR_Filter(IIR_FILER* iir_filter, int16_t x_new, int16_t* reslut)
void IIR_Filter(IIR_FILER* iir_filter, int x_new, int* reslut)
{
	int tmp = 0;
	// 更新序列,x[0]、y[0]是最新值
	for(int i = IIR_ORDER; i > 0; --i)
	{
		iir_filter->data_x[i] = iir_filter->data_x[i - 1];
		iir_filter->data_y[i] = iir_filter->data_y[i - 1];
	}

	iir_filter->data_x[0] = x_new;
	for(int i = 0; i < IIR_ORDER + 1; ++i)
	{
		tmp += IIR_B[i] * iir_filter->data_x[i];
	}

	for(int i = 1; i < IIR_ORDER + 1; ++i)
	{
		tmp -= IIR_A[i] * iir_filter->data_y[i];
	}
	// printf("%d\r\n", (int16_t)tmp);
	
	tmp /= IIR_A[0];
	iir_filter->data_y[0] = tmp;
	iir_filter->length++;
	if(iir_filter->length <= IIR_ORDER)
	{
		*reslut = x_new;
		return;
	}
	// *reslut = (int16_t)iir_filter->data_y[0];
	*reslut = (int)iir_filter->data_y[0];
	return;
}
unsigned char initial_COMB_Filter(COMB_FILER * comb_filter)
{
	comb_b = (comb_rho + 1) / 2;
	for(int i = 0; i < COMB_ORDER; ++i)
	{
		comb_filter->data_x[i] = 0;
		comb_filter->data_y[i] = 0;
	}
	comb_filter->flag = 0;
	comb_filter->index = 0;
	comb_filter->length = 0;
	return comb_filter->flag;
}
void COMB_Filter(COMB_FILER *comb_filter, int x_new, int* result)
{
	float tmp = 0;
	for(int i = COMB_ORDER; i > 0; --i)
	{
		comb_filter->data_x[i] = comb_filter->data_x[i - 1];
		comb_filter->data_y[i] = comb_filter->data_y[i - 1];
	}
	comb_filter->data_x[0] = x_new;
	tmp += comb_b * (float)comb_filter->data_x[0] - comb_b * (float)comb_filter->data_x[COMB_ORDER];
	tmp += comb_rho * (float)comb_filter->data_y[COMB_ORDER];
	comb_filter->data_y[0] = tmp;
	comb_filter->length++;
	if(comb_filter->length <= COMB_ORDER)
	{
		*result = x_new;
		return;
	}
	*result = (int)comb_filter->data_y[0];
}