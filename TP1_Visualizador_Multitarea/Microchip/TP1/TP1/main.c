/* Inclusion de bibliotecas de codigo */
#include <avr/io.h>
#define F_CPU 16000000UL
#include "util/delay.h"
#include <stdint.h>

/* ================================================================
 * neo_send_byte - envia un byte al bus WS2812B (MSB primero)
 *
 * Timing segun datasheet a 16MHz (1 ciclo = 62.5ns):
 *   Bit 1: T1H=0.8us (13 ciclos) + T1L=0.45us (7 ciclos)
 *   Bit 0: T0H=0.4us ( 6 ciclos) + T0L=0.85us (14 ciclos)
 *   SBI ocupa 2 ciclos, CBI ocupa 2 ciclos, cada NOP = 1 ciclo
 *   Bit 1: SBI(2)+11NOPs=13H | CBI(2)+5NOPs=7L
 *   Bit 0: SBI(2)+4NOPs=6H   | CBI(2)+12NOPs=14L
 * ================================================================ */
void neo_send_byte(uint8_t data)
{
    int8_t bit;
    for (bit = 7; bit >= 0; bit--)
    {
        if (data & (1 << bit))
        {
            PORTB |= (1 << PINB0);
            asm volatile ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop");
            PORTB &= ~(1 << PINB0);
            asm volatile ("nop\n\tnop\n\tnop\n\tnop\n\tnop");
        }
        else
        {
            PORTB |= (1 << PINB0);
            asm volatile ("nop\n\tnop\n\tnop\n\tnop");
            PORTB &= ~(1 << PINB0);
            asm volatile ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop");
        }
    }
}

/* ================================================================
 * neo_show - vuelca los buffers g[], r[], b[] a la tira Neopixel
 * Protocolo WS2812B: orden GRB por LED, MSB primero.
 * Al final envia pulso LOW >50us para hacer latch.
 * ================================================================ */
void neo_show(uint8_t g[], uint8_t r[], uint8_t b[])
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        neo_send_byte(g[i]);
        neo_send_byte(r[i]);
        neo_send_byte(b[i]);
    }
    _delay_us(60);
}

/* ================================================================
 * neo_clear - apaga los 8 LEDs Neopixel
 * ================================================================ */
void neo_clear(void)
{
    uint8_t g[8];
    uint8_t r[8];
    uint8_t b[8];
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        g[i] = 0;
        r[i] = 0;
        b[i] = 0;
    }
    neo_show(g, r, b);
}

/* ================================================================
 * btn_presionado - anti-rebote por delay para un pulsador
 * Recibe la mascara del bit (ej: (1<<PINC0)).
 * Retorna 1 si hubo presion valida, 0 si no.
 *
 * Mecanismo:
 *   1. Detecta flanco de bajada (activo bajo con pull-up).
 *   2. Espera 20ms para que los rebotes mecanicos cesen.
 *   3. Confirma que el pin sigue en bajo.
 *   4. Espera a que el usuario suelte + 20ms al soltar.
 * ================================================================ */
uint8_t btn_presionado(uint8_t mascara)
{
    if (!(PINC & mascara))
    {
        _delay_ms(20);
        if (!(PINC & mascara))
        {
            while (!(PINC & mascara));
            _delay_ms(20);
            return 1;
        }
    }
    return 0;
}

/* ================================================================
 * MAIN
 * ================================================================ */
