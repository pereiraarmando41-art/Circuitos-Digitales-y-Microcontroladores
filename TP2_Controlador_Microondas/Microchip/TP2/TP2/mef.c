#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "mef.h"	
#include "lcd.h"
#include "keypad.h"
#include "TIMER.h"

// VARIABLES PRIVADAS (estáticas)

// Estado actual del sistema
static eEstado_micro estado_actual;

// Variables para el ingreso de tiempo
static	uint8_t digitos[4];				// Almacena los 4 dígitos ingresados
static	uint8_t  pos_digito;			// Posición actual (0-4)
static uint16_t tiempo_total_segundos;  // Tiempo total configurado
static uint16_t tiempo_transcurrido;	// Tiempo que falta

// Variables para temporización (usando Timer0 de 1ms)
static uint16_t  tiempo_en_estado;		// Tiempo en el estado actual (en ms)

// Variables para el estado FINALIZAR (parpadeo)
static uint16_t tiempo_parpadeo;      // Tiempo acumulado para parpadeo
static uint8_t mostrar_mensaje;          // Alterna para efecto parpadeo
static uint8_t parpadeo_activo;          // Indica si está en periodo de parpadeo

// Variable para controlar la puerta
static uint8_t puerta_abierta;           // 1 = puerta abierta, 0 = cerrada

// Vector de mapeo directo: posición tecla -> valor numérico
// 0xFF indica que no es numérica
const uint8_t tecla_a_valor[16] = {
	1, 2, 3, 0xFF,  // 0,1,2,3
	4, 5, 6, 0xFF,  // 4,5,6,7
	7, 8, 9, 0xFF,  // 8,9,10,11
	0xFF, 0, 0xFF, 0xFF  // 12,13,14,15
};

// FUNCIONES PRIVADAS 

//  Aplicar salidas según el estado actual (MOORE)
static void Aplicar_Salidas(eEstado_micro  estado){	
	switch(estado) {
        case REPOSO:
            TIMER_apagar_magnetron();
			PORTC &= ~(1 << PORTC4);
            LCDcursorOnBlink();
            LCDGotoXY(6, 0);  // Posicionar cursor al inicio
        break;
		
        case INGRESO:
			TIMER_apagar_magnetron(); 
			PORTC &= ~(1 << PORTC4);
			LCDcursorOnBlink();
			break;
			
        case COCCION:
			TIMER_encender_magnetron();   
			PORTC |= (1 << PORTC4);     //  Encender LED2 (luz interior)
			LCDcursorOFF();
            TIMER_reset();  // Resetear Timer0 al comenzar cocción
			break;
		
        case PAUSA:
			TIMER_apagar_magnetron(); 
			PORTC |= (1 << PORTC4);  // LED2 ENCENDIDO (sigue en proceso)
			LCDcursorOFF();
			break;
		
        case FINALIZAR:
			PORTC |= (1 << PORTC4);    // LED2 ENCENDIDO (proceso terminó, pero sigue)
			TIMER_apagar_magnetron();  
			// Apagar LED3 por si acaso
			PORTC &= ~(1 << PORTC5);
			LCDcursorOFF();
			break;
	}	
}

// Actualizar display con tiempo en formato MM:SS
static void ActualizarDisplayTiempo(uint16_t segundos) {
	char buffer[6]; // Formato "MM:SS"
    uint8_t minutos = segundos / 60;
    uint8_t segs = segundos % 60;
		
	buffer[0] = '0' + (minutos / 10);
	buffer[1] = '0' + (minutos % 10);
	buffer[2] = ':';
	buffer[3] = '0' + (segs / 10);
	buffer[4] = '0' + (segs % 10);
	buffer[5] = '\0';
	
	LCDGotoXY(6,0);
	LCDstring((uint8_t*)buffer, 5);
    
	// IMPORTANTE: Volver a posicionar el cursor donde debe estar según el estado
    // Esto evita que el cursor se quede al final
    if(estado_actual == INGRESO || estado_actual == REPOSO) {
	    if(pos_digito < 2) {
		    LCDGotoXY(6 + pos_digito, 0);
		    } else if(pos_digito < 4) {
		    LCDGotoXY(6 + pos_digito + 1, 0);
		    } else {
		    LCDGotoXY(6, 0);  // Si completó 4 dígitos, volver al inicio
	    }
	    LCDcursorOnBlink();
    }
}

