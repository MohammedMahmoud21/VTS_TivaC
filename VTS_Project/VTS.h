/**************************** Authors : Mohammed Elsayaad *******************************/
/**************************** Date : 23 Aug,2023         *******************************/
/**************************** Description: VTS header file  *******************************/

#ifndef VTS_H_
#define VTS_H_
/*******************************************************/

#include "GPS_Interface.h"
#include "GSM.h"

/*******************************************************/

#define DELAY_1SEC          (1000UL)
#define DELAY_2SEC          (2000UL)
#define DEBUG               (1UL)
#define DATA_AVILABLE       (1UL)
#define DATA_ARRIVIED       (1UL)
#define NULL_PTR            ((void *)0)
#define LAT_BUFFER_SIZE     (15UL)
#define LONG_BUFFER_SIZE    (15UL)
#define GSM_30SEC           (30UL)
#define GSM_1SEC            (1UL)
#define U_TURN_DETECTED     (5.0f)
#define MAX_READ_DIFFERENCE (0.000100f)
#define EMERGENCY_MESSAGE   ("Warning!!.Car Location is changing")
#define USER_NUMBER         ("01550622267")
#define CAR_IS_MOVING       (1.112f)
/*******************************************************/
typedef enum
{
    ON  = 0UL,
    OFF,
    MAX_VALUES
}CarState_t;


/*******************************************************/
/**
 * Description: This function initializes the system. It must be called before any other function in this API.
 * Parameters: None.
 * Returns: None.
 */
extern void Sys_voidInit(void);
/**
 * Description: This function stores the GPS data in the system.
 * Parameters:
            @Copy_f32GPSLat: The latitude of the GPS location.
            @Copy_f32GPSLong: The longitude of the GPS location.
 * Returns: None.
 */
extern void Store_voidGPSData(float Copy_f32GPSLat,float Copy_f32GPSLong);
/**
 * Description: This function converts a float number to a string.
 * Parameters:
            @Copy_f32Number: The float number to be converted.
            @Copy_psBuffer: The buffer to store the converted string.
 * Returns:
            0: The conversion was successful.
            1: The conversion failed.
 */
extern uint8_t Convert_voidFloatToString(float Copy_f32Number, char *const Copy_psBuffer);
/**
 * Description: This function sends the GPS location to the web server.
 * Parameters: None.
 * Returns: None.
 */
extern void Send_voidLocationToWeb(void);
/**
 * Description: This function checks if the car is turning.
 * Parameters:
            @Copy_f32Angle: The angle of the car.
 *Returns:
            0: The car is not turning.
            1: The car is turning.
 */
extern uint8_t Check_u8Turnes(float Copy_f32Angle);
/**
 * Description: This function checks the validity of GPS the frame.
 * Parameters: None.
 * Returns: None.
 */
extern void Check_voidFrameValidity(void);
/**
 * Description: This function sets the module callback function. The module callback function will be called when the GPIO Portf pin0 is triggered.
* Parameters:
           @Copy_PF: The module callback function.
* Returns: None.
 */
extern void Set_voidModuleCallBack(void (*Copy_PF)(void));
/**
 * Description: This function checks the state of the car.
 * Parameters:
        @Copy_f32Velocitiy: The velocity of the car.
 * Returns:
        0: The car is stopped.
        1: The car is moving.
 */
extern uint8_t Chack_u8CarState(float Copy_f32Velocitiy);

#endif /* VTS_H_ */
