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
#include <ctype.h> // tolower and toupper

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
#define MESSAGE_SIZE 30
#define TEXT_DELAY 7
#define INVALID_DATA 255

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
int clkflag = 0;
int buttonflag=0;

int lastpush = 0;

char sending[MESSAGE_SIZE + 1];
int readpos = 0; //what character to edit with remote
int writepos = 0; //what character to type on screen
int cursize = 0; //current size of write buffer

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

void ReadHandler()
{
    unsigned long ulStatus;
    static int x = 0;

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
            drawChar(x * 6, 30, c, WHITE, BLACK, 1);
            x = (x + 1) % 20;
        }
    }
}

DrawNextChar(int i)
{
	drawChar(5 + 6*i , 5, sending[i], WHITE, BLACK, 1); //write next char
}

void GPIOHandler()
{
	GPIOIntClear(GPIOA0_BASE, GPIO_INT_PIN_3);
	buttonflag = 1;
}

void clkHandler()
{
	clkflag = 1;
	lastpush++;
}

int getbit()
{
    unsigned long time;
    while((GPIOPinRead(GPIOA0_BASE, 0x8))!=0x8){/*do nothing*/}
    time = SysTickValueGet();
    clkflag = 0;
    while(GPIOPinRead(GPIOA0_BASE, 0x8) == 0x8){if (250000<(time - (SysTickValueGet() - (clkflag ? 16777216 : 0)))) return 0;}
    time = time - (SysTickValueGet() - (clkflag ? 16777216 : 0));
    if (time < 80000){return 0;}
    else return 1;
}

unsigned char readdata()
{
		unsigned long time = 0;
	    unsigned char data, data1;
	    int valid = 1;

        while(GPIOPinRead(GPIOA0_BASE, 0x8) != 0x8);

        time = SysTickValueGet();
        clkflag = 0;

        while(GPIOPinRead(GPIOA0_BASE, 0x8) == 0x8);

        time = time - (SysTickValueGet() - (clkflag ? 16777216 : 0));
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

// sends, capitalizes, erases, etc
// The index 'i' may be modified by this function
void
runCommand(unsigned char data, char *sending, int *i)
{
	switch(data)
	{
	case 80: // channel +
		break;
	case 81: // channel -
		break;
	case 96: // vol+. capitalize
		sending[*i] = toupper(sending[*i]);
		DrawNextChar(*i);
		break;
	case 97: // vol-. lowercase
		sending[*i] = tolower(sending[*i]);
		DrawNextChar(*i);
		break;
	case 115: // up arrow
		break;
	case 118: // left arrow
		break;
	case 119: // OK. send message over UART
		Message2(sending);
		sending[0] = '\0';
		*i = -1;
		DrawString("                      ", 5, 5); // clear line
		break;
	default:
		{
			sending[*i] = ' ';
			DrawNextChar(*i); // hack to erase characters from the LED
			sending[*i] = '\0'; // so it's a valid string
			(*i)--;
			if(*i < -1)
				*i = -1;
		}
	}
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
    MAP_UARTConfigSetExpClk(UARTA1_BASE,MAP_PRCMPeripheralClockGet(PRCM_UARTA1),
                    UART_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                    UART_CONFIG_PAR_NONE));

    MAP_UARTIntRegister(UARTA1_BASE, ReadHandler);

    MAP_UARTIntEnable(UARTA1_BASE, UART_INT_RX | UART_INT_TX | UART_INT_RT);

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
    SPIInit();

    SysTickEnable();
    SysTickPeriodSet(16777216);
    int x = SysTickPeriodGet();

    SysTickIntEnable();
    SysTickIntRegister(clkHandler);


    GPIOIntTypeSet(GPIOA0_BASE, 0x8, GPIO_FALLING_EDGE);
    GPIOIntRegister(GPIOA0_BASE, GPIOHandler);
    GPIOIntEnable(GPIOA0_BASE, GPIO_INT_PIN_3);

    //
    // Configure the OLED
    //
    Adafruit_Init();

    //
    // Uncomment to run tests instead of listen for input.
    //
    //testloop();


    //Report("Clearing display...");
    fillScreen(BLACK);

    //Report("\r\ncmd#");
    //
    // Reads input from UART0 (Console) and writes it to UART1 (Other CC3200)
    //
    //WriteLoop();
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
