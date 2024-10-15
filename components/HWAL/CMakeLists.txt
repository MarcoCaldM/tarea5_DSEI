idf_component_register(SRCS "HAL.c"
                    REQUIRES driver esp_adc esp_timer
                    INCLUDE_DIRS "." "BSP")
