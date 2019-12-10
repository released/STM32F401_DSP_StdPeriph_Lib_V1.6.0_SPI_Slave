# STM32F401_DSP_StdPeriph_Lib_V1.6.0_SPI_Slave
 STM32F401_DSP_StdPeriph_Lib_V1.6.0_SPI_Slave

update @ 2019/12/10

Modify SPI Slave TX , with DMA function

pin define

- SCK : PB13

- MISO : PB14

- MOSI : PB15

- CS : PB12 (with EXTI)

Scenario : 

1. enable SPI RX/TX by DMA after CS pin (EXTI) trigger by master (check Custom_SPI_DMABufferStart)

2. also create TX data under EXTI IRQ (check DataTxBuffer)

3. enable DMA RX IRQ and DMA TX IRQ (check DMA1_Stream3_IRQHandler , DMA1_Stream4_IRQHandler)