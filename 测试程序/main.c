//****************************************************************//
//	   DHT21ʹ�÷��� 
//��Ƭ�� AT89S52 �� STC89C52RC 
//����   ���ڷ�����ʪ������ ���� 11.0592M ������ 9600 
//Ӳ��   P2.0��ΪͨѶ������DHT22,DHT22�ĵ�Դ�͵����ӵ�Ƭ���ĵ�Դ�͵أ���Ƭ�����ڼ�MAX232���ӵ��� 
//****************************************************************//

#include <reg51.h>
#include <intrins.h> 
#include<stdio.h>
//
typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable 	  �޷���8λ���ͱ���  */
typedef signed   char  S8;       /* defined for signed 8-bits integer variable		  �з���8λ���ͱ���  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable 	  �޷���16λ���ͱ��� */
typedef signed   int   S16;      /* defined for signed 16-bits integer variable 	  �з���16λ���ͱ��� */
typedef unsigned long  U32;      /* defined for unsigned 32-bits integer variable 	  �޷���32λ���ͱ��� */
typedef signed   long  S32;      /* defined for signed 32-bits integer variable 	  �з���32λ���ͱ��� */
typedef float          F32;      /* single precision floating point variable (32bits) �����ȸ�������32λ���ȣ� */
typedef double         F64;      /* double precision floating point variable (64bits) ˫���ȸ�������64λ���ȣ� */
//
#define uchar unsigned char
#define uint unsigned int
#define   Data_0_time    4

//----------------------------------------------//
//----------------IO�ڶ�����--------------------//
//----------------------------------------------//
sbit  P2_0  = P2^0 ;
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;


U8 DisplayData[8];
U8 temp1[8];
U8 code smgduan[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
//----------------------------------------------//
//----------------������--------------------//
//----------------------------------------------//
U8  U8FLAG,k;
U8  U8count,U8temp;
U8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
U8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
U8  U8comdata;
U8  outdata[5];  //���巢�͵��ֽ���	   
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
		while((!P2_0)&&U8FLAG++);  //�ȴ��ߵ�ƽ
		Delay_10us();
	    Delay_10us();				
	    Delay_10us();
		U8temp=0;
		if(P2_0)U8temp=1;  //��ȡ�ߵ�ƽ
		U8FLAG=2;
		while((P2_0)&&U8FLAG++);  //�ȴ��͵�ƽ		  
		if(U8FLAG==1)break;  //��ʱ������forѭ����U8FLAG��������´�0��ʼ�ӣ�
		//�ж�����λ��0����1	 
		   
		// ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1 	 
		U8comdata<<=1;
		U8comdata|=U8temp;        //0
	}
}

