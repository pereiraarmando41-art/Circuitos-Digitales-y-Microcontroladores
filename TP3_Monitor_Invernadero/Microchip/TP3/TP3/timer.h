#ifndef TIMER_H
#define TIMER_H
#include "utils.h"

// Inicialización
void TIMER_Init(void);

void TIMER_Enable(void);
void TIMER_Disable(void);
void TIMER_SetPeriodo(uint8_t segundos);
uint8_t TIMER_GetClkFlag(void);
void TIMER_ClearClkFlag(void);

#endif