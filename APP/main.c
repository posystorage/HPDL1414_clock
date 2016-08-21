#include "stm8s.h"
#include "stm8s_gpio.h"
#include "i2c.h"
#include "HPDL1414.h"
#include "uart.h"
#include "system_int.h"

#define max_time_ms 20000

uint16_t runing_time;
uint8_t display_rotate=0;
unsigned char time_dat[9];

uint16_t key1_time;
uint8_t key2_time;
bool lock_display=0;//������ʾ����
uint16_t lock_refresh_timeout=0;//����ˢ�³�ʱ
bool lock_new_dat=0;

bool one_second_interrupt = 0;
bool ubFrameDone = 0;
bool need_refresh_display=0;
bool effective_18b20=0;

extern uint8_t dat_value;
extern uint8_t CO_Adat;//3
extern uint8_t CT_Bdat;//3
extern uint32_t TM_Cdat;//6
extern uint32_t AM_Ddat;//6
extern uint8_t MO_Edat;//3
extern uint32_t DS_Fdat;//7
extern uint32_t US_Gdat;//7

void displsy_serve(void)
{
  if(runing_time>max_time_ms)
  {
    switch(display_rotate)
    {
    case 0:
      show_time_second(time_dat[3], time_dat[4], time_dat[5], 0);
      dat_value=0;//һ��ѭ���ڴ��ڲ������ݣ�����һѭ��������ʾ
      runing_time=max_time_ms-3000;
      need_refresh_display=1;
    break;
    case 1:
      show_date(time_dat[0],time_dat[1], time_dat[2]); 
      runing_time=max_time_ms-1000;
      need_refresh_display=1;
    break;
    case 2:
      show_week_temp(time_dat,effective_18b20);
      runing_time=max_time_ms-1000;
      need_refresh_display=1;
    break;
    case 3:
      if(show_cpu_inf(CO_Adat,CT_Bdat,dat_value))
      {
        runing_time=max_time_ms-1000;
        need_refresh_display=1;
      }
      else
      {
        runing_time=max_time_ms;
      }
    break;
    case 4:
     if(show_ram_inf(AM_Ddat,TM_Cdat,dat_value))
      {
        runing_time=max_time_ms-1000;
        need_refresh_display=1;
      }
      else
      {
        runing_time=max_time_ms;
      }
    break;
    case 5:
      if(show_ram_occ_rate(MO_Edat,dat_value))
      {
        runing_time=max_time_ms-1000;
        need_refresh_display=1;
      }
      else
      {
        runing_time=max_time_ms;
      }      
    break;
    case 6:
      if(show_net_inf(DS_Fdat,US_Gdat,dat_value))
      {
        runing_time=max_time_ms-1000;
        need_refresh_display=1;
      }
      else
      {
        runing_time=max_time_ms;
      }      
    break;
    
    
    
    }
    display_rotate++;
if(display_rotate>6)display_rotate=0;
  }
}


void key1_serve(void)
{
    if(GPIO_ReadInputPin(GPIOC, GPIO_PIN_5)==0&&key1_time==0)key1_time=1;//���°���һ���Զ�������ʾʱ�䣬����5s
    if(GPIO_ReadInputPin(GPIOC, GPIO_PIN_5)==0&&key1_time>1)
    {
      show_time_second(time_dat[3], time_dat[4], time_dat[5], 0);
      refresh_display();  
      while(GPIO_ReadInputPin(GPIOC, GPIO_PIN_5)==0);
      if(key1_time>600&&lock_display==0)//��������һ����ʱ��
      {
        lock_display=1;
        lock_new_dat = 0;
        runing_time=0;
        key1_time=0;
        display_rotate=1;
        show_time_second(time_dat[3], time_dat[4], time_dat[5], 0);
        refresh_display();  
      }
      else
      {
        show_time_second(time_dat[3], time_dat[4], time_dat[5], 0);
        refresh_display();  
        display_rotate=1;
        
        if(lock_display)
        {
          lock_display=0;//����
          lock_refresh_timeout = 0;
          lock_new_dat = 0;
        }
        runing_time=max_time_ms-5000;//5s
        key1_time=0;
      }
    }
    else    
    {
      if(GPIO_ReadInputPin(GPIOC, GPIO_PIN_5)==1)
      key1_time=0;
    }
}

