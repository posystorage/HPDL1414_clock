#include "HPDL1414.h"
#include "stm8s_gpio.h"
#include "18b20.h"

uint8_t display_cache[12] = {0x20};




void DelayApi(uint32_t uwDelayCnt)
{
    for(; uwDelayCnt > 0; uwDelayCnt--);
}


void HPDL1414_gpio_init(void)
{
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOE);
    GPIO_DeInit(GPIOF);

    
    GPIO_Init(GPIOA, GPIO_PIN_1 | GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_3 ,GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOC, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOD, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_FAST);

    GPIO_Init(GPIOF, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
    
    


}


 


void send_dat(uint8_t data)
{
    if(data & 0x01)GPIO_WriteHigh(GPIOC, GPIO_PIN_2);
    else GPIO_WriteLow(GPIOC, GPIO_PIN_2);
    if(data & 0x02)GPIO_WriteHigh(GPIOC, GPIO_PIN_4);
    else GPIO_WriteLow(GPIOC, GPIO_PIN_4);
    if(data & 0x04)GPIO_WriteHigh(GPIOC, GPIO_PIN_6);
    else GPIO_WriteLow(GPIOC, GPIO_PIN_6);
    if(data & 0x08)GPIO_WriteHigh(GPIOC, GPIO_PIN_7);
    else GPIO_WriteLow(GPIOC, GPIO_PIN_7);
    if(data & 0x10)GPIO_WriteHigh(GPIOD, GPIO_PIN_7);
    else GPIO_WriteLow(GPIOD, GPIO_PIN_7);
    if(data & 0x20)GPIO_WriteHigh(GPIOD, GPIO_PIN_3);
    else GPIO_WriteLow(GPIOD, GPIO_PIN_3);
    if(data & 0x40)GPIO_WriteHigh(GPIOD, GPIO_PIN_0);
    else GPIO_WriteLow(GPIOD, GPIO_PIN_0);
}

void write_one_bit_dat(uint8_t port, uint8_t dat)
{
    switch(port)
    {
    case 0:
    case 4:
    case 8:
        GPIO_WriteHigh(GPIOF, GPIO_PIN_4);//0
        GPIO_WriteHigh(GPIOA, GPIO_PIN_2);//1
        break;
    case 1:
    case 5:
    case 9:
        GPIO_WriteLow(GPIOF, GPIO_PIN_4);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_2);
        break;
    case 2:
    case 6:
    case 10:

        GPIO_WriteHigh(GPIOF, GPIO_PIN_4);
        GPIO_WriteLow(GPIOA, GPIO_PIN_2);
        break;
    case 3:
    case 7:
    case 11:
        GPIO_WriteLow(GPIOF, GPIO_PIN_4);//0
        GPIO_WriteLow(GPIOA, GPIO_PIN_2);//1

        break;
    }
    DelayApi(4);
    switch(port)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        GPIO_WriteLow(GPIOD, GPIO_PIN_2);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        GPIO_WriteLow(GPIOA, GPIO_PIN_1);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
        GPIO_WriteLow(GPIOB, GPIO_PIN_3);
        break;
    }
    DelayApi(4);
    send_dat(dat);
    DelayApi(4);
    GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
    GPIO_WriteHigh(GPIOB, GPIO_PIN_3);
    GPIO_WriteHigh(GPIOA, GPIO_PIN_1);
    DelayApi(4);
}

void refresh_display(void)
{
    uint8_t i;
    for(i = 0; i < 12; i++)
    {
        write_one_bit_dat(i, display_cache[i]);
    }
}

void empty_display(void)
{
    uint8_t i;
    for(i = 0; i < 12; i++)
    {
        write_one_bit_dat(i, 0x20);
    }
}

void write_display_cache(uint8_t port, uint8_t dat)
{
    display_cache[port] = dat;
}

void show_date(uint8_t year, uint8_t mon, uint8_t date)
{
    write_display_cache(0, 0x20);
    write_display_cache(1, 0x32);
    write_display_cache(2, 0x30);
    write_display_cache(3, 0x30 + BYTE1(year));
    write_display_cache(4, 0x30 + BYTE0(year));
    write_display_cache(5, 0x2D);
    write_display_cache(6, 0x30 + BYTE1(mon));
    write_display_cache(7, 0x30 + BYTE0(mon));
    write_display_cache(8, 0x2D);
    write_display_cache(9, 0x30 + BYTE1(date));
    write_display_cache(10, 0x30 + BYTE0(date));
    write_display_cache(11, 0x20);
}




