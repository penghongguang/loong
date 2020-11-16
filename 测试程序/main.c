//****************************************************************//
//	   DHT21使用范例 
//单片机 AT89S52 或 STC89C52RC 
//功能   串口发送温湿度数据 晶振 11.0592M 波特率 9600 
//硬件   P2.0口为通讯口连接DHT22,DHT22的电源和地连接单片机的电源和地，单片机串口加MAX232连接电脑 
//****************************************************************//

#include <reg51.h>
#include <intrins.h> 
#include<stdio.h>
//
typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable 	  无符号8位整型变量  */
typedef signed   char  S8;       /* defined for signed 8-bits integer variable		  有符号8位整型变量  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable 	  无符号16位整型变量 */
typedef signed   int   S16;      /* defined for signed 16-bits integer variable 	  有符号16位整型变量 */
typedef unsigned long  U32;      /* defined for unsigned 32-bits integer variable 	  无符号32位整型变量 */
typedef signed   long  S32;      /* defined for signed 32-bits integer variable 	  有符号32位整型变量 */
typedef float          F32;      /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         F64;      /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */
//
#define uchar unsigned char
#define uint unsigned int
#define   Data_0_time    4

//----------------------------------------------//
//----------------IO口定义区--------------------//
//----------------------------------------------//
sbit  P2_0  = P2^0 ;
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;


