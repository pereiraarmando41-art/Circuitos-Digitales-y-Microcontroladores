/*
 * mef.h - Máquina de Estados del Microondas
 * Estados: REPOSO -> INGRESO -> COCCION -> PAUSA -> FINALIZAR
 */

#ifndef MEF_H
#define MEF_H

#include <stdint.h>


//Estados posibles
typedef enum {
	REPOSO,		// 00:00, esperando entrada 
	INGRESO,	// Usuario ingresando tiempo(digitos)
	COCCION,	// Cocinando (magnetrón encendido)
	PAUSA,		// Coccion pausada
	FINALIZAR	// Tiempo terminado, parpadea "Listo!"
}eEstado_micro;
	
// Inicializar Ja MEF
void MEF_Inicializar(void);

// Actualizar la MEF (llamar periódicamente, ej. cada 10ms)
void MEF_Actualizar(void);

#endif