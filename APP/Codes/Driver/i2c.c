#include "i2c.h"
#include "delay.h"

/*-----------------------------------------------------------------------------------------------------------------------
													I2C 协议
-------------------------------------------------------------------------------------------------------------------------
　i2c简易时序图(半双工)
　　启动信号：

　　SCL为高电平的时候，SDA由高电平向低电平跳变。结束信号：SCL为高电平的时候，SDA由低电平向高电平跳变。

　　应答信号：

　　I2C总线上的所有数据都是以8位字节传送的，发送器每发送一个字节，就在时钟脉冲9期间释放数据线，由接收器反馈一个应答信号。
	应答信号为低电平时，规定为有效应答位（ACK简称应答位），表示接收器已经成功地接收了该字节；
	应答信号为高电平时，规定为非应答位（NACK），一般表示接收器接收该字节没有成功，对于反馈有效应答位ACK的要求是，接收器在第9个时钟脉冲之前的低电平期间将SDA线拉低，并且确保在该时钟的高电平期间为稳定的低电平。如果接收器是主控器，则在它收到最后一个字节后，发送一个NACK信号，以通知被控发送器结束数据发送，并释放SDA线，以便主控接收器发送一个停止信号P。

　　写时序：

　　开始信号：主机+从设备地址+写命令，从机应答，应答成功，表示有这个设备，然后主机+设备内部寄存器地址，此时不用再加写命令控制字，
	从机应答，应答成功，表示设备内有这个地址，主机写入数据，从机应答，是否继续发送，不发送的话，发送停止信号P。

　　读时序：

　　要想读设备，首先要知道将要所读取设备的地址告诉从设备，从设备才能将数据放到（发送）SDA上使主设备读取，
	从设备将数据放入SDA上的过程，由硬件主动完成，不用人为的写入。所以首先先写入从机地址，然后+写控制命令，从机应答，应答成功，
	表示有这个设备，然后写入内部寄存器地址，此时不用再加写命令控制字，从机应答，应答成功，表示设备内有这个地址。然后主机继续发出：
	写入从机地址，然后+读命令，从机应答，应答成功，此时便可以读取数据了，从设备已经将数据放入到SDA上了。地址跟设备已经验证了，不用再进行验证。


i2c 基本用法

i2c 总线是一种简单、双向二线制同步串行总线。所有主机在 SCL 线上产生它们自己的时钟来传输总线上的报文，
	SDA 线传输每个字节必须为 8 位，每次传输可以发送的字节数量不受限制，每个字节后必须跟一个响应位。
	在空闲状态时，SCL 与 SDA 均为高电平。
	通常一些低功耗 i2c 设备，芯片引脚使用上拉输出即可满足与其正常数据交互，还有一些 i2c 设备，则需要在总线上外加一个上拉电阻，
	此时相应的 I/O 配置成开漏输出，其他的按照芯片手册进行标准配置。

2.	如果正常拉高、拉低的情况下，依然无法正常读取数据。通常建议，根据负载电流更换小阻值的电阻。 --电气特性无法满足

3. SDA 死锁
	i2c 设备进行读写操作的过程中，在从机钳住总线的期间，MCU 异常复位，会导致 SDA 死锁，异常产生出现在俩个阶段：从机响应阶段、从机发送数据阶段。
	下面将针对这两种异常，对时钟信号进行解释，并且总结其他原因，得出结论。
	（a） 从机响应阶段
	MCU 在开始信号后发送地址，得到从机设备响应，准备开始返回数据，在这个时候，从机将 SDA 信号拉为低电平(低电平为有效ACK)，
	如果 MCU 异常复位，会导致总线上 SCL 停止发送时钟信号，从机等待 MCU 的时钟信号，产生钳住并且拉低	SDA 的现象。
	如果想要解锁 SDA，从机需要 9 个时钟信号，使得从机完成响应，释放 SDA 。
	
	（b） 从机发送数据阶段
	如果从机响应完成了，开始给 MCU 返回数据。这个数据有八位，每一位都有可能为低，如果在数据低位，MCU 异常复位，停止发送时钟信号，
	从机就会等待 MCU 的时钟信号，产生钳住并且拉低  SDA 的现象。如果想要解锁 SDA，从机需要 １－８ 个时钟信号，使得从机完成数据响应，释放 SDA 。
	（c）其他情况
	
	在从机一个 8 位数据发送完成后，等待 MCU 响应, 即使属于 MCU 的，从机不再钳住 SDA，没有时钟，数据交互停止。
	在主机发送数据阶段，总线所有权在主机，主机异常，数据交互停止，总线释放。所以，这些情况下，不存在 SDA 死锁的情况。

	





-------------------------------------------------------------------------------------------------------------------------
													
-----------------------------------------------------------------------------------------------------------------------*/



//IIC初始化
void IIC_Init(void)
{
    
    

    
    IIC_SDA=1;
    IIC_SCL=1;  
}

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	delay_us(4);			
	IIC_SDA=1;//发送I2C总线结束信号				   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

