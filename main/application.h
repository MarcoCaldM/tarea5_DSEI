#ifndef APPLICATION_H
#define APPLICATION_H

#include "HAL.h"

#if !KMEANS
    #include "dataset.h"
    extern TaskHandle_t kNNHandle;
    void vKNN(void *arg);
#elif KMEANS
    extern TaskHandle_t kMeansHandle;
    void vKMeans(void *arg);

    #define NUM_POINTS 3

    typedef struct {
        double x;
        double y;
        double z;
    } Point;
#endif

#if RTOS
    extern TaskHandle_t adcHandle;
    extern TaskHandle_t systemHandle;
    extern TaskHandle_t buttonHandle;
#endif

typedef struct {
    TaskHandle_t taskHandle;
    bool adcStatus;
    int channel;
    int adcRawRead;
    int sensorNum;
} Sensors;

void systemInit();
void periphInit(void);

#if !RTOS
    void systemBehavior();
#elif RTOS
    void vADC(void *arg);
    void vSystem(void *arg);
    void vSensorMonitor(void *arg);
    void vButton(void *arg);
#endif

#endif