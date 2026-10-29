#include "cli.h"

// Prototipo de función privada
static void procesar_comando(char *comando);

// Función de inicialización del CLI (Interfaz de Línea de Comandos)
void CLI_Init(void) {
	// Inicializar el hardware de comunicación
	UART_Init();
	
	// Enviar mensaje de bienvenida a través de UART
	UART_Send_String("\r\n=== LED RGB ===\r\n");
	UART_Send_String("Sistema iniciado. Esperando datos...\r\n");
	UART_Send_String("Comando: SET_COLOR=r,g,b\r\n");
}

void CLI_Procesar(void) {
	// Actúa si la UART avisa que hay datos
	if (UART_ComandoDisponible()) {
		char comando_buffer[RX_BUFFER_SIZE];
		UART_GetComando(comando_buffer, sizeof(comando_buffer));
		procesar_comando(comando_buffer);
	}
}

static void procesar_comando(char *comando) {
	char buffer_notificacion[60];

	// Validación del encabezado del comando
	if (strncmp(comando, "SET_COLOR=", 10) == 0) {
		int r, g, b;
		
		// Parseo de los 3 valores decimales (0-255) separados por comas
		if (sscanf(comando + 10, "%d,%d,%d", &r, &g, &b) == 3) {
			
			// Verificación de rango para tipos uint8_t
			if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
				
				// Actualizar el objetivo de la máquina de estados de los LEDs
				ControlLed_SetColorObjetivo((uint8_t)r, (uint8_t)g, (uint8_t)b);
				
				sprintf(buffer_notificacion, "OK: Color actualizado a R:%d G:%d B:%d\r\n", r, g, b);
				UART_Send_String(buffer_notificacion);
				} else {
				UART_Send_String("Error: Valores fuera de rango (0-255).\r\n");
			}
			} else {
			UART_Send_String("Error: Formato incorrecto. Use R,G,B\r\n");
		}
		} else if (strlen(comando) > 0) {
		UART_Send_String("Comando no reconocido.\r\n");
	}
}