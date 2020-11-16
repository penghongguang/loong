
#include <headfile.h>

rt_uint8_t command_get[5] = "0000";

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
static rt_device_t serial;

/* 数据到达回调函数*/
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
	int i;
	
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

	if(uart_rx_buffer[0]=='o'&&uart_rx_buffer[1]=='n') 
	{
		command_get[0] = 'o';
		command_get[1] = 'n';
		command_get[2] = '\0';
	}
	if(uart_rx_buffer[0]=='o'&&uart_rx_buffer[1]=='f') 
	{
		command_get[0] = 'o';
		command_get[1] = 'f';
		command_get[2] = 'f';
		command_get[3] = '\0';
	}
	if(uart_rx_buffer[0]=='s'&&uart_rx_buffer[1]=='t') 
	{
		command_get[0] = 's';
		command_get[1] = 't';
		command_get[2] = 'o';
		command_get[3] = 'p';
		command_get[4] = '\0';
	}
	rt_kprintf("sys is %c\n",uart_rx_buffer[0]);
	
    /* 发送消息到消息队列中*/
    //rt_mq_send(&rx_mq, &msg, sizeof(struct rx_msg));
    return RT_EOK;
}

int phone_command_get()
{
    rt_err_t ret = RT_EOK;
	char uart_name[]="uart2";
    serial = rt_device_find(uart_name);
	//改波特率， serial.h里宏定义一个配置变量，drv_uart.c里使用该变量进行初始化	
    if (serial)
    {
        /* 设置回调函数及打开设备*/
        rt_device_set_rx_indicate(serial, uart_input);
        rt_device_open(serial, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
    }
     else
     {
         rt_kprintf(uart_name);
         return RT_ERROR;
     }

    return ret;
}       

