#ifndef RTC_H
#define RTC_H
#include "utils.h"

#define I2C_WRITE 0
#define I2C_READ 1

// Iniciar RTC
void RTC_Init(void);

// Modificar la hora	
void RTC_Set_Time(uint8_t *tiempo);

// Obtener la hora	
void RTC_Get_Time(uint8_t *datos);

#endif