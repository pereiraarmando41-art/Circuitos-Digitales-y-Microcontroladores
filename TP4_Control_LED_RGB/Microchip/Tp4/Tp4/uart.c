#include "uart.h"

#define USART_BAUDRATE 9600
#define BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

// --- Variables para comunicación con la ISR ---
static char TX_Buffer[TX_BUFFER_SIZE];

// --- Buffer de recepción para comandos ---
static volatile char Rx_Buffer[RX_BUFFER_SIZE];
static volatile uint8_t Rx_Index = 0;
static volatile uint8_t Comando_Listo=0; //1 cuando se recibio '\r' o '\n'

// --- Inicialización UART ---
void UART_Init(void) {
	// Configurar baud rate
	// UBRR = (F_CPU / (16 * BAUD)) - 1
	// Para 9600 bps @ 16MHz: UBRR = (16000000 / (16 * 9600)) - 1 = 104.16... ? 103
	UBRR0L = BAUD_PRESCALER;
	UBRR0H = 0;

	// Configurar formato: 8 bits de datos, 1 bit de stop, sin paridad (8N1)
	// UCSZ01 = 1, UCSZ00 = 1
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	// Habilitar TX, RX y activar específicamente la Interrupción de Recepción (RXCIE0)	
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	
	// TX (UDRIE0) se activa solo cuando hay algo para enviar
}

// --- Envío de una cadena por UART ---
void UART_Send_String(char * str) {
    // Esperar a que termine la transmisión anterior.
    // UDRIE0 permanece activo mientras la ISR sigue enviando bytes;
    // cuando llega al '\0' lo desactiva. Si cargamos TX_Buffer antes
    // de ese momento, la ISR empieza a leer el nuevo string desde la
    // posición en que quedó, saltando los primeros caracteres.
	while (UCSR0B & (1 << UDRIE0));
	strncpy(TX_Buffer, str, TX_BUFFER_SIZE-1);
	
	// Activar ISR de transmisi?n (arranca desde i=0 porque se reinici?)
	UCSR0B |= (1 << UDRIE0);
}

// --- Funciones para comandos ---
uint8_t UART_ComandoDisponible(void)
{
	return Comando_Listo;
}

void UART_GetComando(char *buf, uint8_t maxlen) {
	uint8_t i = 0;
	while (i < maxlen - 1 && Rx_Buffer[i] != '\0') {
		buf[i] = (char)Rx_Buffer[i];
		i++;
	}
	buf[i]    = '\0';
	Rx_Index = 0;
	Comando_Listo = 0;
}

// --- ISR de transmisión ---
ISR(USART_UDRE_vect) {
	static uint8_t i = 0;
	if (TX_Buffer[i] != '\0') {
		// Transmitir el carácter actual e incrementar el índice
		UDR0 = TX_Buffer[i];
		i++;
	}
	else {
		i=0;
		// Una vez que llegamos al final de la cadena ('\0'),
		// debemos DESHABILITAR la interrupción UDRIE0.
		// Si no lo hacemos, la CPU entrará en un bucle infinito de interrupciones
		// porque el buffer seguirá vacío pero no tenemos más qué enviar
		UCSR0B &= ~(1 << UDRIE0);
	}
}

/*
 * Interrupcion de recepcion
 * Disparada por la UART cuando hay un nuevo dato en UDR0 listo para ser leido
 */
ISR(USART_RX_vect)
{
	char dato = UDR0; //Leer dato recibido del UDR del receptor UART
	
	if(dato == '\r' || dato == '\n'){
		if(Rx_Index > 0 && !Comando_Listo){
			Rx_Buffer[Rx_Index] = '\0';
			Comando_Listo = 1; // Marcar comando listo para procesar en el main loop
		}
	}
	else{
		if(Rx_Index < RX_BUFFER_SIZE-1 && !Comando_Listo){
			Rx_Buffer[Rx_Index++] = dato;
		}
	}
}