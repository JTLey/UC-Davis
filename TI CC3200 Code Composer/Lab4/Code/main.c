//*****************************************************************************
// Copyright (C) 2014 Texas Instruments Incorporated
//
// All rights reserved. Property of Texas Instruments Incorporated.
// Restricted rights to use, duplicate or disclose this code are
// granted through contract.
// The program may not be used without the written permission of
// Texas Instruments Incorporated or against the terms and conditions
// stipulated in the agreement under which this program has been supplied,
// and under no circumstances can it be used with non-TI connectivity device.
//
//*****************************************************************************

//*****************************************************************************
//
// Application Name     -   MQTT Client
// Application Overview -   This application acts as a MQTT client and connects
//                          to the IBM MQTT broker, simultaneously we can
//                          connect a web client from a web browser. Both
//                          clients can inter-communicate using appropriate
//                          topic names.
//
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_MQTT_Client
// or
// docs\examples\CC32xx_MQTT_Client.pdf
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup mqtt_client
//! @{
//
//*****************************************************************************

// Standard includes
#include <stdlib.h>
#include <ctype.h>

// simplelink includes
#include "simplelink.h"

// driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "interrupt.h"
#include "rom_map.h"
#include "prcm.h"
#include "uart.h"
#include "timer.h"

// common interface includes
#include "network_if.h"
#ifndef NOTERM
#include "uart_if.h"
#endif

// Common interface includes (LAB 3)
#include "pin_mux_config.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h" // Adafruit_Init();
#include "test.h" // test functions
#include "spi.h"
#include "gpio.h"
#include "systick.h"


#include "button_if.h"
#include "gpio_if.h"
#include "timer_if.h"
#include "common.h"
#include "utils.h"


#include "sl_mqtt_client.h"

// application specific includes
// #include "pinmux.h"

#define APPLICATION_VERSION 	"1.1.1"

/*Operate Lib in MQTT 3.1 mode.*/
#define MQTT_3_1_1              false /*MQTT 3.1.1 */
#define MQTT_3_1                true /*MQTT 3.1*/

#define WILL_TOPIC              "Client"
#define WILL_MSG                "Client Stopped"
#define WILL_QOS                QOS1
#define WILL_RETAIN             false

// From Lab 3

#define TR_BUFF_SIZE     100

#define MESSAGE_SIZE 30
#define TEXT_DELAY 7
#define INVALID_DATA 255

/*Defining Broker IP address and port Number*/
//#define SERVER_ADDRESS          "messagesight.demos.ibm.com"
// Enter your AWS Endpoint address as the SERVER ADDRESS
#define SERVER_ADDRESS          "A124RVFMLLIY63.iot.us-west-2.amazonaws.com"
//#define PORT_NUMBER             1883
#define PORT_NUMBER             8883

#define MAX_BROKER_CONN         1

#define SERVER_MODE             MQTT_3_1_1
/*Specifying Receive time out for the Receive task*//*Specifying Receive time out for the Receive task*/
#define RCV_TIMEOUT             30

/*Background receive task priority*/
#define TASK_PRIORITY           3

/* Keep Alive Timer value*/
#define KEEP_ALIVE_TIMER        25

/*Clean session flag*/
#define CLEAN_SESSION           true

/*Retain Flag. Used in publish message. */
#define RETAIN                  0

/*Defining Publish Topic*/
#define PUB_TOPIC_FOR_SW3       "/cc3200/ButtonPressEvtSw3"
#define PUB_TOPIC_FOR_SW2       "/cc3200/ButtonPressEvtSw2"

#define SEND_TOPIC				"/cc3200/SendData"

/*Defining Number of topics*/
#define TOPIC_COUNT             3

/*Defining Subscription Topic Values*/
#define TOPIC1                  "/cc3200/ToggleLEDCmdL1"
#define TOPIC2                  "/cc3200/ToggleLEDCmdL2"
#define TOPIC3                  "/cc3200/ToggleLEDCmdL3"

#define TOPIC_RECEIVE			"/cc3200/ReceiveData"

/*Defining QOS levels*/
#define QOS0                    0
#define QOS1                    1
#define QOS2                    2

/*Spawn task priority and OSI Stack Size*/
#define OSI_STACK_SIZE          2048
#define UART_PRINT              Report

//NEED TO UPDATE THIS FOR IT TO WORK!
#define DATE                05   /* Current Date */
#define MONTH               02     /* Month 1-12 */
#define YEAR                2016  /* Current year */
#define HOUR                17    /* Time - hours */
#define MINUTE              0    /* Time - minutes */
#define SECOND              0     /* Time - seconds */

// SPI macros
#define SPI_IF_BIT_RATE  100000

typedef struct
{
   /* time */
   unsigned long tm_sec;
   unsigned long tm_min;
   unsigned long tm_hour;
   /* date */
   unsigned long tm_day;
   unsigned long tm_mon;
   unsigned long tm_year;
   unsigned long tm_week_day; //not required
   unsigned long tm_year_day; //not required
   unsigned long reserved[3];
}SlDateTime;

