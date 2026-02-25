#include "mpu6050_manager.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <math.h>

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000


#define MPU6050_ADDR 0x68

static const char *TAG = "MPU6050";
// ===== 低通滤波变量 =====
static float roll_filtered = 0;
static float pitch_filtered = 0;
// ===== 校准相关变量 =====
static float roll_offset = 0;
static int calibration_count = 0;
static float calibration_sum = 0;
static bool calibrated = false;




static float roll = 0;
static float pitch = 0;

static void i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

static esp_err_t mpu_write(uint8_t reg, uint8_t data)
{
    uint8_t write_buf[2] = {reg, data};
    return i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR,
                                      write_buf, 2, pdMS_TO_TICKS(100));
}

static esp_err_t mpu_read(uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM,
                                        MPU6050_ADDR,
                                        &reg, 1,
                                        data, len,
                                        pdMS_TO_TICKS(100));
}

// void mpu6050_init(void)
// {
//     i2c_master_init();

//     // 退出睡眠模式
//     mpu_write(0x6B, 0x00);

//     ESP_LOGI(TAG, "MPU6050 Initialized");
// }
void mpu6050_init(void)
{
    i2c_master_init();

    mpu_write(0x6B, 0x00);  // 退出睡眠
    mpu_write(0x1A, 0x03);  // DLPF 42Hz

    ESP_LOGI(TAG, "MPU6050 Initialized");
}

void mpu6050_update(void)
{
    uint8_t data[6];

    mpu_read(0x3B, data, 6);

    int16_t ax = (data[0] << 8) | data[1];
    int16_t ay = (data[2] << 8) | data[3];
    int16_t az = (data[4] << 8) | data[5];

    float fax = ax / 16384.0;
    float fay = ay / 16384.0;
    float faz = az / 16384.0;

    float roll_raw = atan2(fay, faz) * 57.3;

    // ===== 软件低通滤波 =====
    float alpha = 0.1;
    roll_filtered = alpha * roll_raw + (1 - alpha) * roll_filtered;

    // ===== 自动校准阶段 =====
    if (!calibrated)
    {
        calibration_sum += roll_filtered;
        calibration_count++;

        if (calibration_count > 100)
        {
            roll_offset = calibration_sum / calibration_count;
            calibrated = true;
            ESP_LOGI(TAG, "Calibration done, offset=%.2f", roll_offset);
        }
        return;
    }

    roll = roll_filtered - roll_offset;

    ESP_LOGI(TAG, "Roll: %.2f", roll);
}


float mpu_get_roll(void)
{
    // ===== 角度死区处理 =====
    if (fabs(roll) < 1.0)
        return 0;

    return roll;
}

float mpu_get_pitch(void)
{
    return pitch;
}