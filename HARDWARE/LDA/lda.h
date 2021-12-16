#ifndef LDA_H
#define LDA_H

#include "sys.h"
#include "matrix.h"
#include "math.h"


extern struct _Matrix m_feature;	//某个动作的特征矩阵。frame_num*(fea_num*ch_num) = 30*(50*4)维
extern struct _Matrix m_feature_vector;	//当前动作的实时特征向量。fea_num*ch_num = 50*4维

extern struct _Matrix m_mean_1;			//动作1的平均特征向量
extern struct _Matrix m_mean_2;			//动作2的平均特征向量
extern struct _Matrix m_mean_3;			//动作3的平均特征向量

extern struct _Matrix m_cov;					//单个动作的协方差矩阵
extern struct _Matrix m_cov_assemble;			//所有动作的总协方差矩阵 = 各个动作的协方差矩阵的和
extern struct _Matrix m_cov_assemble_inverse;	//总协方差矩阵的逆矩阵


extern u8 motion_flag;		//此处不能赋值

void Training(void);
u32 Classifier(void);

#endif
