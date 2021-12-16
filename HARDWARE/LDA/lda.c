#include "lda.h"
#include "sys.h"
#include "usart.h"


///////////////////////////////// 训练LDA模型 ////////////////////////////////

//将某个动作的特征矩阵m_feature转化为该动作的平均特征向量m_mean_i和协方差矩阵m_cov
//其中，保存该动作的平均特征向量，用于后续的识别
//每个动作的协方差矩阵不保存下来，而是保存所有动作的总协方差矩阵m_cov_assemble	(各个动作的协方差矩阵相加得到)

/*********************************************************************
 * @Function	Training
 * @Brief		采集并提取完一个动作的特征后，进行训练
 * @Parameter	保存该动作的平均特征向量和所有动作的总协方差矩阵(模型)
 *********************************************************************/

void Training(void)
{
	int i = 0;	//用于初始化矩阵的行
	int j = 0;	//用于初始化矩阵的列
	//定义一些中间运算的向量和矩阵
	struct _Matrix m_vector_1;		//1*30		值全为1的行向量(1,1,1,...,1)	30为一个动作的总帧数frame_num
	struct _Matrix m_vector_2;		//1*200		行向量							m_vector_2 = m_vector_1 * m_feature
									//			即将特征矩阵的每一列的30个值相加
	struct _Matrix m_vector_3;		//30*1		值全为1的列向量(1,1,1,...,1)T
	struct _Matrix m_vector_mean;	//1*200		当前动作的平均特征向量			m_vector_mean = 1/30 * m_vector_2
	struct _Matrix m_const;			//1*1		常量	系数（1/30或1）
	struct _Matrix m_cov1;			//30*200	平均特征向量的展开矩阵			m_cov1 = m_vector_3 * m_vector_mean
	struct _Matrix m_cov2;			//30*200	特征矩阵的每个元素减去平均值	m_cov2 = m_feature - m_cov1
	struct _Matrix m_cov3;			//200*30	m_cov3是m_cov2的转置，两者相乘为每个动作的协方差矩阵	m_cov= m_cov3 * m_cov2
	
	/*************************** 初始化以上这些矩阵 ***************************/
	
	//m_vector_1	1*30	值全为1的行向量
	matrix_set_m(&m_vector_1, 1);
	matrix_set_n(&m_vector_1, 30);
	matrix_init(&m_vector_1);
	for (i = 0; i < m_vector_1.m; i++)
	{
		for (j = 0; j < m_vector_1.n; j++)
		{
			matrix_write(&m_vector_1, i, j, 1);
		}
	}
	
	//m_vector_2	1*200	m_vector_2 = m_vector_1 * m_feature
	matrix_set_m(&m_vector_2, 1);
	matrix_set_n(&m_vector_2, 200);
	matrix_init(&m_vector_2);
	for (i = 0; i < m_vector_2.m; i++)
	{
		for (j = 0; j < m_vector_2.n; j++)
		{
			matrix_write(&m_vector_2, i, j, 0);		//初始化为0
		}
	}	
	
	//m_vector_3	30*1	值全为1的列向量
	matrix_set_m(&m_vector_3, 30);
	matrix_set_n(&m_vector_3, 1);
	matrix_init(&m_vector_3);
	for (i = 0; i < m_vector_3.m; i++)
	{
		for (j = 0; j < m_vector_3.n; j++)
		{
			matrix_write(&m_vector_3, i, j, 1);
		}
	}
	
	//m_vector_mean	1*200	m_vector_mean = 1/30 * m_vector_2
	matrix_set_m(&m_vector_mean, 1);
	matrix_set_n(&m_vector_mean, 200);
	matrix_init(&m_vector_mean);
	for (i = 0; i < m_vector_mean.m; i++)
	{
		for (j = 0; j < m_vector_mean.n; j++)
		{
			matrix_write(&m_vector_mean, i, j, 0);	//初始化为0
		}
	}

	//m_const	1*1		m_const = 1/30
	matrix_set_m(&m_const, 1);
	matrix_set_n(&m_const, 1);
	matrix_init(&m_const);
	for (i = 0; i < m_const.m; i++)
	{
		for (j = 0; j < m_const.n; j++)
		{
			matrix_write(&m_const, i, j, 1.0/30.0);	//初始化为1.0/30.0
		}
	}	
	
	//m_cov1	30*200	m_cov1 = m_vector_3 * m_vector_mean
	matrix_set_m(&m_cov1, 30);
	matrix_set_n(&m_cov1, 200);
	matrix_init(&m_cov1);
	for (i = 0; i < m_cov1.m; i++)
	{
		for (j = 0; j < m_cov1.n; j++)
		{
			matrix_write(&m_cov1, i, j, 0);			//初始化为0
		}
	}	
	
	//m_cov2	30*200	m_cov2 = m_feature - m_cov1
	matrix_set_m(&m_cov2, 30);
	matrix_set_n(&m_cov2, 200);
	matrix_init(&m_cov2);
	for (i = 0; i < m_cov2.m; i++)
	{
		for (j = 0; j < m_cov2.n; j++)
		{
			matrix_write(&m_cov2, i, j, 0);			//初始化为0
		}
	}		

	//m_cov3	200*30	m_cov3 = (m_cov2)T
	matrix_set_m(&m_cov3, 200);
	matrix_set_n(&m_cov3, 30);
	matrix_init(&m_cov3);
	for (i = 0; i < m_cov3.m; i++)
	{
		for (j = 0; j < m_cov3.n; j++)
		{
			matrix_write(&m_cov3, i, j, 0);			//初始化为0
		}
	}	

	/*************************** 求解协方差矩阵 ***************************/
	matrix_multiply(&m_vector_1, &m_feature, &m_vector_2);	//m_vector_2 = m_vector_1 * m_feature	特征矩阵每列求和
	matrix_multiply(&m_const, &m_vector_2, &m_vector_mean);	//m_vector_mean = 1/30 * m_vector_2		求当前动作的平均特征向量
	matrix_multiply(&m_vector_3, &m_vector_mean, &m_cov1);	//m_cov1 = m_vector_3 * m_vector_mean	平均特征向量的展开矩阵
	matrix_subtract(&m_feature, &m_cov1, &m_cov2);        	//m_cov2 = m_feature - m_cov1			(x-E(x))
	matrix_transpos(&m_cov2, &m_cov3);                   	//m_cov3 = (m_cov2)T					(x-E(x))T
	matrix_multiply(&m_cov3, &m_cov2, &m_cov);            	//m_cov= m_cov3 * m_cov2	该动作的协方差矩阵(Σ)=(x-E(x))*(x-E(x))T
	matrix_add(&m_cov, &m_cov_assemble, &m_cov_assemble); 	//m_cov_assemble += m_cov	所有动作的总协方差矩阵 = 各个动作的协方差矩阵的和
	
			
	/*********** 把当前动作的平均特征向量赋值给对应的动作，保存下来 ***********/
	matrix_write(&m_const, 0, 0, 1);
	
	switch (motion_flag)
	{
		case 1:
			matrix_multiply(&m_const, &m_vector_mean, &m_mean_1);	//动作1的平均特征向量
			break;
		case 2:
			matrix_multiply(&m_const, &m_vector_mean, &m_mean_2);	//动作2的平均特征向量
			break;
		case 3:
			matrix_multiply(&m_const, &m_vector_mean, &m_mean_3);	//动作3的平均特征向量
			break;	
		default:
			break;
	}
	motion_flag ++;
	if (motion_flag > 3)
	{
		motion_flag = 1;
	}
		
	/*************************** 释放内存 ***************************/
	matrix_free(&m_vector_1);
	matrix_free(&m_vector_2);
	matrix_free(&m_vector_3);
	matrix_free(&m_vector_mean);
	matrix_free(&m_const);
	matrix_free(&m_cov1);
	matrix_free(&m_cov2);
	matrix_free(&m_cov3);		
}


