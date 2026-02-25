idf_component_register(
    SRCS "mpu6050_manager.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer
)