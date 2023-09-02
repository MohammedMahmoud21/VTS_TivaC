/*
 * Stack.h
 *
 *  Created on: Aug 21, 2023
 *      Author: 20115
 */

#ifndef SERVICE_STACK_H_
#define SERVICE_STACK_H_

#include "stdint.h"
/***************************************/

#define STACK_SIZE (50UL)


/***************************************/

uint8_t push(float data);
uint8_t pop(float *Copy_pf32data);
uint8_t Read_u8Top(float *Copy_pf32data);
#endif /* SERVICE_STACK_H_ */
