#ifndef DHT11_H
#define DHT11_H

#include "utils.h"

// Inicialización del sensor
void DHT11_Init(void);

// --- Lectura del Sensor DHT11
uint8_t DHT11_Read(uint8_t *humedad, uint8_t *temperatura);

#endif