void show_time_second(uint8_t hour, uint8_t min, uint8_t sed, uint8_t mode) //mode:24H-0;12H-1
{
    if(mode)
    {
        if(hour > 12)
        {
            write_display_cache(0, 0x50);//p
            hour -= 12;
            write_display_cache(2, 0x30 + BYTE1(hour));
            write_display_cache(3, 0x30 + BYTE0(hour));
        }
        else
        {
            write_display_cache(0, 0x41);//a
            write_display_cache(2, 0x30 + BYTE1(hour));
            write_display_cache(3, 0x30 + BYTE0(hour));
        }
    }
    else
    {
        write_display_cache(0, 0x20);
        write_display_cache(2, 0x30 + BYTE1(hour));
        write_display_cache(3, 0x30 + BYTE0(hour));
    }
    write_display_cache(1, 0x20);
    write_display_cache(4, 0x3A);
    write_display_cache(5, 0x30 + BYTE1(min));
    write_display_cache(6, 0x30 + BYTE0(min));
    write_display_cache(7, 0x3A);
    write_display_cache(8, 0x30 + BYTE1(sed));
    write_display_cache(9, 0x30 + BYTE0(sed));
    write_display_cache(10, 0x20);
    write_display_cache(11, 0x20);
}

void show_three_bit_dat(uint32_t dat,uint8_t lct)
{
#ifdef show_zero_before_num
    write_display_cache(lct, 0x30 + BYTE2(dat));
    write_display_cache(lct+1, 0x30 + BYTE1(dat));
    write_display_cache(lct+2, 0x30 + BYTE0(dat));
#else
    if(dat>99) write_display_cache(lct, 0x30 + BYTE2(dat));
    else write_display_cache(lct, 0x20);
    if(dat>9) write_display_cache(lct+1, 0x30 + BYTE1(dat));
    else write_display_cache(lct+1, 0x20);   
    write_display_cache(lct+2, 0x30 + BYTE0(dat));
  
#endif
}



void show_week_temp(unsigned char *time_dat,bool effective_18b20)
{
  uint8_t temph_18b20,templ_18b20;
  uint8_t temph,templ;
  DS18B20_ReadTemperature(&temph_18b20,&templ_18b20);
    write_display_cache(0, 0x20);
    write_display_cache(4, 0x20);
    write_display_cache(11, 0x20);    
  switch(time_dat[8])
  {
    case 1:
          write_display_cache(1, 'M');
          write_display_cache(2, 'O');
          write_display_cache(3, 'N');
    break;
    case 2:
          write_display_cache(1, 'T');
          write_display_cache(2, 'U');
          write_display_cache(3, 'E');
    break;
    case 3:
          write_display_cache(1, 'W');
          write_display_cache(2, 'E');
          write_display_cache(3, 'D');
    break;
    case 4:
          write_display_cache(1, 'T');
          write_display_cache(2, 'H');
          write_display_cache(3, 'U');
    break;
    case 5:
          write_display_cache(1, 'F');
          write_display_cache(2, 'R');
          write_display_cache(3, 'I');
    break;
    case 6:
          write_display_cache(1, 'S');
          write_display_cache(2, 'A');
          write_display_cache(3, 'T');
    break;
    case 7:
    case 0:  
          write_display_cache(1, 'S');
          write_display_cache(2, 'U');
          write_display_cache(3, 'N');
    break;  
  }
        
  if(effective_18b20)  //有效
  {
     if(temph_18b20>0x07)//判断正负
     {
      templ_18b20=0xff-templ_18b20;
      temph_18b20=0xff-temph_18b20; 
      temph = ((templ_18b20 & 0xf0) >> 4)+((temph_18b20 & 0x0f) << 4);
      show_three_bit_dat(temph,5);
      write_display_cache(5,0x2d);
     }
     else
     {
      temph = ((templ_18b20 & 0xf0) >> 4)+((temph_18b20 & 0x0f) << 4);
      show_three_bit_dat(temph,5);
     }
      templ = (templ_18b20 & 0x0f)*10/16;
      write_display_cache(8,0x2e);
      write_display_cache(9,0x30 + BYTE0(templ));
      write_display_cache(10,'C');
  }
  else
  {
    if(time_dat[6]&0x80)
    {
    time_dat[6]=0xff-time_dat[6];
    time_dat[7]=0xff-time_dat[7];
    show_three_bit_dat(time_dat[6],5);
    }
    else
    {
    show_three_bit_dat(time_dat[6],5);      
    } 
      switch(time_dat[7]&0xc0)
      {
      case 0x00:templ=0;break; 
      case 0x40:templ=3;break;       
      case 0x80:templ=5;break; 
      case 0xc0:templ=8;break;       
      }
      write_display_cache(8,0x2e);
      write_display_cache(9,0x30 + BYTE0(templ));
      write_display_cache(10,'C');    
  }
}

void show_num_name(uint8_t nam1,uint8_t nam2,uint8_t nam3)
{
    write_display_cache(0, nam1);
    write_display_cache(1, nam2);
    write_display_cache(2, nam3);
    write_display_cache(3, 0x3A);//:
}


