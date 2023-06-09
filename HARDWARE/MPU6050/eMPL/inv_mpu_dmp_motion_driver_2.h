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
 *      @file       inv_mpu_dmp_motion_driver.h
 *      @brief      DMP image and interface functions.
 *      @details    All functions are preceded by the dmp_ prefix to
 *                  differentiate among MPL and general driver function calls.
 */
#ifndef _INV_MPU_DMP_MOTION_DRIVER_2_H_
#define _INV_MPU_DMP_MOTION_DRIVER_2_H_

#define TAP_X               (0x01)
#define TAP_Y               (0x02)
#define TAP_Z               (0x04)
#define TAP_XYZ             (0x07)

#define TAP_X_UP            (0x01)
#define TAP_X_DOWN          (0x02)
#define TAP_Y_UP            (0x03)
#define TAP_Y_DOWN          (0x04)
#define TAP_Z_UP            (0x05)
#define TAP_Z_DOWN          (0x06)

#define ANDROID_ORIENT_PORTRAIT             (0x00)
#define ANDROID_ORIENT_LANDSCAPE            (0x01)
#define ANDROID_ORIENT_REVERSE_PORTRAIT     (0x02)
#define ANDROID_ORIENT_REVERSE_LANDSCAPE    (0x03)

#define DMP_INT_GESTURE     (0x01)
#define DMP_INT_CONTINUOUS  (0x02)

#define DMP_FEATURE_TAP             (0x001)
#define DMP_FEATURE_ANDROID_ORIENT  (0x002)
#define DMP_FEATURE_LP_QUAT         (0x004)
#define DMP_FEATURE_PEDOMETER       (0x008)
#define DMP_FEATURE_6X_LP_QUAT      (0x010)
#define DMP_FEATURE_GYRO_CAL        (0x020)
#define DMP_FEATURE_SEND_RAW_ACCEL  (0x040)
#define DMP_FEATURE_SEND_RAW_GYRO   (0x080)
#define DMP_FEATURE_SEND_CAL_GYRO   (0x100)

#define INV_WXYZ_QUAT       (0x100)

/* Set up functions. */
int dmp_load_motion_driver_firmware_2(void);
int dmp_set_fifo_rate_2(unsigned short rate);
int dmp_get_fifo_rate_2(unsigned short *rate);
int dmp_enable_feature_2(unsigned short mask);
int dmp_get_enabled_features_2(unsigned short *mask);
int dmp_set_interrupt_mode_2(unsigned char mode);
int dmp_set_orientation_2(unsigned short orient);
int dmp_set_gyro_bias_2(long *bias);
int dmp_set_accel_bias_2(long *bias);

/* Tap functions. */
int dmp_register_tap_cb_2(void (*func)(unsigned char, unsigned char));
int dmp_set_tap_thresh_2(unsigned char axis, unsigned short thresh);
int dmp_set_tap_axes_2(unsigned char axis);
int dmp_set_tap_count_2(unsigned char min_taps);
int dmp_set_tap_time_2(unsigned short time);
int dmp_set_tap_time_multi_2(unsigned short time);
int dmp_set_shake_reject_thresh_2(long sf, unsigned short thresh);
int dmp_set_shake_reject_time_2(unsigned short time);
int dmp_set_shake_reject_timeout_2(unsigned short time);

/* Android orientation functions. */
int dmp_register_android_orient_cb_2(void (*func)(unsigned char));

/* LP quaternion functions. */
int dmp_enable_lp_quat_2(unsigned char enable);
int dmp_enable_6x_lp_quat_2(unsigned char enable);

/* Pedometer functions. */
int dmp_get_pedometer_step_count_2(unsigned long *count);
int dmp_set_pedometer_step_count_2(unsigned long count);
int dmp_get_pedometer_walk_time_2(unsigned long *time);
int dmp_set_pedometer_walk_time_2(unsigned long time);

/* DMP gyro calibration functions. */
int dmp_enable_gyro_cal_2(unsigned char enable);

/* Read function. This function should be called whenever the MPU interrupt is
 * detected.
 */
int dmp_read_fifo_2(short *gyro, short *accel, long *quat,
    unsigned long *timestamp, short *sensors, unsigned char *more);

#endif  /* #ifndef _INV_MPU_DMP_MOTION_DRIVER_H_ */