typedef struct connection_config{
    SlMqttClientCtxCfg_t broker_config;
    void *clt_ctx;
    unsigned char *client_id;
    unsigned char *usr_name;
    unsigned char *usr_pwd;
    bool is_clean;
    unsigned int keep_alive_time;
    SlMqttClientCbs_t CallBAcks;
    int num_topics;
    char *topic[TOPIC_COUNT];
    unsigned char qos[TOPIC_COUNT];
    SlMqttWill_t will_params;
    bool is_connected;
}connect_config;

typedef enum events
{
    PUSH_BUTTON_SW2_PRESSED,
    PUSH_BUTTON_SW3_PRESSED,
    BROKER_DISCONNECTION
}osi_messages;

//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void
Mqtt_Recv(void *app_hndl, const char  *topstr, long top_len, const void *payload,
          long pay_len, bool dup,unsigned char qos, bool retain);
static void sl_MqttEvt(void *app_hndl,long evt, const void *buf,
                       unsigned long len);
static void sl_MqttDisconnect(void *app_hndl);
void pushButtonInterruptHandler2();
void pushButtonInterruptHandler3();
void ToggleLedState(ledEnum LedNum);
void TimerPeriodicIntHandler(void);
void LedTimerConfigNStart();
void LedTimerDeinitStop();
void BoardInit(void);
static void DisplayBanner(char * AppName);
void MqttClient(void *pvParameters);

static int set_time();


void DrawNextChar(int i);
void runCommand(unsigned char data, char *sending, int *i);
char nextchar(char c);
unsigned char readdata();
int getColor();
void WriteLoop();
void DrawString(char* str, int x, int y);
//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

// timer globals
static volatile unsigned long g_ulBase = TIMERA0_BASE;
int clkflag = 0;
int buttonflag=0;
int lastpush = 0;

char sending[MESSAGE_SIZE + 1];
int readpos = 0; //what character to edit with remote
int writepos = 0; //what character to type on screen
int cursize = 0; //current size of write buffer

int sendMessage = 0; // was OK pressed?

#ifdef USE_FREERTOS
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#endif

char *security_file_list[] = {"/cert/private.der", "/cert/client.der", "/cert/rootCA.der"};  //Order: Private Key, Certificate File, CA File, DH Key (N/A)
SlDateTime g_time;

unsigned short g_usTimerInts;
/* AP Security Parameters */
SlSecParams_t SecurityParams = {0};

/*Message Queue*/
OsiMsgQ_t g_PBQueue;

/* connection configuration */
connect_config usr_connect_config[] =
{
    {
        {
            {
                (SL_MQTT_NETCONN_URL|SL_MQTT_NETCONN_SEC),
                SERVER_ADDRESS,
                PORT_NUMBER,
				SL_SO_SEC_METHOD_TLSV1_2,	//Method (TLS1.2)
				SL_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA,	//Cipher
				3,							//number of files
				security_file_list 			//name of files
            },
            SERVER_MODE,
            true,
        },
        NULL,
        "asdpioagdjip",
        NULL,
        NULL,
        true,
        KEEP_ALIVE_TIMER,
        {Mqtt_Recv, sl_MqttEvt, sl_MqttDisconnect},
        TOPIC_COUNT,
        {TOPIC1, TOPIC2, TOPIC_RECEIVE},
        {QOS1, QOS1, QOS1},
        /*{WILL_TOPIC,WILL_MSG,WILL_QOS,WILL_RETAIN},*/
		NULL,
        false
    }
};

/* library configuration */
SlMqttClientLibCfg_t Mqtt_Client={
    1882,
    TASK_PRIORITY,
    30,
    true,
    (long(*)(const char *, ...))UART_PRINT
};

/*Publishing topics and messages*/
const char *pub_topic_sw2 = PUB_TOPIC_FOR_SW2;
const char *pub_topic_sw3 = PUB_TOPIC_FOR_SW3;
const char *pub_topic_send = SEND_TOPIC;
unsigned char *data_sw2={"Push button sw2 is pressed on CC32XX device"};
unsigned char *data_sw3={"Push button sw3 is pressed on CC32XX device"};
unsigned char *data_send={"Testing"};

