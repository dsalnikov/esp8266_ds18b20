#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "os_type.h"
#include "osapi.h"
#include "gpio.h"

#include <math.h>
#include <stdio.h>

/* Макросы для "дергания ноги" и изменения режима ввода/вывода */
#define THERM_INPUT_MODE()  gpio_output_set(0, 0, 0, BIT2)
#define THERM_OUTPUT_MODE() gpio_output_set(0, BIT2, BIT2, 0)
#define THERM_LOW()         gpio_output_set(0, BIT2, BIT2, 0)
#define THERM_HIGH()        gpio_output_set(BIT2, 0, BIT2, 0)

#define THERM_GET_VAL() GPIO_INPUT_GET(2)

#define _delay_us os_delay_us

uint8_t therm_reset();
void therm_write_bit(uint8_t bit);
uint8_t therm_read_bit(void);
uint8_t therm_read_byte(void);
void therm_write_byte(uint8_t byte);

void therm_read_temperature(uint8_t *buffer);

// команды управления датчиком
#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_WSCRATCHPAD 0x4e
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xb8
#define THERM_CMD_RPWRSUPPLY 0xb4
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_CMD_ALARMSEARCH 0xec
 
#define THERM_DECIMAL_STEPS_12BIT 625 //.0625

#endif //__DS18B20_H__
