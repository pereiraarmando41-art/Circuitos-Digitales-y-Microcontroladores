/*
 * Microondas con MEF y Timer0 de 1ms
 * Autor: Armando
 * Fecha: Mayo 2026
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "mef.h"
#include "timer.h"  

int main(void) {
    // Inicializar la máquina de estados (inicializa Timer0 y periféricos)
    MEF_Inicializar();
    sei();

    // Bucle principal: solo responde a flags
    while (1) {
        if (TIMER_debe_ejecutar_MEF()) {
            MEF_Actualizar();   // Actualizar estados y leer teclado
        }
    }
}