void *app_hndl = (void*)usr_connect_config;
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//****************************************************************************
//! Defines Mqtt_Pub_Message_Receive event handler.
//! Client App needs to register this event handler with sl_ExtLib_mqtt_Init 
//! API. Background receive task invokes this handler whenever MQTT Client 
//! receives a Publish Message from the broker.
//!
//!\param[out]     topstr => pointer to topic of the message
//!\param[out]     top_len => topic length
//!\param[out]     payload => pointer to payload
//!\param[out]     pay_len => payload length
//!\param[out]     retain => Tells whether its a Retained message or not
//!\param[out]     dup => Tells whether its a duplicate message or not
//!\param[out]     qos => Tells the Qos level
//!
//!\return none
//****************************************************************************
static void
Mqtt_Recv(void *app_hndl, const char  *topstr, long top_len, const void *payload,
                       long pay_len, bool dup,unsigned char qos, bool retain)
{
    char *output_str=(char*)malloc(top_len+1);
    memset(output_str,'\0',top_len+1);
    strncpy(output_str, (char*)topstr, top_len);
    output_str[top_len]='\0';


    if(strncmp(output_str,TOPIC_RECEIVE, top_len) == 0)
    {
    	Message("Got it\r\n");
    }
    else if(strncmp(output_str,TOPIC1, top_len) == 0)
    {
        ToggleLedState(LED1);
    }
    else if(strncmp(output_str,TOPIC2, top_len) == 0)
    {
        ToggleLedState(LED2);
    }
    else if(strncmp(output_str,TOPIC3, top_len) == 0)
    {
        ToggleLedState(LED3);
    }

    UART_PRINT("\n\rPublish Message Received");
    UART_PRINT("\n\rTopic: ");
    UART_PRINT("%s",output_str);
    free(output_str);
    UART_PRINT(" [Qos: %d] ",qos);
    if(retain)
      UART_PRINT(" [Retained]");
    if(dup)
      UART_PRINT(" [Duplicate]");
    
    output_str=(char*)malloc(pay_len+1);
    memset(output_str,'\0',pay_len+1);
    strncpy(output_str, (char*)payload, pay_len);
    output_str[pay_len]='\0';
    UART_PRINT("\n\rData is: ");
    UART_PRINT("%s",(char*)output_str);
    UART_PRINT("\n\r");

    DrawString(output_str, 5, 50);
    free(output_str);
    
    return;
}

//****************************************************************************
//! Defines sl_MqttEvt event handler.
//! Client App needs to register this event handler with sl_ExtLib_mqtt_Init 
//! API. Background receive task invokes this handler whenever MQTT Client 
//! receives an ack(whenever user is in non-blocking mode) or encounters an error.
//!
//! param[out]      evt => Event that invokes the handler. Event can be of the
//!                        following types:
//!                        MQTT_ACK - Ack Received 
//!                        MQTT_ERROR - unknown error
//!                        
//!  
//! \param[out]     buf => points to buffer
//! \param[out]     len => buffer length
//!       
//! \return none
//****************************************************************************
static void
sl_MqttEvt(void *app_hndl, long evt, const void *buf,unsigned long len)
{
    int i;
    switch(evt)
    {
      case SL_MQTT_CL_EVT_PUBACK:
        UART_PRINT("PubAck:\n\r");
        UART_PRINT("%s\n\r",buf);
        break;
    
      case SL_MQTT_CL_EVT_SUBACK:
        UART_PRINT("\n\rGranted QoS Levels are:\n\r");
        
        for(i=0;i<len;i++)
        {
          UART_PRINT("QoS %d\n\r",((unsigned char*)buf)[i]);
        }
        break;
        
      case SL_MQTT_CL_EVT_UNSUBACK:
        UART_PRINT("UnSub Ack \n\r");
        UART_PRINT("%s\n\r",buf);
        break;
    
      default:
        break;
  
    }
}

//****************************************************************************
//
//! callback event in case of MQTT disconnection
//!
//! \param app_hndl is the handle for the disconnected connection
//!
//! return none
//
//****************************************************************************
static void
sl_MqttDisconnect(void *app_hndl)
{
    connect_config *local_con_conf;
    osi_messages var = BROKER_DISCONNECTION;
    local_con_conf = app_hndl;
    sl_ExtLib_MqttClientUnsub(local_con_conf->clt_ctx, local_con_conf->topic,
                              TOPIC_COUNT);
    UART_PRINT("disconnect from broker %s\r\n",
           (local_con_conf->broker_config).server_info.server_addr);
    local_con_conf->is_connected = false;
    sl_ExtLib_MqttClientCtxDelete(local_con_conf->clt_ctx);

    //
    // write message indicating publish message
    //
    osi_MsgQWrite(&g_PBQueue,&var,OSI_NO_WAIT);

}

//****************************************************************************
//
//! Push Button Handler1(GPIOS2). Press push button2 (GPIOSW2) Whenever user
//! wants to publish a message. Write message into message queue signaling the
//!    event publish messages
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void pushButtonInterruptHandler2()
{
    osi_messages var = PUSH_BUTTON_SW2_PRESSED;
    //
    // write message indicating publish message
    //
    osi_MsgQWrite(&g_PBQueue,&var,OSI_NO_WAIT);
}

