#include "i2c.h"

void I2C_MASTERMODE_Init(I2C_SpeedMode_TypeDef I2C_Speed)
{
	
	I2C_Init(I2C_Speed, I2C1_SLAVE_ADDRESS7, I2C_DUTYCYCLE_2,\
		I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
	I2C_Cmd(ENABLE);
}


void I2C_DS3231_WriteOneByte(u8 WriteAddr,u8 Byte)
{
	/* 等待空闲 */
	while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
	
	/* 发起始位 */
	I2C_GenerateSTART(ENABLE);
	
	/* 测试EV5 ，检测从器件返回一个应答信号*/
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
	
	/* 设置I2C从器件地址，I2C主设备为写模式*/
	I2C_Send7bitAddress(DS3231_WriteAddress, I2C_DIRECTION_TX);
	
	/* 测试EV6并清除标志位，检测从器件返回一个应答信号 */
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	 
	/* 设置往从器件写数据内部的地址 */
	I2C_SendData(WriteAddr);
	/* 测试EV8并清除标志位 ，检测从器件返回一个应答信号*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	/* 向从器件写一个字节 */
	I2C_SendData(Byte); 
	
	/* 测试EV8并清除标志位，检测从器件返回一个应答信号 */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING)); 
	
	/* 发结束位 */
	I2C_GenerateSTOP(ENABLE);
}


u8 I2C_DS3231_ReadOneByte(u8 ReadAddr)
{
	u8 RxData;
	/* 等待空闲 */
	while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
	
	/* 发起始位 */
	I2C_GenerateSTART(ENABLE);
	
	/* 测试EV5 ，检测从器件返回一个应答信号*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	/* 设置I2C从器件地址，I2C主设备为写模式*/ 
	I2C_Send7bitAddress(DS3231_WriteAddress, I2C_DIRECTION_TX);
	
	/* 测试EV6并清除标志位 */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
        
	I2C_Cmd(ENABLE); 

	I2C_SendData(ReadAddr);

	/* 测试EV8并清除标志位 */
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	/* 发起始位 */ 
	I2C_GenerateSTART(ENABLE);
	
	/* 测试EV5 ，检测从器件返回一个应答信号*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	/* 设置I2C从器件地址，I2C主设备为读模式*/
	I2C_Send7bitAddress(DS3231_WriteAddress, I2C_DIRECTION_RX);
	
	/* 测试EV6 ，检测从器件返回一个应答信号*/
	while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	/* 测试EV7，检测从器件返回一个应答信号 */
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
	RxData=I2C_ReceiveData();
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	
	/* 发结束位*/
	I2C_GenerateSTOP(ENABLE);
        return RxData;
}


unsigned char BCD2HEX(unsigned char val)    //BCD转换为Byte
{
     unsigned char temp;
     temp=val&0x0f;
     val>>=4;
     val&=0x0f;
     val*=10;
     temp+=val;
   
     return temp;
}

unsigned char HEX2BCD(unsigned char val)    //B码转换为BCD码
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
     I2C_DS3231_WriteOneByte(DS3231_YEAR,temp);   //修改年
   
     temp=HEX2BCD(mon);
     I2C_DS3231_WriteOneByte(DS3231_MONTH,temp);  //修改月
   
     temp=HEX2BCD(da);
     I2C_DS3231_WriteOneByte(DS3231_DAY,temp);    //修改日
   
     temp=HEX2BCD(hou);
     I2C_DS3231_WriteOneByte(DS3231_HOUR,temp);   //修改时
   
     temp=HEX2BCD(min);
     I2C_DS3231_WriteOneByte(DS3231_MINUTE,temp); //修改分
   
     temp=HEX2BCD(sec);
     I2C_DS3231_WriteOneByte(DS3231_SECOND,temp); //修改秒
     
     I2C_DS3231_WriteOneByte(DS3231_WEEK,week);//修改星期
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
  	/* 与按键相连的引脚设置为输入模式 */
    GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_IN_PU_IT);
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_FALL_ONLY);
}


void get_dat(unsigned char* Dtemp)
{
  unsigned char Ytemp1,Mtemp1,Dtemp1,Htemp1,Mitemp1,Stemp1;//,Ttemp1,Ttemp3;

	 Ytemp1=I2C_DS3231_ReadOneByte(DS3231_YEAR);        //年
    Dtemp[0]=BCD2HEX(Ytemp1);
   
     Mtemp1=I2C_DS3231_ReadOneByte(DS3231_MONTH);       //月
   Dtemp[1]=BCD2HEX(Mtemp1);
   
     Dtemp1=I2C_DS3231_ReadOneByte(DS3231_DAY);         //日
    Dtemp[2]=BCD2HEX(Dtemp1);

    Htemp1=I2C_DS3231_ReadOneByte(DS3231_HOUR);    //时 24小时制
    Htemp1&=0x3f;                  
    Dtemp[3]=BCD2HEX(Htemp1);
   
     Mitemp1=I2C_DS3231_ReadOneByte(DS3231_MINUTE);  //分
    Dtemp[4]=BCD2HEX(Mitemp1);
   
     Stemp1=I2C_DS3231_ReadOneByte(DS3231_SECOND);  //秒
    Dtemp[5]=BCD2HEX(Stemp1);
   Dtemp[6]=I2C_DS3231_ReadOneByte(DS3231_TEMPERATUREH);    //温度 高字节
   Dtemp[7]=I2C_DS3231_ReadOneByte(DS3231_TEMPERATUREL);    //温度 低字节
    Dtemp[8]=I2C_DS3231_ReadOneByte(DS3231_WEEK);    //星期
}

