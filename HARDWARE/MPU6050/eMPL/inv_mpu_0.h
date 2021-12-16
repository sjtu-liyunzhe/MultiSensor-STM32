/*
 $License:
    Copyright (C) 2011-2012 InvenSense Corporation, All Rights Reserved.
    See included License.txt for License information.
 $
 */
/**
 *  @addtogroup  DRIVERS Sensor Driver Layer
 *  @brief       Hardware drivers to communicate with sensors via I2C.
 *
 *  @{
 *      @file       inv_mpu.h
 *      @brief      An I2C-based driver for Invensense gyroscopes.
 *      @details    This driver currently works for the following devices:
 *                  MPU6050
 *                  MPU6500
 *                  MPU9150 (or MPU6050 w/ AK8975 on the auxiliary bus)
 *                  MPU9250 (or MPU6500 w/ AK8963 on the auxiliary bus)
 */

#ifndef _INV_MPU_0_H_
#define _INV_MPU_0_H_
#include "sys.h"

//定义输出速度
#define DEFAULT_MPU_HZ  (100)		//100Hz

#define INV_X_GYRO      (0x40)
#define INV_Y_GYRO      (0x20)
#define INV_Z_GYRO      (0x10)
#define INV_XYZ_GYRO    (INV_X_GYRO | INV_Y_GYRO | INV_Z_GYRO)
#define INV_XYZ_ACCEL   (0x08)
#define INV_XYZ_COMPASS (0x01)

//移植官方MSP430 DMP驱动过来
struct int_param_s_0 {
//#if defined EMPL_TARGET_MSP430 || defined MOTION_DRIVER_TARGET_MSP430
    void (*cb)(void);
    unsigned short pin;
    unsigned char lp_exit;
    unsigned char active_low;
//#elif defined EMPL_TARGET_UC3L0
//    unsigned long pin;
//    void (*cb)(volatile void*);
//    void *arg;
//#endif
};

#define MPU_INT_STATUS_DATA_READY       (0x0001)
#define MPU_INT_STATUS_DMP              (0x0002)
#define MPU_INT_STATUS_PLL_READY        (0x0004)
#define MPU_INT_STATUS_I2C_MST          (0x0008)
#define MPU_INT_STATUS_FIFO_OVERFLOW    (0x0010)
#define MPU_INT_STATUS_ZMOT             (0x0020)
#define MPU_INT_STATUS_MOT              (0x0040)
#define MPU_INT_STATUS_FREE_FALL        (0x0080)
#define MPU_INT_STATUS_DMP_0            (0x0100)
#define MPU_INT_STATUS_DMP_1            (0x0200)
#define MPU_INT_STATUS_DMP_2            (0x0400)
#define MPU_INT_STATUS_DMP_3            (0x0800)
#define MPU_INT_STATUS_DMP_4            (0x1000)
#define MPU_INT_STATUS_DMP_5            (0x2000)

/* Set up APIs */
int mpu_init_0(void);
// 找不到在哪定义的
int mpu_init_slave_0(void);
int mpu_set_bypass_0(unsigned char bypass_on);

/* Configuration APIs */
int mpu_lp_accel_mode_0(unsigned char rate);
int mpu_lp_motion_interrupt_0(unsigned short thresh, unsigned char time,
    unsigned char lpa_freq);
int mpu_set_int_level_0(unsigned char active_low);
int mpu_set_int_latched_0(unsigned char enable);

int mpu_set_dmp_state_0(unsigned char enable);
int mpu_get_dmp_state_0(unsigned char *enabled);

int mpu_get_lpf_0(unsigned short *lpf);
int mpu_set_lpf_0(unsigned short lpf);

int mpu_get_gyro_fsr_0(unsigned short *fsr);
int mpu_set_gyro_fsr_0(unsigned short fsr);

int mpu_get_accel_fsr_0(unsigned char *fsr);
int mpu_set_accel_fsr_0(unsigned char fsr);

int mpu_get_compass_fsr_0(unsigned short *fsr);

int mpu_get_gyro_sens_0(float *sens);
int mpu_get_accel_sens_0(unsigned short *sens);

int mpu_get_sample_rate_0(unsigned short *rate);
int mpu_set_sample_rate_0(unsigned short rate);
int mpu_get_compass_sample_rate_0(unsigned short *rate);
int mpu_set_compass_sample_rate_0(unsigned short rate);

int mpu_get_fifo_config_0(unsigned char *sensors);
int mpu_configure_fifo_0(unsigned char sensors);

int mpu_get_power_state_0(unsigned char *power_on);
int mpu_set_sensors_0(unsigned char sensors);

int mpu_set_accel_bias_0(const long *accel_bias);

/* Data getter/setter APIs */
int mpu_get_gyro_reg_0(short *data, unsigned long *timestamp);
int mpu_get_accel_reg_0(short *data, unsigned long *timestamp);
int mpu_get_compass_reg_0(short *data, unsigned long *timestamp);
int mpu_get_temperature_0(long *data, unsigned long *timestamp);

int mpu_get_int_status_0(short *status);
int mpu_read_fifo_0(short *gyro, short *accel, unsigned long *timestamp,
    unsigned char *sensors, unsigned char *more);
int mpu_read_fifo_0_stream(unsigned short length, unsigned char *data,
    unsigned char *more);
int mpu_reset_fifo_0(void);

int mpu_write_mem_0(unsigned short mem_addr, unsigned short length,
    unsigned char *data);
int mpu_read_mem_0(unsigned short mem_addr, unsigned short length,
    unsigned char *data);
int mpu_load_firmware_0(unsigned short length, const unsigned char *firmware,
    unsigned short start_addr, unsigned short sample_rate);

int mpu_reg_dump_0(void);
int mpu_read_reg_0(unsigned char reg, unsigned char *data);
int mpu_run_self_test_0(long *gyro, long *accel);
// 没有找到在哪定义
int mpu_register_tap_cb(void (*func)(unsigned char, unsigned char));
//自行添加的一些函数
// void mget_ms(unsigned long *time);
// unsigned short inv_row_2_scale(const signed char *row);
// unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx);
u8 run_self_test_0(void);
u8 mpu_dmp_init_0(void);
u8 mpu_dmp_get_data_0(float *pitch,float *roll,float *yaw);

#endif  /* #ifndef _INV_MPU_H_ */