//****************************************************************************
//
//! Push Button Handler3(GPIOS3). Press push button3 (GPIOSW3) Whenever user
//! wants to publish a message. Write message into message queue signaling the
//!    event publish messages
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void pushButtonInterruptHandler3()
{
    osi_messages var = PUSH_BUTTON_SW3_PRESSED;
    //
    // write message indicating exit from sending loop
    //
    osi_MsgQWrite(&g_PBQueue,&var,OSI_NO_WAIT);

}

//****************************************************************************
//
//!    Toggles the state of GPIOs(LEDs)
//!
//! \param LedNum is the enumeration for the GPIO to be toggled
//!
//!    \return none
//
//****************************************************************************
void ToggleLedState(ledEnum LedNum)
{
    unsigned char ledstate = 0;
    switch(LedNum)
    {
    case LED1:
        ledstate = GPIO_IF_LedStatus(MCU_RED_LED_GPIO);
        if(!ledstate)
        {
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        }
        else
        {
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        }
        break;
    case LED2:
        ledstate = GPIO_IF_LedStatus(MCU_ORANGE_LED_GPIO);
        if(!ledstate)
        {
            GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        }
        else
        {
            GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }
        break;
    case LED3:
        ledstate = GPIO_IF_LedStatus(MCU_GREEN_LED_GPIO);
        if(!ledstate)
        {
            GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        }
        else
        {
            GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        }
        break;
    default:
        break;
    }
}

//*****************************************************************************
//
//! Periodic Timer Interrupt Handler
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void
TimerPeriodicIntHandler(void)
{
    unsigned long ulInts;

    //
    // Clear all pending interrupts from the timer we are
    // currently using.
    //
    ulInts = MAP_TimerIntStatus(TIMERA0_BASE, true);
    MAP_TimerIntClear(TIMERA0_BASE, ulInts);

    //
    // Increment our interrupt counter.
    //
    g_usTimerInts++;
    if(!(g_usTimerInts & 0x1))
    {
        //
        // Off Led
        //
        GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    }
    else
    {
        //
        // On Led
        //
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
    }
}

//****************************************************************************
//
//! Function to configure and start timer to blink the LED while device is
//! trying to connect to an AP
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void LedTimerConfigNStart()
{
    //
    // Configure Timer for blinking the LED for IP acquisition
    //
//    Timer_IF_Init(PRCM_TIMERA0,TIMERA0_BASE,TIMER_CFG_PERIODIC,TIMER_A,0);
//    Timer_IF_IntSetup(TIMERA0_BASE,TIMER_A,TimerPeriodicIntHandler);
//    Timer_IF_Start(TIMERA0_BASE,TIMER_A,100);
}

