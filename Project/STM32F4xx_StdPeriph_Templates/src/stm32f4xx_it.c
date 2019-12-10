/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t uwTimingDelay;

uint8_t Storage[256];
uint32_t pointer = 0;
uint8_t I2C_status = 0,pointer_status = 0;

#define TIM3_Cnt			(500)

uint8_t Initready_flag = 0 ;
extern uint8_t RxDisplay_flag;
extern uint8_t DataReady_flag;
extern uint8_t DataRxBuffer[DataSize+CheckSumSize];
extern uint8_t DataTxBuffer[DataSize+CheckSumSize];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
//	TimingDelay_Decrement();
}

void USART_DEVICE_Handler(void)
{
	if(USART_GetITStatus(USART_DEVICE, USART_IT_RXNE) != RESET)
	{
		Custom_USART_DEVICE_TestCommand();
	}
	USART_ClearFlag(USART_DEVICE , USART_FLAG_RXNE);
}

void TIM2_IRQHandler(void)
{
	static uint16_t TIM2_Counter = 0;
	static uint32_t Cnt = 0;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{ 

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		

		if (++TIM2_Counter>=1000)
		{
			GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
			printf("TIM2 LOGGER Counter (%2d)\r\n",Cnt++);
			TIM2_Counter = 0;
		}

		TimingDelay_Decrement();
		Custom_ButtonDebounce();
		Button_Procedure();
	}
}


void TIM3_IRQHandler(void)
{
//	static uint16_t Cnt = 0;
	static uint16_t TIM3_Counter = 0;
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{ 
		//Insert TIMx function
		if (++TIM3_Counter>=TIM3_Cnt)
		{
//			printf("TIM3 LOGGER %d sec(%2d)\r\n",TIM3_Cnt*10/1000,Cnt++);
			if (!Initready_flag)
			{
				printf("ready to receive\r\n");
				Initready_flag = 1;				
			}
			TIM3_Counter = 0;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		
	}
}

void EXTI15_10_IRQHandler(void)
{
	uint8_t i = 0;
	static uint16_t counter = 1;
	
	if(EXTI_GetITStatus(SPI_DEVICE_CS_EXTI_Line) != RESET)
	{
		if (Is_SPI_NCS_LOW())	//ready to receive data
		{
			if (!Initready_flag)
			{
				return;
			}
			
			#if 1	//create data
			for (i=0;i<DataSize;i++)
			{
				DataTxBuffer[i]= i+0x70*counter;
			}
			DataTxBuffer[DataSize+CheckSumSize-1] = Get_CheckSum(DataTxBuffer,DataSize);

			counter++;
			#else
			DataTxBuffer[0] = 0x01;
			DataTxBuffer[1] = 0x03;
			DataTxBuffer[2] = 0x05;
			DataTxBuffer[3] = 0x07;
			DataTxBuffer[4] = 0x0A;
			DataTxBuffer[5] = 0x0B;
			DataTxBuffer[7] = 0x0C;
			#endif
		
			Custom_SPI_DMABufferStart();			
		}
//		else
//		{
//			Custom_SPI_DMABufferWait(); 
//			Custom_SPI_DEVICE_Slave_Config();
//		}

		EXTI_ClearITPendingBit(SPI_DEVICE_CS_EXTI_Line);
	}
}

void DMA1_Stream3_IRQHandler(void)
{
	uint16_t i = 0;	
	/* Test on DMA Stream Transfer Complete interrupt */
	if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3))
	{
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
		DMA_ITConfig(DMA1_Stream3, DMA_IT_TCIF3, DISABLE);
		
		#if 1	//debug
		if (DataRxBuffer[DataSize+CheckSumSize-1] == Get_CheckSum(DataRxBuffer,DataSize))
		{
			printf("%s RX CHECKSUM OK ",__FUNCTION__);
			printf("\r\n\r\n");			
		}
		else
		{
			printf("%s (RX) : ",__FUNCTION__);
			for (i=0;i<(DataSize+CheckSumSize);i++)
			{
				printf("0x%2X,",DataRxBuffer[i]);
			}
			printf("\r\n\r\n");			
		}
		Custom_SPI_DMABufferWait(); 
		
		#else
		printf("%s (RX) : ",__FUNCTION__);
		for (i=0;i<(8+1);i++)
		{
			printf("0x%2X,",DataRxBuffer[i]);
		}
		printf("\r\n\r\n");

		Custom_SPI_DMABufferWait(); 
		Custom_SPI_DEVICE_Slave_Config();
		
		#endif
	}
}

void DMA1_Stream4_IRQHandler(void)
{	
	uint8_t i = 0;
	
	/* Test on DMA Stream Transfer Complete interrupt */
	if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4))
	{
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
		DMA_ITConfig(DMA1_Stream4, DMA_IT_TCIF4, DISABLE);

		#if 1
		printf("Slave TX DONE\r\n");
		#else
		printf("Slave TX : ");
		for (i=0;i<DataSize;i++)
		{
			printf("0x%2X,",DataTxBuffer[i]);
		}
		printf("\r\n");
		#endif		
	}
}


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
