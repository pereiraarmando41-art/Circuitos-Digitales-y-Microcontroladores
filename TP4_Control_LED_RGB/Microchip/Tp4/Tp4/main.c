#include "utils.h"
#include "adc.h"
#include "control_led.h"
#include "timer2.h"
#include "cli.h"

int main(void) {
	// --- Setup de Subsistemas ---
	ControlLed_Init();
	TIMER2_Init();
	ADC_Init();

	sei();       // Habilitación global de interrupciones
	
	CLI_Init();  // Inicializa UART y comandos
	
	while (1) {
		// Procesar comandos del usuario (CLI)
		CLI_Procesar();
		
		// Gestión del Sensor LDR cada 100ms
		if (TIMER2_MuestraLista()) {
			TIMER2_MuestraProcesada();
			ADC_Read();
		}
		
		// Captura de datos del ADC 
		if (ADC_GetFlag()) {
			ControlLed_ActualizarPeriodo(ADC_GetLuz());
			ADC_ClearFlag();
		}
		
		// Actualización constante de la envolvente PWM
		ControlLed_Actualizar(TIMER2_Get_Milisegundos());
	}
}