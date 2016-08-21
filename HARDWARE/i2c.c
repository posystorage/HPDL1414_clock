#include "i2c.h"

void I2C_MASTERMODE_Init(I2C_SpeedMode_TypeDef I2C_Speed)
{
	
	I2C_Init(I2C_Speed, I2C1_SLAVE_ADDRESS7, I2C_DUTYCYCLE_2,\
		I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
	I2C_Cmd(ENABLE);
}


void I2C_DS3231_WriteOneByte(u8 WriteAddr,u8 Byte)
{
	/* �ȴ����� */
	while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
	
	/* ����ʼλ */
	I2C_GenerateSTART(ENABLE);
	
	/* ����EV5 ��������������һ��Ӧ���ź�*/
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
	
	/* ����I2C��������ַ��I2C���豸Ϊдģʽ*/
	I2C_Send7bitAddress(DS3231_WriteAddress, I2C_DIRECTION_TX);
	
	/* ����EV6�������־λ��������������һ��Ӧ���ź� */
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	 
	/* ������������д�����ڲ��ĵ�ַ */
	I2C_SendData(WriteAddr);
	/* ����EV8�������־λ ��������������һ��Ӧ���ź�*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	/* �������дһ���ֽ� */
	I2C_SendData(Byte); 
	
	/* ����EV8�������־λ��������������һ��Ӧ���ź� */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING)); 
	
	/* ������λ */
	I2C_GenerateSTOP(ENABLE);
}


u8 I2C_DS3231_ReadOneByte(u8 ReadAddr)
{
	u8 RxData;
	/* �ȴ����� */
	while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
	
	/* ����ʼλ */
	I2C_GenerateSTART(ENABLE);
	
	/* ����EV5 ��������������һ��Ӧ���ź�*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	/* ����I2C��������ַ��I2C���豸Ϊдģʽ*/ 
	I2C_Send7bitAddress(DS3231_WriteAddress, I2C_DIRECTION_TX);
	
	/* ����EV6�������־λ */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
        
	I2C_Cmd(ENABLE); 

	I2C_SendData(ReadAddr);

	/* ����EV8�������־λ */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	/* ����ʼλ */ 
	I2C_GenerateSTART(ENABLE);
	
	/* ����EV5 ��������������һ��Ӧ���ź�*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	/* ����I2C��������ַ��I2C���豸Ϊ��ģʽ*/
	I2C_Send7bitAddress(DS3231_WriteAddress, I2C_DIRECTION_RX);
	
	/* ����EV6 ��������������һ��Ӧ���ź�*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	/* ����EV7��������������һ��Ӧ���ź� */
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
	RxData=I2C_ReceiveData();
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	
	/* ������λ*/
	I2C_GenerateSTOP(ENABLE);
        return RxData;
}


unsigned char BCD2HEX(unsigned char val)    //BCDת��ΪByte
{
     unsigned char temp;
     temp=val&0x0f;
     val>>=4;
     val&=0x0f;
     val*=10;
     temp+=val;
   
     return temp;
}

unsigned char HEX2BCD(unsigned char val)    //B��ת��ΪBCD��
{
     unsigned char i,j,k;
     i=val/10;
     j=val%10;
     k=j+(i<<4);
     return k;
}


void ModifyTime(unsigned char yea,unsigned char mon,unsigned char da,unsigned char hou,unsigned char min,unsigned char sec,unsigned char week)
{
     unsigned char temp=0;
   
     temp=HEX2BCD(yea);
     I2C_DS3231_WriteOneByte(DS3231_YEAR,temp);   //�޸���
   
     temp=HEX2BCD(mon);
     I2C_DS3231_WriteOneByte(DS3231_MONTH,temp);  //�޸���
   
     temp=HEX2BCD(da);
     I2C_DS3231_WriteOneByte(DS3231_DAY,temp);    //�޸���
   
     temp=HEX2BCD(hou);
     I2C_DS3231_WriteOneByte(DS3231_HOUR,temp);   //�޸�ʱ
   
     temp=HEX2BCD(min);
     I2C_DS3231_WriteOneByte(DS3231_MINUTE,temp); //�޸ķ�
   
     temp=HEX2BCD(sec);
     I2C_DS3231_WriteOneByte(DS3231_SECOND,temp); //�޸���
     
     I2C_DS3231_WriteOneByte(DS3231_WEEK,week);//�޸�����
}




	



void DS3231_square_out_int(void)
{
  unsigned char temp;
  temp=I2C_DS3231_ReadOneByte(DS3231_CONTROL);
  if(temp&0x10)
  I2C_DS3231_WriteOneByte(DS3231_CONTROL,temp^0x10);
  temp=I2C_DS3231_ReadOneByte(DS3231_CONTROL);
  if(temp&0x08)
  I2C_DS3231_WriteOneByte(DS3231_CONTROL,temp^0x08);
  temp=I2C_DS3231_ReadOneByte(DS3231_CONTROL);
  if(temp&0x04)
  I2C_DS3231_WriteOneByte(DS3231_CONTROL,temp^0x04);
}


void DS3231_Exti_Init(void)
{
  	/* �밴����������������Ϊ����ģʽ */
    GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_IN_PU_IT);
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_FALL_ONLY);
}


void get_dat(unsigned char* Dtemp)
{
  unsigned char Ytemp1,Mtemp1,Dtemp1,Htemp1,Mitemp1,Stemp1;//,Ttemp1,Ttemp3;

	 Ytemp1=I2C_DS3231_ReadOneByte(DS3231_YEAR);        //��
    Dtemp[0]=BCD2HEX(Ytemp1);
   
     Mtemp1=I2C_DS3231_ReadOneByte(DS3231_MONTH);       //��
   Dtemp[1]=BCD2HEX(Mtemp1);
   
     Dtemp1=I2C_DS3231_ReadOneByte(DS3231_DAY);         //��
    Dtemp[2]=BCD2HEX(Dtemp1);

    Htemp1=I2C_DS3231_ReadOneByte(DS3231_HOUR);    //ʱ 24Сʱ��
    Htemp1&=0x3f;                  
    Dtemp[3]=BCD2HEX(Htemp1);
   
     Mitemp1=I2C_DS3231_ReadOneByte(DS3231_MINUTE);  //��
    Dtemp[4]=BCD2HEX(Mitemp1);
   
     Stemp1=I2C_DS3231_ReadOneByte(DS3231_SECOND);  //��
    Dtemp[5]=BCD2HEX(Stemp1);
   Dtemp[6]=I2C_DS3231_ReadOneByte(DS3231_TEMPERATUREH);    //�¶� ���ֽ�
   Dtemp[7]=I2C_DS3231_ReadOneByte(DS3231_TEMPERATUREL);    //�¶� ���ֽ�
    Dtemp[8]=I2C_DS3231_ReadOneByte(DS3231_WEEK);    //����
}

