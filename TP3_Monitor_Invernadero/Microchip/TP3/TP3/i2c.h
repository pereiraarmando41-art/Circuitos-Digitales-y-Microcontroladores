#ifndef I2C_H
#define I2C_H
#include "utils.h"
	
// Inicialización
void I2C_Init(void);
	
// Enviar START y esperar confirmación
void I2C_Start(void);
	
// Enviar datos
void I2C_Write(uint8_t data);
	
// Recibir datos
uint8_t I2C_Read(uint8_t ack);
	
// Enviar STOP
void I2C_Stop(void);
#endif