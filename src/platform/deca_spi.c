/*! ----------------------------------------------------------------------------
 * @file	deca_spi.c
 * @brief	SPI access functions
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#include "deca_spi.h"
#include "deca_device_api.h"
#include "port.h"
#include <util/atomic.h>


int writetospi_serial( uint16 headerLength,
			   	    const uint8 *headerBuffer,
					uint32 bodylength,
					const uint8 *bodyBuffer
				  );

int readfromspi_serial( uint16	headerLength,
			    	 const uint8 *headerBuffer,
					 uint32 readlength,
					 uint8 *readBuffer );
/*! ------------------------------------------------------------------------------------------------------------------
 * Function: openspi()
 *
 * Low level abstract function to open and initialise access to the SPI device.
 * returns 0 for success, or -1 for error
 */
int openspi(/*SPI_TypeDef* SPIx*/)
{
	// done by port.c, default SPI used is SPI1

	return 0;

} // end openspi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: closespi()
 *
 * Low level abstract function to close the the SPI device.
 * returns 0 for success, or -1 for error
 */
int closespi(void)
{
	return 0;

} // end closespi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 */
#pragma GCC optimize ("O3")
int writetospi_serial(uint16 headerLength, const uint8 *headerBuffer, uint32 bodylength, const uint8 *bodyBuffer) {
  int i=0;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    for(i=0; i<headerLength; i++)
    {
      SPDR = headerBuffer[i];
      while (!(SPSR & (1<<SPIF)));
      SPDR;
    }
    for(i=0; i<bodylength; i++)
    {
      SPDR = bodyBuffer[i];
      while (!(SPSR & (1<<SPIF)));
      SPDR;
    }
  }
  return 0;
} // end writetospi()


/*! ------------------------------------------------------------------------------------------------------------------
 * Function: readfromspi()
 *
 * Low level abstract function to read from the SPI
 * Takes two separate byte buffers for write header and read data
 * returns the offset into read buffer where first byte of read data may be found,
 * or returns -1 if there was an error
 */
#pragma GCC optimize ("O3")
int readfromspi_serial(uint16 headerLength, const uint8 *headerBuffer, uint32 readlength, uint8 *readBuffer) {
  int i=0;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    for(i=0; i<headerLength; i++)
    {
      SPDR = headerBuffer[i];
      while(!(SPSR & (1<<SPIF)));
      readBuffer[0] = SPDR ; // Dummy read as we write the header
    }

    for(i=0; i<readlength; i++)
    {
      SPDR = 0;  // Dummy write as we read the message body
      while(!(SPSR & (1<<SPIF)));
      readBuffer[i] = SPDR ;//port_SPIx_receive_data(); //this clears RXNE bit
    }
  }
  return 0;
} // end readfromspi()

