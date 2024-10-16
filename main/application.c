#include "application.h"
#include "BSP.h"
#include "HAL.h"
#include <math.h>

Sensors sensor[3] = {
    {NULL, false, ADC_CHANNEL_6, 0, 1},
    {NULL, false, ADC_CHANNEL_5, 0, 2},
    {NULL, false, ADC_CHANNEL_4, 0, 3}
};

#if RTOS
    //TaskHandle_t adcHandle = NULL;
    TaskHandle_t systemHandle = NULL;
    TaskHandle_t buttonHandle = NULL;
    bool buttonState = true;
#elif !RTOS
#endif

static char message[50];
static float adcRead;
static float current_point[3] = {0.0, 0.0, 0.0};
static int adcStatusAll = 0;

bool systemState = true;

#if !RTOS
    static void IRAM_ATTR systemInterrupt(void* arg){
        systemState = !systemState;
    }
#endif

void periphInit(void){
    ADC_Init();

    GPIO_Set(LED_PIN, GPIO_MODE_OUTPUT);
    UART_Init();
    #if RTOS
        GPIO_Set(BUTTON_PIN, GPIO_MODE_INPUT);
        GPIO_PullMode(BUTTON_PIN, GPIO_PULLUP_ONLY);
    #elif !RTOS     
        GPIO_Set_Interrupt(BUTTON_PIN, systemInterrupt);
    #endif
}

void systemInit(void){
    systemState = true;
    GPIO_Write(LED_PIN, systemState);

    for(int i = 0; i < 3; i++) {
        xTaskCreate(vSensorMonitor, "vSensorMonitor", 2048, (void *)&sensor[i], 5, NULL);
    }
    
}

// Funcion para calcular la distancia euclidiana entre dos puntos
double euclidean_distance(Point a, float b[3]) {
    Sensors *sensors[3];

    for (int i = 0; i < 3; i++) {
        sensors[i] = &sensor[i]; // Apuntando a las estructuras existentes
    }

    return sqrt(pow(a.x - b[0], 2)*sensors[0]->adcStatus +
                pow(a.y - b[1], 2)*sensors[1]->adcStatus +
                pow(a.z - b[2], 2)*sensors[2]->adcStatus);
}

// Funcion para encontrar los K vecinos mas cercanos
void find_k_nearest_neighbors(float test_point[3], int k, int neighbors[]) {
    double* distances = malloc(NUM_POINTS * sizeof(double));
    int i, j;

    // Calcular las distancias entre el punto de prueba y todos los puntos en el dataset
    for (i = 0; i < NUM_POINTS; i++) {
        distances[i] = euclidean_distance(dataset[i], test_point);
    }

    // Ordenar los puntos por distancia usando una seleccion simple de k vecinos
    for (i = 0; i < k; i++) {
        double min_distance = 1e9;
        int min_index = -1;
        for (j = 0; j < NUM_POINTS; j++) {
            if (distances[j] < min_distance) {
                min_distance = distances[j];
                min_index = j;
            }
        }
        neighbors[i] = dataset[min_index].label;  // Almacenar la etiqueta del vecino
        distances[min_index] = 1e9;               // Excluir el punto ya seleccionado
    }

    free(distances); // Liberar memoria
}

// Funcion para clasificar el punto de prueba basado en la mayoria de las etiquetas
int classify(float test_point[3], int k) {
    int neighbors[K];
    int i, label1 = 0, label2 = 0, label3 = 0;

    // Encontrar los k vecinos más cercanos
    find_k_nearest_neighbors(test_point, k, neighbors);

    // Contar las etiquetas de los vecinos
    for (i = 0; i < k; i++) {
        if (neighbors[i] == 1) label1++;
        else if (neighbors[i] == 2) label2++;
        else if (neighbors[i] == 3) label3++;
    }

    // Devolver la etiqueta mayoritaria
    if (label1 > label2 && label1 > label3) return 1;
    else if (label2 > label1 && label2 > label3) return 2;
    else return 3;
}

