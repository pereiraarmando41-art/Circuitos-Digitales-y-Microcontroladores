#include "rtc.h"
#include "i2c.h"

// Arreglo optimizado para enviar al RTC
// Índice 0-2: Datos de tiempo (SS, MM, HH)
static uint8_t tiempo_actual[3] = {0, 30, 14};  // 14:30:00

static uint8_t bcd_to_dec(uint8_t bcd) {
	return ((bcd / 16) * 10) + (bcd % 16);
}

static uint8_t dec_to_bcd(uint8_t dec) {
	return ((dec / 10) * 16) + (dec % 10);
}

// Iniciar RTC
void RTC_Init(void){
    // Inicializar el hardware del periférico TWI (TWBR, TWSR, TWEN)
	I2C_Init();	
	
	// Configurar hora inicial
	RTC_Set_Time(tiempo_actual);
}

// Modificar la hora
void RTC_Set_Time(uint8_t datos[3]){
    // Iniciar comunicación con el RTC
    I2C_Start();
	
    // Transmitir dirección del esclavo (0x68 << 1) + Bit de Escritura (0)
	I2C_Write((DS3231_ADDRESS << 1) | I2C_WRITE);
	
    // Transmitir la Dirección de Acceso (Registro 00h = Segundos)
    I2C_Write(0x00);
	
	for(uint8_t i = 0; i < 3; i++ ) {		// Transmite los datos
		I2C_Write(dec_to_bcd(datos[i]));
	}
	
	// Enviar STOP
	I2C_Stop();
}

// Obtener la hora
void RTC_Get_Time(uint8_t *datos){
	uint8_t seg, min, hor;
	
	// Iniciar comunicación con el RTC
	I2C_Start();
	
	// Transmitir dirección del esclavo (0x68 << 1) + Bit de Escritura (0)
	I2C_Write((DS3231_ADDRESS << 1) | I2C_WRITE);
	
	// Transmitir la Dirección de Acceso (Registro 00h = Segundos)
	I2C_Write(0x00);
	
	// Transmite una condición de INICIO (INICIO REPETIDO)
	I2C_Start();
	
	// Transmitir dirección del esclavo (0x68 << 1) + Bit de Lectura (1)
	I2C_Write((DS3231_ADDRESS << 1) | I2C_READ);
	
	seg = bcd_to_dec(I2C_Read(0));		// Leer segundos y enviar ACK
	min = bcd_to_dec(I2C_Read(0));		// Leer minutos y enviar ACK
	hor = bcd_to_dec(I2C_Read(1));		// Leer horas y enviar NACK (fin de lectura)
	
	// Enviar STOP
	I2C_Stop();
	
	// Verificar que los valores sean válidos
	if (seg > 59) seg = 0;
	if (min > 59) min = 0;
	if (hor > 23) hor = 12;
	
	datos[0] = seg;
	datos[1] = min;
	datos[2] = hor;
}