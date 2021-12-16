#include "feature.h"
#include "sys.h"

u8 feature_extract_times = 0;	//同一动作下，特征提取的次数

//////////////////////////////////// 求解训练时的特征 /////////////////////////////////////

/******************************************************************
 * @Function	Get_Win_Mean
 * @Brief		求vector_i中，某个窗内的点的均值
 * @Parameter	指向vector的指针(基地址); 窗长len; 第index个窗
 ******************************************************************/
//只用窗内信号的均值作为特征
float Get_Win_Mean(float *vector, u16 len, u16 index)
{
	float win_mean;
	u16 i = 0;
	for (i = (index * len); i < (index * len + len); i++)
	{
		win_mean += (*(vector + i));	//求和
	}
	win_mean = win_mean / len;			//该窗的均值
	return win_mean;
}

/****************************************************************************
 * @Function	Get_Feature_Matrix
 * @Brief		求解当前动作的特征矩阵（训练集）	m_feature
 * @Parameter	1个trial，取中间3s，共30帧，通过feature_extract_times控制行
 ***************************************************************************/
//frame每次加1的时候，就调用这个函数得到特征矩阵的某一行。特征矩阵维度：30*200

/*	每一帧提取一次特征，每行为一帧中提取出的特征，mean1--mean50是一个通道中每个窗的均值
						CH1						CH2					CH3			CH4		
			| mean1 mean2... mean50		mean1 mean2... mean50		...			...	|	
m_feature = | mean1 mean2... mean50		mean1 mean2... mean50		...			...	|	
			| mean1 mean2... mean50		mean1 mean2... mean50		...			...	|	
*/

void Get_Feature_Matrix(void)
{
	u8 row_i = 0;	//控制特征矩阵m_feature的行
	u8 ch_i = 0;	//控制是第几个通道
	u8 win_i = 0;	//某个通道中的第几个窗口, 即窗口序号: index of window (start from 0 to win_num-1)
	row_i = feature_extract_times;
	
	for (ch_i = 0; ch_i < 4; ch_i++)	//ch_i: 0 to 3
	{
		switch (ch_i)
		{
			case 0:		//Channel 1
				for (win_i = 0; win_i < win_num; win_i++)
				{
					matrix_write(&m_feature, row_i, ch_i * win_num + win_i, Get_Win_Mean(vector_1, win_len, win_i));
				}
				break;
			case 1:		//Channel 2
				for (win_i = 0; win_i < win_num; win_i++)
				{
					matrix_write(&m_feature, row_i, ch_i * win_num + win_i, Get_Win_Mean(vector_2, win_len, win_i));
				}
				break;	
			case 2:		//Channel 3
				for (win_i = 0; win_i < win_num; win_i++)
				{
					matrix_write(&m_feature, row_i, ch_i * win_num + win_i, Get_Win_Mean(vector_3, win_len, win_i));
				}
				break;		
			case 3:		//Channel 4
				for (win_i = 0; win_i < win_num; win_i++)
				{
					matrix_write(&m_feature, row_i, ch_i * win_num + win_i, Get_Win_Mean(vector_4, win_len, win_i));
				}
				break;		
			default:
				break;
		}			
	}
	if (feature_extract_times < 29)		//获取30帧的特征，每帧特征为特征矩阵的一行
	{
		feature_extract_times++;
	}
	else
	{
		feature_extract_times = 0;
	}	
}


/////////////////////////////// 获取当前动作下的实时特征 ///////////////////////////////

/****************************************************************************
 * @Function	Get_Current_Feature
 * @Brief		求解当前动作的实时特征向量（测试集）	m_feature_vector
 * @Parameter	m_feature_vector为列向量，相当于m_feature的某行的转置
 ***************************************************************************/
//m_feature_vector为列向量，为当前时刻下的实时特征向量，维数200*1
/*	
							CH1							CH2					CH3		CH4
m_feature_vector = (mean1 mean2 ... mean50		mean1 mean2 ... mean50		...		...)T
*/

