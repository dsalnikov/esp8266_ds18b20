#include "ds18b20.h"

// сброс датчика
uint8_t therm_reset(){
    uint8_t i;
    // опускаем ногу вниз на 480uS
    THERM_LOW();
    THERM_OUTPUT_MODE();
    _delay_us(480);             // замените функцию задержки на свою
    // подымаем линию на 60uS
    THERM_INPUT_MODE();
    _delay_us(60);
    // получаем значение на линии в период 480uS
    i=THERM_GET_VAL();
    _delay_us(420);
    // возвращаем значение (0=OK, 1=датчик не найден)
    return i;
}
 
 
// функция отправки бита
void therm_write_bit(uint8_t bit){
    // опускаем на 1uS
    THERM_LOW();
    THERM_OUTPUT_MODE();
    _delay_us(1);
    // если хотим отправить 1, поднимаем линию (если нет, оставляем как есть)
    if(bit) THERM_INPUT_MODE();
    // ждем 60uS и поднимаем линию
    _delay_us(60);
    THERM_INPUT_MODE();
}
 
// чтение бита
uint8_t therm_read_bit(void){
    uint8_t bit=0;
    // опускаем на 1uS
    THERM_LOW();
    THERM_OUTPUT_MODE();
    _delay_us(1);
    // поднимаем на 14uS
    THERM_INPUT_MODE();
    _delay_us(14);
    // читаем состояние
    if(THERM_GET_VAL()) bit=1;
    // ждем 45 мкс и возвращаем значение
    _delay_us(45);
    return bit;
}
 
// читаем байт
uint8_t therm_read_byte(void){
    uint8_t i=8, n=0;
    while(i--){
        // сдвигаем в право на 1 и сохраняем следующее значение
        n>>=1;
        n|=(therm_read_bit()<<7);
    }
    return n;
}
 
// отправляем байт
void therm_write_byte(uint8_t byte){
    uint8_t i=8;
    while(i--){
        // отправляем бит и сдвигаем вправо на 1
        therm_write_bit(byte&1);
        byte>>=1;
    }
}
 
// читаем температуру с датчика
void therm_read_temperature(uint8_t *buffer){
    uint8_t temperature[2];
    int8_t digit;
    uint16_t decimal;
    uint8_t tmp;
 
    tmp = therm_reset();
    if (tmp)
    {
        os_sprintf(buffer, "Not found.\r\n");
        return;
    }
    therm_write_byte(THERM_CMD_SKIPROM);
    therm_write_byte(THERM_CMD_CONVERTTEMP);
 
    while(!therm_read_bit());
 
    therm_reset();
    therm_write_byte(THERM_CMD_SKIPROM);
    therm_write_byte(THERM_CMD_RSCRATCHPAD);
 
    temperature[0]=therm_read_byte();
    temperature[1]=therm_read_byte();
    therm_reset();
 
    digit=temperature[0]>>4;
    digit|=(temperature[1]&0x7)<<4;
 
    decimal=temperature[0]&0xf;
    decimal*=THERM_DECIMAL_STEPS_12BIT;
     
    os_sprintf(buffer, "%+d.%04u C\r\n", digit, decimal);
}