// Mostrar tiempo restante (para estado COCCION)
static void MostrarTiempoRestante(void) {
    uint16_t restante = (tiempo_total_segundos > tiempo_transcurrido) ? 
                         (tiempo_total_segundos - tiempo_transcurrido) : 0;
    ActualizarDisplayTiempo(restante);
}


// Procesar dígitos ingresados
static void ProcesarDigito(uint8_t digito) {
	if(pos_digito < 4) {
		digitos[pos_digito++] = digito;
		
		// Calcular minutos y segundos según dígitos ingresados
		uint8_t minutos = 0, segundos = 0;
		
		switch(pos_digito) {
			case 1:
			minutos = digitos[0];
			break;
			case 2:
			minutos = digitos[0] * 10 + digitos[1];
			if(minutos > 59) minutos = 59;
			break;
			case 3:
			minutos = digitos[0] * 10 + digitos[1];
			segundos = digitos[2];
			if(minutos > 59) minutos = 59;
			break;
			case 4:
			minutos = digitos[0] * 10 + digitos[1];
			segundos = digitos[2] * 10 + digitos[3];
			if(minutos > 59) minutos = 59;
			if(segundos > 59) segundos = 59;
			
			tiempo_total_segundos = minutos * 60 + segundos;
			tiempo_transcurrido = 0;
			break;
		}
		
		ActualizarDisplayTiempo(minutos * 60 + segundos);
		
		// Posicionar cursor en el siguiente dígito
		if(pos_digito < 2) {
			LCDGotoXY(6 + pos_digito, 0);
			} else if(pos_digito < 4) {
			LCDGotoXY(6 + pos_digito + 1, 0);  // Saltar ':'
		}
	}
}

// Resetear el ingreso de tiempo (vuelve a 00:00)
static void ResetearIngreso(void){
	pos_digito = 0;
	for (uint8_t i=0; i<4; i++)
	{
		digitos[i] = 0;
	}
	tiempo_total_segundos = 0;
	tiempo_transcurrido = 0;
	
    // Actualizar display con "00:00"
	ActualizarDisplayTiempo(0);
	
	// Posicionar cursor al inicio
    LCDGotoXY(6, 0);
    LCDcursorOnBlink();
}

static void ResetearDisplayCompleto(void) {
	// Limpiar toda la pantalla
	LCDclr();
	
	// Mostrar "00:00" en la posición correcta
	LCDGotoXY(6, 0);
	LCDstring((uint8_t*)"00:00", 5);
	
	// Limpiar segunda línea
	LCDGotoXY(0, 1);
	LCDstring((uint8_t*)"              ", 14);
	
	// Posicionar cursor al inicio de los minutos (primer dígito)
	LCDGotoXY(6, 0);
	LCDcursorOnBlink();
	
	// Resetear variables de ingreso
	pos_digito = 0;
	for (uint8_t i = 0; i < 4; i++) {
		digitos[i] = 0;
	}
	tiempo_total_segundos = 0;
	tiempo_transcurrido = 0;
}


// FUNCIONES PUBLICAS

// Inicializar MEF (llamar UNA vez al inicio)
void MEF_Inicializar (void) {
    // Inicializar todos los periféricos
	LCDinit();
	LCDclr();
	KEYPAD_Init();
	TIMER_inicializar();
	
	// Configurar PC5 como salida (LED3 - Alarma)
	DDRC |= (1 << PORTC5);
	PORTC &= ~(1 << PORTC5);  // Iniciar apagado
	
	// Configurar PC4 como salida (LED2 - Luz Interior)
	DDRC |= (1 << PORTC4);
	PORTC &= ~(1 << PORTC4);  // Iniciar apagado
	
    // Configurar estado inicial (REPOSO)
    estado_actual = REPOSO;
    tiempo_en_estado = 0;
    tiempo_parpadeo  = 0;
    mostrar_mensaje = 1;
    parpadeo_activo = 0;
    puerta_abierta = 0;	
	
    // Resetear variables de ingreso
    ResetearIngreso();
	
    // Aplicar salidas del estado inicial
    Aplicar_Salidas(REPOSO);
    
	ResetearDisplayCompleto();
}