void lock_refresh_dispaly()//�����е�ˢ��
{
   switch(display_rotate)
    {
    case 1:
      show_time_second(time_dat[3], time_dat[4], time_dat[5], 0);
      need_refresh_display=1;
    break;
    case 2:
      show_date(time_dat[0],time_dat[1], time_dat[2]); 
      need_refresh_display=1;
    break;
    case 3:
      show_week_temp(time_dat,effective_18b20);
      need_refresh_display=1;
    break;
    case 4:
      if(show_cpu_inf(CO_Adat,CT_Bdat,dat_value))
      {
        lock_refresh_timeout=1;
        need_refresh_display=1;
      }
    break;
    case 5:
     if(show_ram_inf(AM_Ddat,TM_Cdat,dat_value))
      {
        lock_refresh_timeout=1;
        need_refresh_display=1;
      }
    break;
    case 6:
      if(show_ram_occ_rate(MO_Edat,dat_value))
      {
        lock_refresh_timeout=1;
        need_refresh_display=1;
      }
    break;
    case 0:
      if(show_net_inf(DS_Fdat,US_Gdat,dat_value))
      {
        lock_refresh_timeout=1;
        need_refresh_display=1;
      }     
    break;
    }
    if(need_refresh_display)
  {
    refresh_display();
    need_refresh_display=0;
  }
}

//���°�������������ǰ���棬�ٴΰ��£�������//Ҫ�����ĸ�״̬����һ�ΰ��£��ڶ��ΰ��£��������̵Ļ���ˢ�£�������ݲ�ˢ�£���ʱ�Զ�����
void key2_serve(void)
{
    if(GPIO_ReadInputPin(GPIOC, GPIO_PIN_3) == 0 && key2_time == 0)key2_time = 1;
    if(GPIO_ReadInputPin(GPIOC, GPIO_PIN_3) == 0 && key2_time > 1)
    {
        if(lock_display)
        {
            lock_display = 0;
            display_rotate++;//����һ��
            runing_time = max_time_ms;
            lock_refresh_timeout = 0;
            lock_new_dat = 0;
        }
        else
        {
          runing_time=0;
          lock_display = 1;
          lock_refresh_dispaly();
          GPIO_WriteReverse(GPIOD, GPIO_PIN_4);     
        }
      while(GPIO_ReadInputPin(GPIOC, GPIO_PIN_3)==0);
        key2_time = 0;
    }
    else
    {
        if(GPIO_ReadInputPin(GPIOC, GPIO_PIN_3) == 1)
        key2_time = 0;
    }
    if(lock_display && lock_refresh_timeout > 5000) //��5s��ˢ��
    {
        lock_display = 0;
        display_rotate++;//����һ��
        runing_time = max_time_ms;
        lock_refresh_timeout = 0;
        lock_new_dat = 0;
    }
    if(lock_display && lock_new_dat)//�����е�����ˢ��
    {
        lock_refresh_dispaly();
        lock_new_dat=0;
    }
}




int main( void )
{
    sys_int();
     runing_time=max_time_ms;
    while(1)
    {   
        if(one_second_interrupt == 1)
        {
          if(lock_display)lock_new_dat=1;
        one_second_interrupt = 0;
        get_dat(time_dat) ;
        if(display_rotate==1)
        {
          show_time_second(time_dat[3], time_dat[4], time_dat[5], 0);
          need_refresh_display=1;
        }
        }
        if(ubFrameDone==1)
        {
         if(lock_display)lock_new_dat=1;
         GPIO_WriteReverse(GPIOD, GPIO_PIN_4);
         data_analysis();
         ubFrameDone=0;
        }
        displsy_serve();
        if(need_refresh_display)
        {
          refresh_display();
          need_refresh_display=0;
        }
        key1_serve();    
        key2_serve();
    }
}