//****************************************************************************
//
//! Disable the LED blinking Timer as Device is connected to AP
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void LedTimerDeinitStop()
{
    //
    // Disable the LED blinking Timer as Device is connected to AP
    //
//    Timer_IF_Stop(TIMERA0_BASE,TIMER_A);
//    Timer_IF_DeInit(TIMERA0_BASE,TIMER_A);

}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void BoardInit(void)
{
    /* In case of TI-RTOS vector table is initialize by OS itself */
    #ifndef USE_TIRTOS
    //
    // Set vector table base
    //
    #if defined(ccs)
        IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
    #endif
    #if defined(ewarm)
        IntVTableBaseSet((unsigned long)&__vector_table);
    #endif
    #endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void
DisplayBanner(char * AppName)
{

    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\t\t    CC3200 %s Application       \n\r", AppName);
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\n\n\n\r");
}
  
//*****************************************************************************
//
//! Task implementing MQTT client communication to other web client through
//!    a broker
//!
//! \param  none
//!
//! This function
//!    1. Initializes network driver and connects to the default AP
//!    2. Initializes the mqtt library and set up MQTT connection configurations
//!    3. set up the button events and their callbacks(for publishing)
//!    4. handles the callback signals
//!
//! \return None
//!
//*****************************************************************************
void MqttClient(void *pvParameters)
{
    
    long lRetVal = -1;
    int iCount = 0;
    int iNumBroker = 0;
    int iConnBroker = 0;
    
    connect_config *local_con_conf = (connect_config *)app_hndl;

    //
    // Configure LED
    //
    GPIO_IF_LedConfigure(LED1|LED2|LED3);

    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);

    //
    // Reset The state of the machine
    //
    Network_IF_ResetMCUStateMachine();

    //
    // Start the driver
    //
    lRetVal = Network_IF_InitDriver(ROLE_STA);
    if(lRetVal < 0)
    {
       UART_PRINT("Failed to start SimpleLink Device\n\r",lRetVal);
       LOOP_FOREVER();
    }

    // switch on Green LED to indicate Simplelink is properly up
    GPIO_IF_LedOn(MCU_ON_IND);

    // Start Timer to blink Red LED till AP connection
    LedTimerConfigNStart();

    // Initialize AP security params
    SecurityParams.Key = (signed char *)SECURITY_KEY;
    SecurityParams.KeyLen = strlen(SECURITY_KEY);
    SecurityParams.Type = SECURITY_TYPE;

    //
    // Connect to the Access Point
    //
    lRetVal = Network_IF_ConnectAP(SSID_NAME, SecurityParams);
    if(lRetVal < 0)
    {
       UART_PRINT("Connection to an AP failed\n\r");
       LOOP_FOREVER();
    }

    //
    // Disable the LED blinking Timer as Device is connected to AP
    //
    LedTimerDeinitStop();

    //
    // Switch ON RED LED to indicate that Device acquired an IP
    //
    GPIO_IF_LedOn(MCU_IP_ALLOC_IND);

    set_time();
     if(lRetVal < 0)
         {
             UART_PRINT("Unable to set time in the device");
     }

    UtilsDelay(20000000);

    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
   
    //
    // Register Push Button Handlers
    //
    Button_IF_Init(pushButtonInterruptHandler2,pushButtonInterruptHandler3);
    
    //
    // Initialze MQTT client lib
    //
    lRetVal = sl_ExtLib_MqttClientInit(&Mqtt_Client);
    if(lRetVal != 0)
    {
        // lib initialization failed
        UART_PRINT("MQTT Client lib initialization failed\n\r");
        LOOP_FOREVER();
    }
    
    /******************* connection to the broker ***************************/
    iNumBroker = sizeof(usr_connect_config)/sizeof(connect_config);
    if(iNumBroker > MAX_BROKER_CONN)
    {
        UART_PRINT("Num of brokers are more then max num of brokers\n\r");
        LOOP_FOREVER();
    }

    while(iCount < iNumBroker)
    {
        //create client context
        local_con_conf[iCount].clt_ctx =
        sl_ExtLib_MqttClientCtxCreate(&local_con_conf[iCount].broker_config,
                                      &local_con_conf[iCount].CallBAcks,
                                      &(local_con_conf[iCount]));

        //
        // Set Client ID
        //
        sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                            SL_MQTT_PARAM_CLIENT_ID,
                            local_con_conf[iCount].client_id,
                            strlen((char*)(local_con_conf[iCount].client_id)));

        //
        // Set will Params
        //
        if(local_con_conf[iCount].will_params.will_topic != NULL)
        {
            sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                    SL_MQTT_PARAM_WILL_PARAM,
                                    &(local_con_conf[iCount].will_params),
                                    sizeof(SlMqttWill_t));
        }

        //
        // setting username and password
        //
        if(local_con_conf[iCount].usr_name != NULL)
        {
            sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                SL_MQTT_PARAM_USER_NAME,
                                local_con_conf[iCount].usr_name,
                                strlen((char*)local_con_conf[iCount].usr_name));

            if(local_con_conf[iCount].usr_pwd != NULL)
            {
                sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                SL_MQTT_PARAM_PASS_WORD,
                                local_con_conf[iCount].usr_pwd,
                                strlen((char*)local_con_conf[iCount].usr_pwd));
            }
        }

        //
        // connectin to the broker
        //
        if((sl_ExtLib_MqttClientConnect((void*)local_con_conf[iCount].clt_ctx,
                            local_con_conf[iCount].is_clean,
                            local_con_conf[iCount].keep_alive_time) & 0xFF) != 0)
        {
            UART_PRINT("\n\rBroker connect fail for conn no. %d \n\r",iCount+1);
            
            //delete the context for this connection
            sl_ExtLib_MqttClientCtxDelete(local_con_conf[iCount].clt_ctx);
            
            break;
        }
        else
        {
            UART_PRINT("\n\rSuccess: conn to Broker no. %d\n\r ", iCount+1);
            local_con_conf[iCount].is_connected = true;
            iConnBroker++;
        }

        //
        // Subscribe to topics
        //

        if(sl_ExtLib_MqttClientSub((void*)local_con_conf[iCount].clt_ctx,
                                   local_con_conf[iCount].topic,
                                   local_con_conf[iCount].qos, TOPIC_COUNT) < 0)
        {
            UART_PRINT("\n\r Subscription Error for conn no. %d\n\r", iCount+1);
            UART_PRINT("Disconnecting from the broker\r\n");
            sl_ExtLib_MqttClientDisconnect(local_con_conf[iCount].clt_ctx);
            local_con_conf[iCount].is_connected = false;
            
            //delete the context for this connection
            sl_ExtLib_MqttClientCtxDelete(local_con_conf[iCount].clt_ctx);
            iConnBroker--;
            break;
        }
        else
        {
            int iSub;
            UART_PRINT("Client subscribed on following topics:\n\r");
            for(iSub = 0; iSub < local_con_conf[iCount].num_topics; iSub++)
            {
                UART_PRINT("%s\n\r", local_con_conf[iCount].topic[iSub]);
            }
        }
        iCount++;
    }

    if(iConnBroker < 1)
    {
        //
        // no succesful connection to broker
        //
        goto end;
    }

    iCount = 0;

    unsigned char data;
	unsigned char prevData = INVALID_DATA;
	int i = -1;
	while(1)
	{

		if(sendMessage)
		{
			sendMessage = 0;
			sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
			pub_topic_send,sending,strlen((char*)sending),QOS1,RETAIN);

			sending[0] = '\0';
			i = -1;
			DrawString("                      ", 5, 5); // clear line
		}

		if (buttonflag)
		{
			data = readdata(); //read in the data
			if (data != INVALID_DATA)
			{
				if(data < 10) // data
				{
					if(data != prevData || lastpush > TEXT_DELAY) // different button
					{
						i = (i + 1) % 20;
						sending[i] = data + '0'; // put in first character
						sending[i+1] = '\0';
					}
					else
					{
						sending[i] = nextchar(sending[i]);
						sending[i+1] = '\0';
					}
				}
				else //command
				{
					runCommand(data, sending, &i); // runCommand may modify 'i'
				}
				lastpush = 0;
				prevData = data;
				Report("%s\r\n", sending);
				DrawNextChar(i);
			}

			GPIOIntEnable(GPIOA0_BASE, GPIO_INT_PIN_3); //reenable interrupt
			buttonflag = 0; //turn off flag
		}

	}

    //for(;;)
    //{
    //	Message("Hello!\r\n");

    	//WriteLoop();
        /*osi_MsgQRead( &g_PBQueue, &RecvQue, OSI_WAIT_FOREVER);
        
        if(PUSH_BUTTON_SW2_PRESSED == RecvQue)
        {
            Button_IF_EnableInterrupt(SW2);
            //
            // send publish message
            //
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                    pub_topic_sw2,data_sw2,strlen((char*)data_sw2),QOS1,RETAIN);
            UART_PRINT("\n\r CC3200 Publishes the following message \n\r");
            UART_PRINT("Topic: %s\n\r",pub_topic_sw2);
            UART_PRINT("Data: %s\n\r",data_sw2);
        }
        else if(PUSH_BUTTON_SW3_PRESSED == RecvQue)
        {
            Button_IF_EnableInterrupt(SW3);
            //
            // send publish message
            //
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                    pub_topic_sw3,data_sw3,strlen((char*)data_sw3),QOS1,RETAIN);
            UART_PRINT("\n\r CC3200 Publishes the following message \n\r");
            UART_PRINT("Topic: %s\n\r",pub_topic_sw3);
            UART_PRINT("Data: %s\n\r",data_sw3);
        }
        else if(BROKER_DISCONNECTION == RecvQue)
        {
            iConnBroker--;
            if(iConnBroker < 1)
            {
                //
                // device not connected to any broker
                //
                goto end;
            }
        } */
    //}
