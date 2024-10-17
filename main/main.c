#include "application.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if !KMEANS
    TaskHandle_t kNNHandle;
#elif KMEANS
    TaskHandle_t kMeansHandle;
#endif

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
        xTaskCreate(vButton, "vButton", 2048, NULL, 10, &buttonHandle);

        #if !KMEANS
            xTaskCreate(vKNN, "vKNN", 4096, NULL, 5, &kNNHandle);
        #elif KMEANS
            xTaskCreate(vKMeans, "vKMeans", 2048, NULL, 5, &kMeansHandle);
        #endif
        
    #endif
}
