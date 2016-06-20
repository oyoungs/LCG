#include "usart.h"
#include "stm32f4xx_usart.h"
#include "buffer.h"

#include <stdio.h>
#include <string.h>


void USART1_Init(void);
void USART2_Init(void);
void USART3_Init(void);
void USART4_Init(void);
void USART5_Init(void);
void USART6_Init(void);

static USART_TypeDef *Coms[COMn] = {0, 0, 0, 0, 0, 0 };
static Buffer *ComBuffers[COMn] = {0, 0, 0, 0, 0, 0 };


#ifdef  USE_USART1
static ReceiveHandler U1_Receive;
static Buffer Com1TxBuffer;
#endif

#ifdef  USE_USART2
static ReceiveHandler U2_Receive;
static Buffer Com2TxBuffer;
#endif

#ifdef  USE_USART3
static ReceiveHandler U3_Receive;
static Buffer Com3TxBuffer;
#endif

#ifdef  USE_USART4
static ReceiveHandler U4_Receive;
static Buffer Com4TxBuffer;
#endif

#ifdef  USE_USART5
static ReceiveHandler U5_Receive;
static Buffer Com5TxBuffer;
#endif

#ifdef  USE_USART6
static ReceiveHandler U6_Receive;
static Buffer Com6TxBuffer;
#endif




void InitCom(Com_TypeDef com)
{
	switch(com)
	{
#ifdef USE_USART1
		case COM1:
			USART1_Init();break;
#endif
#ifdef USE_USART2
		case COM2:
			USART2_Init();break;
#endif
#ifdef USE_USART3
		case COM3:
			USART3_Init();break;
#endif
#ifdef USE_USART4
		case COM4:
			USART4_Init();break;
#endif
#ifdef USE_USART5
		case COM5:
			USART5_Init();break;
#endif
#ifdef USE_USART6
		case COM6:
			USART6_Init();break;
#endif
		default:break;
	}
}


void SerialPutBuffer(Com_TypeDef com,  const char *buffer, int size, int immediate)
{
	Buffer *txBuffer = ComBuffers[com];
	BF_Write(txBuffer, buffer, size);
	USART_ITConfig(Coms[com], USART_IT_TXE, ENABLE );
	if(immediate != 0) SerialSendStart(com);
}
void SerialPutString(Com_TypeDef com, const char *string)
{
	int length = strlen(string);
	SerialPutBuffer(com, string, length, 1);
}

void SerialSendStart(Com_TypeDef com)
{
//	Buffer *buffer = ComBuffers[com];
	USART_TypeDef *Ux = Coms[com];
	USART_ITConfig(Ux, USART_IT_TXE, ENABLE );
//	if(BF_Empty(buffer)) {
//		USART_ITConfig(Ux, USART_IT_TXE, DISABLE );
//	} else {
//		USART_ITConfig(Ux, USART_IT_TXE, ENABLE );
//		USART_SendData(Ux, BF_ReadByte(buffer));
//	}
}
	
void SerialHandlerSet(Com_TypeDef com, ReceiveHandler handler)
{
	switch(com)
	{
		
#ifdef USE_USART1
		case COM1:
			U1_Receive = handler;break;
#endif
#ifdef USE_USART2
		case COM2:
			U2_Receive = handler;break;
#endif
#ifdef USE_USART3
		case COM3:
			U3_Receive = handler;break;
#endif
#ifdef USE_USART4
		case COM4:
			U4_Receive = handler;break;
#endif
#ifdef USE_USART5
		case COM5:
			U5_Receive = handler;break;
#endif
#ifdef USE_USART6
		case COM6:
			U6_Receive = handler;break;
#endif
		default:break;
	}
}

/*---------------------private functions--------------------------------------------*/
	
#ifdef USE_USART1
/*
* 串口1初始化
*/
void USART1_Init()
{
	
	//TODO
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
  //串口1对应引脚复用映射
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
  
  //USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
  GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
  USART_InitStructure.USART_BaudRate = COM1_BAUDRATE;//波特率设置
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  
  USART_Cmd(USART1, ENABLE);  //使能串口1 
  
  USART_ClearFlag(USART1, USART_FLAG_TC);
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 3;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    //子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;      //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);  //根据指定的参数初始化VIC寄存器、

	
	//
	Coms[COM1] = USART1;
	BF_Init(&Com1TxBuffer);
	ComBuffers[COM1] = &Com1TxBuffer;
}

