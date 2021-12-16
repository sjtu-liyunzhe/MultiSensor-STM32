#ifndef __EMGProcessing_H
#define __EMGProcessing_H

#define PLF_L 80     

typedef struct
{
	int data[PLF_L];
	int flag;
	int index;
	int length;
} PLN_FILER ;

int initial_PLF(PLN_FILER *);
int add_PLF(PLN_FILER *, int , int *);

#endif
