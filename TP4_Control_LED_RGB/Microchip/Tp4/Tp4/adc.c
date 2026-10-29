#include "adc.h"

static volatile uint8_t lectura=0;

static volatile uint8_t adc_flag=0;

void ADC_Init(){
	//Habilita ADC
	ADCSRA |= 1<<ADEN;
	
	//Clock prescaler en 128 -> sample frequency 125kHz
	ADCSRA |= 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;
	
	//Establece Vcc como referencia
	ADMUX |= 1<<REFS0;
	
	//Establece channel 3 (ADC3)
	ADMUX |= 1<<MUX1 | 1<<MUX0;
	
	//Deshabilita digital input en el pin ADC3
	DIDR0 |= 1<<ADC3D;
	
	// Alinea el resultado a la izquierda. Como el ADC es de 10 bits, los 8 bits más significativos quedan en el registro ADCH
	ADMUX  |= 1<<ADLAR;
	
	//Habilita las interupciones
	ADCSRA |= 1<<ADIE;
	
}

// Inicia el proceso de conversión poniendo en alto el bit ADSC (Start Conversion)
void ADC_Read(){
	ADCSRA |= 1<< ADSC;
}

// Mecanismo dE bandera para que el programa principal sepa cuándo hay una nueva lectura disponible sin quedarse bloqueado esperando
uint8_t ADC_GetFlag(){
	return adc_flag;
}

void ADC_ClearFlag(){
	adc_flag=0;
}

// Devuelve el último valor almacenado en la variable lectura.
uint8_t ADC_GetLuz(){
	return lectura;
}

// Se ejecuta automáticamente cuando el ADC termina de procesar la muestra
ISR(ADC_vect){
	// Captura los 8 bits superiores del resultado (gracias a ADLAR).
	lectura = ADCH;
	
	// Avisa al sistema que el dato es nuevo.
	adc_flag=1;
}	