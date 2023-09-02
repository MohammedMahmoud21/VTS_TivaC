/*
 * GSM.c
 *
 *  Created on: Aug 12, 2023
 *      Author: Mohamed
 */

#include "GSM.h"



volatile uint8_t Response_Status=0;
volatile uint8_t CRLF_COUNT = 0;
volatile uint16_t Counter = 0;
volatile uint32_t TimeOut = 0;
uint8_t RESPONSE_BUFFER[DEFAULT_BUFFER_SIZE];
volatile char  _buffer[200];

/*********************************************/
static void UARTStringPut(uint32_t ui32Base,  char *ucString);
/*********************************************/
void uart_init(void)
{


     SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

     SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

     GPIOPinConfigure(GPIO_PE4_U5RX);
     GPIOPinConfigure(GPIO_PE5_U5TX);

     GPIOPinConfigure(GPIO_PA0_U0RX);
     GPIOPinConfigure(GPIO_PA1_U0TX);


     GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
     GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

     UARTConfigSetExpClk(UART5_BASE, SysCtlClockGet(), 9600,
             (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

     UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
                  (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


     UARTIntRegister(UART5_BASE,&ISR);

     IntMasterEnable(); //enable processor interrupts
     IntEnable(INT_UART5); //enable the UART interrupt

     UARTIntEnable(UART5_BASE, UART_INT_RX|UART_INT_RT);


}





void GSM_Initialize(void)
{
    while(!SIM900HTTP_Start());
    while(!(SIM900HTTP_Connect(APN, USERNAME, PASSWORD)));
    HTTP_Terminate();
    delayMs(100);
    SIM900HTTP_Init();
}

void GSM_CheckConnection(void)
{

    if (!HTTP_Connected()) /* Check whether GPRS connected */
    {
        SIM900HTTP_Connect(APN, USERNAME, PASSWORD);
        SIM900HTTP_Init();
    }
}
void SEND_LAT_LONG(char *lat , char *lng)
{
    HTTP_SetURL(URL); HTTP_Save();
    //sprintf(_buffer,"AT+HTTPPARA=\"URL\",\"nti-vts-project.000webhostapp.com/gpsdata.php?lat=%s&lng=%s&door=1&ignition=2&velocity=3.4&angle=5.6\"\r\n",lat,lng);
    sprintf(_buffer,"AT+HTTPPARA=\"URL\",\"vts-nti-project.000webhostapp.com/gpsdata.php?lat=%s&lng=%s\"\r\n",lat,lng);
    //sprintf(_buffer,"AT+HTTPPARA=\"URL\",\"sherif0123.000webhostapp.com/gpsdata.php?lat=%s&lng=%s\"\r\n",lat,lng);
    UARTStringPut(UART5_BASE,_buffer);

    UARTStringPut(UART5_BASE,"AT+HTTPACTION=0\r");
    memset(_buffer, 0, 200);
  //  SysCtlDelay(600);
}

void Read_Response()        /* Read response */
{
    static char CRLF_BUF[2];
    static char CRLF_FOUND;
    uint16_t i=0;
    uint32_t TimeCount = 0, ResponseBufferLength;
    while(1)
    {
        if(TimeCount >= (DEFAULT_TIMEOUT+TimeOut))
        {
            CRLF_COUNT = 0; TimeOut = 0;
            Response_Status = SIM900_RESPONSE_TIMEOUT;
            //H_Lcd_Void_LCDGoTo(1, 2);
            UARTStringPut(UART0_BASE,"TIMEOUT\n\r");
            return;
        }

        if(Response_Status == SIM900_RESPONSE_STARTING)
        {
            CRLF_FOUND = 0;
            memset(CRLF_BUF, 0, 2);
            Response_Status = SIM900_RESPONSE_WAITING;
            //H_Lcd_Void_LCDGoTo(2, 2);
            //H_Lcd_Void_LCDWriteString("Initializing .... ");
        }
        ResponseBufferLength = strlen(RESPONSE_BUFFER);

        if (ResponseBufferLength)
        {

           delayMs(1);
            TimeCount++;
            if (ResponseBufferLength==strlen(RESPONSE_BUFFER))
            {
                for (i=0;i<ResponseBufferLength;i++)
                {
                    memmove(CRLF_BUF, CRLF_BUF + 1, 1);
                    CRLF_BUF[1] = RESPONSE_BUFFER[i];
                    if(!strncmp(CRLF_BUF, "\r\n", 2))
                    {

                        if(++CRLF_FOUND == (DEFAULT_CRLF_COUNT+CRLF_COUNT))
                        {
                            CRLF_COUNT = 0; TimeOut = 0;
                            Response_Status = SIM900_RESPONSE_FINISHED;
                            return;
                        }
                    }
                }
                CRLF_FOUND = 0;
            }
        }
        delayMs(1);
        TimeCount++;
    }
}

void Start_Read_Response()
{
    Response_Status = SIM900_RESPONSE_STARTING;
    do {
        Read_Response();
    } while(Response_Status == SIM900_RESPONSE_WAITING);
}

void Buffer_Flush()     /* Flush all variables */
{
    memset(RESPONSE_BUFFER, 0, DEFAULT_BUFFER_SIZE);
    Counter=0;
}

/* Remove CRLF and other default strings from response */
void GetResponseBody(char* Response, uint16_t ResponseLength)
{
    uint16_t i = 12;
    char buffer[5];
    while(Response[i] != '\r' && i < 100)
        ++i;

    strncpy(buffer, Response + 12, (i - 12));
    ResponseLength = atoi(buffer);

    i += 2;
    uint16_t tmp = strlen(Response) - i;
    memcpy(Response, Response + i, tmp);

    if(!strncmp(Response + tmp - 6, "\r\nOK\r\n", 6))
        memset(Response + tmp - 6, 0, i + 6);
}

bool WaitForExpectedResponse(char* ExpectedResponse)
{
    Buffer_Flush();
    delayMs(200);
    Start_Read_Response();      /* First read response */

    if((Response_Status != SIM900_RESPONSE_TIMEOUT) && (strstr(RESPONSE_BUFFER, ExpectedResponse) != NULL))
    {
        return true;        /* Return true for success */
    }
    return false;           /* Else return false */
}

bool SendATandExpectResponse(char* ATCommand, char* ExpectedResponse)
{
    UARTStringPut(UART5_BASE,ATCommand); /* Send AT command to SIM900 */
    UARTCharPut(UART5_BASE,'\r');
    //_delay_ms(200);
    return WaitForExpectedResponse(ExpectedResponse);
}

bool HTTP_Parameter(char* Parameter, char* Value)/* Set HTTP parameter and return response */
{

    UARTStringPut(UART5_BASE,"AT+HTTPPARA=\"");
    UARTStringPut(UART5_BASE,Parameter);
    UARTStringPut(UART5_BASE,"\",\"");
    UARTStringPut(UART5_BASE,Value);
    UARTStringPut(UART5_BASE,"\"\r");
    return WaitForExpectedResponse("OK");
}

bool SIM900HTTP_Start()         /* Check SIM900 board */
{
    uint8_t i=0;
    for (i=0;i<5;i++)
    {
        if(SendATandExpectResponse("ATE0","OK")||SendATandExpectResponse("AT","OK"))
        {
            HTTP_Parameter("CID","1");


            return true;
        }
    }
    return false;
}

bool SIM900HTTP_Connect(char* _APN, char* _USERNAME, char* _PASSWORD) /* Connect to GPRS */
{

    UARTStringPut(UART5_BASE,"AT+CREG?\r");
    if(!WaitForExpectedResponse("+CREG: 0,1"))
        return false;

    UARTStringPut(UART5_BASE,"AT+SAPBR=0,1\r");
    WaitForExpectedResponse("OK");

    UARTStringPut(UART5_BASE,"AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r");
    WaitForExpectedResponse("OK");

    UARTStringPut(UART5_BASE,"AT+SAPBR=3,1,\"APN\",\"");
    UARTStringPut(UART5_BASE,_APN);
    UARTStringPut(UART5_BASE,"\"\r");
    WaitForExpectedResponse("OK");

    UARTStringPut(UART5_BASE,"AT+SAPBR=3,1,\"USER\",\"");
    UARTStringPut(UART5_BASE,_USERNAME);
    UARTStringPut(UART5_BASE,"\"\r");
    WaitForExpectedResponse("OK");

    UARTStringPut(UART5_BASE,"AT+SAPBR=3,1,\"PWD\",\"");
    UARTStringPut(UART5_BASE,_PASSWORD);
    UARTStringPut(UART5_BASE,"\"\r");
    WaitForExpectedResponse("OK");

    UARTStringPut(UART5_BASE,"AT+SAPBR=1,1\r");
    return WaitForExpectedResponse("OK");
}

bool HTTP_Init()        /* Initiate HTTP */
{
    UARTStringPut(UART5_BASE,"AT+HTTPINIT\r");
    return WaitForExpectedResponse("OK");
}

bool HTTP_Terminate()       /* terminate HTTP */
{
    UARTStringPut(UART5_BASE,"AT+HTTPTERM\r");
    return WaitForExpectedResponse("OK");
}

bool HTTP_SetURL(char * url)    /* Set URL */
{
    return HTTP_Parameter("URL", url);
}

bool HTTP_Connected()       /* Check for connected */
{
    UARTStringPut(UART5_BASE,"AT+SAPBR=2,1\r");
    CRLF_COUNT = 2;                                     /* Make additional crlf count for response */
    return WaitForExpectedResponse("+SAPBR: 1,1");
}

bool HTTP_SetPost_json()    /* Set Json Application format for post */
{
    return HTTP_Parameter("CONTENT", "application/json");
}

bool HTTP_Save()        /* Save the application context */
{
    UARTStringPut(UART5_BASE,"AT+HTTPSCONT\r");
    return WaitForExpectedResponse("OK");
}

bool HTTP_Data(char* data)  /* Load HTTP data */
{
    char _buffer[25];
    sprintf(_buffer, "AT+HTTPDATA=%d,%d\r", strlen(data), 10000);
    UARTStringPut(UART5_BASE,_buffer);

    if(WaitForExpectedResponse("DOWNLOAD"))
    {

        return SendATandExpectResponse(data, "OK");
    }

    else
    {
        return false;
    }

}

bool HTTP_Action(char method)   /* Select HTTP Action */
{
    if(method == GET)
        UARTStringPut(UART5_BASE,"AT+HTTPACTION=0\r");
    if(method == POST)
        UARTStringPut(UART5_BASE,"AT+HTTPACTION=1\r");
    return WaitForExpectedResponse("OK");
}

bool HTTP_Read(uint8_t StartByte, uint16_t ByteSize) /* Read HTTP response */
{
    char Command[25];
    sprintf(Command,"AT+HTTPREAD=%d,%d\r",StartByte,ByteSize);
    Command[25] = 0;
    UARTStringPut(UART5_BASE,Command);

    CRLF_COUNT = 2;                                     /* Make additional crlf count for response */
    if(WaitForExpectedResponse("+HTTPREAD"))
    {
        GetResponseBody(RESPONSE_BUFFER, ByteSize);
        return true;
    }
    else
        return false;
}

uint8_t HTTP_Post(char* Parameters, uint16_t ResponseLength)
{
    HTTP_Parameter("CID","1");
    if(!(HTTP_Data(Parameters) && HTTP_Action(POST)))
    {
        return SIM900_RESPONSE_TIMEOUT;
    }


    bool status200 = WaitForExpectedResponse(",200,");

    if(Response_Status == SIM900_RESPONSE_TIMEOUT)
        return SIM900_RESPONSE_TIMEOUT;
    if(!status200)
        return SIM900_RESPONSE_ERROR;

    HTTP_Read(0, ResponseLength);
    return SIM900_RESPONSE_FINISHED;
}

uint8_t HTTP_get(char * _URL, uint16_t ResponseLength)
{
    HTTP_Parameter("CID","1");
    HTTP_Parameter("URL", _URL);
    HTTP_Action(GET);
    WaitForExpectedResponse("+HTTPACTION:0,");
    if(Response_Status == SIM900_RESPONSE_TIMEOUT)
        return SIM900_RESPONSE_TIMEOUT;

    HTTP_Read(0, ResponseLength);
    return SIM900_RESPONSE_FINISHED;
}

bool SIM900HTTP_Init()
{
    HTTP_Terminate();
    return HTTP_Init();
}

static void UARTStringPut(uint32_t ui32Base,  char* ucString)
{
    uint8_t Local_u8Counter = 0;
    while(ucString[Local_u8Counter] != '\0')
    {
        UARTCharPut(ui32Base, ucString[Local_u8Counter]);
        Local_u8Counter++;
    }
}






void ISR(void)      /* Receive ISR routine */
{

    RESPONSE_BUFFER[Counter] = UARTCharGetNonBlocking(UART5_BASE);  /* Copy data to buffer & increment counter */
    Counter++;
    if(Counter == DEFAULT_BUFFER_SIZE)
        Counter = 0;

}



void delayMs(uint32_t ui32Ms) {

    // 1 clock cycle = 1 / SysCtlClockGet() second
    // 1 SysCtlDelay = 3 clock cycle = 3 / SysCtlClockGet() second
    // 1 second = SysCtlClockGet() / 3
    // 0.001 second = 1 ms = SysCtlClockGet() / 3 / 1000

    SysCtlDelay(ui32Ms * (SysCtlClockGet() / 3.0/ 1000.0));
}



void GSM_Send_Msg(char *num,char *sms)
{
    char sms_buffer[35];
    buffer_pointer=0;
    sprintf(sms_buffer,"AT+CMGS=\"%s\"\r",num);
    USART_SendString(sms_buffer); /*send command AT+CMGS="Mobile No."\r */
    _delay_ms(200);
    while(1)
    {
        if(buff[buffer_pointer]==0x3e) /* wait for '>' character*/
        {
           buffer_pointer = 0;
           memset(buff,0,strlen(buff));
           USART_SendString(sms); /* send msg to given no. */
           USART_TxChar(0x1a); /* send Ctrl+Z */
           break;
        }
        buffer_pointer++;
    }
    _delay_ms(300);
    buffer_pointer = 0;
    memset(buff,0,strlen(buff));
    memset(sms_buffer,0,strlen(sms_buffer));
}














