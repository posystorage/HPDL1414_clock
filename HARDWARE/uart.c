#include "uart.h"
#include "stm8s_uart1.h"
#include "i2c.h"

uint8_t ubUart1RxBuf[100] = {0};
uint8_t ubUart1RxDat=0;
uint8_t dat_value=0;//记录哪几个数据发送了，哪几个没有

uint8_t CO_Adat;//3
uint8_t CT_Bdat;//3
uint32_t TM_Cdat;//6
uint32_t AM_Ddat;//6
uint8_t MO_Edat;//3
uint32_t DS_Fdat;//7
uint32_t US_Gdat;//7
//日期
uint8_t TIMEYY_Hdat;
uint8_t TIMEMM_Hdat;
uint8_t TIMEDD_Hdat;
uint8_t TIMEH_Hdat;
uint8_t TIMEM_Hdat;
uint8_t TIMES_Hdat;
uint8_t TIMEW_Hdat;


void get_dat_32asc2hex(uint8_t *dat_adr,uint32_t *hex_dat,uint8_t *i)
{
  uint8_t j=0;
  *hex_dat=0;
  for(j=1;j<8;j++)
  {
    (*hex_dat)*=10;*hex_dat+=dat_adr[*i+j]-'0';
    if(dat_adr[*i+j+1]=='@'){*i+=j; break;}  
  }
}

void get_dat_8asc2hex(uint8_t *dat_adr,uint8_t *hex_dat,uint8_t *i)
{
    if(dat_adr[*i+2]=='@'){*hex_dat=dat_adr[*i+1]-'0';*i+=1; return;}
    if(dat_adr[*i+3]=='@'){*hex_dat=dat_adr[*i+1]-'0';*hex_dat*=10;*hex_dat+=dat_adr[*i+2]-'0';*i+=2; return;} 
    if(dat_adr[*i+4]=='@'){*hex_dat=dat_adr[*i+1]-'0';*hex_dat*=10;*hex_dat+=dat_adr[*i+2]-'0';*hex_dat*=10;CO_Adat+=dat_adr[*i+3]-'0';*i+=3; return;} 
}
void get_2bit_dat_asc2hex(uint8_t *dat_adr,uint8_t *hex_dat,uint8_t *i)
{
*hex_dat=dat_adr[*i+1]-'0';
*hex_dat*=10;
*hex_dat+=dat_adr[*i+2]-'0';
*i+=2;
}


void data_analysis(void)
{
    uint8_t i;

    dat_value = 0;
    for(i = 0; i < ubUart1RxDat; i++)
    {
        if(ubUart1RxBuf[i] == '&')goto end;
        if(ubUart1RxBuf[i] == '$' || ubUart1RxBuf[i] == '@')
        {
            i++;
            switch(ubUart1RxBuf[i])
            {
            case 'A':
            {
                dat_value = dat_value | 0x01;
                get_dat_8asc2hex(ubUart1RxBuf, &CO_Adat, &i);
                break;
            }
            case 'H':
            {
              dat_value = dat_value | 0x80;
              get_2bit_dat_asc2hex(ubUart1RxBuf,&TIMEYY_Hdat,&i);
              get_2bit_dat_asc2hex(ubUart1RxBuf,&TIMEMM_Hdat,&i);
              get_2bit_dat_asc2hex(ubUart1RxBuf,&TIMEDD_Hdat,&i);
              get_2bit_dat_asc2hex(ubUart1RxBuf,&TIMEH_Hdat,&i);
              get_2bit_dat_asc2hex(ubUart1RxBuf,&TIMEM_Hdat,&i);
              get_2bit_dat_asc2hex(ubUart1RxBuf,&TIMES_Hdat,&i);
              TIMEW_Hdat=ubUart1RxBuf[i+1];
              if(TIMEW_Hdat==0)TIMEW_Hdat=7;
              ModifyTime(TIMEYY_Hdat,TIMEMM_Hdat,TIMEDD_Hdat,TIMEH_Hdat,TIMEM_Hdat,TIMES_Hdat,TIMEW_Hdat);
                break;
            }
            case 'B':
            {
                dat_value = dat_value | 0x02;
                get_dat_8asc2hex(ubUart1RxBuf, &CT_Bdat, &i);
                break;
            }
            case 'C':
            {
              dat_value = dat_value | 0x04;
                get_dat_32asc2hex(ubUart1RxBuf, &TM_Cdat, &i);
                break;
            }
            case 'D':
            {
                dat_value = dat_value | 0x08;
                get_dat_32asc2hex(ubUart1RxBuf, &AM_Ddat, &i);
                break;
            }
            case 'E':
            {
                dat_value = dat_value | 0x10;
                get_dat_8asc2hex(ubUart1RxBuf, &MO_Edat, &i);
                break;
            }
            case 'F':
            {
                dat_value = dat_value | 0x20;
                get_dat_32asc2hex(ubUart1RxBuf, &DS_Fdat, &i);
                break;
            }
            case 'G':
            {
                dat_value = dat_value | 0x40;
                get_dat_32asc2hex(ubUart1RxBuf, &US_Gdat, &i);
                break;
            }
            }
        }
    }
end:
    ;
}



void InitUart1(void)
{
    UART1_DeInit();
    
    //115200bps, 8bit, 1 stopbit, both rx and tx
    UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, 
                UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
                
    UART1_ClearFlag(UART1_FLAG_RXNE);
    
    //Enable Rx interrupt
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
     ITC->ISPR5 &= 0xcf;
   // itc_change_SoftwarePriority_uartRX();
    UART1_Cmd(ENABLE);   
}




