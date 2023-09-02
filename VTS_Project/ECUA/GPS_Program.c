/**************************** Author : Mohammed Elsayaad *******************************/
/**************************** Date : 9 Aug,2023         *******************************/
/**************************** Description: GPS singleton design based module  *******************************/


#include "GPS_Interface.h"
#include "GPS_Private.h"

/***************************Global vars Definations*******************************/
volatile  static uint8_t GPS_u8GGA[GPS_GGA_SIZE];
const uint8_t *const GPS_pu8DesiredFrame = "$GPRMC,";
volatile static uint8_t GPS_u8DataAvilable;
/***************************Static Vars Definations*******************************/

/************************************************************************/
/* Singleton Design pattern would be very efficient for this module, so first
    create one static object of GPS_t which will hold all GPS info, after that
    limit the access to this object and creation of it so no one else can
    create it or modify it except through APIs (Setter and getter).
    to have access to this object only call GPS_GetObject() after init.
*/
static GPS_t    GPS_Object;

/************************************************************************/

/*************** Global functions implementation **************/
extern void GPS_vInit(void)
{
-        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

        GPIOPinConfigure(GPIO_PE0_U7RX);
        GPIOPinConfigure(GPIO_PE1_U7TX);
        GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);

        UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 9600,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

        UARTIntRegister(UART7_BASE,&GPS_vRecieveFrame);

        IntMasterEnable(); //enable processor interrupts
        IntEnable(INT_UART7); //enable the UART interrupt

        UARTIntEnable(UART7_BASE, UART_INT_RX|UART_INT_RT);

       // UART_SetCallBack(UART_RX,&GPS_vRecieveFrame);

}

GPS GPS_GetObject(void)
{
    /*init all objects with default values if needed*/
    return (GPS)(&GPS_Object);

}

