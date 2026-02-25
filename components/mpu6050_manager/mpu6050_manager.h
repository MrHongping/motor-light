#ifndef MPU6050_MANAGER_H
#define MPU6050_MANAGER_H

void mpu6050_init(void);
void mpu6050_update(void);

float mpu_get_roll(void);
float mpu_get_pitch(void);

#endif