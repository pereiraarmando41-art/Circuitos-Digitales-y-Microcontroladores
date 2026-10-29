#include "keypad.h"

// Arrays privados (estáticos para que no se vean desde afuera)
static const uint8_t col_pins[4] = {COL0_PIN, COL1_PIN, COL2_PIN, COL3_PIN};
static const uint8_t filas_portb[3] = {FILA0_PIN, FILA1_PIN, FILA2_PIN};
static const uint8_t fila3_portd = FILA3_PIN;

// Inicialización: se llama UNA SOLA VEZ al inicio
void KEYPAD_Init(void)
{
	// Configurar COLUMNAS como ENTRADAS con pull-up
	DDRD &= ~MASK_COLUMNAS;
	PORTD |= MASK_COLUMNAS;
	
	// Configurar FILAS como SALIDAS
	DDRB |= MASK_FILAS_PORTB;
	DDRD |= MASK_FILAS_PORTD;
	
	// Dejar todas las filas en alto (inactivas)
	PORTB |= MASK_FILAS_PORTB;
	PORTD |= MASK_FILAS_PORTD;
}


uint8_t KepadUpdate(void)
{
	uint8_t fila, col;
	
	// Escanear cada fila (activar una por vez)
	for (fila  = 0; fila  < 4; fila ++) {
        // Desactivar todas las filas (poner en alto)
        PORTB |= MASK_FILAS_PORTB;
        PORTD |= MASK_FILAS_PORTD;
        
		// Activar fila actual (poner en bajo)
		if(fila < 3){
			PORTB &= ~(1<<filas_portb[fila]);	
		}else{
			PORTD &= ~(1<<fila3_portd);
		}
		
		_delay_us(1);	// Esperar estabilización
		
        // LEER COLUMNAS
        for (col = 0; col < 4; col++){
			if(!(PIND & (1<<col_pins[col]))){
				return (fila*4+col);
			}
		}
	}
	
	return 0xFF; //tecla No presionada
}

uint8_t KEYPAD_Scan (uint8_t *pkey)
{
	static uint8_t Old_key, Last_valid_key=0xFF; // no hay tecla presionada;
	uint8_t Key;
	Key= KepadUpdate();
	if(Key==0xFF){
		Old_key=0xFF; // no hay tecla presionada
		Last_valid_key=0xFF;
		return 0;
	}
	if(Key==Old_key) { //2da verificación
		if(Key!=Last_valid_key){ //evita múltiple detección
			*pkey=Key;
			Last_valid_key = Key;
			return 1;
		}
	}
	Old_key=Key; //1era verificación
	return 0;
}