bool show_cpu_inf(uint8_t cpu_o,uint8_t cpu_t,uint8_t dat_value)
{
  if(dat_value&0x03)
  {
   show_num_name('C','P','U');
   switch(dat_value&0x03)
   {
   case 0x01:
      show_three_bit_dat(cpu_o,5);
      write_display_cache(8, 0x25);//%
      write_display_cache(4, 0x20);
      write_display_cache(9, 0x20);
      write_display_cache(10, 0x20);
      write_display_cache(11, 0x20);
     break;
   case 0x02:
      show_three_bit_dat(cpu_t,5);
      write_display_cache(8, 0x43);//c 
      write_display_cache(4, 0x20);
      write_display_cache(9, 0x20);
      write_display_cache(10, 0x20);
      write_display_cache(11, 0x20);
     break;
   case 0x03:
        show_three_bit_dat(cpu_o,4);
        write_display_cache(7, 0x25);//%
        show_three_bit_dat(cpu_t,8);
        write_display_cache(11, 0x43);//c  
     break;     
   }
     return 1;
  }
 else return 0;     
}


void show_three_bit_dat_change_unit(uint32_t dat,uint8_t lct,uint8_t s_unit,uint8_t b_unit)
{
    if(dat>999)
    {
      if(dat>9999)
      {
      dat*=10;  
      dat>>=10; 
      if(BYTE0(dat)>8)dat+=1;
      dat/=10;
      show_three_bit_dat(dat,lct); 
      }
      else
      {
       write_display_cache(lct, 0x30 + BYTE3(dat)); 
       write_display_cache(lct+1, 0x2e); //.       
       write_display_cache(lct+2, 0x30 + BYTE2(dat)); 
      }

    write_display_cache(lct+3, b_unit);
    }
    else
    {
    write_display_cache(lct+3, s_unit);
    show_three_bit_dat(dat,lct);
    }
}

bool show_net_inf(uint32_t net_down, uint32_t net_up, uint8_t dat_value)
{



        show_num_name('N', 'E', 'T');
        switch(dat_value & 0x60)
        {

        case 0x60://all

            show_three_bit_dat_change_unit(net_down, 4, 0x4b, 0x4d);
            show_three_bit_dat_change_unit( net_up , 8, 0x4b, 0x4d);

            return 1;
            break;
        case 0x20://down_only
            write_display_cache(4, 0x44);//d

            if(net_down > 1000)show_three_bit_dat(net_down / 1000, 5);
            else
            {
#ifdef show_zero_before_num
                write_display_cache(5, 0x30);
                write_display_cache(6, 0x30);
                write_display_cache(7, 0x30);
#else
                write_display_cache(5, 0x20);
                write_display_cache(6, 0x20);
                write_display_cache(7, 0x20);
#endif
            }
            show_three_bit_dat(net_down, 8);

            write_display_cache(11, 0x4b);//k
            return 1;
            break;
        case 0x40://up_only
            write_display_cache(4, 'U');

            if(net_up > 1000)show_three_bit_dat(net_up / 1000, 5);
            else
            {
#ifdef show_zero_before_num
                write_display_cache(5, 0x30);
                write_display_cache(6, 0x30);
                write_display_cache(7, 0x30);
#else
                write_display_cache(5, 0x20);
                write_display_cache(6, 0x20);
                write_display_cache(7, 0x20);
#endif
            }
            show_three_bit_dat(net_up, 8);

            write_display_cache(11, 0x4b);//k
            return 1;
            break;
        default:
          return 0;
            break;

    }
}

bool show_ram_inf(uint32_t a_ram,uint32_t t_ram,uint8_t dat_value)
{
  if((dat_value&0x04)&&(dat_value&0x08))
  {
    show_three_bit_dat_change_unit(a_ram,4,'M','G');
    show_three_bit_dat_change_unit(t_ram,8,'M','G');

    show_num_name('R','A','M'); 
    if(display_cache[8]==0x20)
    {
      display_cache[8]=0x2f;
    }
    else
    {
      if(display_cache[7]==display_cache[11]) 
        display_cache[7]=0x2f; 
    }   
  return 1;
  }
  else return 0;
}

bool show_ram_occ_rate(uint32_t oc_ram,uint8_t dat_value)
{
  if(dat_value&0x10)
  {
  show_num_name('R','A','M'); 
  show_three_bit_dat(oc_ram,5);
  write_display_cache(8, 0x25);//%    
  write_display_cache(4, 0x20);
  write_display_cache(9, 0x20);
  write_display_cache(10, 0x20);
  write_display_cache(11, 0x20); 
  return 1;
  }
  else return 0;

}