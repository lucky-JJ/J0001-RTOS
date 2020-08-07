#include "spi.h"

/*-----------------------------------------------------------------------------------------------------------------------
													SPI 协议
-------------------------------------------------------------------------------------------------------------------------

	SPI 是英语 Serial Peripheral interface 的缩写，顾名思义就是串行外围设备接口。
	SPI 接口主要应用在 EEPROM， FLASH，实时时钟， AD 转换器，还有数字信号处理器和数字信号解码器之间。 
	SPI，是一种高速的，全双工，同步的通信总线

	注:如果应用中必须使用高速数据传输，那么SPI是必然的选择。因为SPI是全双工，IIC的不是。
	
1. 采用主-从模式(Master-Slave) 的控制方式
	SPI 规定了两个 SPI 设备之间通信必须由主设备 (Master) 来控制次设备 (Slave). 
    一个 Master 设备可以通过提供 Clock 以及对 Slave 设备进行片选 (Slave Select) 来控制多个 Slave 设备, 
    SPI 协议还规定 Slave 设备的 Clock 由 Master 设备通过 SCK 管脚提供给 Slave 设备, 
    Slave 设备本身不能产生或控制 Clock, 没有 Clock 则 Slave 设备不能正常工作.
	
2. 采用同步方式(Synchronous)传输数据
	Master 设备会根据将要交换的数据来产生相应的时钟脉冲(Clock Pulse), 时钟脉冲组成了时钟信号(Clock Signal) , 
	时钟信号通过时钟极性 (CPOL) 和 时钟相位 (CPHA) 控制着两个 SPI 设备间何时数据交换以及何时对接收到的数据进行采样, 
	来保证数据在两个设备之间是同步传输的.
	
3. 数据交换(Data Exchanges)
	SPI 设备间的数据传输之所以又被称为数据交换, 是因为 SPI 协议规定一个 SPI 设备不能在数据通信过程中仅仅只充当一个 
	"发送者(Transmitter)" 或者 "接收者(Receiver)". 在每个 Clock 周期内, SPI 设备都会发送并接收一个 bit 大小的数据, 
	相当于该设备有一个 bit 大小的数据被交换了.
	一个 Slave 设备要想能够接收到 Master 发过来的控制信号, 必须在此之前能够被 Master 设备进行访问 (Access). 
	所以, Master 设备必须首先通过 SS/CS pin 对 Slave 设备进行片选, 把想要访问的 Slave 设备选上.
	在数据传输的过程中,  每次接收到的数据必须在下一次数据传输之前被采样. 如果之前接收到的数据没有被读取, 
	那么这些已经接收完成的数据将有可能会被丢弃,  导致 SPI 物理模块最终失效. 因此, 在程序中一般都会在 SPI 传输完数据后, 
	去读取 SPI 设备里的数据, 即使这些数据(Dummy Data)在我们的程序里是无用的.

　　SPI优点：支持全双工通信、通信简单、数据传输速率块。
	   缺点：没有指定的流控制，没有应答机制确认是否接收到数据，所以跟IIC总线协议比较在数据 ，可靠性上有一定的缺陷。

	SPI包含四根线：

	1、SS（Slave Select）：片选信号线，当有多个SPI设备与SPI模块相连时，每个设备的这个片选信号线是与不同的ss相连的，
						   而其他SCK，MOSI，MISO线则为多个设备连接到相同的SPI总线上，当SS信号线为低电平时，片选有效，开始SPI通信

	2、SCK（Serial Clock）：时钟信号线，由主通信设备产生，不同的设备支持的时钟频率不一样。

	3、MOSI（Master Output，Slave Input）：主设备输出、从设备输入引脚

	4、MISO（Master Input，Slave Output）：主设备输入、从设备输出引脚

	我们SPI通信有4种不同的模式，不同的从设备可能在出厂是就是配置为某种模式，这是不能改变的；
	但我们的通信双方必须是工作在同一模式下，所以我们可以对我们的主设备的SPI模式进行配置，
	通过CPOL（时钟极性）和CPHA（时钟相位）来控制我们主设备的通信模式，具体如下：
	Mode0：CPOL=0，CPHA=0
	Mode1：CPOL=0，CPHA=1
	Mode2：CPOL=1，CPHA=0
	Mode3：CPOL=1，CPHA=1

	CPOL=0，CPHA=0：此时空闲态时，SCLK处于低电平，数据采样是在第1个边沿，也就是
	SCLK由低电平到高电平的跳变，所以数据采样是在上升沿，数据发送是在下降沿。

	CPOL=0，CPHA=1：此时空闲态时，SCLK处于低电平，数据发送是在第1个边沿，也就是
	SCLK由低电平到高电平的跳变，所以数据采样是在下降沿，数据发送是在上升沿。

	CPOL=1，CPHA=0：此时空闲态时，SCLK处于高电平，数据采集是在第1个边沿，也就是
	SCLK由高电平到低电平的跳变，所以数据采集是在下降沿，数据发送是在上升沿。

	CPOL=1，CPHA=1：此时空闲态时，SCLK处于高电平，数据发送是在第1个边沿，也就是
	SCLK由高电平到低电平的跳变，所以数据采集是在上升沿，数据发送是在下降沿。
	
	CLKPolarity: 时钟极性, 表示 SPI 在空闲时, 时钟信号是高电平还是低电平. 
				若 CPOL 被设为 1, 那么该设备在空闲时 SCK 管脚下的时钟信号为高电平. 
				当 CPOL 被设为 0 时则正好相反.
	CLKPolarity = 0: SCK idle phase is low; 
	CLKPolarity = 1: SCK idle phase is high;
	
	CLKPhase: 时钟相位, 表示 SPI 设备是在 SCK 管脚上的时钟信号变为上升沿时触发数据采样, 还是在时钟信号变为下降沿时触发数据采样. 
					CPHA=0，表示数据采样是在第1个边沿，数据发送在第2个边沿
					CPHA=1，表示数据采样是在第2个边沿，数据发送在第1个边沿
	CLKPhase = 0: Output data at negedge of clock while receiving data at posedge of clock;
	CLKPhase = 1: Output data at posedge of clock while receiving data at negedge of clock;

	主设备这边SPI时钟极性的配置（即SDO的配置）跟从设备的SDI接收数据的极性是相反的，跟从设备 SDO发送数据的极性是相同的。

	STM32F429 的 SPI 功能很强大， SPI 时钟最高可以到 45Mhz，支持 DMA，可以配置为 SPI协议或者 I2S 协议（支持全双工 I2S）


-------------------------------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------------------------*/

