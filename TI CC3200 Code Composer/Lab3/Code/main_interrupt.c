// Lab 2, Part 1
// Mazar Farran and Jacob Ley


//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// Application Name     - SPI Demo
// Application Overview - The demo application focuses on showing the required 
//                        initialization sequence to enable the CC3200 SPI 
//                        module in full duplex 4-wire master and slave mode(s).
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_SPI_Demo
// or
// docs\examples\CC32xx_SPI_Demo.pdf
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup SPI_Demo
//! @{
//
//*****************************************************************************

// Standard includes
#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "interrupt.h"
#include "gpio.h"
#include "systick.h"

// Common interface includes
#include "uart_if.h"
#include "pin_mux_config.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h" // Adafruit_Init();
#include "test.h" // test functions
#include "glcdfont.h" // font table

#define APPLICATION_VERSION     "1.1.1"
//*****************************************************************************
//
// Application Master/Slave mode selector macro
//
// MASTER_MODE = 1 : Application in master mode
// MASTER_MODE = 0 : Application in slave mode
//
//*****************************************************************************
#define MASTER_MODE      1

#define SPI_IF_BIT_RATE  100000
#define TR_BUFF_SIZE     100

#define MASTER_MSG       "This is CC3200 SPI Master Application\n\r"
#define SLAVE_MSG        "This is CC3200 SPI Slave Application\n\r"

#define MAX_STRING_LENGTH 80
#define MAX_PERIOD 16777216

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
static unsigned char g_ucTxBuff[TR_BUFF_SIZE];
static unsigned char g_ucRxBuff[TR_BUFF_SIZE];
static unsigned char ucTxBuffNdx;
static unsigned char ucRxBuffNdx;

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
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
        drawChar(x + i*10, y, s[i], WHITE, BLACK, 1);
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

void
Message2(const char *str)
{
    if(str != NULL)
    {
        while(*str!='\0')
        {
            MAP_UARTCharPut(UARTA1_BASE,*str++);
            MAP_UtilsDelay(100000);
        }
    }
}

void
WriteLoop()
{
    char cString[MAX_STRING_LENGTH+1];
    char cCharacter;
    int iStringLength = 0;

    while(1)
    {
        //
        // Fetching the input from the terminal.
        //
        cCharacter = MAP_UARTCharGet(CONSOLE);
        if(cCharacter == '\r' || cCharacter == '\n' ||
           (iStringLength >= MAX_STRING_LENGTH -1))
        {
            if(iStringLength >= MAX_STRING_LENGTH - 1)
            {
                MAP_UARTCharPut(CONSOLE, cCharacter);
                cString[iStringLength] = cCharacter;
                iStringLength++;
            }
            cString[iStringLength] = '\0';
            iStringLength = 0;
            //
            // Echoes the input string
            //
            Report("\n\rcmd#");
            Message2(cString);
        }
        else
        {
            MAP_UARTCharPut(CONSOLE, cCharacter);
            cString[iStringLength] = cCharacter;
            iStringLength++;
        }
    }
}

int g_place = 0;
int g_line = 0;

void ReadHandler()
{
    unsigned long ulStatus;

    ulStatus = MAP_UARTIntStatus(UARTA1_BASE,true);

    MAP_UARTIntClear(UARTA1_BASE,UART_INT_RX | UART_INT_TX);

    if(ulStatus & UART_INT_TX)
    {
    }

    if(ulStatus & (UART_INT_RX | UART_INT_RT))
    {
        while(UARTCharsAvail(UARTA1_BASE))
        {
            char c = MAP_UARTCharGetNonBlocking(UARTA1_BASE);
            drawChar(g_place * 6, 5 + g_line * 10, c, WHITE, BLACK, 1);
            g_place = (g_place + 1) % 20;
            if(g_place == 0){
                g_line = (g_line + 1) % 12;
            }
        }
    }
}

volatile int g_isSet = 0;
volatile int g_time = PERIOD_MAX;
volatile int g_delta = 0;
volatile int g_count = 0;

void testHandler()
{

	//
	// Clear all interrupts for GPIO
	//
	GPIOIntClear(GPIOA0_BASE, GPIO_INT_PIN_3);

	g_isSet = 1;
	g_count++;
	int newTime = SysTickValueGet();
	if(g_time > newTime)
	{
		g_delta = g_time - newTime;
	}
	else
	{
		g_delta = g_time + (MAX_PERIOD - newTime);
	}
	g_time = newTime;
	GPIOIntEnable(GPIOA0_BASE, GPIO_INT_PIN_3);
}

void
blankHandler()
{
	// do nothing
}

//*****************************************************************************
//
//! Main function for spi demo application
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************
void main()
{
    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Muxing UART and SPI lines.
    //
    PinMuxConfig();

    //
    // Enable the SPI module clock
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    //
    // Initialising the Terminal.
    //
    InitTerm();

    //
    // Initializing UART1
    //
    //MAP_UARTConfigSetExpClk(UARTA1_BASE,MAP_PRCMPeripheralClockGet(PRCM_UARTA1),
    //                UART_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
    //                UART_CONFIG_PAR_NONE));

    //MAP_UARTIntRegister(UARTA1_BASE, ReadHandler);

    //MAP_UARTIntEnable(UARTA1_BASE, UART_INT_RX | UART_INT_TX | UART_INT_RT);

    //
    // Clearing the Terminal.
    //
    ClearTerm();

    //
    // Display the Banner
    //
    Message("\n\n\n\r");
    Message("\t\t   ********************************************\n\r");
    Message("\t\t           Lab 3 Program  \n\r");
    Message("\t\t   ********************************************\n\r");
    Message("\n\n\n\r");

    //
    // Reset the peripheral
    //
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    //
    // Set up SPI for the OLED
    //
    //SPIInit();

    SysTickEnable();
    SysTickPeriodSet(16777216);

    SysTickIntEnable();
    SysTickIntRegister(blankHandler);

    GPIOIntTypeSet(GPIOA0_BASE, 0x8, GPIO_FALLING_EDGE);
    GPIOIntRegister(GPIOA0_BASE, testHandler);
    GPIOIntEnable(GPIOA0_BASE, GPIO_INT_PIN_3);

    //
    // Configure the OLED
    //
    //Adafruit_Init();

    //unsigned long time = 0;

    while(1)
    {
    	if(g_isSet)
    	{
    		Report("%d\r\n", g_count);
    		g_isSet = 0;
    	}
    }

    /*while(1)
    {
    	while(GPIOPinRead(GPIOA0_BASE, 0x8)){}
    	time = SysTickValueGet();
    //	Report("Recieved a value of : %lu", time);
    	while(!(GPIOPinRead(GPIOA0_BASE, 0x8))){}
    	time = time - SysTickValueGet();
    	Report("time between signal: %lu\n\r", time);
    }*/
}
