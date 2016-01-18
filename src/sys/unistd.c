/*! ----------------------------------------------------------------------------
 * @file	unistd.h
 * @brief	sleep implementation instead of library sleep
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */


#include "compiler.h"
#include <util/delay.h>

unsigned __weak sleep(unsigned seconds)
{
  _delay_ms(((double) seconds)*1000);
	return 0;
}

int __weak usleep(useconds_t useconds)
{
  _delay_us(useconds);
	return 0;
}
