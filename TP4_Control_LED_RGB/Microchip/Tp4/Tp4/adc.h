#ifndef ADC_H_
#define ADC_H_

#include "utils.h"

void ADC_Init();
uint8_t ADC_GetLuz();
void ADC_Read();
void ADC_ClearFlag();
uint8_t ADC_GetFlag();

#endif /* ADC_H_ */