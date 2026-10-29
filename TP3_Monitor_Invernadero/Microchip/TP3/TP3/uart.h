#ifndef UART_H
#define UART_H

#include "utils.h"

#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 32

// --- Inicialización UART ---
void UART_Init(void);

// --- Envío de una cadena por UART ---
void UART_Send_String(char *str);

// --- Funciones para recepción de comandos ---
uint8_t UART_ComandoDisponible(void);
void	UART_GetComando(char *buf, uint8_t maxlen);

#endif