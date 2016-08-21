
#ifndef __I2C_H
#define __I2C_H

#include "stm8s.h"

typedef enum{
  STANDARDSPEED=100000,
  FASTSPEED=400000
}I2C_SpeedMode_TypeDef;


#define I2C1_SLAVE_ADDRESS7    0xA0

#define DS3231_WriteAddress 0xD0    //����д��ַ
#define DS3231_SECOND       0x00    //��
#define DS3231_MINUTE       0x01    //��
#define DS3231_HOUR         0x02    //ʱ
#define DS3231_WEEK         0x03    //����
#define DS3231_DAY          0x04    //��
#define DS3231_MONTH        0x05    //��
#define DS3231_YEAR         0x06    //��
//����1            
#define DS3231_SALARM1ECOND 0x07    //��
#define DS3231_ALARM1MINUTE 0x08    //��
#define DS3231_ALARM1HOUR   0x09    //ʱ
#define DS3231_ALARM1WEEK   0x0A    //����/��
//����2
#define DS3231_ALARM2MINUTE 0x0b    //��
#define DS3231_ALARM2HOUR   0x0c    //ʱ
#define DS3231_ALARM2WEEK   0x0d    //����/��
#define DS3231_CONTROL      0x0e    //���ƼĴ���
#define DS3231_STATUS       0x0f    //״̬�Ĵ���
#define BSY                 2       //æ
#define OSF                 7       //����ֹͣ��־
#define DS3231_XTAL         0x10    //�����ϻ��Ĵ���
#define DS3231_TEMPERATUREH 0x11    //�¶ȼĴ������ֽ�(8λ)
#define DS3231_TEMPERATUREL 0x12    //�¶ȼĴ������ֽ�(��2λ)


void I2C_MASTERMODE_Init(I2C_SpeedMode_TypeDef I2C_Speed);
void I2C_DS3231_WriteOneByte(u8 WriteAddr,u8 Byte);
u8 I2C_DS3231_ReadOneByte(u8 ReadAddr);
void DS3231_square_out_int(void);
void DS3231_Exti_Init(void);

void get_dat(unsigned char* Dtemp);
void ModifyTime(unsigned char yea,unsigned char mon,unsigned char da,unsigned char hou,unsigned char min,unsigned char sec,unsigned char week);
#endif /* __I2C_H */



