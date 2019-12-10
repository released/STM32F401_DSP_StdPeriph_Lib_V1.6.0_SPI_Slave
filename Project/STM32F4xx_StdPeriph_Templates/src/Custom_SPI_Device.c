/* Includes ------------------------------------------------------------------*/
#include "Custom_SPI_DEVICE.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define	DBG_LOG(func,num)				(printf("%s:%d\r\n",func,num))
/* Private variables ---------------------------------------------------------*/

/*SPI variable*/
uint8_t Dummy=0x5A;

uint8_t DataRxBuffer[DataSize+CheckSumSize] = {0};	
uint8_t DataTxBuffer[DataSize+CheckSumSize] = {0};	

extern uint8_t Initready_flag;

uint8_t RxDisplay_flag = 0;
uint8_t DataReady_flag = 0;
DMA_InitTypeDef DMA_InitStructure_Slave;

/* Private functions ---------------------------------------------------------*/
static void Delay(uint32_t u32Delay)
{
	//uint32_t i;
	//i = 0xffff;
	while(u32Delay--);
}


void Custom_SPI_DMABufferStart(void)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;

	DMA_InitStructure_Slave.DMA_Priority = DMA_Priority_High;	//RX	
	DMA_InitStructure_Slave.DMA_DIR = DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure_Slave.DMA_Memory0BaseAddr = (uint32_t)&DataRxBuffer[0];
	DMA_InitStructure_Slave.DMA_BufferSize = (uint32_t)(DataSize+CheckSumSize);
	DMA_Init(DMA1_Stream3, &DMA_InitStructure_Slave);

	DMA_InitStructure_Slave.DMA_Priority = DMA_Priority_Low;	//TX
	DMA_InitStructure_Slave.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
	DMA_InitStructure_Slave.DMA_Memory0BaseAddr = (uint32_t)&DataTxBuffer[0];
	DMA_InitStructure_Slave.DMA_BufferSize = (uint32_t)(DataSize+CheckSumSize);
	DMA_Init(DMA1_Stream4, &DMA_InitStructure_Slave);
	
	DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);	
	/* I2S DMA IRQ Channel configuration */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);	
	/* I2S DMA IRQ Channel configuration */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		

//	DMA_SetCurrDataCounter(DMA1_Stream3,DataSize);
//	DMA_SetCurrDataCounter(DMA1_Stream4,DataSize);

//	DMA_MemoryTargetConfig(DMA1_Stream3,(uint32_t)&DataRxBuffer[0],DMA_Memory_0);
//	DMA_MemoryTargetConfig(DMA1_Stream4,(uint32_t)&DataTxBuffer[0],DMA_Memory_0);
	
	/* Enable DMA SPI RX Stream */
	DMA_Cmd(DMA1_Stream3,ENABLE);

	/* Enable DMA SPI TX Stream */
	DMA_Cmd(DMA1_Stream4,ENABLE);
	
	/* Enable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(SPI_DEVICE, SPI_I2S_DMAReq_Rx, ENABLE);
	/* Enable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(SPI_DEVICE, SPI_I2S_DMAReq_Tx, ENABLE);
	
	/* The Data transfer is performed in the SPI using Direct Memory Access */
	SPI_Cmd(SPI_DEVICE, ENABLE); /*!< SPI_DEVICE_SPI enable */

}

void Custom_SPI_DMABufferWait(void)
{
	/* Waiting the end of Data transfer */
//	while (DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3)==RESET);
//	while (DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4)==RESET);	

//	while (SPI_I2S_GetFlagStatus(SPI_DEVICE, SPI_I2S_FLAG_TXE) == RESET);
//	while (SPI_I2S_GetFlagStatus(SPI_DEVICE, SPI_I2S_FLAG_BSY) != RESET);

	/* Clear DMA Transfer Complete Flags */
	DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);
	DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
	
	/* Disable DMA SPI RX Stream */
	DMA_Cmd(DMA1_Stream3,DISABLE);
	/* Disable DMA SPI TX Stream */
	DMA_Cmd(DMA1_Stream4,DISABLE);
	
	/* Disable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(SPI_DEVICE, SPI_I2S_DMAReq_Rx, DISABLE);
	/* Disable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(SPI_DEVICE, SPI_I2S_DMAReq_Tx, DISABLE);
	
	SPI_Cmd(SPI_DEVICE, DISABLE);
}

void Custom_SPI_DMABufferConfig(void)
{

	/* DMA configuration -------------------------------------------------------*/
	/* Deinitialize DMA Streams */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

	DMA_DeInit(DMA1_Stream3);//RX
	DMA_DeInit(DMA1_Stream4);//TX
	
	/* Configure DMA Initialization Structure */
//	DMA_InitStructure.DMA_BufferSize = DataSize;
	DMA_InitStructure_Slave.DMA_FIFOMode = DMA_FIFOMode_Disable ;
	DMA_InitStructure_Slave.DMA_FIFOThreshold = DMA_FIFOThreshold_Full ;
	DMA_InitStructure_Slave.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	DMA_InitStructure_Slave.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure_Slave.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure_Slave.DMA_Mode = DMA_Mode_Normal;

	DMA_InitStructure_Slave.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure_Slave.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure_Slave.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/* Configure RX DMA */
	DMA_InitStructure_Slave.DMA_PeripheralBaseAddr =(uint32_t) (&(SPI_DEVICE->DR)) ;
	DMA_InitStructure_Slave.DMA_Channel = DMA_Channel_0 ;
	
