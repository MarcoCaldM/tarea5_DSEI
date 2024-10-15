#ifndef BSP_H
#define BSP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

//Definiciones generales
#define TRUE    1
#define FALSE   0
#define LOW FALSE
#define HIGH TRUE

// Definiciones de pines y canales
#ifdef ESP32
    #define LED_PIN     GPIO_NUM_2
    #define BUTTON_PIN  GPIO_NUM_19
    #define UART_PORT   UART_NUM_0
    #define LED_ON 1
    #define LED_OFF 0
#endif

#ifdef ESP32C3
    #define LED_PIN     GPIO_NUM_8
    #define BUTTON_PIN  GPIO_NUM_0
    #define ADC_CHANNEL ADC_CHANNEL_1
    #define UART_PORT   UART_NUM_0
    #define LED_ON 0
    #define LED_OFF 1
#endif

#endif