//--------------------------------
//-----ʪ�ȶ�ȡ�ӳ��� ------------
//--------------------------------
//----���±�����Ϊȫ�ֱ���--------
//----�¶ȸ�8λ== U8T_data_H------
//----�¶ȵ�8λ== U8T_data_L------
//----ʪ�ȸ�8λ== U8RH_data_H-----
//----ʪ�ȵ�8λ== U8RH_data_L-----
//----У�� 8λ == U8checkdata-----
//----��������ӳ�������----------
//---- Delay();, Delay_10us();,COM(); 
//--------------------------------
void RH(void)
{
  //��������18ms 
   P2_0=0;
   Delay(180);
   P2_0=1;
 //������������������ ������ʱ20us
   Delay_10us();
   Delay_10us();
   Delay_10us();
   Delay_10us();	    
 //������Ϊ���� �жϴӻ���Ӧ�ź� 
   P2_0=1;
 //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
   if(!P2_0)		 //T !	  
   {
   U8FLAG=2;
 //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
   while((!P2_0)&&U8FLAG++);
   U8FLAG=2;
 //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
   while((P2_0)&&U8FLAG++);
 //���ݽ���״̬		 
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
 //����У�� 
 
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
* �� �� ��         : datapros()
* ��������		   : ��ʪ�ȶ�ȡ����ת������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void datapros() 	 
{
	hum=U8RH_data_H*256+U8RH_data_L;//����ʪ��ֵ
	temp=U8T_data_H*256+U8T_data_L;//�¶�		�¶�Ϊ��δ����
	//�¶�
	DisplayData[0]=smgduan[temp%10];
	temp1[0]= temp%10;
	DisplayData[1]=smgduan[temp/10%10]|0x80;   //С���㣬��0x80
	temp1[1]= temp/10%10;
	DisplayData[2]=smgduan[temp/100%10];
	temp1[2]= temp/100%10;
	if(U8T_data_H>127)//���¶����λΪ1����ʾ��ֵ
	{
		DisplayData[3]=0x40;//��ʾ����
		temp1[3]=0;
	}
	else
	{
		DisplayData[3]=smgduan[(temp/1000)%10];
		temp1[3]= (temp/1000)%10;
	}
	//ʪ��
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
* ������         :DigDisplay()
* ��������		 :�������ʾ����
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void DigDisplay()
{
	U8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//��ʾ��0λ
			case(1):
				LSA=1;LSB=0;LSC=0; break;//��ʾ��1λ
			case(2):
				LSA=0;LSB=1;LSC=0; break;//��ʾ��2λ
			case(3):
				LSA=1;LSB=1;LSC=0; break;//��ʾ��3λ
			case(4):
				LSA=0;LSB=0;LSC=1; break;//��ʾ��4λ
			case(5):
				LSA=1;LSB=0;LSC=1; break;//��ʾ��5λ
			case(6):
				LSA=0;LSB=1;LSC=1; break;//��ʾ��6λ
			case(7):
				LSA=1;LSB=1;LSC=1; break;//��ʾ��7λ	
		}
		P0=DisplayData[i];//��������
		Delay(10); //���һ��ʱ��ɨ��	
		P0=0x00;//����
	}		
}
/*******************************************************************************
* ������         :UsartInit()
* ��������		   :���ô���
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void UsartInit()
{
	SCON=0X50;			//����Ϊ������ʽ1
	TMOD=0X20;			//���ü�����������ʽ2
	PCON=0X80;			//�����ʼӱ�
	TH1=0XFA;				//��������ʼֵ���ã�ע�Ⲩ������4800��
	TL1=0XFA;
	ES=1;						//�򿪽����ж�
	EA=1;						//�����ж�
	TR1=1;					//�򿪼�����
}
/*******************************************************************************
* �� �� ��         : txd_data()
* ��������		   : ������ʪ�ȶ�ȡ����ת������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/	

/*************************�������ݵĺ���********************************/
void txd_data(char send_data)    
{
 SBUF = send_data; //����Ҫ���͵����ݷ��뷢�ͻ�����
 while(!TI);           //�ȴ���������
 TI = 0;
}
//----------------------------------------------
//main()��������:  AT89C51  11.0592MHz 	���ڷ� 
//����ʪ������,������ 9600 
//----------------------------------------------
 
void main()
{	
	char i;
	UsartInit();   // initialization
	Delay(1000);         //��ʱ100US��12M����)
	while(1)
	{  
	   //������ʪ�ȶ�ȡ�ӳ���
	   RH();
	   datapros(); //���ݴ���
	   DigDisplay();//�������ʾ����  
	   //��ȡģ���������ڲ���С�� 2S 
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
 y = SBUF;                       //��ȡ���յ���Ϣ��Ȼ���ж��Ƿ�Ϊ�����¶�����
 if(y == '0')             //���Ϊ��������ͽ���ǰ���ݻ��������ڵ����ݷ��͸���λ��
 {
         for(i = 0;i <= 3;i++)
         {
                 txd_data(temp1[i] + 48);
         }
 }
 RI = 0;           
}
 */