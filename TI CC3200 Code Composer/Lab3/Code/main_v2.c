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
#define MESSAGE_SIZE 30
#define TEXT_DELAY 7

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
static unsigned char g_ucTxBuff[TR_BUFF_SIZE];
static unsigned char g_ucRxBuff[TR_BUFF_SIZE];
static unsigned char ucTxBuffNdx;
static unsigned char ucRxBuffNdx;

int clkflag = 0;
int buttonflag=0;
int change[MESSAGE_SIZE];
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

void DrawNextChar(){ //print only one char of message at a time
	if (writepos >= MESSAGE_SIZE){
		writepos = 0; //if outside the current message, restart
		return;
	}
	else {
		if (change[writepos]){
			drawChar(5+ 10*(writepos%12) , 5 + 10*(writepos/12), sending[writepos], WHITE, (writepos == readpos) ? RED : BLACK, 1); //write next char
			change[writepos]=0;
		}
		writepos++; //move char over
		return;
	}
}

void GPIOHandler(){
	GPIOIntClear(GPIOA0_BASE, GPIO_INT_PIN_3);
	buttonflag = 1;
}

void clkHandler()
{
	clkflag = 1;
	lastpush++;
}

int getbit(){
    unsigned long time;
    while((GPIOPinRead(GPIOA0_BASE, 0x8))!=0x8){/*do nothing*/}
    time = SysTickValueGet();
    clkflag = 0;
    while(GPIOPinRead(GPIOA0_BASE, 0x8) == 0x8){if (250000<(time - (SysTickValueGet() - (clkflag ? 16777216 : 0)))) return 0;}
    time = time - (SysTickValueGet() - (clkflag ? 16777216 : 0));
    if (time < 80000){return 0;}
    else return 1;
}

unsigned char readdata(){
		unsigned long time = 0;
	    unsigned char data, data1;
	    int valid = 1;
        time = SysTickValueGet();
        clkflag = 0;
        while(GPIOPinRead(GPIOA0_BASE, 0x8) != 0x8){}
       // Report("%d - ", time - (SysTickValueGet() - (clkflag ? 16777216 : 0)));
       // if ((time < 700000) || (time > 850000)){return 255;}
        time = SysTickValueGet();
        clkflag =0;
        while(GPIOPinRead(GPIOA0_BASE, 0x8) == 0x8){/*do nothing*/}
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
        if (valid)// && (data ^ data1))
        	return data;
        else return 255;
     //   Report("The signal you sent was : %u\n\r", data);
      //  Report("The confirmation signal was : %u\n\r", data1);
}

void shiftright(){
	int i;
	change[readpos] = 1;
	if (cursize == 0) {
		cursize++;
		return;
	}
	if (cursize < MESSAGE_SIZE){
		i = cursize;
		if (cursize != 0)
			readpos++;
		cursize++;
	}
	else {
		i = MESSAGE_SIZE - 1;
		if (readpos < MESSAGE_SIZE - 1)
			readpos++;
	}
	for (; i>readpos; i--){
		sending[i]=sending[i-1];
		change[i] = 1;
	}
}

void shiftleft(){
	int i;
	for (i = readpos; i<cursize; i++){
		change[i] = 1;
		sending[i] = sending[i+1];
	}
	if (i!=0){
		cursize--;
		if (readpos == cursize)
			readpos--;
	}
}

