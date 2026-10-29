#include "utils.h"
#include "dht11.h"
#include "uart.h"
#include "i2c.h"
#include "rtc.h"
#include "timer.h"

// Variables globales
static uint8_t alerta_activa = 0;
static uint8_t contador_tramas = 0;
static uint8_t error_dht11 = 0;

// Prototipos
static void verificar_rangos(uint8_t temp, uint8_t hum, uint8_t *tiempo);
static uint8_t es_dia(uint8_t hora);
static void enviar_alerta(char *tipo, uint8_t valor, char *unidad, uint8_t *tiempo);
static void procesar_comando(char *comando);



// --- Programa Principal ---
int main(void) {
	uint8_t temp, hum;
	uint8_t tiempo[3];
	char buffer[80];

	// Inicializar UART
	UART_Init();

	// Inicializar RTC
	RTC_Init();
		
	DHT11_Init();

	// Inicializar Timer
	TIMER_Init();
	TIMER_Enable();
	
	sei();  // Habilitar interrupciones globales
		
	// Enviar mensaje de inicio
	UART_Send_String("\r\n=== MONITOR DE INVERNADERO ===\r\n");
	UART_Send_String("Sistema iniciado. Esperando datos...\r\n");
	UART_Send_String("Comandos: SET_TIME=HH:MM:SS | SET_TM=SS | HELP\r\n\r\n");

	while (1) {
		// Procesar comandos recibidos
		if (UART_ComandoDisponible())
		{
			char comando_buffer[RX_BUFFER_SIZE];
			UART_GetComando(comando_buffer, sizeof(comando_buffer));
			procesar_comando(comando_buffer);
		}

		// Enviar telemetría periódicamente
		if (TIMER_GetClkFlag()) {
			TIMER_ClearClkFlag();		// Limpia el flag del timer
			
			if (DHT11_Read(&hum, &temp)) {
				RTC_Get_Time(tiempo);
				
				error_dht11 = 0;
				
				contador_tramas++;
				
				// Verificar rangos
				verificar_rangos(temp, hum, tiempo);
				
				// Formatear datos
				sprintf(buffer, "[%02u:%02u:%02u] T: %u°C | H: %u%% | Estado: %s\r\n",
						tiempo[2], tiempo[1], tiempo[0], temp, hum,
						alerta_activa ? "ALERTA" : "NORMAL");
				
				UART_Send_String(buffer);
			} 
			else {
				RTC_Get_Time(tiempo);
				sprintf(buffer, "[%02u:%02u:%02u] ERROR: Sensor DHT11 no responde\r\n",
						tiempo[2], tiempo[1], tiempo[0]);
				UART_Send_String(buffer);
				error_dht11 = 0;
			}
		}
	}

	return 0;
}

/**
 * Verifica si la temperatura y humedad están dentro de los rangos según la hora
 */
static void verificar_rangos(uint8_t temp, uint8_t hum, uint8_t *tiempo)
{
	uint8_t temp_min, temp_max, hum_min, hum_max;
	uint8_t alerta = 0;
	uint8_t hora = tiempo[2];  // horas está en índice 2

	if (es_dia(hora))
	{
		// Rango diurno (07:00 - 18:59)
		temp_min = 20;
		temp_max = 30;
		hum_min = 50;
		hum_max = 70;
	}
	else
	{
		// Rango nocturno (19:00 - 06:59)
		temp_min = 15;
		temp_max = 22;
		hum_min = 60;
		hum_max = 80;
	}

	if (temp < temp_min || temp > temp_max)
	{
		alerta = 1;
		char tipo[40];
		sprintf(tipo, "Temperatura fuera de rango %s",
				es_dia(hora) ? "diurno" : "nocturno");
		enviar_alerta(tipo, temp, "°C", tiempo);
	}

	if (hum < hum_min || hum > hum_max)
	{
		alerta = 1;
		char tipo[40];
		sprintf(tipo, "Humedad fuera de rango %s",
				es_dia(hora) ? "diurno" : "nocturno");
		enviar_alerta(tipo, hum, "%", tiempo);
	}

	alerta_activa = alerta;
}

/**
 * Determina si es horario diurno (07:00 - 18:59)
 */
static uint8_t es_dia(uint8_t hora)
{
	return (hora >= 7 && hora < 19);
}

/**
 * Envía mensaje de alerta (cada 2 tramas de telemetría)
 */
static void enviar_alerta(char *tipo, uint8_t valor, char *unidad, uint8_t *tiempo)
{
	// Mostrar en tramas impares (1, 3, 5, 7...)
	if (contador_tramas % 2 == 1)
	{
		char buffer[80];
		sprintf(buffer, "[ALERTA] [%02u:%02u:%02u] %s! Valor: %u%s\r\n",
				tiempo[2], tiempo[1], tiempo[0],
				tipo, valor, unidad);
		UART_Send_String(buffer);
	}
}

/**
 * Procesa comandos recibidos por UART
* Comandos soportados:
*   SET_TIME=HH:MM:SS  -> Actualiza hora en el RTC
*   SET_TM=SS          -> Cambia el intervalo de reporte (2-60 s)
 */
static void procesar_comando(char *comando)
{
	char buffer[60];

	if (strncmp(comando, "HELP", 4) == 0 || strncmp(comando, "help", 4) == 0)
	{
		UART_Send_String("Comandos disponibles:\r\n");
		UART_Send_String("  SET_TIME=HH:MM:SS  - Actualiza la hora del RTC\r\n");
		UART_Send_String("  SET_TM=SS          - Cambia intervalo (2-60 segundos)\r\n");
		UART_Send_String("  HELP               - Muestra esta ayuda\r\n");
	}
	else if (strncmp(comando, "SET_TIME=", 9) == 0)
	{
		int h, m, s;
		if (sscanf(comando + 9, "%d:%d:%d", &h, &m, &s) == 3)
		{
			if (h >= 0 && h <= 23 && m >= 0 && m <= 59 && s >= 0 && s <= 59)
			{
				uint8_t nueva_hora[3] = {s, m, h};  // RTC espera {segundos, minutos, horas}
				RTC_Set_Time(nueva_hora);
				sprintf(buffer, "Hora actualizada a %02d:%02d:%02d\r\n", h, m, s);
				UART_Send_String(buffer);
			}
			else
			{
				UART_Send_String("Error: Formato de hora inválido. Use HH:MM:SS\r\n");
			}
		}
		else
		{
			UART_Send_String("Error: Formato incorrecto. Use SET_TIME=HH:MM:SS\r\n");
		}
	}
	else if (strncmp(comando, "SET_TM=", 7) == 0)
	{
		int segundos;
		if (sscanf(comando + 7, "%d", &segundos) == 1)
		{
			if (segundos >= 2 && segundos <= 60)
			{
				TIMER_SetPeriodo(segundos);
				sprintf(buffer, "Tasa de muestreo cambiada a %d segundos\r\n", segundos);
				UART_Send_String(buffer);
			}
			else
			{
				UART_Send_String("Error: El valor debe estar entre 2 y 60 segundos\r\n");
			}
		}
		else
		{
			UART_Send_String("Error: Formato incorrecto. Use SET_TM=SS\r\n");
		}
	}
	else if (strlen(comando) > 0)
	{
		UART_Send_String("Comando no reconocido. Escriba HELP para ayuda.\r\n");
	}
}