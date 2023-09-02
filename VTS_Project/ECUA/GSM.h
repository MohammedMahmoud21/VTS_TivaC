


/*File Guard*/
#ifndef HAL_GSM_GSM_H_
#define HAL_GSM_GSM_H_

#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>



#include "stdint.h"
#include "stdio.h"
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





#define DEFAULT_BUFFER_SIZE	200	    /* Define default buffer size */
#define DEFAULT_TIMEOUT		20000	/* Define default timeout */
#define DEFAULT_CRLF_COUNT	2	    /* Define default CRLF count */

#define POST		1	/* Define method */
#define GET			0
#define POST_DEMO			/* Define POST demo */

/* Define Required fields shown below */
#define URL					"vts-nti-project.000webhostapp.com/gpsdata.php?"/* Server Name */
#define API_WRITE_KEY		""/* Write key */
#define CHANNEL_ID			""	/* Channel ID */
#define APN					"internet.etisalat" /* APN of GPRS n/w provider  "internet.vodafone.net" */
#define USERNAME			""
#define PASSWORD			""

enum SIM900_RESPONSE_STATUS		/* Enumerate response status */
{
	SIM900_RESPONSE_WAITING,
	SIM900_RESPONSE_FINISHED,
	SIM900_RESPONSE_TIMEOUT,
	SIM900_RESPONSE_BUFFER_FULL,
	SIM900_RESPONSE_STARTING,
	SIM900_RESPONSE_ERROR
};

void GSM_Initialize(void);
void SEND_LAT_LONG(char *lat , char *lng);
void GSM_CheckConnection(void);
void Read_Response();
void Start_Read_Response();
void Buffer_Flush();
void GetResponseBody(char* Response, uint16_t ResponseLength);
bool WaitForExpectedResponse(char* ExpectedResponse);
bool SendATandExpectResponse(char* ATCommand, char* ExpectedResponse);
bool HTTP_Parameter(char* Parameter, char* Value);
bool SIM900HTTP_Start()	;
bool SIM900HTTP_Connect(char* _APN, char* _USERNAME, char* _PASSWORD);
bool HTTP_Init();
bool HTTP_Terminate();
bool HTTP_SetURL(char * url);
bool HTTP_Connected();
bool HTTP_SetPost_json();
bool HTTP_Save();
bool HTTP_Data(char* data);
bool HTTP_Action(char method);
bool HTTP_Read(uint8_t StartByte, uint16_t ByteSize);
uint8_t HTTP_Post(char* Parameters, uint16_t ResponseLength);
uint8_t HTTP_get(char * _URL, uint16_t ResponseLength);
bool SIM900HTTP_Init();
void GSM_Send_Msg(char *num,char *sms);
void SEND_LAT_LONG(char *lat , char *lng);
extern void uart_init(void);
extern void delayMs(uint32_t ui32Ms);
void ISR(void);







#endif /* HAL_GSM_GSM_H_ */
