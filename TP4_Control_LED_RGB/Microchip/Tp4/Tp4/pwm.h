#ifndef PWM_H
#define PWM_H
#include "utils.h"

// ---------- Pines del LED RGB (Anodo Comun) ----------
#define LED_R_PIN   PB5   // PWM por software (Timer0)
#define LED_G_PIN   PB2   // OC1B (Timer1)
#define LED_B_PIN   PB1   // OC1A (Timer1)

// Inicializa Timer1 (hw, Verde/Azul) y Timer0 (sw, Rojo)
void PWM_Init(void);
// duty: 0-255 -> 0 = apagado, 255 = maximo brillo (la inversion por
// anodo comun se maneja internamente en cada setter)
void PWM_Set_Rojo(uint8_t duty);
void PWM_Set_Verde(uint8_t duty);
void PWM_Set_Azul(uint8_t duty);
#endif