#ifndef LDA_H
#define LDA_H

#include "sys.h"
#include "matrix.h"
#include "math.h"


extern struct _Matrix m_feature;	//ĳ����������������frame_num*(fea_num*ch_num) = 30*(50*4)ά
extern struct _Matrix m_feature_vector;	//��ǰ������ʵʱ����������fea_num*ch_num = 50*4ά

extern struct _Matrix m_mean_1;			//����1��ƽ����������
extern struct _Matrix m_mean_2;			//����2��ƽ����������
extern struct _Matrix m_mean_3;			//����3��ƽ����������

extern struct _Matrix m_cov;					//����������Э�������
extern struct _Matrix m_cov_assemble;			//���ж�������Э������� = ����������Э�������ĺ�
extern struct _Matrix m_cov_assemble_inverse;	//��Э�������������


extern u8 motion_flag;		//�˴����ܸ�ֵ

void Training(void);
u32 Classifier(void);

#endif
