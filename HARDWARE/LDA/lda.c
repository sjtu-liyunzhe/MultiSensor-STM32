#include "lda.h"
#include "sys.h"
#include "usart.h"


///////////////////////////////// ѵ��LDAģ�� ////////////////////////////////

//��ĳ����������������m_featureת��Ϊ�ö�����ƽ����������m_mean_i��Э�������m_cov
//���У�����ö�����ƽ���������������ں�����ʶ��
//ÿ��������Э������󲻱������������Ǳ������ж�������Э�������m_cov_assemble	(����������Э���������ӵõ�)

/*********************************************************************
 * @Function	Training
 * @Brief		�ɼ�����ȡ��һ�������������󣬽���ѵ��
 * @Parameter	����ö�����ƽ���������������ж�������Э�������(ģ��)
 *********************************************************************/

void Training(void)
{
	int i = 0;	//���ڳ�ʼ���������
	int j = 0;	//���ڳ�ʼ���������
	//����һЩ�м�����������;���
	struct _Matrix m_vector_1;		//1*30		ֵȫΪ1��������(1,1,1,...,1)	30Ϊһ����������֡��frame_num
	struct _Matrix m_vector_2;		//1*200		������							m_vector_2 = m_vector_1 * m_feature
									//			�������������ÿһ�е�30��ֵ���
	struct _Matrix m_vector_3;		//30*1		ֵȫΪ1��������(1,1,1,...,1)T
	struct _Matrix m_vector_mean;	//1*200		��ǰ������ƽ����������			m_vector_mean = 1/30 * m_vector_2
	struct _Matrix m_const;			//1*1		����	ϵ����1/30��1��
	struct _Matrix m_cov1;			//30*200	ƽ������������չ������			m_cov1 = m_vector_3 * m_vector_mean
	struct _Matrix m_cov2;			//30*200	���������ÿ��Ԫ�ؼ�ȥƽ��ֵ	m_cov2 = m_feature - m_cov1
	struct _Matrix m_cov3;			//200*30	m_cov3��m_cov2��ת�ã��������Ϊÿ��������Э�������	m_cov= m_cov3 * m_cov2
	
	/*************************** ��ʼ��������Щ���� ***************************/
	
	//m_vector_1	1*30	ֵȫΪ1��������
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
			matrix_write(&m_vector_2, i, j, 0);		//��ʼ��Ϊ0
		}
	}	
	
	//m_vector_3	30*1	ֵȫΪ1��������
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
			matrix_write(&m_vector_mean, i, j, 0);	//��ʼ��Ϊ0
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
			matrix_write(&m_const, i, j, 1.0/30.0);	//��ʼ��Ϊ1.0/30.0
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
			matrix_write(&m_cov1, i, j, 0);			//��ʼ��Ϊ0
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
			matrix_write(&m_cov2, i, j, 0);			//��ʼ��Ϊ0
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
			matrix_write(&m_cov3, i, j, 0);			//��ʼ��Ϊ0
		}
	}	

	/*************************** ���Э������� ***************************/
	matrix_multiply(&m_vector_1, &m_feature, &m_vector_2);	//m_vector_2 = m_vector_1 * m_feature	��������ÿ�����
	matrix_multiply(&m_const, &m_vector_2, &m_vector_mean);	//m_vector_mean = 1/30 * m_vector_2		��ǰ������ƽ����������
	matrix_multiply(&m_vector_3, &m_vector_mean, &m_cov1);	//m_cov1 = m_vector_3 * m_vector_mean	ƽ������������չ������
	matrix_subtract(&m_feature, &m_cov1, &m_cov2);        	//m_cov2 = m_feature - m_cov1			(x-E(x))
	matrix_transpos(&m_cov2, &m_cov3);                   	//m_cov3 = (m_cov2)T					(x-E(x))T
	matrix_multiply(&m_cov3, &m_cov2, &m_cov);            	//m_cov= m_cov3 * m_cov2	�ö�����Э�������(��)=(x-E(x))*(x-E(x))T
	matrix_add(&m_cov, &m_cov_assemble, &m_cov_assemble); 	//m_cov_assemble += m_cov	���ж�������Э������� = ����������Э�������ĺ�
	
			
	/*********** �ѵ�ǰ������ƽ������������ֵ����Ӧ�Ķ������������� ***********/
	matrix_write(&m_const, 0, 0, 1);
	
	switch (motion_flag)
	{
		case 1:
			matrix_multiply(&m_const, &m_vector_mean, &m_mean_1);	//����1��ƽ����������
			break;
		case 2:
			matrix_multiply(&m_const, &m_vector_mean, &m_mean_2);	//����2��ƽ����������
			break;
		case 3:
			matrix_multiply(&m_const, &m_vector_mean, &m_mean_3);	//����3��ƽ����������
			break;	
		default:
			break;
	}
	motion_flag ++;
	if (motion_flag > 3)
	{
		motion_flag = 1;
	}
		
	/*************************** �ͷ��ڴ� ***************************/
	matrix_free(&m_vector_1);
	matrix_free(&m_vector_2);
	matrix_free(&m_vector_3);
	matrix_free(&m_vector_mean);
	matrix_free(&m_const);
	matrix_free(&m_cov1);
	matrix_free(&m_cov2);
	matrix_free(&m_cov3);		
}


//////////////////////////////////// ������ ///////////////////////////////////////

/******************************************************************
 * @Function	Classifier
 * @Brief
 * @Parameter	���ط���Ķ������
 ******************************************************************/

