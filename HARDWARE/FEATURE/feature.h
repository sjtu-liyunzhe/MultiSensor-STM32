#ifndef FEATURE_H
#define FEATURE_H

#include "sys.h"
#include "matrix.h"
#include "math.h"

#define	sample_num	1000	//��������
#define win_len		20		//�ָ��	
#define win_num		50		//win_num = sample_num / win_len

//�����ڴ�ռ䣬���������4������ͨ���Ĳ���ֵ��ÿ��ͨ��1000����
extern float vector_1[sample_num];
extern float vector_2[sample_num];
extern float vector_3[sample_num];
extern float vector_4[sample_num];
extern float vector_online[sample_num];	//������߲���ʱ�ĳ�������ֵ����

extern struct _Matrix m_feature;		//ĳ����������������frame_num*(fea_num*ch_num) = 30*(50*4)ά
extern struct _Matrix m_feature_vector;	//��ǰ������ʵʱ����������fea_num*ch_num = 50*4ά

extern struct _Matrix m_mean_1;			//����1��ƽ����������	1*200ά
extern struct _Matrix m_mean_2;			//����2��ƽ����������	1*200ά
extern struct _Matrix m_mean_3;			//����3��ƽ����������	1*200ά

extern struct _Matrix m_cov;					//����������Э�������	200*200ά��
extern struct _Matrix m_cov_assemble;			//���ж�������Э������� = ����������Э�������ĺ�
extern struct _Matrix m_cov_assemble_inverse;	//��Э�������������

extern u8 motion_flag;		//�˴����ܸ�ֵ


float Get_Win_Mean(float *vector, u16 len, u16 index);	//��vector_i��ĳ�����ڵĵ�ľ�ֵ
void Get_Feature_Matrix(void);	//��ÿ����������������ѵ������	m_feature
void Get_Current_Feature(void);	//��⵱ǰ������ʵʱ�������������Լ���	m_feature_vector

#endif
