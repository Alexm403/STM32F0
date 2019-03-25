
void UART_init(void){
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
    
 
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
	  NVIC_SetPriority(USART1_IRQn, 4); /* (3) */
 // NVIC_EnableIRQ(USART1_IRQn); /* (4) */

	
    USART_Cmd(USART1, ENABLE);
}

int putU1 (int C) {
while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);Delay (10);
USART_SendData (USART1, C);
return C;};
 
void putsU1 (char *s){ ;while (*s) putU1 (*s++);}
 
void intU1(int Number)
{
//putU1 = Number
/*
int i ;
for(i=0; i<4; i++)
 putU1((Number>>(i*8)) & 0xff); 
*/
     
/*
    putU1((Number/10000)+48);
 
    Number%=10000;
 
    putU1((Number/1000)+48);
 
    Number%=1000;
 
    putU1((Number/100)+48);
 
    Number%=100;
 
    putU1((Number/10)+48);
 
    putU1((Number%10)+48);
*/


/*
int data;data=Number;
         unsigned char temp[10],count=0;
        if (data<0) 
        {
                data=0-data;
                putU1('-');
        }
                
        if (data)
        {
                while (data)
                {
                        temp[count++]=data%10+'0';
                        data/=10;                       
                }               
                
                while (count)   
                {
                        putU1(temp[--count]);                
                }
        }
        else putU1('0');  
*/


    int tmp_int;
    int tmp_byte;
    if (Number < 0)
    {
			Number =  ((Number)*(-1));
        putU1('-');
        
    }
 
		if(Number<0)Number =Number*(-1);
		
		
		
    tmp_int = Number;
    if (Number >= 10000)
    {
        tmp_byte = tmp_int / 10000;
        putU1('0' + tmp_byte);
 
        while (tmp_byte > 0)
        {
            tmp_int = tmp_int - 10000;
            tmp_byte--;
        }
    }
    if (Number >= 1000)
    {
        tmp_byte = tmp_int / 1000;
        putU1('0' + tmp_byte);
 
        while (tmp_byte > 0)
        {
            tmp_int = tmp_int - 1000;
            tmp_byte--;
        }
    }
    if (Number >= 100)
    {
        tmp_byte = tmp_int / 100;
        putU1('0' + tmp_byte);
 
        while (tmp_byte > 0)
        {
            tmp_int = tmp_int - 100;
            tmp_byte--;
        }
    }
    if (Number >= 10)
    {
        tmp_byte = tmp_int / 10;
        putU1('0' + tmp_byte);
 
        while (tmp_byte > 0)
        {
            tmp_int = tmp_int - 10;
            tmp_byte--;
        }
    }
    putU1('0' + tmp_int);
 
		
		
		
		
    }



#define SPI1_DR_8bit          (*(__IO uint8_t *)((uint32_t)&(SPI1->DR))) 

uint16_t Spi1_send (uint8_t data){

        while (!(SPI1->SR & SPI_SR_TXE));     
        SPI1_DR_8bit = data;                       
        while (!(SPI1->SR & SPI_SR_RXNE));     
        return (SPI1_DR_8bit); 
}










int spi_init (void){
	   RCC->APB2ENR |=RCC_APB2ENR_SPI1EN;
  SPI1->CR1 |=
      //  SPI_CR1_BR_0 |   
      // SPI_CR1_BR_1 |
        SPI_CR1_MSTR |
        SPI_CR1_SSI  |             
        SPI_CR1_SSM	 
	//|SPI_CR1_CPOL
	//|SPI_CR1_CPHA
	;  
   SPI1->CR2  = 0x700;//8 bit mode
   SPI1->CR2 |= SPI_CR2_FRXTH;
   SPI1->CR1 |= SPI_CR1_SPE;  
	
	//SPI_RxFIFOThresholdConfig (SPI1, SPI_RxFIFOThreshold_QF);
	
}


