


#ifndef GPS_INTERFACE_H_
#define GPS_INTERFACE_H_

#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "stdio.h"


#include "stdint.h"
#include "stdbool.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_uart.h"
#include "inc/hw_ints.h"

#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

/***
*   Struct to save the needed information from GPS frame after decoding it.
***/
typedef struct
{
    float Long;
    float Lat;
    float Speed;
    float Angle;
    uint8_t Lat_Direction;
    uint8_t Long_Direction;
    uint8_t IsDataAvilable;
}GPS_t;

typedef GPS_t * GPS;

/***********************************************************/
/***
*   Init function to init UART before using any API in this module.
***/
extern void GPS_vInit(void);
/***
*   API to handle all GPS info to user, to use it follow these steps:
        1. Create in your main GPS My_Data =NULL;
        2. Call GPS_GetObject --->  My_Data = GPS_GetObject();
***/
extern GPS GPS_GetObject(void);
/***
*   Decodes the GGA Data
***/
extern void GPS_vReceiveData(void);
uint8_t GPS_vTestReceiveData(void);
extern void UARTStringPut(uint32_t ui32Base,  char* ucString);

#endif /* GPS_INTERFACE_H_ */