u32 Classifier(void)
{
	int i = 0;	//���ڳ�ʼ���������
	int j = 0;	//���ڳ�ʼ���������
	float predict[3];				//�洢ÿ�ද�����б�ʽ��ֵ
	float max = 0;					//�洢�б�ʽ�����ֵ
	u32 class_flag = 1;	//���������صĽ��
	
	//����һЩ�м�����������;���
	//�б�ʽ:	P(x)=((ui)T)((��)-1)X - 1/2((ui)T)((��)-1)(ui)
	//(ui)T: m_mean_i	200*1,	(��): 200*200,	X: m_feature_vector	200*1
	
 	struct _Matrix vector;			//200*1		vector = m_cov_assemble_inverse * m_feature_vector	((��)-1)X
	struct _Matrix vector_w;		//200*1		vector_w = (m_mean_i)T								(ui)
	struct _Matrix vector_for_w;	//200*1		vector_for_w = m_cov_assemble_inverse * vector_w	((��)-1)(ui)
	
	struct _Matrix const1;			//1*1		const1 = m_mean_1 * vector							((u1)T)((��)-1)X
	struct _Matrix const2;			//1*1		const2 = m_mean_2 * vector							((u2)T)((��)-1)X
	struct _Matrix const3;			//1*1		const3 = m_mean_3 * vector							((u3)T)((��)-1)X

	struct _Matrix const1_w;		//1*1		const1_w = m_mean_1 * vector_for_w					((u1)T)((��)-1)u1
	struct _Matrix const2_w;		//1*1		const2_w = m_mean_2 * vector_for_w					((u2)T)((��)-1)u2
	struct _Matrix const3_w;		//1*1		const3_w = m_mean_3 * vector_for_w					((u3)T)((��)-1)u3

	/*************************** ��ʼ��������Щ���� ***************************/
	//vector	200*1
	matrix_set_m(&vector, 200);
	matrix_set_n(&vector, 1);
	matrix_init(&vector);
	for (i = 0; i < vector.m; i++)
	{
		for (j = 0; j < vector.n; j++)
		{
			matrix_write(&vector, i, j, 0);		//��ʼ��Ϊ0
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
			matrix_write(&vector_w, i, j, 0);	//��ʼ��Ϊ0
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
			matrix_write(&vector_for_w, i, j, 0);	//��ʼ��Ϊ0
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
			matrix_write(&const1, i, j, 0);	//��ʼ��Ϊ0
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
			matrix_write(&const2, i, j, 0);	//��ʼ��Ϊ0
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
			matrix_write(&const3, i, j, 0);	//��ʼ��Ϊ0
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
			matrix_write(&const1_w, i, j, 0);	//��ʼ��Ϊ0
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
			matrix_write(&const2_w, i, j, 0);	//��ʼ��Ϊ0
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
			matrix_write(&const3_w, i, j, 0);	//��ʼ��Ϊ0
		}
	}					
	
	/*************************** ������ද�����б�ʽ ***************************/
	//�б�ʽ:	P(x)=((ui)T)((��)-1)X - 1/2((ui)T)((��)-1)(ui)
	//ѵ�������ж����󣬻������Э����������m_cov_assemble_inverse
	matrix_multiply(&m_cov_assemble_inverse, &m_feature_vector, &vector);	//vector = m_cov_assemble_inverse * m_feature_vector		((��)-1)X
	
	//����1
	matrix_multiply(&m_mean_1, &vector, &const1);      						//const1 = m_mean_1 * vector								((ui)T)((��)-1)X
	//printf_matrix(&const1);
	matrix_transpos(&m_mean_1, &vector_w);            						//m_mean_1: 1*200, vector_w: 200*1							ui
	matrix_multiply(&m_cov_assemble_inverse, &vector_w, &vector_for_w);    	//vector_for_w = m_cov_assemble_inverse * vector_w			((��)-1)(ui)
	matrix_multiply(&m_mean_1, &vector_for_w, &const1_w);         			//const1_w = m_mean_1 * vector_for_w						((ui)T)((��)-1)ui
	//printf_matrix(&const1_w);
	predict[0] = matrix_read(&const1, 0, 0) - 0.5 * matrix_read(&const1_w, 0, 0);	//����1���б���
	
	//����2
	matrix_multiply(&m_mean_2, &vector, &const2);
	matrix_transpos(&m_mean_2, &vector_w);
	matrix_multiply(&m_cov_assemble_inverse, &vector_w, &vector_for_w);
	matrix_multiply(&m_mean_2, &vector_for_w, &const2_w);
	predict[1] = matrix_read(&const2, 0, 0) - 0.5 * matrix_read(&const2_w, 0, 0);	//����2���б���
	
	//����3
	matrix_multiply(&m_mean_3, &vector, &const3);
	matrix_transpos(&m_mean_3, &vector_w);
	matrix_multiply(&m_cov_assemble_inverse, &vector_w, &vector_for_w);
	matrix_multiply(&m_mean_3, &vector_for_w, &const3_w);
	predict[2] = matrix_read(&const3, 0, 0) - 0.5 * matrix_read(&const3_w, 0, 0);	//����3���б���
	
//	for(i = 0; i < sizeof(predict); i++)
//	{
//		printf("%f\n", predict[i]);
//	}

	/*************************** ���б������ֵ����Ӧ���� ***************************/
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
			class_flag = i+1;	//�������ֵ����Ӧ��(������+1), Ϊ��Ӧ�Ķ������
		}
	}
	return class_flag;			//���ط�����
	
	/*************************** �ͷ��ڴ� ***************************/
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



