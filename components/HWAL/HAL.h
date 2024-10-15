#ifndef HAL_H
#define HAL_H

#include "BSP.h"

//Declaracion de macros
#define GPIO_Write(gpio_num, value) gpio_set_level(gpio_num, value)
#define GPIO_Read(gpio_num) gpio_get_level(gpio_num)
#define GPIO_Set(gpio_num, mode) gpio_set_direction(gpio_num, mode)
#define GPIO_PullMode(gpio_num, mode) gpio_set_pull_mode(gpio_num, mode)

/*-----------------------------------------------------------*/
// Declaraciones de funciones

// ADC (Conversor Analógico-Digital)
void ADC_Init(void);    // Inicializar ADC
void ADC_Channel_Init(adc_channel_t channel);
void ADC_Read(adc_channel_t channel, int *adc_reading);   // Leer valor ADC


// GPIO
void GPIO_Set_Interrupt(gpio_num_t puerto, gpio_isr_t function);                               // Inicializar GPIO

// UART / Consola
void UART_Init(void);               // Inicializar UART
void UART_Write(const char* data);   // Enviar datos a la consola

#endif