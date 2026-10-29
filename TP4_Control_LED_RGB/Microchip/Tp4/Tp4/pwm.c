#include "pwm.h"

#define PWM_PERIOD 256

// Ánodo común -> lógica invertida: "encendido" = pin en bajo (0 lógico)
#define PWM_ON()   (PORTB &= ~(1 << LED_R_PIN))
#define PWM_OFF()  (PORTB |=  (1 << LED_R_PIN))

static volatile uint8_t  duty_rojo    = 0;
static volatile uint16_t PWM_position = 0;

// ---------------- Timer1: PWM Hardware Phase Correct (Verde y Azul) ----------------
static void Timer1_Init(void) {
	// Configurar pines PB1 (Azul) y PB2 (Verde) como salidas
	DDRB |= (1 << LED_G_PIN) | (1 << LED_B_PIN);

	// Phase Correct PWM 8 bits (WGM13:0 = 0001 -> WGM10 = 1)
	// Modo Invertido para Ánodo Común (COM1A1:0 = 11, COM1B1:0 = 11)
	TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM10);
	
	// Prescaler = 1 (CS10 = 1, WGM12 = 0, WGM13 = 0) -> Frecuencia ~31.37 kHz
	TCCR1B = (1 << CS10);

	OCR1A = 0; // Azul apagado
	OCR1B = 0; // Verde apagado
}

// ---------------- Timer0: PWM Software en Modo Phase Correct (Rojo) ----------------
static void Timer0_Init(void) {
	DDRB |= (1 << LED_R_PIN); // PB5 configurado como salida

	// Phase Correct PWM 8 bits (WGM00 = 1)
	TCCR0A = (1 << WGM00);
	
	// Prescaler = 1 (CS00 = 1) -> Frecuencia final de PWM ~122.5 Hz (> 30 Hz)
	TCCR0B = (1 << CS00);

	// Interrupción al desbordar en BOTTOM/TOP para mantener flujo contínuo
	OCR0A  = 255;
	TIMSK0 = (1 << OCIE0A);
}

static void PWM_Soft_Update(void) {
	if (++PWM_position >= PWM_PERIOD) {
		PWM_position = 0;
	}

	if (PWM_position < duty_rojo) {
		PWM_ON();
		} else {
		PWM_OFF();
	}
}

ISR(TIMER0_COMPA_vect) {
	PWM_Soft_Update();
}

void PWM_Init(void) {
	Timer1_Init();
	Timer0_Init();
}

void PWM_Set_Rojo(uint8_t duty) {
	duty_rojo = duty;
}

void PWM_Set_Verde(uint8_t duty) {
	OCR1B = duty; // En modo invertido, mayor OCR1B = mayor tiempo en BAJO (encendido)
}

void PWM_Set_Azul(uint8_t duty) {
	OCR1A = duty; // En modo invertido, mayor OCR1A = mayor tiempo en BAJO (encendido)
}