/*
* 串口1中断
*/
void USART1_IRQHandler()
{
	char cChar;
  if( USART_GetITStatus( USART1, USART_IT_TXE ) == SET ) {
    /* The interrupt was caused by the THR becoming empty.  Are there any
    more characters to transmit? */

    if(BF_Empty(&Com1TxBuffer)) {
			USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
		} else {
			USART_SendData(USART1, BF_ReadByte(&Com1TxBuffer));
		}
    USART_ClearITPendingBit(USART1, USART_IT_TXE);
  }
  
  if( USART_GetITStatus( USART1, USART_IT_RXNE ) == SET ) {
    cChar = USART_ReceiveData( USART1 );
    if(U1_Receive) U1_Receive( cChar );
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
  if( USART_GetITStatus( USART1, USART_IT_ORE ) == SET ) {
    cChar = USART_ReceiveData( USART1);
  }	
}


#endif /*----------USE_USART1  END-------*/

#ifdef USE_USART2
/*
* 串口2初始化
*/	
void USART2_Init()
{
	Coms[COM2] = USART2;
	BF_Init(&Com2TxBuffer);
}

/*
* 串口2中断
*/
void USART2_IRQHandler()
{
	
}
#endif /*----------USE_USART2  END-------*/

#ifdef USE_USART3
/*
* 串口3初始化
*/
void USART3_Init()
{
	Coms[COM3] = USART3;
	BF_Init(&Com3TxBuffer);
	ComBuffers[COM3] = &Com3TxBuffer;
}

/*
* 串口3中断
*/
void USART3_IRQHandler()
{
	char cChar;
  if( USART_GetITStatus( USART3, USART_IT_TXE ) == SET ) {
    /* The interrupt was caused by the THR becoming empty.  Are there any
    more characters to transmit? */

    if(BF_Empty(&Com3TxBuffer)) {
			USART_ITConfig( USART3, USART_IT_TXE, DISABLE );
		} else {
			cChar = BF_ReadByte(&Com3TxBuffer);
			USART_SendData(USART3, cChar);
		}
    USART_ClearITPendingBit(USART3, USART_IT_TXE);
  }
  
  if( USART_GetITStatus( USART3, USART_IT_RXNE ) == SET ) {
    cChar = USART_ReceiveData( USART3 );
    if(U3_Receive) U3_Receive( cChar );
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  }
  if( USART_GetITStatus( USART3, USART_IT_ORE ) == SET ) {
    cChar = USART_ReceiveData( USART3);
  } 	
}


#endif /*----------USE_USART3  END-------*/

#ifdef USE_USART4
/*
* 串口4初始化
*/
void USART4_Init()
{
	
}

/*
* 串口4中断
*/
void USART4_IRQHandler()
{
	
}

#endif /*----------USE_USART4  END-------*/

#ifdef USE_USART5
/*
* 串口5初始化
*/
void USART5_Init()
{
	
}

/*
* 串口5中断
*/
void USART5_IRQHandler()
{
	
}


#endif /*----------USE_USART5  END-------*/

#ifdef USE_USART6
/*
* 串口6初始化
*/
void USART6_Init()
{
	//TODO
	Coms[COM6] = USART6;
	BF_Init(&Com6TxBuffer);
	ComBuffers[COM6] = &Com6TxBuffer;
}

/*
* 串口6中断
*/
void USART6_IRQHandler()
{
	char cChar;
  if( USART_GetITStatus( USART6, USART_IT_TXE ) == SET ) {
    /* The interrupt was caused by the THR becoming empty.  Are there any
    more characters to transmit? */

    if(BF_Empty(&Com6TxBuffer)) {
			USART_ITConfig( USART6, USART_IT_TXE, DISABLE );
		} else {
			cChar = BF_ReadByte(&Com6TxBuffer);
			USART_SendData(USART6, cChar);
		}
    USART_ClearITPendingBit(USART6, USART_IT_TXE);
  }
  
  if( USART_GetITStatus( USART6, USART_IT_RXNE ) == SET ) {
    cChar = USART_ReceiveData( USART6 );
    if(U6_Receive) U6_Receive( cChar );
    USART_ClearITPendingBit(USART6, USART_IT_RXNE);
  }
  if( USART_GetITStatus( USART6, USART_IT_ORE ) == SET ) {
    cChar = USART_ReceiveData( USART6 );
  } 
}



#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif

#endif /*----------USE_USART6  END-------*/
