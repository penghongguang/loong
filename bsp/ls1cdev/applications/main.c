#include "headfile.h"

#define led 66 
#define red_out 62//LCD
#define motor  63 
#define purple_light 64  //
#define beep 65//11
#define key 85 //
#define key_off 86

#define on gpio_level_high
#define off gpio_level_low
#define proper_PPB 140
#define proper_PPB_norm 20  

rt_timer_t timer_min30;  
rt_uint16_t min30_cnt = 0;  
rt_uint16_t start_cnt = 0;
rt_uint8_t man_come_flag = 0;
rt_uint8_t motor_on_flag = 0;
rt_uint8_t main_control_flag = 0;
rt_uint8_t stop_flag = 0;
rt_uint8_t over_flag = 0;

void min30_control();
void min30_control_timeout();  //定时中断处理函数
void sensor_init();
void red_out_control();
void tem_humidity_print();
void purple_light_control();

int main(int argc, char** argv)
{   	
	static char command_last[5]="0000";
	char i;
	static char cnt=0;
	sensor_init();
	
	while(1)
	{
		if(!gpio_get(key)) {main_control_flag = 1;stop_flag=0;}
		if(!gpio_get(key)) {main_control_flag = 1;stop_flag=0;}
		gpio_set(motor,gpio_level_low);
		if(stop_flag)
		{
			LCD_CLS();
			LCD_showstr(45,3,"stopping!!!");
		}
		/***控制***/
		if((command_get[0]=='o' && command_get[1]=='n') || !gpio_get(key)) 
		{
			main_control_flag = 1;
			stop_flag=0;
			for(i=0;i<4;i++) {command_last[i] = command_get[i];command_get[i] = '\0';}
		}
		if(command_get[0]=='s' && command_get[1]=='t'&&command_last[0]=='o'&&command_last[1]=='n') 
		{
			stop_flag = 1;
			for(i=0;i<4;i++) {command_last[i] = command_get[i];command_get[i] = '\0';}
		}
		if((command_get[0]=='o' && command_get[1]=='f' && command_get[2]=='f') || !gpio_get(key_off))
		{
			main_control_flag = 0;
			LCD_CLS();
			min30_cnt = 1;
			gpio_set(motor,gpio_level_low);
			LCD_showstr(0,3,"interrupt disinfect");
			for(i=0;i<4;i++) {command_last[i] = command_get[i];command_get[i] = '\0';}  //判断后需清除，否则重复置1，无法30s关闭，一句bug30min
		}
		/***控制***/
		purple_light_control();
		//rt_kprintf("%d\n",min30_cnt);
		if(min30_cnt) 
		{
			LCD_CLS();
			/***温湿度打印***/
			tem_humidity_print();
			/***温湿度打印***/
			/***臭氧浓度打印***/
			LCD_showstr(0,4,"O3_concentration:");
			LCD_showint16(0,5,O3_concentration);
			/***臭氧浓度打印***/
			LCD_showstr(10,6,"disinfect over!!!");
		}
		if(O3_concentration<proper_PPB_norm+3 && over_flag)
		{
			gpio_set(motor,gpio_level_high);
			LCD_CLS();
			LCD_showstr(0,3,"waiting for next use!");
			min30_cnt = 0;  //30s计时，30*60 = 1800 
			man_come_flag = 0;
			motor_on_flag = 0;
			over_flag = 0;
			main_control_flag = 0;
		}
		
		if(main_control_flag)
		{
			red_out_control(); 
			if(O3_concentration>proper_PPB) //臭氧判断
			{
				main_control_flag = 0;
				LCD_CLS();
				min30_cnt = 1;
				over_flag = 1;
				gpio_set(motor,gpio_level_low);
			}
			
			if(!man_come_flag)
			{
				/***温湿度打印***/
				tem_humidity_print();
				/***温湿度打印***/
				/***臭氧浓度打印***/
				LCD_showstr(0,4,"O3_concentration:");
				LCD_showint16(0,5,O3_concentration);
				/***臭氧浓度打印***/
			}
		}
		// 测试IO口
		// cnt = ~cnt;
		// gpio_set(led,cnt);
		// gpio_set(red_out,cnt);
		// gpio_set(motor,cnt);
		// gpio_set(purple_light,cnt);
		// gpio_set(beep,cnt);
		rt_thread_delay(1000);
	}
    
	return 0;
}