void DMA_USART_INIT (void)
{
	USART_Cmd(USART1, DISABLE);
	
	USART1->CR3 |= 0x80;
	//USARTx->CR1 |= USART_CR1_UE;
	 RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;               // включить тактирование SYSCFG
   RCC->AHBENR |= RCC_AHBENR_DMA1EN;                  // включить тактирование DMA1
   SYSCFG->CFGR1 |= SYSCFG_CFGR1_USART1TX_DMA_RMP;         // переключить запрос USART1 TXE на канал 4
   DMA1_Channel4->CPAR = (__IO uint32_t)&(USART1->TDR);           // адрес перифериии
//   DMA1_Channel4->CMAR = (__IO uint32_t)g.USART1_outbuf;   // адрес памяти
//   DMA1_Channel4->CNDTR = DMA_CNDTR_NDT & g.usart1_outlen;   // длина пересылки

   DMA1_Channel4->CCR &= ~DMA_CCR_MEM2MEM;   // не из памяти в память
   DMA1_Channel4->CCR &= ~DMA_CCR_PL;      // приоритет низкий
   DMA1_Channel4->CCR |= DMA_CCR_PL_1;   // приоритет высокий
   DMA1_Channel4->CCR &= ~DMA_CCR_MSIZE;   // размер данных 8 бит
   DMA1_Channel4->CCR &= ~DMA_CCR_PSIZE;   // размер периферии 8 бит
   DMA1_Channel4->CCR |= DMA_CCR_MINC;   // память инкрементировать
   DMA1_Channel4->CCR &= ~DMA_CCR_PINC;   // периферию не инкрементировать
   DMA1_Channel4->CCR &= ~DMA_CCR_CIRC;   // циркулярный режим выключен
   DMA1_Channel4->CCR |= DMA_CCR_DIR;      // направление от памяти в периферию
	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);	//IRQ_ON
	
	NVIC_SetPriority(DMA1_Channel4_5_IRQn, 1); //4. Set Priority to 1
  
	
	
	USART_Cmd(USART1, ENABLE);
}

void Send_DMA_USART_SEND (void* source, uint16_t len )
{
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;   // запретить DMA4
	 DMA1_Channel4->CMAR = (__IO uint32_t)source;   // адрес памяти буфера
   DMA1_Channel4->CNDTR = len;                  //загрузить количество данных для обмена
   DMA1_Channel4->CCR |= DMA_CCR_EN;            //разрешить работу канала
}



void DMA1_Channel4_5_IRQHandler (void)
{
   if (DMA1->ISR & DMA_ISR_GIF4)         // прерывание по окончанию вывода в USART1
   {
      DMA1_Channel4->CCR &= ~DMA_CCR_EN;   // запретить DMA4
      DMA1->IFCR = DMA_IFCR_CGIF4;      // сбросить все флаги прерывания ДМА4
   }
}

void DMA_SPI_INIT (void)
{
	SPI_Cmd(SPI1, DISABLE);
	
	SPI1->CR2 |= 0x2;
	//USARTx->CR1 |= USART_CR1_UE;
	 RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;               // включить тактирование SYSCFG
   RCC->AHBENR |= RCC_AHBENR_DMA1EN;                  // включить тактирование DMA1
   //SYSCFG->CFGR1 |= SYSCFG_CFGR1_USART1TX_DMA_RMP;         // переключить запрос USART1 TXE на канал 4
   DMA1_Channel3->CPAR = (__IO uint32_t)&(SPI1->DR);           // адрес перифериии
//   DMA1_Channel4->CMAR = (__IO uint32_t)g.USART1_outbuf;   // адрес памяти
//   DMA1_Channel4->CNDTR = DMA_CNDTR_NDT & g.usart1_outlen;   // длина пересылки

   DMA1_Channel3->CCR &= ~DMA_CCR_MEM2MEM;   // не из памяти в память
   DMA1_Channel3->CCR &= ~DMA_CCR_PL;      // приоритет низкий
   DMA1_Channel3->CCR |= DMA_CCR_PL_1;   // приоритет высокий
   DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE;   // размер данных 8 бит
   DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE;   // размер периферии 8 бит
   DMA1_Channel3->CCR |= DMA_CCR_MINC;   // память инкрементировать
   DMA1_Channel3->CCR &= ~DMA_CCR_PINC;   // периферию не инкрементировать
   DMA1_Channel3->CCR &= ~DMA_CCR_CIRC;   // циркулярный режим выключен
   DMA1_Channel3->CCR |= DMA_CCR_DIR;      // направление от памяти в периферию
	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);	//IRQ_ON
	
	NVIC_SetPriority(DMA1_Channel4_5_IRQn, 1); //4. Set Priority to 1
  
	
	
	SPI_Cmd(SPI1, ENABLE);
}

void Send_DMA_SPI_SEND (void* source, uint16_t len )
{
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;   // запретить DMA4
	 DMA1_Channel3->CMAR = (__IO uint32_t)source;   // адрес памяти буфера
   DMA1_Channel3->CNDTR = len;                  //загрузить количество данных для обмена
   DMA1_Channel3->CCR |= DMA_CCR_EN;            //разрешить работу канала
}


uint8_t dataBuffer[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
 

DMA_USART_INIT ();
DMA_SPI_INIT ();


	Send_DMA_USART_SEND (&dataBuffer[0], 16 );
	Send_DMA_SPI_SEND (&dataBuffer[0], 2100 );
