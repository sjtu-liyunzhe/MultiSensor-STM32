#ifndef MATRIX_H  
#define MATRIX_H

//头文件  
#include <stdio.h>  
#include <stdlib.h>  
  
//////////////////////// 矩阵数据结构 //////////////////////////
//二维矩阵  
struct _Matrix  
{   
    int m;  		//行数
    int n;  		//列数
    float *arr; 	//矩阵所占内存的地址指针
}; 


////////////////////////// 矩阵方法 //////////////////////////

void matrix_set_m(struct _Matrix *m,int mm); 	//设置m  
void matrix_set_n(struct _Matrix *m,int nn);  	//设置n  
void matrix_init(struct _Matrix *m);  	//初始化矩阵，申请内存  
void matrix_free(struct _Matrix *m);  	//释放矩阵所占内存

//读取i,j坐标的数据  
//失败返回-31415,成功返回值  
float matrix_read(struct _Matrix *m,int i,int j);  

//写入i,j坐标的数据  
//失败返回-1,成功返回1  
int matrix_write(struct _Matrix *m,int i,int j,float val); 

//打印矩阵到串口
void printf_matrix(struct _Matrix *A);  

//////////////////////////// 矩阵运算 ///////////////////////////
//成功返回1,失败返回-1  
//C = A + B  
int matrix_add(struct _Matrix *A,struct _Matrix *B,struct _Matrix *C);  

//C = A - B  
//成功返回1,失败返回-1  
int matrix_subtract(struct _Matrix *A,struct _Matrix *B,struct _Matrix *C);  

//C = A * B  
//成功返回1,失败返回-1  
int matrix_multiply(struct _Matrix *A,struct _Matrix *B,struct _Matrix *C);  

//行列式的值,只能计算2 * 2,3 * 3  
//失败返回-31415,成功返回值  
float matrix_det(struct _Matrix *A);  

//求转置矩阵,B = A^T  
//成功返回1,失败返回-1  
int matrix_transpos(struct _Matrix *A,struct _Matrix *B);  

//求逆矩阵,B = A^(-1)  
//成功返回1,失败返回-1  
int matrix_inverse(struct _Matrix *A,struct _Matrix *B);  
  
#endif  
