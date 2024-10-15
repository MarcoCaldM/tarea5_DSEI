#include "HAL.h"

adc_oneshot_unit_handle_t adc_handle = NULL;
adc_cali_handle_t cali_handle = NULL;

void GPIO_Set_Interrupt(gpio_num_t puerto, gpio_isr_t function){
    GPIO_Set(puerto, GPIO_MODE_INPUT);
    GPIO_PullMode(puerto, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(puerto, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(puerto, function, NULL);
    gpio_intr_enable(puerto);
}


void ADC_Init(){
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &config);
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_5, &config);
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_4, &config);


    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle));
}

void ADC_Channel_Init(adc_channel_t channel){
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    adc_oneshot_config_channel(adc_handle, channel, &config);
}

void ADC_Read(adc_channel_t channel, int *adc_reading){
    int raw_reading;
    adc_oneshot_read(adc_handle, channel, &raw_reading);
    adc_cali_raw_to_voltage(cali_handle, raw_reading, adc_reading);

}

void UART_Init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,                    // BaudRate: 115200
        .data_bits = UART_DATA_8_BITS,          // Resolución: 8 bits
        .parity    = UART_PARITY_DISABLE,       // Sin paridad
        .stop_bits = UART_STOP_BITS_1,          // Un bit de paro
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE   // Control de flujo desactivado
    };
    
    // UART Principal
    uart_param_config(UART_PORT, &uart_config);    // Configurado en el UART0, pines predeterminados
    uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, 1024*2, 0, 0, NULL, 0);
}

void UART_Write(const char* data) {
    uart_write_bytes(UART_PORT, data, strlen(data));
}