void nextchar(unsigned char data){
	switch (data){ //case statement
	case 0: //ascii translation
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '0') && (sending[readpos] != ' '))){
			shiftright();
		}
		if (sending[readpos] == ' ')
			sending[readpos] = '0';
		else sending[readpos] = ' ';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 1:
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '1') && (sending[readpos] != '.')  && (sending[readpos] != '?') && (sending[readpos] != '!'))){
			shiftright();
		}
		if (sending[readpos] == '.')
			sending[readpos] = '?';
		else if (sending[readpos] == '?')
			sending[readpos] = '!';
		else if (sending[readpos] == '!')
			sending[readpos] = '1';
		else sending[readpos] = '.';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 2: //goes from a -> b -> c -> 2 -> a...
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '2') && (sending[readpos] != 'a')  && (sending[readpos] != 'b') && (sending[readpos] != 'c'))){
			shiftright();
		}
		if (sending[readpos] == 'a')
			sending[readpos] = 'b';
		else if (sending[readpos] == 'b')
			sending[readpos] = 'c';
		else if (sending[readpos] == 'c')
			sending[readpos] = '2';
		else sending[readpos] = 'a';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 3:
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '3') && (sending[readpos] != 'd')  && (sending[readpos] != 'e') && (sending[readpos] != 'f'))){
			shiftright();
		}
		if (sending[readpos] == 'd')
			sending[readpos] = 'e';
		else if (sending[readpos] == 'e')
			sending[readpos] = 'f';
		else if (sending[readpos] == 'f')
			sending[readpos] = '3';
		else sending[readpos] = 'd';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 4:
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '4') && (sending[readpos] != 'g')  && (sending[readpos] != 'h') && (sending[readpos] != 'i'))){
			shiftright();
		}
		if (sending[readpos] == 'g')
			sending[readpos] = 'h';
		else if (sending[readpos] == 'h')
			sending[readpos] = 'i';
		else if (sending[readpos] == 'i')
			sending[readpos] = '4';
		else sending[readpos] = 'g';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 5:
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '5') && (sending[readpos] != 'j')  && (sending[readpos] != 'k') && (sending[readpos] != 'l'))){
			shiftright();
		}
		if (sending[readpos] == 'j')
			sending[readpos] = 'k';
		else if (sending[readpos] == 'k')
			sending[readpos] = 'l';
		else if (sending[readpos] == 'l')
			sending[readpos] = '5';
		else sending[readpos] = 'j';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 6:
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '6') && (sending[readpos] != 'm')  && (sending[readpos] != 'n') && (sending[readpos] != 'o'))){
			shiftright();
		}
		if (sending[readpos] == 'm')
			sending[readpos] = 'n';
		else if (sending[readpos] == 'n')
			sending[readpos] = 'o';
		else if (sending[readpos] == 'o')
			sending[readpos] = '6';
		else sending[readpos] = 'm';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 7:
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '7') && (sending[readpos] != 'p')  && (sending[readpos] != 'q') && (sending[readpos] != 'r') && (sending[readpos] != 's'))){
			shiftright();
		}
		if (sending[readpos] == 'p')
			sending[readpos] = 'q';
		else if (sending[readpos] == 'q')
			sending[readpos] = 'r';
		else if (sending[readpos] == 'r')
			sending[readpos] = 's';
		else if (sending[readpos] == 's')
			sending[readpos] = '7';
		else sending[readpos] = 'p';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 8:
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '8') && (sending[readpos] != 't')  && (sending[readpos] != 'u') && (sending[readpos] != 'v'))){
			shiftright();
		}
		if (sending[readpos] == 't')
			sending[readpos] = 'u';
		else if (sending[readpos] == 'u')
			sending[readpos] = 'v';
		else if (sending[readpos] == 'v')
			sending[readpos] = '8';
		else sending[readpos] = 't';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 9:
		if (lastpush > TEXT_DELAY || ((sending[readpos] != '9') && (sending[readpos] != 'w')  && (sending[readpos] != 'x') && (sending[readpos] != 'y') && (sending[readpos] != 'z'))){
			shiftright();
		}
		if (sending[readpos] == 'w')
			sending[readpos] = 'x';
		else if (sending[readpos] == 'x')
			sending[readpos] = 'y';
		else if (sending[readpos] == 'y')
			sending[readpos] = 'z';
		else if (sending[readpos] == 'z')
			sending[readpos] = '9';
		else sending[readpos] = 'w';
		lastpush = 0;
		change[readpos]=1;
		break;
	case 115: //makes uppercase
		if (sending[readpos] > 96 && sending[readpos] < 123){
			sending[readpos]= sending[readpos] - 32;
			change[readpos]=1;
		}
		break;
	case 116: //makes lowercase
		if (sending[readpos] > 64 && sending[readpos] < 91){
			sending[readpos]= sending[readpos] + 32;
			change[readpos] = 1;
		}
		break;
	case 117: //moves cursor forward
		if (readpos < cursize - 1) {
			change[readpos]=1;
			readpos++;
			change[readpos]=1;
		}
		break;
	case 118: //moves cursor backward
		if (readpos>0) {
			change[readpos]=1;
			readpos--;
			change[readpos]=1;
		}
		break;
	case 119: //send value... could be optimized
		sending[MESSAGE_SIZE] = '\0';
		Message2(sending);
		for (data = 0; data<=MESSAGE_SIZE; data++){
			sending[data] = '\0';
			change[data] = 1;
		}
		fillRect(0, 0, SSD1351WIDTH, SSD1351HEIGHT/2, BLACK);
		cursize = 0;
		readpos = 0;
		break;
	case 87:
		shiftleft();
		change[readpos] = 1;
		break;
	/*case 96:
		for(data=0; data<MESSAGE_SIZE; data++){
			Report("%d", change[data]);
		}
		break;
	/*case 97: Report("Vol-\n\r"); break;
	case 80: Report("Chan+\n\r"); break;
	case 81: Report("Chan-\n\r"); break;
	case 100: Report("mute\n\r"); break;*/
	default: break;
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
   // InitTerm();

    //
    // Initializing UART1
    //
    //MAP_UARTConfigSetExpClk(UARTA1_BASE,MAP_PRCMPeripheralClockGet(PRCM_UARTA1),
    //                UART_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
   //                 UART_CONFIG_PAR_NONE));

   // MAP_UARTIntRegister(UARTA1_BASE, ReadHandler);

   // MAP_UARTIntEnable(UARTA1_BASE, UART_INT_RX | UART_INT_TX | UART_INT_RT);


    // Clearing the Terminal.
    //
    ClearTerm();

    //
    // Display the Banner

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

    fillScreen(BLACK);

    Message("\n\n\n\r");
    Message("\t\t   ********************************************\n\r");
    Message("\t\t           Lab 3 Program  \n\r");
    Message("\t\t   ********************************************\n\r");
    Message("\n\n\n\r");

    //Report("Clearing display...");
    //fillScreen(BLACK);

    //Report("\r\ncmd#");
    //
    // Reads input from UART0 (Console) and writes it to UART1 (Other CC3200)
    //
    //WriteLoop();
    unsigned char data;
    for (data = 0; data<=MESSAGE_SIZE; data++){
    	sending[data] = '\0';
    	change[data] = 0;
    }
    while(1){
    	if (buttonflag){
    		data = readdata(); //read in the data
    		nextchar(data);
    		GPIOIntEnable(GPIOA0_BASE, GPIO_INT_PIN_3); //reenable interrupt
    		buttonflag = 0; //turn off flag
    		//if (data!=255)
    		//	Report("%s\n\r", sending);
    	}
    	DrawNextChar();
    }
}
