#include "18b20.h"
#include "stm8s_gpio.h"


#define DS18B20_GPIOx GPIOB
#define DS18B20_GPIO_Pin GPIO_PIN_0

#define DS18B20_DQ_OUT  DS18B20_GPIOx->DDR |= (uint8_t)DS18B20_GPIO_Pin//输出
#define DS18B20_DQ_IN  DS18B20_GPIOx->DDR &= (uint8_t)(~(DS18B20_GPIO_Pin))//输入
#define DS18B20_DQ_HIGH DS18B20_GPIOx->ODR |= (uint8_t)DS18B20_GPIO_Pin //拉高
#define DS18B20_DQ_LOW DS18B20_GPIOx->ODR &= (uint8_t)(~(DS18B20_GPIO_Pin)) //拉低
#define DS18B20_DQ_PULL_UP DS18B20_GPIOx->CR1 |= (uint8_t)DS18B20_GPIO_Pin //上拉
#define DS18B20_DQ_PUSH_PULL DS18B20_GPIOx->CR1 |= (uint8_t)DS18B20_GPIO_Pin //推挽
#define DS18B20_DQ_OPEN_DRAIN DS18B20_GPIOx->CR1 &= (uint8_t)(~(DS18B20_GPIO_Pin)) //开漏
#define DS18B20_DQ_VALUE GPIO_ReadInputPin(DS18B20_GPIOx, DS18B20_GPIO_Pin)//读


void _delay_us(u16 z)     //微秒延时@16mhz
{
  while(z--)
  {
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
  }
}

void DS18B20_Init(void)
{
DS18B20_DQ_OUT;
DS18B20_DQ_PUSH_PULL;
DS18B20_DQ_HIGH;
_delay_us(10);
DS18B20_DQ_LOW;
_delay_us(600); //复位脉冲

DS18B20_DQ_IN;
DS18B20_DQ_PULL_UP;
_delay_us(100);
while(DS18B20_DQ_VALUE == 1);
_delay_us(400);
}

//等待DS18B20的回应
//返回0:未检测到DS18B20的存在
//返回1:存在
bool DS18B20_Check(void)
{
 uint8_t retry=0;    
    DS18B20_DQ_OUT;
    DS18B20_DQ_PUSH_PULL;
    DS18B20_DQ_HIGH;
    _delay_us(10);
    DS18B20_DQ_LOW;
    _delay_us(600); //复位脉冲
    
    DS18B20_DQ_IN;
    DS18B20_DQ_PULL_UP;
    _delay_us(15);
    while (DS18B20_DQ_VALUE&&retry<200) { retry++; _delay_us(1); };
       if(retry>=200)return 0;
       else  retry=0;
    while (!DS18B20_DQ_VALUE&&retry<240) {retry++; _delay_us(1); };
       if(retry>=240)return 0;  
       return 1;
}


void DS18B20_WriteByte(unsigned char _data)
{
unsigned char i = 0;

DS18B20_DQ_OUT;
for (i = 0; i < 8; i++)
{
DS18B20_DQ_LOW;
_delay_us(2);
if (_data & 0x01)
{
DS18B20_DQ_HIGH;
}
_data >>= 1;
_delay_us(60);
DS18B20_DQ_HIGH;
}
}

unsigned char DS18B20_ReadByte(void)
{
unsigned char i = 0, _data = 0;

for (i = 0; i < 8; i++)
{
DS18B20_DQ_OUT;
DS18B20_DQ_LOW;
_delay_us(5);
_data >>= 1;
DS18B20_DQ_HIGH;
DS18B20_DQ_IN;
if (DS18B20_DQ_VALUE)
{
_data |= 0x80;
}
DS18B20_DQ_OUT;
DS18B20_DQ_HIGH;
_delay_us(60);
}

return _data;
}


void DS18B20_ReadTemperature(uint8_t *temph,uint8_t *templ)
{

DS18B20_Init();
DS18B20_WriteByte(0xcc);
DS18B20_WriteByte(0x44);

DS18B20_Init();
DS18B20_WriteByte(0xcc);
DS18B20_WriteByte(0xbe);

*templ = DS18B20_ReadByte();
*temph = DS18B20_ReadByte();
}