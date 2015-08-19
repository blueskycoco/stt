#include "si_hal.h"
uint8_t     g_halMsgLevel=0x0;
volatile u16 TIM4_tout;
#define dead_time() { /* _asm("nop"); _asm("nop"); */ }
#define delay(a)          { TIM4_tout= a; while(TIM4_tout); }
#define tout()            (TIM4_tout)
#define set_tout_ms(a)    { TIM4_tout= a; }
volatile u16 TIM2_tout,set;
#define dead_time2() { /* _asm("nop"); _asm("nop"); */ }
#define delay2(a)          { TIM2_tout= a; while(TIM2_tout); }
//#define tout3()            (TIM3_tout)
//#define set_tout_ms3(a)    { TIM3_tout= a; }

@far @interrupt void TIM4InterruptHandle (void) 
{
  u8 dly= 10;
  
  TIM4->SR1= 0;
  
  if(TIM4_tout)
    if(--TIM4_tout == 0)
      _asm("nop");
  while(dly--);
}
void set_tout_ms2(u16 a)
{ 
	TIM2_tout= a; 
}
u16 tout2()
{
	return TIM2_tout;
}
@far @interrupt void TIM2InterruptHandle (void) 
{
  u8 dly= 10;
  
  TIM2->SR1= 0;
  TIM2_tout++;
  if(TIM2_tout==0xffff)
  {
  	TIM2_tout=0;
	_asm("nop");
  }
  //if(TIM3_tout)
  //  if(--TIM3_tout == 0)
  //    _asm("nop");
  while(dly--);
}

@far @interrupt void I2C_error_Interrupt_Handler (void) 
{
	I2C->SR2= 0;
	// STOP=1, generate stop
	I2C->CR2 |= 2;  
	// Disable Timout 
	TIM4_tout= 0;

}