#if !RTOS
    void systemBehavior(void){
        int32_t currentMillis = (int32_t) esp_timer_get_time()/1000;
        if(currentMillis - startMilis >= 1000){
            //ESP_LOGI(TAG, "ESTADO DEL SISTEMA: %s", systemState ? "ENCENDIDO" : "APAGADO");
            sprintf(message, "ESTADO DEL SISTEMA: %s\n", systemState ? "ENCENDIDO" : "APAGADO");
            if(!systemState)
                //ESP_LOGI(TAG, "NO DISPONIBLE");
                UART_Write("NO DISPONIBLE\n");
            else{
                //ESP_LOGI(TAG, "LECTURA DEL ADC: %d V", VOLTAGE_READ(ADC_CHANNEL));
                sprintf(message, "LECTURA DEL ADC: %d V\n", VOLTAGE_READ(ADC_CHANNEL));
                UART_Write(message);
            }
            startMilis = currentMillis;
        }
        GPIO_Write(LED_PIN, systemState ? LED_ON : LED_OFF);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

#elif RTOS
    void vADC(void *arg){
        Sensors *sensor = (Sensors*)arg;

        while(1){
            ADC_Read(sensor->channel, &sensor->adcRawRead);

            adcRead = ((((float)sensor->adcRawRead) / 1000)*(20.96/3.3))+16.94;
            current_point[sensor->sensorNum-1] = adcRead;
            sprintf(message,"Sensor%d: %.1f C\n", sensor->sensorNum, adcRead);
            UART_Write(message);
            //sprintf(message,"%s \n", sensor->adcStatus ? "ENCENDIDO" : "APAGADO");
            //UART_Write(message);

            vTaskDelay(750 / portTICK_PERIOD_MS);
        }
    }

    void vSystem(void *arg){
        while (1){
            GPIO_Write(LED_PIN, systemState);
            
            if(kNNHandle !=NULL){
                if(!systemState)
                    vTaskSuspend(kNNHandle);
                else
                    vTaskResume(kNNHandle);
            }

            // Verifica si no hay sensores conectados
            if (adcStatusAll == 0) {
                UART_Write("No hay dispositivos conectados.\n");
            }
            
            vTaskDelay(750 / portTICK_PERIOD_MS);
        }
    }

    void vSensorMonitor(void *arg){
        Sensors *sensor = (Sensors*)arg;
        bool adcLastState = false;
        while(1) {
            ADC_Read(sensor->channel, &sensor->adcRawRead);

            adcRead = ((float)sensor->adcRawRead) / 1000;
            bool adcCurrentState = (adcRead > 1);

            if (adcCurrentState != adcLastState) {
                if (adcCurrentState) {
                    // Sensor conectado
                    if (!sensor->adcStatus) {
                        xTaskCreate(vADC, "ADC_Task", 2048, (void *)sensor, 5, &sensor->taskHandle);
                        sensor->adcStatus = true;
                        adcStatusAll++;
                        //UART_Write("Sensor conectado, tarea ADC creada.\n");
                    }
                }
                else {
                    // Sensor desconectado
                    if (sensor->adcStatus) {
                        if (sensor->taskHandle != NULL) {
                            current_point[sensor->sensorNum-1] = 0;
                            vTaskDelete(sensor->taskHandle);
                            sensor->taskHandle = NULL;
                        }
                        sensor->adcStatus = false;
                        adcStatusAll--;
                        //UART_Write("Sensor desconectado, tarea ADC eliminada.\n");
                    }
                }
                adcLastState = adcCurrentState;
            }

            if(sensor->taskHandle !=NULL) {
                if(!systemState)
                    vTaskSuspend(sensor->taskHandle);
                else
                    vTaskResume(sensor->taskHandle);
            }

            vTaskDelay(750 / portTICK_PERIOD_MS);
        }
    }

    void vButton(void *arg){
        static int8_t buttonPressed = 0;
        static int32_t startMilis;
        while ((1))
        {
            int B = GPIO_Read(BUTTON_PIN);
            if (B != buttonState) {
                if(B == LOW){
                    if(buttonPressed == 0)
                        startMilis = (int32_t) esp_timer_get_time()/1000;
                    buttonPressed = buttonPressed * !(buttonPressed == 2);
                    buttonPressed++;  
                }
                buttonState = B;
            }

            int32_t currentMillis = (int32_t) esp_timer_get_time()/1000;;

            if(currentMillis - startMilis >= 1000 && buttonPressed != 0){
                systemState = (buttonPressed == 1);
                buttonPressed = 0;
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }

    void vKNN(void *arg) {
        while (1) {
            int label = classify(current_point, K);
            sprintf(message, "El punto (%.1f, %.1f, %.1f) pertenece a la clase: %d\n",
                    current_point[0], current_point[1], current_point[2], label);
            UART_Write(message);
            vTaskDelay(750 / portTICK_PERIOD_MS);
        }
    }
#endif