void GPS_vReceiveData(void)
{
    uint16_t Local_u8GPSBufferIndex=0;
    float   Local_f32TempLocation =0.0;
    uint8_t Local_u8TempBuffer[12]={0};
    uint8_t Local_u8TempBufferIndex=0;
    uint8_t Local_u8GGABufferTemp[GPS_GGA_SIZE];
    //$GPRMC, 123519, A, 4807.038, N, 01131.000, E,022.4, 084.4, 230394, 003.1, W*6A
    /*************************************************/
    memcpy((void *)Local_u8GGABufferTemp,(void *)GPS_u8GGA,GPS_GGA_SIZE);
    /*************************************************/
    while(Local_u8GGABufferTemp[Local_u8GPSBufferIndex] != ',')
    {
        Local_u8GPSBufferIndex++;
    }
    Local_u8GPSBufferIndex++;
    /********************Check if it's valid*********************/
    if('V' == Local_u8GGABufferTemp[Local_u8GPSBufferIndex])
    {
        GPS_Object.IsDataAvilable=0;
        return;
    }
    else{;}

    Local_u8GPSBufferIndex += 2;
    /******************Start parsing Lat*************************/

    GPS_Object.IsDataAvilable=1;
    while(Local_u8GGABufferTemp[Local_u8GPSBufferIndex] != ',')
    {
        Local_u8TempBuffer[Local_u8TempBufferIndex] = Local_u8GGABufferTemp[Local_u8GPSBufferIndex];
        Local_u8GPSBufferIndex++;
        Local_u8TempBufferIndex++;
    }
    /*Get Lat Direction North, South , etc*/
    Local_u8GPSBufferIndex++;
    GPS_Object.Lat_Direction = Local_u8GGABufferTemp[Local_u8GPSBufferIndex];

    Local_u8TempBuffer[Local_u8TempBufferIndex] = '\0'; /*End string with null*/
    Local_f32TempLocation = (atof((void *)Local_u8TempBuffer)/100.0); /*Extract Lat value from buffer*/
    GPS_Object.Lat = Local_f32TempLocation;
    /*************Start Parsing Long************************/

    memset(Local_u8TempBuffer,'\0',12);/*Refill all buffer with nulls to use it again*/
    Local_u8GPSBufferIndex += 2; /* Move to first number after ',' , which is start of Long*/
    /*Start fill buffer till next ','*/
    Local_u8TempBufferIndex=0;
    while(Local_u8GGABufferTemp[Local_u8GPSBufferIndex] != ',')
    {
        Local_u8TempBuffer[Local_u8TempBufferIndex] = Local_u8GGABufferTemp[Local_u8GPSBufferIndex];
        Local_u8GPSBufferIndex++;
        Local_u8TempBufferIndex++;
    }
    Local_u8GPSBufferIndex++; /*Move to next value after reaching ',', which is direction of Long*/
    GPS_Object.Long_Direction = Local_u8GGABufferTemp[Local_u8GPSBufferIndex];
    Local_u8TempBuffer[Local_u8TempBufferIndex] = '\0'; /*End string with null*/
    Local_f32TempLocation = (atof((void *)Local_u8TempBuffer)/100.0); /*Extract Long value from buffer*/
    GPS_Object.Long = Local_f32TempLocation;

    /**************Start Parsing Speed*****************************/

    memset(Local_u8TempBuffer,'\0',12);/*Refill all buffer with nulls to use it again*/
    Local_u8GPSBufferIndex += 2; /* Move to first number after ',' , which is start of Long*/
    /*Start fill buffer till next ','*/
    Local_u8TempBufferIndex=0;
    while(Local_u8GGABufferTemp[Local_u8GPSBufferIndex] != ',')
    {
        Local_u8TempBuffer[Local_u8TempBufferIndex] = Local_u8GGABufferTemp[Local_u8GPSBufferIndex];
        Local_u8GPSBufferIndex++;
        Local_u8TempBufferIndex++;
    }
    Local_f32TempLocation = (atof((void *)Local_u8TempBuffer))*1.852; /*convert from knot to kmh .... 1knot = 1.852Kmh*/
    GPS_Object.Speed = Local_f32TempLocation;

    /**************Start Track angle*****************************/

    memset(Local_u8TempBuffer,'\0',12);/*Refill all buffer with nulls to use it again*/
    Local_u8GPSBufferIndex++;           /* Move to first number after ',' , which is start of Long*/
    /*Start fill buffer till next ','*/
    Local_u8TempBufferIndex=0;
    while(Local_u8GGABufferTemp[Local_u8GPSBufferIndex] != ',')
    {
        Local_u8TempBuffer[Local_u8TempBufferIndex] = Local_u8GGABufferTemp[Local_u8GPSBufferIndex];
        Local_u8GPSBufferIndex++;
        Local_u8TempBufferIndex++;
    }
    Local_f32TempLocation = (atof((void *)Local_u8TempBuffer));
    GPS_Object.Angle = Local_f32TempLocation;

}

uint8_t GPS_vTestReceiveData(void)
{
    if(1 == GPS_u8DataAvilable)
    {
        GPS_u8DataAvilable=0;
        return (1);
    }
    else
    {
        return (0);
    }

}



/*************** Private functions implementation **************/

static void GPS_vRecieveFrame(void)
{
    static uint8_t GGA_u8Counter=1;
    static uint16_t GGA_u16Index;
    uint8_t Local_u8Temp;
    /***************************/
    Local_u8Temp =  UARTCharGetNonBlocking(UART7_BASE);
    //UARTCharPut(UART5_BASE,Local_u8Temp);
    if((NEW_FRAME != Local_u8Temp) && (7 == GGA_u8Counter))
    {
        GPS_u8GGA[GGA_u16Index] = Local_u8Temp;
        GGA_u16Index++;
        GPS_u8DataAvilable=1;
    }
    else
    {
        if(NEW_FRAME == Local_u8Temp)
        {
            GGA_u8Counter =1;
            GGA_u16Index =0;
        }
        else if(END_OF_FRAME == Local_u8Temp)
        {
            //GPS_u8GGA[GGA_u16Index] = '\0';
        }
        else if(('\0' != GPS_pu8DesiredFrame[GGA_u8Counter]) && (Local_u8Temp == GPS_pu8DesiredFrame[GGA_u8Counter]))
        {
            GGA_u8Counter++;
        }
        else{;}
    }

    /*******************/
}

void UARTStringPut(uint32_t ui32Base,  char* ucString)
{
    uint8_t Local_u8Counter = 0;
    while(ucString[Local_u8Counter] != '\0')
    {
        UARTCharPut(ui32Base, ucString[Local_u8Counter]);
        Local_u8Counter++;
    }
}
