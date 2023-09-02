/**************************** Authors : Mohammed Elsayaad *******************************/
/**************************** Date : 23 Aug,2023         *******************************/
/**************************** Description: VTS c file  *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "VTS.h"
#include "Stack.h"
#include <driverlib/fpu.c>
#include <driverlib/fpu.h>
/****************************************************/

volatile static void (*CallBack)(void) = NULL_PTR;
static void Portf_voidHandler(void);
/****************************************************/
void Sys_voidInit(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_12 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                           SYSCTL_XTAL_16MHZ);// 16_mhz
    /*
     * Enable lazy stacking for interrupt handlers.  This allows floating-point.
     * */
    FPUEnable();
    FPULazyStackingEnable();
/*************************************/
    /*Initialize GPIO_PORTF PIN0 as INPUT*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOIntRegister(GPIO_PORTF_BASE,Portf_voidHandler);         /* Portf_voidHandler is the callback*/
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0);               /*set pin0 as input*/
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_HIGH_LEVEL);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);                     /*Enable interrupt*/
/*************************************/
    GPS_vInit(); //gps
    uart_init(); // gsm
    delayMs(300);
    GSM_Initialize();
/*************************************/
#if DEBUG
    UARTStringPut(UART0_BASE,"HELLO GSM\n\r");
#endif
}

uint8_t Convert_voidFloatToString(float Copy_f32Number, char *const Copy_psBuffer)
{
    uint8_t Local_u8ErrorState = 0;

    if(0 == Copy_psBuffer)
    {
        Local_u8ErrorState = 1; /*1 means NULL pointer was passed*/
    }
    else
    {
        /*Number to convert - Number of digits - Buffer*/
        sprintf(Copy_psBuffer, "%.10lf", Copy_f32Number);
    }
    return (Local_u8ErrorState);
}

void Store_voidGPSData(float Copy_f32GPSLat,float Copy_f32GPSLong)
{
    uint8_t Local_u8StackStatus = 0;
    float Local_f32LatTemp = 0.0f;
    float Local_f32LongTemp = 0.0f;

    /******************************************/
    Local_u8StackStatus = push(Copy_f32GPSLat);
    if(1 == Local_u8StackStatus) // 1 means stack is full
    {
        pop(&Local_f32LatTemp);
        push(Copy_f32GPSLat);
    }
    else
    {

    }
    /*Push Long*/
    Local_u8StackStatus = push(Copy_f32GPSLong);
    if(1 == Local_u8StackStatus)
    {
        pop(&Local_f32LongTemp);
        push(Copy_f32GPSLong);
    }
    else
    {

    }
}

void Send_voidLocationToWeb(void)
{
    float   Local_f32Lat  = 0.0f;
    float   Local_f32Long = 0.0f;
    char Lat_Buffer[LAT_BUFFER_SIZE]   =  {0} ;
    char Long_Buffer[LONG_BUFFER_SIZE] =  {0} ;

    /* Send Data to web */
    pop(&Local_f32Long); /*Get Long Value*/
    pop(&Local_f32Lat);  /*Get Lat Value*/

    /* Convert to string */
    Convert_voidFloatToString(Local_f32Lat,Lat_Buffer);
    Convert_voidFloatToString(Local_f32Long,Long_Buffer);
    /*********************/
    SEND_LAT_LONG(Lat_Buffer,Long_Buffer);
    delayMs(DELAY_2SEC);
#if DEBUG
    UARTStringPut(UART0_BASE,"FINISH\n\r");
#endif
}

uint8_t Check_u8Turnes(float Copy_f32Angle)
{
    uint8_t Local_u8State = 0;
    static float Previous_f32Angle = 0.0f;
    static uint8_t Entrance_u8Flag = 0;
    float Local_f32AngleDifference = 0.0f;

    if(0 == Entrance_u8Flag)
    {
        /*First time enter this function don't need to check angle as system in it's start*/
        Previous_f32Angle = Copy_f32Angle;
        Entrance_u8Flag =1;
        Local_u8State = 1;
    }
    else
    {
        Local_f32AngleDifference = abs(Copy_f32Angle - Previous_f32Angle);
        if(U_TURN_DETECTED <= Local_f32AngleDifference)
        {
            /*Car is taking u-turn now*/
            Local_u8State = 0; /*Car Rotate state*/
        }
        else
        {
            Local_u8State =1; /*Car state is the same*/
        }
    }


    return (Local_u8State);
}

void Check_voidFrameValidity(void)
{

    float   Local_f32CurrentLong = 0.0f;
    static float Previous_f32Long = 0.0f;
    static uint8_t Entrance_u8Flag = 0;
    float Local_f32ReadingDifference = 0.0f;
    if(0 == Entrance_u8Flag)
    {
        /*First time to enter function no check is needed*/
        Entrance_u8Flag = 1;
        Read_u8Top(&Previous_f32Long);
    }
    else
    {
        Read_u8Top(&Local_f32CurrentLong); /*Get Current Long*/
        Local_f32ReadingDifference = abs(Local_f32CurrentLong - Previous_f32Long);
        if(Local_f32ReadingDifference > MAX_READ_DIFFERENCE)
        {
            /*Frame is wrong pop this reading from stack*/
            pop(&Local_f32CurrentLong);
            pop(&Local_f32CurrentLong);
        }
        else
        {
            /*Frame is valid*/
        }
    }

}

void Set_voidModuleCallBack(void (*Copy_PF)(void))
{
    if(NULL_PTR == Copy_PF)
    {
        /*Do Nothing*/
    }
    else
    {
        CallBack = Copy_PF;
    }
}

uint8_t Chack_u8CarState(float Copy_f32Velocitiy)
{
    uint8_t Local_u8ErrorState = 0;

    if(CAR_IS_MOVING <= Copy_f32Velocitiy)
    {
        Local_u8ErrorState = 1; /*Car is moving*/
    }
    else
    {
        /*Do nothing*/
    }
    return (Local_u8ErrorState);
}


/********************** Private APIs Implementations ****************************/

static void Portf_voidHandler(void)
{
    if(NULL_PTR != CallBack)
    {
        CallBack();
    }
    else
    {
        /*Do Nothing*/
    }
}

