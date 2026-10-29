#ifndef CLI_H_
#define CLI_H_

#include "utils.h"
#include "uart.h"
#include "control_led.h"

// Envíar el mensaje de bienvenida
void CLI_Init(void);

// Procesar datos de la UART
void CLI_Procesar(void);

#endif /* CLI_H_ */