U8 DisplayData[8];
U8 temp1[8];
U8 code smgduan[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
//----------------------------------------------//
//----------------定义区--------------------//
//----------------------------------------------//
U8  U8FLAG,k;
U8  U8count,U8temp;
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
U8  U8comdata;
U8  outdata[5];  //定义发送的字节数	   
U8  indata[5];
U8  count, count_r=0;
U8 str[5]={"RS232"};
U16 U16temp1,U16temp2;
U16 temp,hum;
SendData(U8 *a)
{
	outdata[0] = a[0]; 
	outdata[1] = a[1];
	outdata[2] = a[2];
	outdata[3] = a[3];
	outdata[4] = a[4];
	count = 1;
	SBUF=outdata[0];
}

void Delay(U16 j)
{  
	U8 i;
   	for(;j>0;j--)
  	{ 	
		for(i=0;i<27;i++);
  	}
}
void  Delay_10us(void)
{
	U8 i;
	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
}
	
void  COM(void)
{
    U8 i;
	for(i=0;i<8;i++)	   
	{	
	    U8FLAG=2;	
		while((!P2_0)&&U8FLAG++);  //等待高电平
		Delay_10us();
	    Delay_10us();				
	    Delay_10us();
		U8temp=0;
		if(P2_0)U8temp=1;  //读取高电平
		U8FLAG=2;
		while((P2_0)&&U8FLAG++);  //等待低电平		  
		if(U8FLAG==1)break;  //超时则跳出for循环（U8FLAG溢出，重新从0开始加）
		//判断数据位是0还是1	 
		   
		// 如果高电平高过预定0高电平值则数据位为 1 	 
		U8comdata<<=1;
		U8comdata|=U8temp;        //0
	}
}

//--------------------------------
//-----湿度读取子程序 ------------
//--------------------------------
//----以下变量均为全局变量--------
//----温度高8位== U8T_data_H------
//----温度低8位== U8T_data_L------
//----湿度高8位== U8RH_data_H-----
//----湿度低8位== U8RH_data_L-----
//----校验 8位 == U8checkdata-----
//----调用相关子程序如下----------
//---- Delay();, Delay_10us();,COM(); 
//--------------------------------
void RH(void)
{
  //主机拉低18ms 
   P2_0=0;
   Delay(180);
   P2_0=1;
 //总线由上拉电阻拉高 主机延时20us
   Delay_10us();
   Delay_10us();
   Delay_10us();
   Delay_10us();	    
 //主机设为输入 判断从机响应信号 
   P2_0=1;
 //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
   if(!P2_0)		 //T !	  
   {
   U8FLAG=2;
 //判断从机是否发出 80us 的低电平响应信号是否结束	 
   while((!P2_0)&&U8FLAG++);
   U8FLAG=2;
 //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
   while((P2_0)&&U8FLAG++);
 //数据接收状态		 
   COM();
   U8RH_data_H_temp=U8comdata;
   COM();
   U8RH_data_L_temp=U8comdata;
   COM();
   U8T_data_H_temp=U8comdata;
   COM();
   U8T_data_L_temp=U8comdata;
   COM();
   U8checkdata_temp=U8comdata;
   P2_0=1;
 //数据校验 
 
   U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
   if(U8temp==U8checkdata_temp)
   {
   	  U8RH_data_H=U8RH_data_H_temp;
   	  U8RH_data_L=U8RH_data_L_temp;
	  U8T_data_H=U8T_data_H_temp;
   	  U8T_data_L=U8T_data_L_temp;
   	  U8checkdata=U8checkdata_temp;
   }//fi
   }//fi
}
/*******************************************************************************
* 函 数 名         : datapros()
* 函数功能		   : 温湿度读取处理转换函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void datapros() 	 
{
	hum=U8RH_data_H*256+U8RH_data_L;//计算湿度值
	temp=U8T_data_H*256+U8T_data_L;//温度		温度为负未考虑
	//温度
	DisplayData[0]=smgduan[temp%10];
	temp1[0]= temp%10;
	DisplayData[1]=smgduan[temp/10%10]|0x80;   //小数点，或0x80
	temp1[1]= temp/10%10;
	DisplayData[2]=smgduan[temp/100%10];
	temp1[2]= temp/100%10;
	if(U8T_data_H>127)//即温度最高位为1，表示负值
	{
		DisplayData[3]=0x40;//显示负号
		temp1[3]=0;
	}
	else
	{
		DisplayData[3]=smgduan[(temp/1000)%10];
		temp1[3]= (temp/1000)%10;
	}
	//湿度
	DisplayData[4]=smgduan[hum%10];
	temp1[4]= hum%10;
	DisplayData[5]=smgduan[hum/10%10]|0x80;
	temp1[5]= hum/10%10;
	DisplayData[6]=smgduan[hum/100%10];
	temp1[6]= hum/100%10;
	DisplayData[7]=smgduan[hum/1000%10];
	temp1[7]= hum/1000%10;
}
/*******************************************************************************
* 函数名         :DigDisplay()
* 函数功能		 :数码管显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void DigDisplay()
{
	U8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//显示第0位
			case(1):
				LSA=1;LSB=0;LSC=0; break;//显示第1位
			case(2):
				LSA=0;LSB=1;LSC=0; break;//显示第2位
			case(3):
				LSA=1;LSB=1;LSC=0; break;//显示第3位
			case(4):
				LSA=0;LSB=0;LSC=1; break;//显示第4位
			case(5):
				LSA=1;LSB=0;LSC=1; break;//显示第5位
			case(6):
				LSA=0;LSB=1;LSC=1; break;//显示第6位
			case(7):
				LSA=1;LSB=1;LSC=1; break;//显示第7位	
		}
		P0=DisplayData[i];//发送数据
		Delay(10); //间隔一段时间扫描	
		P0=0x00;//消隐
	}		
}
/*******************************************************************************
* 函数名         :UsartInit()
* 函数功能		   :设置串口
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void UsartInit()
{
	SCON=0X50;			//设置为工作方式1
	TMOD=0X20;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XFA;				//计数器初始值设置，注意波特率是4800的
	TL1=0XFA;
	ES=1;						//打开接收中断
	EA=1;						//打开总中断
	TR1=1;					//打开计数器
}
/*******************************************************************************
* 函 数 名         : txd_data()
* 函数功能		   : 发送温湿度读取处理转换函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/	

/*************************发送数据的函数********************************/
void txd_data(char send_data)    
{
 SBUF = send_data; //将需要发送的数据放入发送缓冲区
 while(!TI);           //等待发送数据
 TI = 0;
}
//----------------------------------------------
//main()功能描述:  AT89C51  11.0592MHz 	串口发 
//送温湿度数据,波特率 9600 
//----------------------------------------------
 
void main()
{	
	char i;
	UsartInit();   // initialization
	Delay(1000);         //延时100US（12M晶振)
	while(1)
	{  
	   //调用温湿度读取子程序
	   RH();
	   datapros(); //数据处理
	   DigDisplay();//数码管显示函数  
	   //读取模块数据周期不宜小于 2S 
	   //Delay(20000);		
		 for(i = 7;i>= 0;i--)
         {
          txd_data(temp1[i] + 48);
         }
	}	
}
/*
void uart() interrupt 4                      
{
 char y,i;
 while(!RI);
 y = SBUF;                       //读取接收的信息，然后判断是否为发送温度命令
 if(y == '0')             //如果为发送命令，就将当前数据缓冲数组内的数据发送给上位机
 {
         for(i = 0;i <= 3;i++)
         {
                 txd_data(temp1[i] + 48);
         }
 }
 RI = 0;           
}
 */