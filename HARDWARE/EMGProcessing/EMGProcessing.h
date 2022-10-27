#ifndef __EMGProcessing_H
#define __EMGProcessing_H

#define PLF_L 80
#define IIR_ORDER 5   
#define COMB_ORDER 20

typedef struct
{
	int data[PLF_L];
	int flag;
	int index;
	int length;
} PLN_FILER ;

typedef struct
{
	int16_t data_x[IIR_ORDER + 1];
	int16_t data_y[IIR_ORDER + 1];
	unsigned char flag;
	unsigned char index;
	unsigned int length;
} IIR_FILER;

typedef struct
{
	int data_x[COMB_ORDER + 1];
	float data_y[COMB_ORDER + 1];
	unsigned char flag;
	unsigned char index;
	unsigned int length;
} COMB_FILER;

int initial_PLF(PLN_FILER *);
int add_PLF(PLN_FILER *, int , int *);

unsigned char initial_IIR_Filter(IIR_FILER *);
// void IIR_Filter(IIR_FILER *, int16_t, int16_t*);
void IIR_Filter(IIR_FILER *, int, int*);

unsigned char initial_COMB_Filter(COMB_FILER *);
void COMB_Filter(COMB_FILER *, int, int*);

#endif
