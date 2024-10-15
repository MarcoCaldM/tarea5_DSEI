#ifndef APPLICATION_H
#define APPLICATION_H

#include "HAL.h"

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

//----------------------------
#define NUM_POINTS 3

typedef struct {
    double x;
    double y;
    double z;
} Point;
//----------------------------

void systemInit();
void periphInit(void);

#if !RTOS
    void systemBehavior();
#elif RTOS
    void vADC(void *arg);
    void vSystem(void *arg);
    void vSensorMonitor(void *arg);
    void vButton(void *arg);
    void vKMeans(void *arg);
#endif

#endif