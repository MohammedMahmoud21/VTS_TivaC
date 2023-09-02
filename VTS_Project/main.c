
/***
 * @Author  : Mohammed ELsayaad.
 * @Date    : 23 Aug,2023.
 * @Version : 1.00.
 * @ Description:

    This describes the real-time vehicle tracking system that uses a GPS module to monitor the location of the car and a GSM module to send the data to a user-friendly web server. The system can also detect if the car is shut down and is moving, which means someone is trying to steal the car. The system also validates the GPS frame to ensure that the data inside the frame is valid with the last 1 second previous frame location. If the car is taking a turn, the rate of sending the data will increase.
    If the car is being stolen, an SMS will be sent to the user.The TM4c (Tiva C) microcontroller was used as the main MCU.
* @ System Overview :

The real-time vehicle tracking system consists of the following components:

1.GPS module: The GPS module is used to determine the location of the car. It sends the latitude and longitude of the car to the MCU.
2.GSM module: The GSM module is used to send the data from the MCU to the web server. It also sends SMS messages to the user if the car is being stolen.
3.MCU: The MCU is the central processing unit of the system. It controls the GPS module, the GSM module, and the other components of the system.
4.Web server: The web server is used to display the location of the car on a map. The user can also access the web server to view other information about the car, such as its speed and direction.
*@ System Operation:
The system works as follows:

a.The GPS module sends the latitude and longitude of the car to the MCU.
b.The MCU validates the GPS frame to ensure that the data inside the frame is valid with the last 1 second previous frame location.
c.If the car is taking a turn, the rate of sending the data will increase.
d.If the car is shut down and is moving, the system will detect that someone is trying to steal the car and send an SMS message to the user.
e.The MCU sends the data from the GPS module to the GSM module.
f.The GSM module sends the data to the web server.
g.The web server displays the location of the car on a map.
 */

/*****************Module Libraries include section start**************************/
#include "VTS.h"
/*****************Module Libraries include section end****************************/

/**************Global Variable definition section start*********************/
static CarState_t Car_enumStatus = ON;
static void Car_voidState(void);
/**************Global Variable definition section end***********************/

/***************************main function call*******************************************/
int main(void)
{
    GPS      Local_pstructMyGPS;         /*GPS instance to receive data in it*/
    uint8_t  Local_u8GPSDataArrived = 0;
    uint8_t  Local_u8GPSDataReady = 0;
    uint8_t  Local_u8CheckAlgorithmState = 0;
    uint16_t Local_u16GSMPeriodicity = 0;
    uint8_t Local_u8PeriodicityValue = GSM_30SEC; /*This var is used to control the frequancy of messages sent to web*/
    /****************** Init Section ************************/

    /*Initialize all modules*/
    Sys_voidInit();
    Set_voidModuleCallBack(&Car_voidState);
    /*For Debug needs*/

#if DEBUG
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 );
#endif

    /****************** End of Init Section *****************/

    while(1)
    {

        switch(Car_enumStatus)
        {
        case ON:
            delayMs(DELAY_1SEC);
#if DEBUG
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
#endif
            Local_u8GPSDataArrived = GPS_vTestReceiveData();
            if (DATA_AVILABLE  == Local_u8GPSDataArrived)
            {
                /*
                 * 1) Read Data By calling (GPS_vReceiveData()).
                 * 2) If Data Valid then convert float to string then put it in the Stack.
                 * 3) if not don't continue.
                 */
#if DEBUG
                UARTStringPut(UART0_BASE, "GPS\n\r");
#endif
                GPS_vReceiveData();
                if(1 == Local_pstructMyGPS->IsDataAvilable)
                {
                    /*  Store GPS Data.
                     */
                    Store_voidGPSData(Local_pstructMyGPS->Lat,Local_pstructMyGPS->Long);
                    Local_u8GPSDataReady = 1;
#if DEBUG
                    UARTStringPut(UART0_BASE, "GPS AVilable\n\r");
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);//blue
#endif

                }
                else {/*Do Nothing*/}
            }
            else
            {
#if DEBUG
                UARTStringPut(UART0_BASE, "GPS NOT AVilable\n\r");
                //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
#endif
            }
            /*********************** GPS Check algorithm section start *****************************/
            /***********************                                   *****************************/
            /* 1) check if car is rotate by calling Check_u8Turnes();                    */
            /* 2) check if frame is valid frame by calling Check_voidFrameValidity();    */
            if(DATA_ARRIVIED == Local_u8GPSDataReady)
            {
                /* 1) Check U-Turn algorithm*/
                Local_u8CheckAlgorithmState = Check_u8Turnes(Local_pstructMyGPS->Angle);
                if(1 == Local_u8CheckAlgorithmState)
                {
                    Local_u8PeriodicityValue = GSM_30SEC;
                }
                else
                {
                    Local_u8PeriodicityValue = GSM_1SEC;
                }
                /* 2) Check Valid frame, this API checks if frame is valid and if not
                 *  it removes frame invalid readings from stack.
                 *  */
                Check_voidFrameValidity();
            }
            else
            {
                /*No valid data, no check is needed*/
            }
            /***********************                                 ******************************/
            /*********************** GPS Check algorithm section End ******************************/

            /*********************** GSM Part *****************************/
            /**
             * 1) Check if Data is ready
             * 2) If ready
             *      a)  convert from float to string.
             *      b)  send data to web site.
             * 3) if Not Don't send any thing.
             */

            GSM_CheckConnection();
            //Send_voidLocationToWeb();
            if((DATA_ARRIVIED == Local_u8GPSDataReady) && (Local_u8PeriodicityValue <= Local_u16GSMPeriodicity))
            {
                Send_voidLocationToWeb();
#if DEBUG
                UARTStringPut(UART0_BASE,"FINISH\n\r");
#endif

                /* Clear DataReady */
                Local_u8GPSDataReady    = 0;
                Local_u16GSMPeriodicity = 0;
            }
            else {/* Do Nothing */}
            Local_u16GSMPeriodicity++;
            break;
        case OFF:
            /*Go Sleep*/
            /*
             * 1)   if off and car position changed, start to take an action.
             * */
            Local_u8GPSDataArrived = GPS_vTestReceiveData();
            if (DATA_AVILABLE  == Local_u8GPSDataArrived)
            {
                /*
                 * 1) Read Data By calling (GPS_vReceiveData()).
                 * 2) If Data Valid then convert float to string then put it in the Stack.
                 * 3) if not don't continue.
                 */
#if DEBUG
                UARTStringPut(UART0_BASE, "GPS\n\r");
#endif
                GPS_vReceiveData();
                if(1 == Local_pstructMyGPS->IsDataAvilable)
                {
                    /*  Store GPS Data.
                     */
                    Store_voidGPSData(Local_pstructMyGPS->Lat,Local_pstructMyGPS->Long);
                    Local_u8GPSDataReady = 1;
#if DEBUG
                    UARTStringPut(UART0_BASE, "GPS AVilable\n\r");
#endif
                }
                else {/*Do Nothing*/}
            }
            else
            {
#if DEBUG
                UARTStringPut(UART0_BASE, "GPS NOT AVilable\n\r");
                //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
#endif
             }
            if(Chack_u8CarState())
            {
                /*Send emergency message*/
                GSM_Send_Msg(USER_NUMBER,EMERGENCY_MESSAGE);
            }
            else
            {
                /*Noting*/
            }
            break;
        default:
            /*Do Nothing*/
            break;
        }

    }

}


static void Car_voidState(void)
{
    /*Change state of the car*/
    Car_enumStatus ^= 0x01;
}