uint8_t HalTimerExpired( uint8_t timer )
{
	return 0;
}
void HalTimerInit( void )
{
	TIM4->ARR = 0x10;                // init timer 4 1ms inetrrupts
	TIM4->PSCR= 4;
	TIM4->IER = 1;
	TIM4->CR1 |= 1;
	TIM2->ARRL = 0x08;                // init timer 4 1ms inetrrupts
	TIM2->ARRH = 0x00;
	TIM2->PSCR= 0;
	TIM2->IER = 1;
	TIM2->CR1 |= 1;
	TIM4_tout=0;
	TIM2_tout=0;
	enableInterrupts();
}
void    HalUartInit( void )
{

}
BOOL HAL_RemoteRequestHandler( void )
{
	return true;
}
uint8_t HalGpioReadRotarySwitch ( uint8_t i_want_it_now )
{
	return 0;
}
void HalI2cBus0WriteByte(int index, uint8_t deviceID, uint8_t offset, uint8_t value )
{
	if(index==0)
	{
			enableInterrupts();
			set_tout_ms(10);
		while((I2C->SR3 & 2) && tout())       									// Wait while the bus is busy
		{
			I2C->CR2 |= 2;                        								// STOP=1, generate stop
			while((I2C->CR2 & 2) && tout());      								// wait until stop is performed
		}

		I2C->CR2 |= 1;                        									// START=1, generate start
		while(((I2C->SR1 & 1)==0) && tout()); 									// Wait for start bit detection (SB)
		dead_time();                          									// SB clearing sequence
		if(tout())
		{
			I2C->DR = (u8)(deviceID << 1);   							// Send 7-bit device address & Write (R/W = 0)
		}
		while(!(I2C->SR1 & 2) && tout());     									// Wait for address ack (ADDR)
		dead_time();                          									// ADDR clearing sequence
		I2C->SR3;
		while(!(I2C->SR1 & 0x80) && tout());  									// Wait for TxE
		if(tout())
		{
			I2C->DR = offset;                 								// send Offset command
		}																							// write data loop start
		while(!(I2C->SR1 & 0x80) && tout());  								// test EV8 - wait for TxE
		I2C->DR = value;           								// send next data byte

		while(((I2C->SR1 & 0x84) != 0x84) && tout()); 					// Wait for TxE & BTF
		dead_time();                          									// clearing sequence

		I2C->CR2 |= 2;                        									// generate stop here (STOP=1)
		while((I2C->CR2 & 2) && tout());      									// wait until stop is performed  
	}
	else
	{

	}
}
uint8_t HalI2cBus0ReadByte(int index, uint8_t device_id, uint8_t addr )
{
	uint8_t byte=0;
	if(index==0)
	{			
		enableInterrupts();
		set_tout_ms(10);
		/*--------------- BUSY? -> STOP request ---------------------*/
		while(I2C->SR3 & I2C_SR3_BUSY  &&  tout())					// Wait while the bus is busy
		{
			I2C->CR2 |= I2C_CR2_STOP;									// Generate stop here (STOP=1)
			while(I2C->CR2 & I2C_CR2_STOP  &&  tout()); 				// Wait until stop is performed
		}
		I2C->CR2 |= I2C_CR2_ACK;										// ACK=1, Ack enable
		/*--------------- Start communication -----------------------*/  
		I2C->CR2 |= I2C_CR2_START;									// START=1, generate start
		while((I2C->SR1 & I2C_SR1_SB)==0	&&	tout());				// Wait for start bit detection (SB)
		/*------------------ Address send ---------------------------*/	   
		if(tout())
		{
			I2C->DR = (u8)(device_id << 1);							// Send 7-bit device address & Write (R/W = 0)
		}
		while(!(I2C->SR1 & I2C_SR1_ADDR) &&  tout()); 				// test EV6 - wait for address ack (ADDR)
		dead_time();													// ADDR clearing sequence
		I2C->SR3;
		/*--------------- Register/Command send ----------------------*/
		while(!(I2C->SR1 & I2C_SR1_TXE) &&  tout());					// Wait for TxE
		if(tout())
		{  
			I2C->DR = addr;									// Send register address
		} 															// Wait for TxE & BTF
		while((I2C->SR1 & (I2C_SR1_TXE | I2C_SR1_BTF)) != (I2C_SR1_TXE | I2C_SR1_BTF)  &&  tout()); 
		dead_time();													// clearing sequence
		/*-------------- Stop/Restart communication -------------------*/  
#ifndef TEN_BITS_ADDRESS
#ifdef NO_RESTART																		// if 7bit address and NO_RESTART setted
		I2C->CR2 |= I2C_CR2_STOP; 							// STOP=1, generate stop
		while(I2C->CR2 & I2C_CR2_STOP  &&  tout());			// wait until stop is performed
#endif // NO_RESTART
#endif // TEN_BITS_ADDRESS
		/*--------------- Restart communication ---------------------*/  
		I2C->CR2 |= I2C_CR2_START;									// START=1, generate re-start
		while((I2C->SR1 & I2C_SR1_SB)==0	&&	tout());				// Wait for start bit detection (SB)
		/*------------------ Address send ---------------------------*/	   
		if(tout())
		{
			I2C->DR = (u8)(device_id << 1) | 1;			// Send 7-bit device address & Write (R/W = 1)
		}
		while(!(I2C->SR1 & I2C_SR1_ADDR)	&&	tout());			// Wait for address ack (ADDR)
		/*------------------- Data Receive --------------------------*/

		I2C->CR2 &=~I2C_CR2_ACK;; 							// Clear ACK 
		disableInterrupts();									// Errata workaround (Disable interrupt)
		I2C->SR3; 										// Clear ADDR Flag	 
		I2C->CR2 |= I2C_CR2_STOP; 						// generate stop here (STOP=1)
		enableInterrupts();																// Errata workaround (Enable interrupt)
		while(!(I2C->SR1 & I2C_SR1_RXNE)	&&	tout());		// test EV7, wait for RxNE
		byte = I2C->DR; 							// Read Data byte

		/*--------------- All Data Received -----------------------*/
		while((I2C->CR2 & I2C_CR2_STOP)  &&  tout()); 			// Wait until stop is performed (STOPF = 1)
		I2C->CR2 &=~I2C_CR2_POS;		
	}
	else
	{

	}
	return byte;
}
void    HalTimerSet( uint8_t timer, uint16_t m_sec )
{
	set_tout_ms2(m_sec);
}
uint16_t HalTimerElapsed ( void )
{
	return TIM2_tout;
}
BOOL    HalInitialize( void )
{	
	
	//CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
  /* Set High speed internal clock prescaler */
  //CLK->CKDIVR |= (uint8_t)0x00;
	#ifdef FAST_I2C_MODE
  CLK->CKDIVR = 0x00;             // sys clock / 1
	#else
  CLK->CKDIVR = 0x01;             // sys clock / 2
	#endif
	GPIOB->ODR |= 0x30;                //define SDA, SCL outputs, HiZ, Open drain, Fast
	GPIOB->DDR |= 0x30;
	GPIOB->CR2 |= 0x30;
	//CLK->PCKENR1 |=CLK_PCKENR1_TIM2;
#ifdef FAST_I2C_MODE
	I2C->FREQR = 16;               // input clock to I2C - 16MHz 
	I2C->CCRL = 15;                // 900/62.5= 15, (SCLhi must be at least 600+300=900ns!)
	I2C->CCRH = 0x80;              // fast mode, duty 2/1 (bus speed 62.5*3*15~356kHz)
	I2C->TRISER = 5;               // 300/62.5 + 1= 5  (maximum 300ns)
#else
	I2C->FREQR = 8;                // input clock to I2C - 8MHz
	I2C->CCRL = 40;                // CCR= 40 - (SCLhi must be at least 4000+1000=5000ns!)
	I2C->CCRH = 0;                 // standard mode, duty 1/1 bus speed 100kHz
	I2C->TRISER = 9;               // 1000ns/(125ns) + 1  (maximum 1000ns)
#endif
	I2C->OARL = 0xA0;              // own address A0;
	I2C->OARH |= 0x40;
	I2C->ITR = 1;                  // enable error interrupts
	I2C->CR2 |= 0x04;              // ACK=1, Ack enable
	I2C->CR1 |= 0x01;              // PE=1

	return true;
}
BOOL HalI2cBus0ReadBlock( uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes )
{
	return true;
}
BOOL HalI2cBus0WriteBlock( uint8_t device_id, uint8_t addr, uint8_t *p_data, uint16_t nbytes )
{
	return true;
}
void HalTimerWait( uint16_t m_sec )
{
	//set_tout_ms(m_sec);
	//while(TIM4_tout>0);
	set_tout_ms2(1);
	while(TIM2_tout<m_sec);
}