// Función para cerrar la puerta (simula el cierre)
void MEF_cerrar_puerta(void) {
	puerta_abierta = 0;
}

// Actualizar la MEF (llamar periódicamente, cada 10ms)
void MEF_Actualizar(void){
	uint8_t tecla;
    uint8_t tecla_valor;
    
    // INCREMENTAR TEMPORIZADOR DE ESTADO
    tiempo_en_estado += 10; // Cada llamada = 10ms
	
    switch (estado_actual){
	    case COCCION:
	    // Verificar si pasó 1 segundo (esto debe ejecutarse siempre)
	    if(TIMER_ha_pasado_un_segundo()) {
		    tiempo_transcurrido++;
		    MostrarTiempoRestante();
		    
		    if(tiempo_transcurrido >= tiempo_total_segundos) {
			    estado_actual = FINALIZAR;
			    Aplicar_Salidas(estado_actual);
			    tiempo_en_estado = 0;
			    tiempo_parpadeo = 0;
			    parpadeo_activo = 1;
			    mostrar_mensaje = 1;
			    LCDGotoXY(0, 1);
			    LCDstring((uint8_t*)"Listo!        ", 14);
		    }
	    }
	    break;
	    
	    case FINALIZAR:
	    // PARPADEO DEL DISPLAY DURANTE 5 SEGUNDOS
	    if(parpadeo_activo){
		    // Cada 500ms alternar el mensaje
		    if(tiempo_en_estado - tiempo_parpadeo >= 500){
			    tiempo_parpadeo = tiempo_en_estado;
			    mostrar_mensaje = !mostrar_mensaje;
			    
			    if(mostrar_mensaje) {
				    LCDGotoXY(0, 1);
				    LCDstring((uint8_t*)"Listo!        ", 14);
					// Encender LED3
					PORTC |= (1 << PORTC5);
				    } else {
				    LCDGotoXY(0, 1);
				    LCDstring((uint8_t*)"              ", 14);
					// Apagar LED3
					PORTC &= ~(1 << PORTC5);
			    }
		    }
			
		    // Salir después de 5 segundos (5000ms)
		    if(tiempo_en_estado >= 5000) {
			    parpadeo_activo = 0;
				// Apagar LED3
				PORTC &= ~(1 << PORTC5);
				PORTC &= ~(1 << PORTC4);     // Apagar LED2
				
				ResetearDisplayCompleto();
				
			    estado_actual = REPOSO;
			    Aplicar_Salidas(estado_actual);
			    tiempo_en_estado = 0;
		    }
	    }
	    break;
	    
	    default:
	    break;
    }
	
    // Leer teclado (no bloqueante)
	if(KEYPAD_Scan(&tecla)) {
	    tecla_valor = tecla_a_valor[tecla];
	
        // MÁQUINA DE ESTADOS
		switch (estado_actual){
			case REPOSO:
				if(tecla_valor != 0xFF) {
                    // Cualquier tecla numérica pasa a INGRESO
					estado_actual = INGRESO;
                    Aplicar_Salidas(estado_actual);
                    ProcesarDigito(tecla_valor);
                    tiempo_en_estado = 0;
				}
				else if(tecla == 11){ // Tecla 'C' = +30 seg (inicio rápido)
					tiempo_total_segundos = 30;
					tiempo_transcurrido  = 0;
					estado_actual = COCCION;
					Aplicar_Salidas(estado_actual);
					tiempo_en_estado = 0;
					LCDGotoXY(0,1);
                    LCDstring((uint8_t*)"Cocinando...  ", 14);
                    MostrarTiempoRestante();
				}
				break;

			case INGRESO:
				if(tecla_valor != 0xFF) {
					ProcesarDigito(tecla_valor);
				}
				else if(tecla == 3) {  // 'A' = START
					if(tiempo_total_segundos > 0) {
						estado_actual = COCCION;
						Aplicar_Salidas(estado_actual);
						tiempo_en_estado = 0;
						LCDGotoXY(0, 1);
                        LCDstring((uint8_t*)"Cocinando...  ", 14);
	                    MostrarTiempoRestante();
					}
				}
				else if(tecla == 7){ // 'B' = CLEAR
					ResetearIngreso();
					estado_actual = REPOSO;
					Aplicar_Salidas(estado_actual);
					tiempo_en_estado = 0;
				}
				break;
			
			case COCCION:
				// Verificar si la puerta se abre durante la cocción
				if(tecla == 15) {  // 'D' = abrir puerta
					puerta_abierta = 1;
					estado_actual = PAUSA;
					Aplicar_Salidas(estado_actual);
					tiempo_en_estado = 0;
					LCDGotoXY(0, 1);
					LCDstring((uint8_t*)"Puerta abierta", 14);
					LCDcursorOFF();
				}		

                // Procesar otras teclas durante cocción
				if(tecla == 7){ // 'B' = PAUSA
					puerta_abierta = 0;
					estado_actual = PAUSA;
					Aplicar_Salidas(estado_actual);
					tiempo_en_estado = 0;
					LCDGotoXY(0, 1);
					LCDstring((uint8_t*)"Pausa...      ", 14);
					LCDcursorOFF();
				}
				else if(tecla == 11){ // 'C' = +30 seg
					tiempo_total_segundos += 30;
					if(tiempo_total_segundos > 3599)
						tiempo_total_segundos = 3599;
					MostrarTiempoRestante();
				}
				break;
			
			case PAUSA:
                // Si la pausa fue por puerta, verificar si se cierra
				if(puerta_abierta && tecla == 15) {  // 'D' = abrir puerta
                    // Vuelve a presionar 'D' para cerrar puerta
					puerta_abierta = 0;
					LCDGotoXY(0, 1);
					LCDstring((uint8_t*)"Pausa...      ", 14);
					LCDcursorOFF();
				}
				
				if(tecla == 3) {  // 'A' = START (reanudar)
					if(!puerta_abierta){
						estado_actual = COCCION;
						Aplicar_Salidas(estado_actual);
						tiempo_en_estado = 0;
						LCDGotoXY(0, 1);
						LCDstring((uint8_t*)"Cocinando...  ", 14);
						MostrarTiempoRestante();
					}
					else{
                        // Mostrar mensaje de que la puerta está abierta
                        LCDGotoXY(0, 1);
                        LCDstring((uint8_t*)"Cierre puerta  ", 14);
					}
				}
				else if(tecla == 7){ // 'B' = CANCELAR
					PORTC &= ~(1 << PORTC4); // Apagar LED2
					
					// Apagar LED3
					PORTC &= ~(1 << PORTC5);
					ResetearDisplayCompleto();  // Usar la función unificada
					
					estado_actual = REPOSO;
					Aplicar_Salidas(estado_actual);
					tiempo_en_estado = 0;
					puerta_abierta = 0;
				}
				else if(tecla == 11){ // Tecla 'C' = +30 seg
					tiempo_total_segundos += 30;
					if(tiempo_total_segundos > 3599)
					tiempo_total_segundos = 3599;
					MostrarTiempoRestante();
				}
				break;				
				
			case FINALIZAR:
                // PARPADEO DEL DISPLAY DURANTE 5 SEGUNDOS
				if(parpadeo_activo){
					LCDcursorOFF();
					 
                    // Cada 500ms alternar el mensaje (500ms = 50 * 10ms)
					if(tiempo_en_estado - tiempo_parpadeo >= 500){
                        tiempo_parpadeo = tiempo_en_estado;
						mostrar_mensaje = !mostrar_mensaje;
						
 						if(mostrar_mensaje) {
	 						LCDGotoXY(0, 1);
	 						LCDstring((uint8_t*)"Listo!        ", 14);
	 						// Encender LED3
	 						PORTC |= (1 << PORTC5);
	 					} else {
	 						LCDGotoXY(0, 1);
	 						LCDstring((uint8_t*)"              ", 14);
	 						// Apagar LED3
	 						PORTC &= ~(1 << PORTC5);
 						}
					}
					
					// Salir después de 5 segundos (5000ms)
                    if(tiempo_en_estado >= 5000) {
						parpadeo_activo = 0;
						// Apagar LED3
						PORTC &= ~(1 << PORTC5);	// Apagar LED1
						PORTC &= ~(1 << PORTC4);     // Apagar LED2
						ResetearDisplayCompleto();
						estado_actual = REPOSO;
						Aplicar_Salidas(estado_actual);
						tiempo_en_estado = 0;
					}
				}
				break;
			}
	}
}