void min30_control_timeout()  //定时中断处理函数
{
	if(min30_cnt) min30_cnt++;
	start_cnt++;
}

void min30_control()
{
	/* 创建定时器1 */
	timer_min30 = rt_timer_create("timer_min30",  /* 定时器名字是 timer_test */
						min30_control_timeout, /* 超时时回调的处理函数 */
						RT_NULL,  /* 超时函数的入口参数 */
						1000,       /* 定时长度，以OS Tick为单位，即100个OS Tick */
						RT_TIMER_FLAG_PERIODIC); /* 周期性定时器 */
	/* 启动定时器 */
	if (timer_min30 != RT_NULL) rt_timer_start(timer_min30);	
}

void red_out_control()
{
	rt_uint8_t dat;
	dat = gpio_get(red_out);
	
	if(dat) man_come_flag = 1;
	else man_come_flag = 0;
	
	if(man_come_flag)
	{
		gpio_set(led,gpio_level_low);
		gpio_set(beep, gpio_level_high);
		LCD_CLS();
		LCD_showstr(30,4,"Man Come!!!");
	}
	else
	{
		gpio_set(led,gpio_level_high);
		gpio_set(beep, gpio_level_low);
	}
	
	if(man_come_flag) stop_flag = 1;
	else stop_flag = 0;
}

void purple_light_control()
{
	if(main_control_flag)
	{
		if(stop_flag)
			gpio_set(purple_light,gpio_level_low);  //off	
		else
		{gpio_set(purple_light,gpio_level_high);}  //on
	}
	else 
		gpio_set(purple_light,gpio_level_low);  //off
}

void sensor_init()
{
	pin_set_purpose(led, PIN_PURPOSE_GPIO);
	pin_set_purpose(purple_light, PIN_PURPOSE_GPIO);
	pin_set_purpose(red_out, PIN_PURPOSE_GPIO);
	pin_set_purpose(motor, PIN_PURPOSE_GPIO);
	pin_set_purpose(beep, PIN_PURPOSE_GPIO);

	gpio_init(led, gpio_mode_output); 
	gpio_init(purple_light, gpio_mode_output); 
	gpio_init(red_out, gpio_mode_output); 
	gpio_init(motor, gpio_mode_output); 
	gpio_init(beep, gpio_mode_output);
	gpio_init(key, gpio_mode_input);
	gpio_init(key_off, gpio_mode_input);
	
	//gpio_set(key, gpio_level_low);
	gpio_set(beep, gpio_level_low);
	gpio_set(led, gpio_level_high);       // 指示灯默认熄灭
	gpio_set(motor, gpio_level_high);       // 电机初始不转
	gpio_set(red_out, gpio_level_low);       // 红外初始为低
	gpio_set(purple_light, gpio_level_low);       // 紫外初始为低  //
	
	//O3_get();  //初始化串口3，中断获取臭氧传感器数据
	phone_command_get();
	//tem_humidity_get();
	LCD_Init();
	LCD_showstr(25,3,"welcome to use!");
	LCD_showstr(30,4,"on/off/stop");
	min30_control();
}

void tem_humidity_print()
{
	int cnt=0;
	rt_uint8_t hum[6] = "0000";
	rt_uint8_t temp[6] = "0000";
	rt_uint8_t i;
		
	for(i=0;i<3;i++) hum[i] = temp_hum[i];
	hum[3] = '.';hum[4] = temp_hum[3];
	
	for(i=0;i<3;i++) temp[i] = temp_hum[i+4];
	temp[3] = '.';temp[4] = temp_hum[7];
	
	temp[5] = '\0';
	hum[5] = '\0';
	
	LCD_CLS();
	LCD_showstr(0,0,"temperature:");
	LCD_showstr(0,1,temp);
	LCD_showstr(0,2,"humidity:");
	LCD_showstr(0,3,hum);
}