void Get_Current_Feature(void)
{
	u8 col_i = 0;	//控制实时特征向量m_feature_vector的列
	u8 ch_i = 0;	//控制是第几个通道
	u8 win_i = 0;	//某个通道中的第几个窗口, 即窗口序号: index of window (start from 0 to win_num-1)
	
	for (ch_i = 0; ch_i < 4; ch_i++)	//ch_i: 0 to 3
	{
		switch (ch_i)
		{
			case 0:		//Channel 1
				for (win_i = 0; win_i < win_num; win_i++)
				{
					matrix_write(&m_feature_vector, ch_i * win_num + win_i, col_i, Get_Win_Mean(vector_1, win_len, win_i));
				}
				break;
			case 1:		//Channel 2
				for (win_i = 0; win_i < win_num; win_i++)
				{
					matrix_write(&m_feature_vector, ch_i * win_num + win_i, col_i, Get_Win_Mean(vector_2, win_len, win_i));
				}
				break;
			case 2:		//Channel 3
				for (win_i = 0; win_i < win_num; win_i++)
				{
					matrix_write(&m_feature_vector, ch_i * win_num + win_i, col_i, Get_Win_Mean(vector_3, win_len, win_i));
				}
				break;
			case 3:		//Channel 4
				for (win_i = 0; win_i < win_num; win_i++)
				{
					matrix_write(&m_feature_vector, ch_i * win_num + win_i, col_i, Get_Win_Mean(vector_4, win_len, win_i));
				}
				break;	
			default:
				break;
		}
	}			
}


/////////////////////////// 初始化特征运算中的矩阵 ////////////////////////////

/******************************************************************
 * @Function	Feature_Matrix_Init
 * @Brief		为特征运算中的矩阵申请内存，设定矩阵的初始值
 * @Parameter
 ******************************************************************/
void Feature_Matrix_Init(void)
{
	int i = 0;	//用于初始化矩阵的行
	int j = 0;	//用于初始化矩阵的列
	
	//当前动作的特征矩阵m_feature	30*200
	matrix_set_m(&m_feature, 30);
	matrix_set_n(&m_feature, 200);
	matrix_init(&m_feature);
	for (i = 0; i < m_feature.m; i++)
	{
		for (j = 0; j < m_feature.n; j++)
		{
			matrix_write(&m_feature, i, j, 0);		//初始化为0
		}
	}	
	
	//预测时,当前动作的实时特征向量m_feature_vector	200*1
	matrix_set_m(&m_feature_vector, 200);
	matrix_set_n(&m_feature_vector, 1);
	matrix_init(&m_feature_vector);
	for (i = 0; i < m_feature_vector.m; i++)
	{
		for (j = 0; j < m_feature_vector.n; j++)
		{
			matrix_write(&m_feature_vector, i, j, 1);	//初始化为1
		}
	}
	
	//单个动作的协方差矩阵m_cov						200*200
	matrix_set_m(&m_cov, 200);
	matrix_set_n(&m_cov, 200);
	matrix_init(&m_cov);
	for (i = 0; i < m_cov.m; i++)
	{
		for (j = 0; j < m_cov.n; j++)
		{
			matrix_write(&m_cov, i, j, 0);		//初始化为0
		}
	}
	
	//所有动作的总协方差矩阵m_cov_assemble			200*200
	matrix_set_m(&m_cov_assemble, 200);
	matrix_set_n(&m_cov_assemble, 200);
	matrix_init(&m_cov_assemble);
	for (i = 0; i < m_cov_assemble.m; i++)
	{
		for (j = 0; j < m_cov_assemble.n; j++)
		{
			if(i == j)
			{
			  matrix_write(&m_cov_assemble, i, j, 1);   //对角线初始化为1		？？？？？？
			}
			else
			{
			  matrix_write(&m_cov_assemble, i, j, 0);   //其余位置初始化为0
			}
		}
	}
	
	//所有动作的总协方差矩阵的逆矩阵m_cov_assemble_inverse	200*200
	matrix_set_m(&m_cov_assemble_inverse, 200);
	matrix_set_n(&m_cov_assemble_inverse, 200);
	matrix_init(&m_cov_assemble_inverse);
	for (i = 0; i < m_cov_assemble_inverse.m; i++)
	{
		for (j = 0; j < m_cov_assemble_inverse.n; j++)
		{
			matrix_write(&m_cov_assemble_inverse, i, j, 0);		//初始化为0
		}
	}
	
	//动作1的平均特征向量	1*200
	matrix_set_m(&m_mean_1, 1);
	matrix_set_n(&m_mean_1, 200);
	matrix_init(&m_mean_1);
	
	//动作2的平均特征向量	1*200
	matrix_set_m(&m_mean_2, 1);
	matrix_set_n(&m_mean_2, 200);
	
	matrix_init(&m_mean_2);
	//动作3的平均特征向量	1*200
	matrix_set_m(&m_mean_3, 1);
	matrix_set_n(&m_mean_3, 200);
	matrix_init(&m_mean_3);
	
	motion_flag = 1;				//默认从动作1开始
	
}



/*训练时 每采一次后提取一次特征*/


/*预测时提取当前的特征*/


