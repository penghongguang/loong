
#include <headfile.h>

rt_uint8_t temp_hum[9] = "00000000";
/* UART接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};

/* 用于接收消息的消息队列*/
static struct rt_messagequeue rx_mq;
/* 接收线程的接收缓冲区*/
static unsigned char uart_rx_buffer[64];

/* 串口设备*/
static rt_device_t serial1;

/* 数据到达回调函数*/
static rt_err_t uart1_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
	int i;
	unsigned char dat[8];
	
	rt_uint32_t rx_length;
	rt_err_t result = RT_EOK;
    msg.dev = dev;
    msg.size = size;
	
	rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ?
			msg.size : sizeof(uart_rx_buffer) - 1;

	/* 读取消息*/
	rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0],
			rx_length);
	uart_rx_buffer[rx_length] = '\0';

	for(i=0;i<8;i++) 
	{ 
		temp_hum[i] = uart_rx_buffer[i];
	}
	temp_hum[8] = '\0';
	rt_kprintf("%s",&temp_hum[0]);
	// temperature = (int)((dat[0]-'0')*1000 + (dat[1]-'0')*100 + (dat[2]-'0')*10 + (dat[3]-'0'));
	// humidity = (int)((dat[4]-'0')*1000 + (dat[5]-'0')*100 + (dat[6]-'0')*10 + (dat[7]-'0'));
	 
    /* 发送消息到消息队列中*/
    //rt_mq_send(&rx_mq, &msg, sizeof(struct rx_msg));
    return RT_EOK;
}

int tem_humidity_get()
{
    rt_err_t ret = RT_EOK;
	char uart_name[]="uart3";
    serial1 = rt_device_find(uart_name);
	//改波特率， serial.h里宏定义一个配置变量，drv_uart.c里使用该变量进行初始化	
    if (serial1)
    {
        /* 设置回调函数及打开设备*/
        rt_device_set_rx_indicate(serial1, uart1_input);
        rt_device_open(serial1, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
    }
     else
     {
         rt_kprintf(uart_name);
         return RT_ERROR;
     }
	 
    return ret;
}       