end:
    //
    // Deinitializating the client library
    //
    sl_ExtLib_MqttClientExit();
    UART_PRINT("\n\r Exiting the Application\n\r");
    
    LOOP_FOREVER();
}

void
SPIInit()
{
    //
    // Reset SPI
    //
    MAP_SPIReset(GSPI_BASE);

    //
    // Set up SPI FIFO
    //
    MAP_SPIFIFOEnable(GSPI_BASE, SPI_TX_FIFO | SPI_RX_FIFO);

    //
    // Configure SPI interface
    //
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                     SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_3, // Inverse clock (1 on idle), Sampled on 2nd (rising) edge.
                     (SPI_SW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVELOW | // 0 means Write, 1 means don't write
                     SPI_WL_8));

    //
    // Enable SPI for communication
    //
    MAP_SPIEnable(GSPI_BASE);
}

void
DrawString(char *s, int x, int y)
{
    int i;
    for(i = 0; i < strlen(s); i++)
    {
        drawChar(x + i*6, y, s[i], WHITE, BLACK, 1);
    }
}

void
DrawFontTable()
{
    int numchars = 127;
    int width = 128;
    int height = 128;
    int x, y;
    char i = 0;
    while(i < numchars)
    {
        for(y = 0; y < height; y += height/10)
        {
            for(x = 0; x < width; x += width / 10)
            {
                drawChar(x, y, i, WHITE, BLACK, 1);
                i++;
            }
        }
    }
}

void
testloop()
{
    while(1)
    {
            fillScreen(BLACK);
            DrawFontTable();
            MAP_UtilsDelay(20000000);
            fillScreen(BLACK);
            DrawString("Hello World!", 5, 5);
            MAP_UtilsDelay(20000000);
            lcdTestPattern();
            MAP_UtilsDelay(10000000);
            lcdTestPattern2();
            MAP_UtilsDelay(10000000);
            fillScreen(BLACK);
            testlines(BLUE); // takes too long
            fillScreen(BLACK);
            testfastlines(RED, BLUE);
            MAP_UtilsDelay(20000000);
            fillScreen(BLACK);
            testdrawrects(CYAN);
            MAP_UtilsDelay(20000000);
            fillScreen(BLACK);
            testfillrects(GREEN, MAGENTA);
            fillScreen(BLACK);
            testfillcircles(20, MAGENTA);
            MAP_UtilsDelay(20000000);
            fillScreen(BLACK);
            testdrawcircles(20, YELLOW);
            MAP_UtilsDelay(20000000);
            fillScreen(BLACK);
            testroundrects();
            MAP_UtilsDelay(20000000);
            fillScreen(BLACK);
            testtriangles();
            MAP_UtilsDelay(10000000);
    }
}

