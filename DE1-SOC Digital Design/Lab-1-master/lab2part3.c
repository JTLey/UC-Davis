// Simple test program 
#include <stdlib.h>
#include <stdio.h>


void scroll(int *pause);
void helloworld(int *pause);

volatile unsigned int * led = (unsigned int *) 0xFF200000; // red LED address
volatile unsigned int * switchptr = (unsigned int *) 0xFF200040; // SW slider switch address
volatile unsigned int * Hex3_Hex0 = (unsigned int *) 0xFF200030; // Hex 3-0
volatile unsigned int * Hex5_Hex4 = (unsigned int *) 0xFF200010; // Hex 5-4
volatile unsigned int * PushButtons = (unsigned int *) 0xFF200020; // Button

int main(void)
{
	while(1)
	{
		
	}
	/*int pause = 10000;
	unsigned int switch_value;

	while (1)
	{
	        int c;
	        int d;
	    d=1;
		
		while(1){
		  //  scroll(pause); 


		  	if(d == 1){
		    	helloworld(&pause);
		     //*(Hex5_Hex4)= 0x00007679;
		     //*(Hex3_Hex0)= 0x38383F00; 
		    	 d = 0 ;//hello
		    // *(Hex5_Hex4)= 0x00003E3E;
		    // *(Hex3_Hex0)= 0x3F77383F; //world
		    }
		    
		 	else if (d==0){
				switch_value = *(switchptr);
				*(led) = switch_value;
		//	switch_value = *(PushButtons);
				*(Hex3_Hex0) = switch_value;
				while (button(&pause)){
					scroll(&pause);
				}
				d = 1;
		 	}
	    }//while
		  
	}//while
	*/
	return 0;
}//main

void clearfirst(){
	Hex3_Hex0 = Hex3_Hex0 / 256;
	Hex3_Hex0 = Hex3_Hex0 * 256;
}

int button(int *pause){
	if (*PushButtons == 2){
		pause = pause *2;
	}

	else if (*PushButtons == 4){
	pause = pause / 2;
	}


	else if (*PushButtons == 1){
		for (c = 1; c <= 327670000; c++)
			{};
		
		while(1){
			if(*PushButtons ==1)
			   break;
		}//while
	}//else if
	if (*PushButtons == 8){
		return 1;
	}
	else return 0;
}

void helloworld(int *pause){
	while (1){
		Hex3_Hex0 = 0;
		Hex5_Hex4 = 0;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 118;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 121;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 56;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 56;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 63;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 0;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 62;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 62;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 63;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 119;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 56;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		Hex3_Hex0 += 63;
		scroll(pause);
		clearfirst();
		if (button()) {break};
		scroll(pause);
		clearfirst();
		if (button()) {break};
		scroll(pause);
		clearfirst();
		if (button()) {break};
		scroll(pause);
		clearfirst();
		if (button()) {break};
		scroll(pause);
		clearfirst();
		if (button()) {break};
		scroll(pause);
		clearfirst();
		if (button()) {break};
	}
}

void scroll(int *pause){
  unsigned int overflow, d, c;
  for ( c = 1 ; c <= 32767 ; c++ )
       for ( d = 1 ; d <= *pause ; d++ )
       {}
  overflow = *(Hex5_Hex4) / 256;
  overflow = overflow % 256;
  *(Hex5_Hex4) = *(Hex5_Hex4) * 256;
  *(Hex5_Hex4) = *(Hex5_Hex4) + (*(Hex3_Hex0) / 16777216);
  *(Hex3_Hex0) = *(Hex3_Hex0) * 256;
  *(Hex3_Hex0) = *(Hex3_Hex0) + overflow;

}