//////////////////////////////////// 分类器 ///////////////////////////////////////

/******************************************************************
 * @Function	Classifier
 * @Brief
 * @Parameter	返回分类的动作结果
 ******************************************************************/

u32 Classifier(void)
{
	int i = 0;	//用于初始化矩阵的行
	int j = 0;	//用于初始化矩阵的列
	float predict[3];				//存储每类动作的判别式的值
	float max = 0;					//存储判别式的最大值
	u32 class_flag = 1;	//分类器返回的结果
	
	//定义一些中间运算的向量和矩阵
	//判别式:	P(x)=((ui)T)((Σ)-1)X - 1/2((ui)T)((Σ)-1)(ui)
	//(ui)T: m_mean_i	200*1,	(Σ): 200*200,	X: m_feature_vector	200*1
	
 	struct _Matrix vector;			//200*1		vector = m_cov_assemble_inverse * m_feature_vector	((Σ)-1)X
	struct _Matrix vector_w;		//200*1		vector_w = (m_mean_i)T								(ui)
	struct _Matrix vector_for_w;	//200*1		vector_for_w = m_cov_assemble_inverse * vector_w	((Σ)-1)(ui)
	
	struct _Matrix const1;			//1*1		const1 = m_mean_1 * vector							((u1)T)((Σ)-1)X
	struct _Matrix const2;			//1*1		const2 = m_mean_2 * vector							((u2)T)((Σ)-1)X
	struct _Matrix const3;			//1*1		const3 = m_mean_3 * vector							((u3)T)((Σ)-1)X

	struct _Matrix const1_w;		//1*1		const1_w = m_mean_1 * vector_for_w					((u1)T)((Σ)-1)u1
	struct _Matrix const2_w;		//1*1		const2_w = m_mean_2 * vector_for_w					((u2)T)((Σ)-1)u2
	struct _Matrix const3_w;		//1*1		const3_w = m_mean_3 * vector_for_w					((u3)T)((Σ)-1)u3

	/*************************** 初始化以上这些矩阵 ***************************/
	//vector	200*1
	matrix_set_m(&vector, 200);
	matrix_set_n(&vector, 1);
	matrix_init(&vector);
	for (i = 0; i < vector.m; i++)
	{
		for (j = 0; j < vector.n; j++)
		{
			matrix_write(&vector, i, j, 0);		//初始化为0
		}
	}
	
	//vector_w	200*1
	matrix_set_m(&vector_w, 200);
	matrix_set_n(&vector_w, 1);
	matrix_init(&vector_w);
	for (i = 0; i < vector_w.m; i++)
	{
		for (j = 0; j < vector_w.n; j++)
		{
			matrix_write(&vector_w, i, j, 0);	//初始化为0
		}
	}	
	
	//vector_for_w	200*1
	matrix_set_m(&vector_for_w, 200);
	matrix_set_n(&vector_for_w, 1);
	matrix_init(&vector_for_w);
	for (i = 0; i < vector_for_w.m; i++)
	{
		for (j = 0; j < vector_for_w.n; j++)
		{
			matrix_write(&vector_for_w, i, j, 0);	//初始化为0
		}
	}		
	
	//const1	1*1
	matrix_set_m(&const1, 1);
	matrix_set_n(&const1, 1);
	matrix_init(&const1);
	for (i = 0; i < const1.m; i++)
	{
		for (j = 0; j < const1.n; j++)
		{
			matrix_write(&const1, i, j, 0);	//初始化为0
		}
	}	
	
	//const2	1*1
	matrix_set_m(&const2, 1);
	matrix_set_n(&const2, 1);
	matrix_init(&const2);
	for (i = 0; i < const2.m; i++)
	{
		for (j = 0; j < const2.n; j++)
		{
			matrix_write(&const2, i, j, 0);	//初始化为0
		}
	}
	
	//const3	1*1
	matrix_set_m(&const3, 1);
	matrix_set_n(&const3, 1);
	matrix_init(&const3);
	for (i = 0; i < const3.m; i++)
	{
		for (j = 0; j < const3.n; j++)
		{
			matrix_write(&const3, i, j, 0);	//初始化为0
		}
	}	
	
	//const1_w	1*1
	matrix_set_m(&const1_w, 1);
	matrix_set_n(&const1_w, 1);
	matrix_init(&const1_w);
	for (i = 0; i < const1_w.m; i++)
	{
		for (j = 0; j < const1_w.n; j++)
		{
			matrix_write(&const1_w, i, j, 0);	//初始化为0
		}
	}	
	
	//const2_w	1*1
	matrix_set_m(&const2_w, 1);
	matrix_set_n(&const2_w, 1);
	matrix_init(&const2_w);
	for (i = 0; i < const2_w.m; i++)
	{
		for (j = 0; j < const2_w.n; j++)
		{
			matrix_write(&const2_w, i, j, 0);	//初始化为0
		}
	}	
	
	//const3_w	1*1
	matrix_set_m(&const3_w, 1);
	matrix_set_n(&const3_w, 1);
	matrix_init(&const3_w);
	for (i = 0; i < const3_w.m; i++)
	{
		for (j = 0; j < const3_w.n; j++)
		{
			matrix_write(&const3_w, i, j, 0);	//初始化为0
		}
	}					
	
	/*************************** 计算各类动作的判别式 ***************************/
	//判别式:	P(x)=((ui)T)((Σ)-1)X - 1/2((ui)T)((Σ)-1)(ui)
	//训练完所有动作后，会求解总协方差矩阵的逆m_cov_assemble_inverse
	matrix_multiply(&m_cov_assemble_inverse, &m_feature_vector, &vector);	//vector = m_cov_assemble_inverse * m_feature_vector		((Σ)-1)X
	
	//动作1
	matrix_multiply(&m_mean_1, &vector, &const1);      						//const1 = m_mean_1 * vector								((ui)T)((Σ)-1)X
	//printf_matrix(&const1);
	matrix_transpos(&m_mean_1, &vector_w);            						//m_mean_1: 1*200, vector_w: 200*1							ui
	matrix_multiply(&m_cov_assemble_inverse, &vector_w, &vector_for_w);    	//vector_for_w = m_cov_assemble_inverse * vector_w			((Σ)-1)(ui)
	matrix_multiply(&m_mean_1, &vector_for_w, &const1_w);         			//const1_w = m_mean_1 * vector_for_w						((ui)T)((Σ)-1)ui
	//printf_matrix(&const1_w);
	predict[0] = matrix_read(&const1, 0, 0) - 0.5 * matrix_read(&const1_w, 0, 0);	//动作1的判别函数
	
	//动作2
	matrix_multiply(&m_mean_2, &vector, &const2);
	matrix_transpos(&m_mean_2, &vector_w);
	matrix_multiply(&m_cov_assemble_inverse, &vector_w, &vector_for_w);
	matrix_multiply(&m_mean_2, &vector_for_w, &const2_w);
	predict[1] = matrix_read(&const2, 0, 0) - 0.5 * matrix_read(&const2_w, 0, 0);	//动作2的判别函数
	
	//动作3
	matrix_multiply(&m_mean_3, &vector, &const3);
	matrix_transpos(&m_mean_3, &vector_w);
	matrix_multiply(&m_cov_assemble_inverse, &vector_w, &vector_for_w);
	matrix_multiply(&m_mean_3, &vector_for_w, &const3_w);
	predict[2] = matrix_read(&const3, 0, 0) - 0.5 * matrix_read(&const3_w, 0, 0);	//动作3的判别函数
	
//	for(i = 0; i < sizeof(predict); i++)
//	{
//		printf("%f\n", predict[i]);
//	}

	/*************************** 求判别函数最大值及对应的类 ***************************/
	max = predict[0];
	for (i = 0; i < sizeof(predict); i++)
	{
		if (max < predict[i])
		{
			max = predict[i];
		}
	}
	for (i = 0; i < sizeof(predict); i++)
	{
		if (predict[i] == max)
		{
			class_flag = i+1;	//返回最大值所对应的(索引号+1), 为对应的动作类别
		}
	}
	return class_flag;			//返回分类结果
	
	/*************************** 释放内存 ***************************/
	matrix_free(&vector);
	matrix_free(&vector_w);
	matrix_free(&vector_for_w);
	
	matrix_free(&const1);
	matrix_free(&const2);
	matrix_free(&const3);

	matrix_free(&const1_w);
	matrix_free(&const2_w);
	matrix_free(&const3_w);
	
}



