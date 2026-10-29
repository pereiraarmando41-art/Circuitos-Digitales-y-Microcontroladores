#include "dht11.h"

// Definimos el pin para mayor legibilidad (asumiendo PC0)
#define DHT11_PIN PC0

// Inicialización del sensor
void DHT11_Init(void) {
	// Configurar pin como salida en alto (estado libre)
	DDRC |= (1 << DHT11_PIN);
	PORTC |= (1 << DHT11_PIN);
	
}

// --- Lectura del Sensor DHT11
uint8_t DHT11_Read(uint8_t *humedad, uint8_t *temperatura) {
	uint8_t bits[5] = {0, 0, 0, 0, 0}; // 5 bytes: RH(2), T(2), Checksum(1)
	uint8_t i, j;
	  
	// Señal de Inicio (MCU -> Sensor)
	DDRC |= (1 << DHT11_PIN);    // Pin como salida
	PORTC &= ~(1 << DHT11_PIN);  // Poner pin en BAJO (Pull-down)
	_delay_ms(18);                    // Mantener bajo por al menos 18ms

	// Liberar la línea y esperar respuesta del sensor
	PORTC |= (1 << DHT11_PIN);   // Poner pin en ALTO (Pull-up)
	_delay_us(30);                    // Esperar 20-40us
	DDRC &= ~(1 << DHT11_PIN);   // Cambiar a entrada (para escuchar)

	// Esperar respuesta del sensor con timeout (80us BAJO, 80us ALTO) 
	// Esperar a que el pin esté en BAJO (respuesta del sensor)
    uint16_t timeout = 0;
    while ((PINC & (1 << DHT11_PIN)) && (timeout < 100)) {
	    timeout++;
	    _delay_us(1);
    }
    if (timeout >= 100) {
	    return 0;  // Error: No se detectó pulso bajo
    }
	
	// Esperar a que el pin esté en ALTO (fin del pulso bajo)
    while (!(PINC & (1 << DHT11_PIN)));
    
	// Esperar a que el pin esté en BAJO (fin del pulso alto de respuesta)
    while ((PINC & (1 << DHT11_PIN)) && (timeout < 100));
	
	// Lectura de los 40 bits de datos (5 bytes)
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 8; j++) {
			// Esperar el inicio del pulso ALTO (bit de datos)
			while (!(PINC & (1 << DHT11_PIN))); //Espera ~50us en bajo

			// Decidir si es '0' o '1' basado en la duración del pulso ALTO
			// El punto de decisión es ~40us,El fabricante indica que un bit "0" dura entre 26-28 ?s y un bit "1" dura 70 ?s
			//Usar 40 es es más seguro ya que se encuentra exactamente en el medio de ambos tiempos, reduciendo errores de lectura por pequeñas variaciones de reloj.
			_delay_us(40);

			if (PINC & (1 << DHT11_PIN)) {
				// Si después de 40us sigue en ALTO -> es un '1'
				bits[i] = (bits[i] << 1) | 1;
				// Esperar a que termine el pulso ALTO (para sincronizar)
				while ((PINC & (1 << DHT11_PIN)));
				} else {
				// Si después de 30us ya está en BAJO -> es un '0'
				bits[i] = (bits[i] << 1);
			}
		}
	}

	// Verificación del Checksum 
	// Checksum = (Byte1 + Byte2 + Byte3 + Byte4) & 0xFF
	uint8_t checksum_calculado = bits[0] + bits[1] + bits[2] + bits[3];
	if (bits[4] != checksum_calculado) {
		return 0; // Error de checksum
	}

	// Asignar valores (solo la parte entera, la decimal siempre es 0)
	*humedad = bits[0];      // Byte 1: Parte entera de la Humedad
	*temperatura = bits[2];  // Byte 3: Parte entera de la Temperatura

	return 1; // Lectura exitosa	
}
