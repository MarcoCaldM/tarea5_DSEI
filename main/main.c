#include "application.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t kMeansHandle;

void app_main(void)
{   
    periphInit();
    systemInit();
    #if !RTOS
        while(1){
            systemBehavior();
        }
    #elif RTOS
        //xTaskCreate(vADC, "vADC", 4096, NULL, 9, &adcHandle);
        xTaskCreate(vSystem, "vADC", 2048, NULL, 8, &systemHandle);
        xTaskCreate(vKMeans, "vKMeans", 2048, NULL, 5, &kMeansHandle);
        xTaskCreate(vButton, "vButton", 2048, NULL, 10, &buttonHandle);
    #endif
}