//	DMA_InitStructure_Slave.DMA_Priority = DMA_Priority_High;	
//	DMA_InitStructure_Slave.DMA_DIR = DMA_DIR_PeripheralToMemory ;
//	DMA_InitStructure_Slave.DMA_Memory0BaseAddr = (uint32_t)&DataRxBuffer[0];
//	DMA_Init(DMA1_Stream3, &DMA_InitStructure);

	/* Configure TX DMA */
//	DMA_InitStructure_Slave.DMA_Priority = DMA_Priority_Low;	
//	DMA_InitStructure_Slave.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
//	DMA_InitStructure_Slave.DMA_Memory0BaseAddr = (uint32_t)&DataTxBuffer[0];
//	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
	
}


void Custom_SPI_DEVICE_TestCommand(void)
{
//	uint16_t i = 0;
	#if 0

	if ((DataReady_flag)&&(Initready_flag))
	{
		DataReady_flag = 0;
		
//		while (SPI_I2S_GetFlagStatus(SPI_DEVICE, SPI_I2S_FLAG_TXE) == RESET);	
//		while (SPI_I2S_GetFlagStatus(SPI_DEVICE, SPI_I2S_FLAG_RXNE) == RESET);
		while (DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3)==RESET);
		while (DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4)==RESET);	


//		if(DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3) == RESET)
		{
			#if 1	//debug
			printf("DataBuffer:\r\n");
			for (i=0;i<DataSize;i++)
			{
				printf("0x%2X,",DataRxBuffer[i]);
			}
			printf("\r\n");
			
			Custom_SPI_DMABufferWait(); 
			Custom_SPI_DEVICE_Slave_Config();
			
			#endif		

		}

	}
	#endif

}

void Custom_SPI_DEVICE_Slave_Config(void)
{
//	GPIO_InitTypeDef 	GPIO_InitStructure;
	SPI_InitTypeDef  	SPI_InitStructure;
//	EXTI_InitTypeDef   	EXTI_InitStructure;
//	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable the SPI periph */
	SPI_DEVICE_CLK_INIT(SPI_DEVICE_CLK, ENABLE);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPI_DEVICE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;	
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SLAVE_SPI_BAUDRATE;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_DEVICE, &SPI_InitStructure);

	/* Enable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(SPI_DEVICE, SPI_I2S_DMAReq_Rx, ENABLE);
	/* Enable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(SPI_DEVICE, SPI_I2S_DMAReq_Tx, ENABLE);	

}

void Custom_SPI_DEVICE_Slave_EXTI_Config(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	SPI_InitTypeDef  	SPI_InitStructure;
	EXTI_InitTypeDef   	EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable the SPI periph */
	SPI_DEVICE_CLK_INIT(SPI_DEVICE_CLK, ENABLE);

	/* Enable SCK, MOSI, MISO and NSS GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPI_DEVICE_MOSI_GPIO_CLK|SPI_DEVICE_MISO_GPIO_CLK |SPI_DEVICE_SCK_GPIO_CLK , ENABLE);

	GPIO_PinAFConfig(SPI_DEVICE_SCK_GPIO_PORT, SPI_DEVICE_SCK_GPIO_SOURCE, SPI_DEVICE_SCK_GPIO_AF);			//CLK
	GPIO_PinAFConfig(SPI_DEVICE_MISO_GPIO_PORT, SPI_DEVICE_MISO_GPIO_SOURCE, SPI_DEVICE_MISO_GPIO_AF);   	//MISO
	GPIO_PinAFConfig(SPI_DEVICE_MOSI_GPIO_PORT, SPI_DEVICE_MOSI_GPIO_SOURCE, SPI_DEVICE_MOSI_GPIO_AF);		//MOSI

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	/*!< Configure SPI_DEVICE_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = SPI_DEVICE_SCK_PIN;
	GPIO_Init(SPI_DEVICE_SCK_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure SPI_DEVICE_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin =  SPI_DEVICE_MISO_PIN;
	GPIO_Init(SPI_DEVICE_MISO_GPIO_PORT, &GPIO_InitStructure);  
	/*!< Configure SPI_DEVICE_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin =  SPI_DEVICE_MOSI_PIN;
	GPIO_Init(SPI_DEVICE_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPI_DEVICE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;	
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SLAVE_SPI_BAUDRATE;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_DEVICE, &SPI_InitStructure);

	/* Enable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(SPI_DEVICE, SPI_I2S_DMAReq_Rx, ENABLE);
	/* Enable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(SPI_DEVICE, SPI_I2S_DMAReq_Tx, ENABLE);

	#if 1	//for slave CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = SPI_DEVICE_CS_PIN;
	GPIO_Init(SPI_DEVICE_CS_GPIO_PORT, &GPIO_InitStructure);

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  	SYSCFG_EXTILineConfig(SPI_DEVICE_CS_EXTI_PortSource, SPI_DEVICE_CS_EXTI_PinSource);

	EXTI_InitStructure.EXTI_Line = SPI_DEVICE_CS_EXTI_Line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = SPI_DEVICE_CS_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	#endif

}   

uint8_t Get_CheckSum(uint8_t data[],uint32_t len)
{
	int i,res=0;
	for(i=0;i<len;i++)
	{
		res += data[i];
	}
	return (uint8_t)(0xFF - res);
}


