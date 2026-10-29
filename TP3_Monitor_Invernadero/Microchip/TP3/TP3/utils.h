#ifndef UTILS_H_

#define UTILS_H_

#define F_CPU 16000000UL
#define SCL_CLOCK  100000L
// Dirección I2C del DS3231/DS3232
#define DS3231_ADDRESS 0x68

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <stdint.h>

#endif