int main(void)
{
    /* Setup */
    DDRD  = 0xFF;
    PORTD = 0x00;

    DDRC  &= ~((1 << PORTC0) | (1 << PORTC1));
    PORTC |=  ((1 << PINC0)  | (1 << PINC1));

    DDRB  |=  (1 << PORTB0);
    PORTB &= ~(1 << PINB0);

    /* Variables de estado - Secuencias LED Puerto D */
    uint8_t seq_led   = 0;
    uint8_t seq_a_pos = 0;
    int8_t  seq_b_pos = 7;
    int8_t  seq_b_dir = -1;

    /* Variables de estado - Secuencias Neopixel */
    uint8_t seq_neo     = 0;
    uint8_t seq_c_phase = 0;
    uint8_t  seq_d_pos   = 7;

    /* Buffers de color Neopixel */
    uint8_t g[8];
    uint8_t r[8];
    uint8_t b[8];
    uint8_t i;

    /* Bandera para actualizar Neopixel después del botón */
    uint8_t actualizar_neopixel = 0;

    /* Contadores de tiempo no bloqueantes */
    uint32_t ultimo_tiempo_led = 0;
    uint32_t ultimo_tiempo_neo = 0;
    uint32_t contador_ms = 0;

    neo_clear();

    /* Loop */
    while (1)
    {
        /* Base de tiempo: delay de 1ms no bloqueante */
        _delay_ms(1);
        contador_ms++;	
			
        /* BTN0 (PC0): alterna secuencia LED A <-> B */
        if (btn_presionado(1 << PINC0))
        {
            if (seq_led == 0) seq_led = 1;
            else              seq_led = 0;
            seq_a_pos = 0;
            seq_b_pos = 7;
            seq_b_dir = -1;
        }

        /* BTN1 (PC1): alterna secuencia Neopixel C <-> D */
        /* Boton 1 - Neopixels */
        if (btn_presionado(1 << PINC1))
        {
            seq_neo = !seq_neo;  // Alternar
            seq_c_phase = 0;
            seq_d_pos = 7;
            actualizar_neopixel = 1;  // Marcar que hay que actualizar
        }

        /* ========================================================
         * ACTUALIZAR NEOPIXEL INMEDIATAMENTE DESPUÉS DEL BOTÓN
         * (Fuera del anti-rebote, sin delays bloqueantes)
         * ======================================================== */
        if (actualizar_neopixel)
        {
            actualizar_neopixel = 0;
            
            if (seq_neo == 0)  // Secuencia C
            {
                for (i = 0; i < 8; i++)
                {
                    if (seq_c_phase == 0)
                    {
                        if (i % 2 == 0) { g[i]=0; r[i]=255; b[i]=0; }
                        else            { g[i]=0; r[i]=0;   b[i]=0; }
                    }
                    else
                    {
                        if (i % 2 == 1) { g[i]=0; r[i]=0;   b[i]=255; }
                        else            { g[i]=0; r[i]=0;   b[i]=0; }
                    }
                }
            }
            else  // Secuencia D
            {
                for (i = 0; i < 8; i++)
                {
                    if (i == seq_d_pos) { g[i]=255; r[i]=0; b[i]=0; }
                    else                { g[i]=0;   r[i]=0; b[i]=0; }
                }
            }
            neo_show(g, r, b);  // Actualizar físicamente
        }
        /* T2: LEDs Puerto D - actualizar cada 100ms (no bloqueante) 
		   * Sec A: un LED de LSB (PD0) a MSB (PD7) ciclicamente.
		   * Sec B: un LED desde MSB, rebota en los extremos. */
        if ((contador_ms - ultimo_tiempo_led) >= 100)
        {
			ultimo_tiempo_led = contador_ms;
 
			if (seq_led == 0)
			{
				PORTD = (uint8_t)(1 << seq_a_pos);
				seq_a_pos = (seq_a_pos + 1) & 0x07;
			}
			else
			{
				PORTD = (uint8_t)(1 << seq_b_pos);
				if (seq_b_pos == 0) seq_b_dir = +1;
				else if (seq_b_pos == 7) seq_b_dir = -1;
				seq_b_pos += seq_b_dir;
			}
		}
        /* T3: Neopixel - actualizar cada 150ms (no bloqueante) 
         * Sec C fase 0: pares (0,2,4,6) = ROJO, impares apagados.
         * Sec C fase 1: impares (1,3,5,7) = AZUL, pares apagados.
         * Sec D: un LED VERDE de indice 7 a 0, luego reinicia. */
        if ((contador_ms - ultimo_tiempo_neo) >= 150)
        {
	        ultimo_tiempo_neo = contador_ms;
			
			if (seq_neo == 0)  // Secuencia C: pares rojo / impares azul
			{
				for (i = 0; i < 8; i++)
				{
					if (seq_c_phase == 0)
					{
						if (i % 2 == 0) { g[i]=0; r[i]=255; b[i]=0;   }
						else            { g[i]=0; r[i]=0;   b[i]=0;   }
					}
					else
					{
						if (i % 2 == 1) { g[i]=0; r[i]=0;   b[i]=255; }
						else            { g[i]=0; r[i]=0;   b[i]=0;   }
					}
				}
				neo_show(g, r, b);
				seq_c_phase ^= 1; // Alternar fase
			}
			else // Secuencia D: verde de derecha a izquierda
			{
                /* Primero decrementar la posición */
                if (seq_d_pos == 0)
                {
	                seq_d_pos = 7;  // Cuando llega a 0, vuelve a 7
                }
                else
                {
	                seq_d_pos--;     // Decrementar normalmente
                }
				
				/* Llenar el buffer con el LED en la nueva posición */
				for (i = 0; i < 8; i++)
				{
					if (i == seq_d_pos) { g[i]=255; r[i]=0; b[i]=0; }
					else                          { g[i]=0;   r[i]=0; b[i]=0; }
				}
				neo_show(g, r, b);
			}
		}
		
    } /* fin while(1) */
}