void DrawNextChar(int i)
{
	drawChar(5 + 6*i , 5, sending[i], getColor(), BLACK, 1); //write next char
}

void GPIOHandler()
{
	GPIOIntClear(GPIOA0_BASE, GPIO_INT_PIN_3);
	buttonflag = 1;
}

// Lets us know the clock wrapped
void
TimerBaseIntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    Timer_IF_InterruptClear(g_ulBase);
    clkflag = 1;
    lastpush++;
}

int getbit()
{
    unsigned long time;
    while((GPIOPinRead(GPIOA0_BASE, 0x8))!=0x8){/*do nothing*/}
    time = TimerValueGet(g_ulBase, TIMER_A);
    clkflag = 0;
    while(GPIOPinRead(GPIOA0_BASE, 0x8) == 0x8){if (250000<(time - (TimerValueGet(g_ulBase, TIMER_A) - (clkflag ? 16777216 : 0)))) return 0;}
    time = time - (TimerValueGet(g_ulBase, TIMER_A) - (clkflag ? 16777216 : 0));
    if (time < 80000){return 0;}
    else return 1;
}

unsigned char readdata()
{
		unsigned long time = 0;
	    unsigned char data, data1;
	    int valid = 1;

        while(GPIOPinRead(GPIOA0_BASE, 0x8) != 0x8);

        time = TimerValueGet(g_ulBase, TIMER_A);
        clkflag = 0;

        while(GPIOPinRead(GPIOA0_BASE, 0x8) == 0x8);

        time = time - (TimerValueGet(g_ulBase, TIMER_A) - (clkflag ? 16777216 : 0));
        if ((time < 300000) || (time > 450000)){valid = 0;}
        if (getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (!getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (!getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (getbit()){valid = 0;}
        if (!getbit()){valid = 0;}
        if (!getbit()){valid = 0;}
        if (!getbit()){valid = 0;}
        data = 0;
        data |= (getbit()) << 0;
        data |= (getbit()) << 1;
        data |= (getbit()) << 2;
        data |= (getbit()) << 3;
        data |= (getbit()) << 4;
        data |= (getbit()) << 5;
        data |= (getbit()) << 6;
        data |= (getbit()) << 7;
        data1 = 0;
        data1 |= (getbit()) << 0;
        data1 |= (getbit()) << 1;
        data1 |= (getbit()) << 2;
        data1 |= (getbit()) << 3;
        data1 |= (getbit()) << 4;
        data1 |= (getbit()) << 5;
        data1 |= (getbit()) << 6;
        data1 |= (getbit()) << 7;

        return valid ? data : INVALID_DATA;
}

char nextchar(char c)
{
	char next = INVALID_DATA;

	switch (c)
	{
		case '0': //ascii translation
			next = ' ';
			break;
		case ' ':
			next = '0';
			break;

		case '1':
			next = '1';
			break;

		case '2':
			next = 'a';
			break;
		case 'a':
			next = 'b';
			break;
		case 'b':
			next = 'c';
			break;
		case 'c':
			next = '2';
			break;

		case '3':
			next = 'd';
			break;
		case 'd':
			next = 'e';
			break;
		case 'e':
			next = 'f';
			break;
		case 'f':
			next = '3';
			break;

		case '4':
			next = 'g';
			break;
		case 'g':
			next = 'h';
			break;
		case 'h':
			next = 'i';
			break;
		case 'i':
			next = '4';
			break;

		case '5':
			next = 'j';
			break;
		case 'j':
			next = 'k';
			break;
		case 'k':
			next = 'l';
			break;
		case 'l':
			next = '5';
			break;

		case '6':
			next = 'm';
			break;
		case 'm':
			next = 'n';
			break;
		case 'n':
			next = 'o';
			break;
		case 'o':
			next = '6';
			break;

		case '7':
			next = 'p';
			break;
		case 'p':
			next = 'q';
			break;
		case 'q':
			next = 'r';
			break;
		case 'r':
			next = 's';
			break;
		case 's':
			next = '7';
			break;

		case '8':
			next = 't';
			break;
		case 't':
			next = 'u';
			break;
		case 'u':
			next = 'v';
			break;
		case 'v':
			next = '8';
			break;

		case '9':
			next = 'w';
			break;
		case 'w':
			next = 'x';
			break;
		case 'x':
			next = 'y';
			break;
		case 'y':
			next = 'z';
			break;
		case 'z':
			next = '9';
			break;
	}
	return next;
}

static int g_channelColors[] = { WHITE, RED, BLUE, GREEN, YELLOW, CYAN, MAGENTA };
static int g_colorIndex = 0;

int getColor()
{
	return g_channelColors[g_colorIndex];
}

// sends, capitalizes, erases, etc
// The index 'i' may be modified by this function
void
runCommand(unsigned char data, char *sending, int *i)
{


	switch(data)
	{
	case 80: // channel +. change color
		g_colorIndex = (g_colorIndex + 1) % 7;
		break;
	case 81: // channel -. change color
		g_colorIndex = (g_colorIndex + 6) % 7;
		break;
	case 115: // up arrow
	case 96: // vol+. capitalize
		sending[*i] = toupper(sending[*i]);
		DrawNextChar(*i);
		break;
	case 116: // down arrow
	case 97: // vol-. lowercase
		sending[*i] = tolower(sending[*i]);
		DrawNextChar(*i);
		break;
	case 117: // right arrow. shortcut for space?
		break;
	case 119: // OK. send message over UART
		//Message2(sending);
		Message("Not implemented\r\n");
		sendMessage = 1;
		break;
	case 118: // left arrow
		{
			sending[*i] = ' ';
			DrawNextChar(*i); // hack to erase characters from the LED
			sending[*i] = '\0'; // so it's a valid string
			(*i)--;
			if(*i < -1)
				*i = -1;
		}
	default:
		break;
	}
}

void WriteLoop()
{
	unsigned char data;
	unsigned char prevData = INVALID_DATA;
	int i = -1;
	while(1)
	{
		if (buttonflag)
		{
			data = readdata(); //read in the data
			if (data != INVALID_DATA)
			{
				if(data < 10) // data
				{
					if(data != prevData || lastpush > TEXT_DELAY) // different button
					{
						i = (i + 1) % 20;
						sending[i] = data + '0'; // put in first character
						sending[i+1] = '\0';
					}
					else
					{
						sending[i] = nextchar(sending[i]);
						sending[i+1] = '\0';
					}
				}
				else //command
				{
					runCommand(data, sending, &i); // runCommand may modify 'i'
				}
				lastpush = 0;
				prevData = data;
				Report("%s\r\n", sending);
				DrawNextChar(i);
			}

			GPIOIntEnable(GPIOA0_BASE, GPIO_INT_PIN_3); //reenable interrupt
			buttonflag = 0; //turn off flag
		}

	}
}

//*****************************************************************************
//
//! Main 
//!
//! \param  none
//!
//! This function
//!    1. Invokes the SLHost task
//!    2. Invokes the MqttClient
//!
//! \return None
//!
//*****************************************************************************
void main()
{ 
    long lRetVal = -1;
    //
    // Initialize the board configurations
    //
    BoardInit();

    //
    // Pinmux for UART
    //
    PinMuxConfig();

    //
    // Configuring UART
    //
    InitTerm();

    //
    // Display Application Banner
    //
    DisplayBanner("MQTT_Client");

    // Set up and turn on a generic timer
    Timer_IF_Init(PRCM_TIMERA0, g_ulBase, TIMER_CFG_PERIODIC, TIMER_A, 0);
    Timer_IF_IntSetup(g_ulBase, TIMER_A, TimerBaseIntHandler);
    Timer_IF_Start(g_ulBase, TIMER_A, 16777216);


	GPIOIntTypeSet(GPIOA0_BASE, 0x8, GPIO_FALLING_EDGE);
	GPIOIntRegister(GPIOA0_BASE, GPIOHandler);
	GPIOIntEnable(GPIOA0_BASE, GPIO_INT_PIN_3);

    // Adafruit stuff
    SPIInit();
    Adafruit_Init();
    //testloop();
    fillScreen(BLACK);

    //
    // Start the SimpleLink Host
    //
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    //
    // Start the MQTT Client task
    //
    osi_MsgQCreate(&g_PBQueue,"PBQueue",sizeof(osi_messages),10);
    lRetVal = osi_TaskCreate(MqttClient,
                            (const signed char *)"Mqtt Client App",
                            OSI_STACK_SIZE, NULL, 2, NULL );

    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
    //
    // Start the task scheduler
    //
    osi_start();
}

//*****************************************************************************
//
//! This function updates the date and time of CC3200.
//!
//! \param None
//!
//! \return
//!     0 for success, negative otherwise
//!
//*****************************************************************************

static int set_time()
{
    long retVal;

    g_time.tm_day = DATE;
    g_time.tm_mon = MONTH;
    g_time.tm_year = YEAR;
    g_time.tm_sec = HOUR;
    g_time.tm_hour = MINUTE;
    g_time.tm_min = SECOND;

    retVal = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
                          SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
                          sizeof(SlDateTime),(unsigned char *)(&g_time));

    ASSERT_ON_ERROR(retVal);
    return SUCCESS;
}
