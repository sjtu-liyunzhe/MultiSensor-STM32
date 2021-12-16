#ifndef FEATURE_H
#define FEATURE_H

#include "sys.h"
#include "matrix.h"
#include "math.h"

#define	sample_num	1000	//采样点数
#define win_len		20		//分割窗长	
#define win_num		50		//win_num = sample_num / win_len

//开辟内存空间，用向量存放4个超声通道的采样值，每个通道1000个点
extern float vector_1[sample_num];
extern float vector_2[sample_num];
extern float vector_3[sample_num];
extern float vector_4[sample_num];
extern float vector_online[sample_num];	//存放在线测试时的超声采样值？？

extern struct _Matrix m_feature;		//某个动作的特征矩阵。frame_num*(fea_num*ch_num) = 30*(50*4)维
extern struct _Matrix m_feature_vector;	//当前动作的实时特征向量。fea_num*ch_num = 50*4维

extern struct _Matrix m_mean_1;			//动作1的平均特征向量	1*200维
extern struct _Matrix m_mean_2;			//动作2的平均特征向量	1*200维
extern struct _Matrix m_mean_3;			//动作3的平均特征向量	1*200维

extern struct _Matrix m_cov;					//单个动作的协方差矩阵	200*200维？
extern struct _Matrix m_cov_assemble;			//所有动作的总协方差矩阵 = 各个动作的协方差矩阵的和
extern struct _Matrix m_cov_assemble_inverse;	//总协方差矩阵的逆矩阵

extern u8 motion_flag;		//此处不能赋值


float Get_Win_Mean(float *vector, u16 len, u16 index);	//求vector_i中某个窗内的点的均值
void Get_Feature_Matrix(void);	//求每个动作的特征矩阵（训练集）	m_feature
void Get_Current_Feature(void);	//求解当前动作的实时特征向量（测试集）	m_feature_vector

#endif
