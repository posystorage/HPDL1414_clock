#ifndef __18B20_H
#define __18B20_H
#include "stm8s.h"


bool DS18B20_Check(void);
void DS18B20_ReadTemperature(uint8_t *temph,uint8_t *templ);
#endif