// HARDWARE


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
       // SPI_CR1_BR_0 |   
       //SPI_CR1_BR_1 |
        SPI_CR1_MSTR |
        SPI_CR1_SSI  |             
        SPI_CR1_SSM	 
	//|SPI_CR1_CPOL|SPI_CR1_CPHA
	;  
   SPI1->CR2  = 0x700;//8 bit mode
   SPI1->CR2 |= SPI_CR2_FRXTH;
   SPI1->CR1 |= SPI_CR1_SPE;  
	
	//SPI_RxFIFOThresholdConfig (SPI1, SPI_RxFIFOThreshold_QF);
	
}

void CS (int in){if(in==1){GPIO_SetBits(GPIOA,GPIO_Pin_10);}else{ GPIO_ResetBits(GPIOA,GPIO_Pin_10);};}
void CE (int in){if(in==1){GPIO_SetBits(GPIOA,GPIO_Pin_4);}else{ GPIO_ResetBits(GPIOA,GPIO_Pin_4);};}



void CS_EE(int in){if(in==1){GPIO_SetBits(GPIOA,GPIO_Pin_1);}else{ GPIO_ResetBits(GPIOA,GPIO_Pin_1);};}

void EE_WE(void){CS_EE(0);Spi1_send(0x06);CS_EE(1);}//Write Enable
void EE_WD(void){CS_EE(0);Spi1_send(0x04);CS_EE(1);}//Write Disable
uint8_t EE_RS(void){uint8_t data; CS_EE(0);Spi1_send(0x05);data = Spi1_send(0);CS_EE(1); return data;}//Read Status
void EE_Wait (void){while(1){	char z;CS_EE(0);Spi1_send(0x05);z = Spi1_send(0xff);CS_EE(1);if(~z&0x01)break;}}// Wait Write in progress

void EE_Erase_Chip(void){EE_Wait();CS_EE(0);Spi1_send( 0xc7 );CS_EE(1);EE_Wait();}//about 50sec

void EE_Erase_Sector(int16_t adr16)////0to256
{
	uint32_t adr;
	adr=adr16*4096;EE_Wait();CS_EE(0);
	Spi1_send(  0x20 );//20
	Spi1_send(  adr>>16|0x00 );
	Spi1_send(  adr>>8 );
	Spi1_send(  adr );
	CS_EE(1);EE_Wait();}

	
	void EE_Erase_Blok32 (int16_t adr16)////0to32
{
	uint32_t adr;
	adr=adr16*32768;EE_Wait();CS_EE(0);
	Spi1_send(  0x52 );//20
	Spi1_send(  adr>>16|0x00 );
	Spi1_send(  adr>>8 );
	Spi1_send(  adr );
	CS_EE(1);EE_Wait();}

	
void EE_Erase_Blok64 (int16_t adr16)////0to16
{
	uint32_t adr;
	adr=adr16*65536;EE_Wait();CS_EE(0);
	Spi1_send(  0xd8 );//20
	Spi1_send(  adr>>16|0x00 );
	Spi1_send(  adr>>8 );
	Spi1_send(  adr );
	CS_EE(1);EE_Wait();}

void EE_Write_INT16 (int16_t sec, uint16_t adr16, int16_t data)//sector 1-255, adr 0-2048, data int16_t
{
	EE_Wait();
CS_EE(0);uint32_t adr;
adr=adr16*2;adr=adr|(sec*4096);
Spi1_send(  0x02 );//write
Spi1_send(  adr>>16|0x00 );
Spi1_send(  adr>>8 );
Spi1_send(  adr );

Spi1_send(  data>>8 );
Spi1_send(  data );
CS_EE(1);EE_Wait();}


int16_t EE_Read_INT16 (int16_t sec, uint16_t adr16)//sector 1-255, adr 0-2048, data int16_t
{
	EE_Wait();
CS_EE(0);uint32_t adr;
adr=adr16*2;adr=adr|(sec*4096);
Spi1_send(  0x03 );//read
Spi1_send(  adr>>16|0x00 );
Spi1_send(  adr>>8 );
Spi1_send(  adr );

int16_t data;
data = Spi1_send(0)<<8;
data |= Spi1_send(0);
CS_EE(1);
return data;}
