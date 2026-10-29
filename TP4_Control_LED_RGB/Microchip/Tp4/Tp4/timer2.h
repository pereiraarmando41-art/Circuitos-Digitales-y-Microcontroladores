#ifndef TIMER2_H
#define TIMER2_H

#include "utils.h"

void TIMER2_Init(void);       // Timer2, tick de 1ms
uint32_t TIMER2_Get_Milisegundos(void); // Lectura atomica del contador de ms
uint8_t TIMER2_MuestraLista(void);   // 1 cuando pasaron 100ms desde la ultima muestra
void TIMER2_MuestraProcesada(void);  // Limpia la bandera
#endif