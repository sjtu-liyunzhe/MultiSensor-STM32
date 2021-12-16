#include "EMGProcessing.h"


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
		*z = y - 
			(x->data[x->index] // it is the last element
			+ x->data[(x->index+20)%PLF_L] 
			); //without use double calculation, save a lot time
		x->data[x->index] = y*0.5;//0.25;//0.5
		x->index = (++(x->index))%PLF_L;
		return 1;
	}
}