SPI_HandleTypeDef SPI5_Handler; //SPI句柄

void SPI5_Init(void)
{
    SPI5_Handler.Instance = SPI5;                                    //SP5
    SPI5_Handler.Init.Mode = SPI_MODE_MASTER;                        //设置SPI工作模式，设置为主模式
    SPI5_Handler.Init.Direction = SPI_DIRECTION_2LINES;              //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI5_Handler.Init.DataSize = SPI_DATASIZE_8BIT;                  //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI5_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH;               //串行同步时钟的空闲状态为高电平
    SPI5_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;                    //在串行同步时钟的第一个跳变沿（上升或下降）数据被采样
    SPI5_Handler.Init.NSS = SPI_NSS_SOFT;                            //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI5_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; //定义波特率预分频的值:波特率预分频值为256
    SPI5_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;                   //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI5_Handler.Init.TIMode = SPI_TIMODE_DISABLE;                   //关闭TI模式
    SPI5_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   //关闭硬件CRC校验
    SPI5_Handler.Init.CRCPolynomial = 7;                             //CRC值计算的多项式
    HAL_SPI_Init(&SPI5_Handler);                                     //初始化

    SPI5_ReadWriteByte(0Xff); //启动传输
}

//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_256
//APB1分频系数为4 , fAPB1时钟一般为180/4 = 45Mhz：
void SPI5_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); //判断有效性
    __HAL_SPI_DISABLE(&SPI5_Handler);                               //关闭SPI
    SPI5_Handler.Instance->CR1 &= 0XFFC7;                           //位3-5清零，用来设置波特率
    SPI5_Handler.Instance->CR1 |= SPI_BaudRatePrescaler;            //设置SPI速度
    __HAL_SPI_ENABLE(&SPI5_Handler);                                //使能SPI
}

//SPI5 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI5_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI5_Handler, &TxData, &Rxdata, 1, 1000);
    return Rxdata; //返回收到的数据
}
