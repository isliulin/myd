
//#include <reg51.h>
//#include <intrins.h>
//#define  unsigned char unsigned char
//#define  uint unsigned int
/*
sbit	R_S=	P3^5;//指令/数据选择信号
sbit	RES=	P3^4;//复位信号
sbit	W_R= 	P3^1;//写信号
sbit	R_D=	P3^0;//读信号
sbit    C_S=    P3^7;//片选


*/
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "i2c_fram.h"
#include "LCDST7567.h"


/****************Serial*************************/


 

#define RS_L GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define RS_H GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define RES_L GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define RES_H GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define CS_L GPIO_ResetBits(GPIOC, GPIO_Pin_5)
#define CS_H GPIO_SetBits(GPIOC, GPIO_Pin_5)

#define BL_L GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define BL_H GPIO_SetBits(GPIOB, GPIO_Pin_5)


/**********************************************/


unsigned char Page = 0xb0	;
                           			                    


void delayms(uint16_t ms)
{	
	uint16_t j;
	while(ms--)
	  for(j=0;j<=0x1002;j++); //20us=0.02ms//  
}		
	


/************Serial*************/

void WrateC(unsigned char com)

{
    
	    CS_L;
	    RS_L;  
   	/*     SCK=0;
	
	for(i=0;i<8;i++)
	    { 
		 j=com;
		 SCK=0;
		 SDA=com&0x80;
         SCK=1;	     
		 com=j<<1;
		}
		*/
		I2C_SendByte(com);
	    CS_H;
		RS_H;
}


void WrateD(unsigned char dat)
{
    
	    CS_L;
	    RS_H;  
       /* SCK=0;
	for(i=0;i<8;i++)
	    { 
		 j=dat;
		 SCK=0;
		 SDA=dat&0x80;
         SCK=1;	     
		 dat=j<<1;
		}*/
		I2C_SendByte(dat);
	    CS_H;
		RS_L;
}



void ClearDisplay(unsigned char const *P)
{

//   page=0x80;
   unsigned char  i,j;
   for(i=0xb0;i<0xB8;i++)   
    {
         WrateC(i);               // (Page = 0xB0)//设置位置
 	 WrateC(0x00);
	 WrateC(0x10);	  
       for(j=0;j<128;j++) 			//发送数据
          { 
            WrateD(*P++);
          }
          
    }
delayms(1);
}  

void LCDGPIOinit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*--------------------------------------------------------------*/
	/*NRST------------------->B9*/
	/*LCD_CLK---------------->B6*/
	/*LCD_A0----------------->B8*/
	/*LCD_CS----------------->C5*/
	/*LCD_IO----------------->B7*/
	/*LCD_LED+--------------->B5*/
	/*--------------------------------------------------------------*/

	/*Enable or disable APB2 peripheral clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	
	/*Configure GPIO pin : PB */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PB */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

		/*Enable or disable APB2 peripheral clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	
	/*Configure GPIO pin : PB */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

}




void LcdInit(void)
{		

	LCDGPIOinit();
	BL_L;
	RES_L;
	RS_L;
	delayms(50);//�090钌�2um
	RES_H;
	RS_H;
	BL_H;
	delayms(20);
	RES_L;
	RS_L;
	BL_L;
	delayms(50);
	RES_H;
	RS_H;
	BL_H;
	WrateC(0xe2);//软件复位		
	WrateC(0xA3);//Bais set		显示偏压
	WrateC(0xA0);//ADC seg镜像选择 0xa0正常，0xA1左右镜像
	WrateC(0xc8);//com output scan direction，com镜像选择 0xc0正常，0xC8上下镜像
/****Select internal resistor ratio****/
	WrateC(0x2c);//内部电源管理，
	WrateC(0x2e);//三条指令间隔时间2um
	WrateC(0x2F);//
/**************************************/

	WrateC(0x81);//电压模式选择
	WrateC(0x1d);//电压调整寄存器低位 范围：0x00-0x3f
	WrateC(0x24);//电压调整寄存器高位 范围：0x21-0x27 ，值越大，显示效果越浓（底影越浓）

	WrateC(0xAF);//显示开         
	WrateC(0x40);//从首行开始显示
	BL_H;
	
}
	







void Setadd(unsigned char xl,unsigned char yl)
{    unsigned char i;
     i=xl&0x0f;
     xl=(xl>>4)+0x10;

     WrateC(0xb0+yl); 	  
	 WrateC(xl);
 	 WrateC(i);	
}

/*

void ShowOneDig(unsigned char col,unsigned char pag,unsigned char d)
{
	
{
	unsigned char i = 0 ;
	unsigned char j;
	if(col==0x80)        
	{ 
	   col=0;
	   pag+=1;
	 }
	Setadd(col,pag);   	   	  
	for(j=0;j<8;j++)
	{ 
	 
		WrateD(Number[d][i++]); 
	}          
	pag++;  
	
	Setadd(col,pag);   	   	  
	for(j=0;j<8;j++)
	{ 
		WrateD(Number[d][i++]); 
	}          
	pag++;   
   
}

delayms(1);
}


*/

//--------------------------------------------
/*
void main()
{	
  LcdInit();
      while(1)
	{
	//unsigned char a = 0 ;
	
	
	ClearDisplay(pJH);
	
	ClearDisplay(pJH1);
	
	
	
	ShowOneDig(3,0,0);    //开始数字是SEG起始线,中间数字表示page第*页，后面数字表示"查表"第*行
	LcdInit();
	ShowOneDig(10,0,1);
	LcdInit();
	ShowOneDig(17,0,2);
	LcdInit();
	ShowOneDig(24,0,3);
	LcdInit();
	ShowOneDig(31,0,4);
	LcdInit();
	ShowOneDig(38,0,5);
	LcdInit();
	ShowOneDig(45,0,6);
	LcdInit();
	ShowOneDig(52,0,7);
	ShowOneDig(59,0,0);    //开始数字是SEG起始线,中间数字表示page第*页，后面数字表示"查表"第*行
	ShowOneDig(63,0,1);
	ShowOneDig(70,0,2);
	ShowOneDig(77,0,3);
	ShowOneDig(84,0,4);
	ShowOneDig(91,0,5);
	ShowOneDig(98,0,6);
	ShowOneDig(105,0,7);
	ShowOneDig(112,0,2);
	ShowOneDig(119,0,3);
	ShowOneDig(127,0,4);
	
	//ShowOneDig(88,3,9);
	
	
	
	}
}
*/




