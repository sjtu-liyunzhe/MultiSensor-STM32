#include "feature.h"
#include "sys.h"

u8 feature_extract_times = 0;	//ͬһ�����£�������ȡ�Ĵ���

//////////////////////////////////// ���ѵ��ʱ������ /////////////////////////////////////

/******************************************************************
 * @Function	Get_Win_Mean
 * @Brief		��vector_i�У�ĳ�����ڵĵ�ľ�ֵ
 * @Parameter	ָ��vector��ָ��(����ַ); ����len; ��index����
 ******************************************************************/
//ֻ�ô����źŵľ�ֵ��Ϊ����
float Get_Win_Mean(float *vector, u16 len, u16 index)
{
	float win_mean;
	u16 i = 0;
	for (i = (index * len); i < (index * len + len); i++)
	{
		win_mean += (*(vector + i));	//���
	}
	win_mean = win_mean / len;			//�ô��ľ�ֵ
	return win_mean;
}

/****************************************************************************
 * @Function	Get_Feature_Matrix
 * @Brief		��⵱ǰ��������������ѵ������	m_feature
 * @Parameter	1��trial��ȡ�м�3s����30֡��ͨ��feature_extract_times������
 ***************************************************************************/
//frameÿ�μ�1��ʱ�򣬾͵�����������õ����������ĳһ�С���������ά�ȣ�30*200

/*	ÿһ֡��ȡһ��������ÿ��Ϊһ֡����ȡ����������mean1--mean50��һ��ͨ����ÿ�����ľ�ֵ
						CH1						CH2					CH3			CH4		
			| mean1 mean2... mean50		mean1 mean2... mean50		...			...	|	
m_feature = | mean1 mean2... mean50		mean1 mean2... mean50		...			...	|	
			| mean1 mean2... mean50		mean1 mean2... mean50		...			...	|	
*/

void Get_Feature_Matrix(void)
{
	u8 row_i = 0;	//������������m_feature����
	u8 ch_i = 0;	//�����ǵڼ���ͨ��
	u8 win_i = 0;	//ĳ��ͨ���еĵڼ�������, ���������: index of window (start from 0 to win_num-1)
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
	if (feature_extract_times < 29)		//��ȡ30֡��������ÿ֡����Ϊ���������һ��
	{
		feature_extract_times++;
	}
	else
	{
		feature_extract_times = 0;
	}	
}


/////////////////////////////// ��ȡ��ǰ�����µ�ʵʱ���� ///////////////////////////////

/****************************************************************************
 * @Function	Get_Current_Feature
 * @Brief		��⵱ǰ������ʵʱ�������������Լ���	m_feature_vector
 * @Parameter	m_feature_vectorΪ���������൱��m_feature��ĳ�е�ת��
 ***************************************************************************/
//m_feature_vectorΪ��������Ϊ��ǰʱ���µ�ʵʱ����������ά��200*1
/*	
							CH1							CH2					CH3		CH4
m_feature_vector = (mean1 mean2 ... mean50		mean1 mean2 ... mean50		...		...)T
*/

void Get_Current_Feature(void)
{
	u8 col_i = 0;	//����ʵʱ��������m_feature_vector����
	u8 ch_i = 0;	//�����ǵڼ���ͨ��
	u8 win_i = 0;	//ĳ��ͨ���еĵڼ�������, ���������: index of window (start from 0 to win_num-1)
	
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


/////////////////////////// ��ʼ�����������еľ��� ////////////////////////////

/******************************************************************
 * @Function	Feature_Matrix_Init
 * @Brief		Ϊ���������еľ��������ڴ棬�趨����ĳ�ʼֵ
 * @Parameter
 ******************************************************************/
void Feature_Matrix_Init(void)
{
	int i = 0;	//���ڳ�ʼ���������
	int j = 0;	//���ڳ�ʼ���������
	
	//��ǰ��������������m_feature	30*200
	matrix_set_m(&m_feature, 30);
	matrix_set_n(&m_feature, 200);
	matrix_init(&m_feature);
	for (i = 0; i < m_feature.m; i++)
	{
		for (j = 0; j < m_feature.n; j++)
		{
			matrix_write(&m_feature, i, j, 0);		//��ʼ��Ϊ0
		}
	}	
	
	//Ԥ��ʱ,��ǰ������ʵʱ��������m_feature_vector	200*1
	matrix_set_m(&m_feature_vector, 200);
	matrix_set_n(&m_feature_vector, 1);
	matrix_init(&m_feature_vector);
	for (i = 0; i < m_feature_vector.m; i++)
	{
		for (j = 0; j < m_feature_vector.n; j++)
		{
			matrix_write(&m_feature_vector, i, j, 1);	//��ʼ��Ϊ1
		}
	}
	
	//����������Э�������m_cov						200*200
	matrix_set_m(&m_cov, 200);
	matrix_set_n(&m_cov, 200);
	matrix_init(&m_cov);
	for (i = 0; i < m_cov.m; i++)
	{
		for (j = 0; j < m_cov.n; j++)
		{
			matrix_write(&m_cov, i, j, 0);		//��ʼ��Ϊ0
		}
	}
	
	//���ж�������Э�������m_cov_assemble			200*200
	matrix_set_m(&m_cov_assemble, 200);
	matrix_set_n(&m_cov_assemble, 200);
	matrix_init(&m_cov_assemble);
	for (i = 0; i < m_cov_assemble.m; i++)
	{
		for (j = 0; j < m_cov_assemble.n; j++)
		{
			if(i == j)
			{
			  matrix_write(&m_cov_assemble, i, j, 1);   //�Խ��߳�ʼ��Ϊ1		������������
			}
			else
			{
			  matrix_write(&m_cov_assemble, i, j, 0);   //����λ�ó�ʼ��Ϊ0
			}
		}
	}
	
	//���ж�������Э�������������m_cov_assemble_inverse	200*200
	matrix_set_m(&m_cov_assemble_inverse, 200);
	matrix_set_n(&m_cov_assemble_inverse, 200);
	matrix_init(&m_cov_assemble_inverse);
	for (i = 0; i < m_cov_assemble_inverse.m; i++)
	{
		for (j = 0; j < m_cov_assemble_inverse.n; j++)
		{
			matrix_write(&m_cov_assemble_inverse, i, j, 0);		//��ʼ��Ϊ0
		}
	}
	
	//����1��ƽ����������	1*200
	matrix_set_m(&m_mean_1, 1);
	matrix_set_n(&m_mean_1, 200);
	matrix_init(&m_mean_1);
	
	//����2��ƽ����������	1*200
	matrix_set_m(&m_mean_2, 1);
	matrix_set_n(&m_mean_2, 200);
	
	matrix_init(&m_mean_2);
	//����3��ƽ����������	1*200
	matrix_set_m(&m_mean_3, 1);
	matrix_set_n(&m_mean_3, 200);
	matrix_init(&m_mean_3);
	
	motion_flag = 1;				//Ĭ�ϴӶ���1��ʼ
	
}



/*ѵ��ʱ ÿ��һ�κ���ȡһ������*/


/*Ԥ��ʱ��ȡ��ǰ������*/


