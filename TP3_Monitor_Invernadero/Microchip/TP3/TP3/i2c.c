#include "i2c.h"

//	Inicialización
void I2C_Init(void){
	// Configurar frecuencia SCL = 100kHz
	// Para 16MHz: TWBR = ((16000000/100000) - 16) / 2 = 72
	TWBR = 72;
	TWSR = 0;  // Prescaler = 1
}

//	Enviar START y esperar confirmación
void I2C_Start(void){
	// Habilita a la interfaz TWI y transmite una condición de INICIO
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));						// Espera fin de condición de INICIO
}

//	Enviar datos
void I2C_Write(uint8_t data){
	// El TWDR (TWI Data Register) es el buffer donde se coloca el byte que se desea enviar
	TWDR = data;
	
	//TWEN (TWI Enable): Mantiene el perif?rico encendido y bajo el control de las terminales de comunicaci?n
	//TWINT (TWI Interrupt Flag): Es el bit m?s importante en esta l?nea.
	// la transmisi?n f?sica no comienza hasta que el software limpia esta bandera escribi?ndole un '1' l?gico
	// Al hacer esto, el hardware comienza a desplazar el byte contenido en TWDR hacia el bus
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

// Recibir datos
uint8_t I2C_Read(uint8_t ack){
	if (ack) {
		// Si es el último byte, enviar NACK
		TWCR = (1 << TWINT) | (1 << TWEN);
		} else {
		// Si no es el último, enviar ACK
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	}
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

// Enviar STOP
void I2C_Stop(void){
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}