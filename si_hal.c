#include "si_hal.h"
uint8_t     g_halMsgLevel=0x0;

uint8_t HalTimerExpired( uint8_t timer )
{
	return 0;
}
void HalTimerInit( void )
{

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
void HalI2cBus0WriteByte( uint8_t deviceID, uint8_t offset, uint8_t value )
{
	
}
uint8_t HalI2cBus0ReadByte( uint8_t device_id, uint8_t addr )
{
	return 0;
}
void    HalTimerSet( uint8_t timer, uint16_t m_sec )
{
}
uint16_t HalTimerElapsed ( void )
{
	return 0;
}
BOOL    HalInitialize( void )
{
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
}


