#ifndef __HPDL1414_H
#define __HPDL1414_H
#include "stm8s.h"

#define BYTE0(dwTemp)       (dwTemp%10)
#define BYTE1(dwTemp)       ((dwTemp%100)/10)
#define BYTE2(dwTemp)       ((dwTemp%1000 )/100)
#define BYTE3(dwTemp)       ((dwTemp%10000 )/1000)

//#define show_zero_before_num//解除屏蔽则在各种参数加0


void HPDL1414_gpio_init(void);
void send_dat(uint8_t data);
void write_one_bit_dat(uint8_t port, uint8_t dat);
void refresh_display(void);
void empty_display(void);
void write_display_cache(uint8_t port, uint8_t dat);
void show_date(uint8_t year, uint8_t mon, uint8_t date);
void show_week_temp(unsigned char *time_dat,bool effective_18b20);
void show_time_second(uint8_t hour, uint8_t min, uint8_t sed, uint8_t mode);
bool show_cpu_inf(uint8_t cpu_o,uint8_t cpu_t,uint8_t dat_value);
bool show_net_inf(uint32_t net_down,uint32_t net_up,uint8_t dat_value);
bool show_ram_inf(uint32_t a_ram,uint32_t t_ram,uint8_t dat_value);
bool show_ram_occ_rate(uint32_t oc_ram,uint8_t dat_value);
#endif