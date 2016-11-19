/*

  Lever Logic - Requires players to get all five outputs to be ON by pulling levers.
				Each lever toggles the state of a fixed set of outputs.
				
  2016 Shawn Yates
  Want to automate this puzzle?  Visit our website to learn how:
  www.cluecontrol.com
  
  written and tested with Arduino 1.6.9
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

  
  written and tested with Arduino 1.6.9
 
   -------------------
*/

/*
// pin assignments
 * Signal     Pin                
 *            Arduino Uno     
 * ------------------------
 * 0   Serial RX
 * 1   Serial TX
 * 2   SW1 - 5 lever/button inputs	    
 * 3   SW2
 * 4   SW3
 * 5   SW4    
 * 6   SW5  
 * 7   SW6 - game reset input
 
 * 8   Master Output
 * 9       
 * 10	ChipSelect for Ethernet
 * 11   SPI MOSI for Ethernet
 * 12   SPI MISO for Ethernet
 * 13   SPI SCK  for Ethernet
 
 * 14  (A0)     DL1			four output dials
 * 15  (A1)     DL2
 * 16  (A2)		DL3
 * 17  (A3)     DL4
 * 18  (A4/SDA) DL5
 * 19  (A5/SCK)
 
 
 The Ethernet Shield SPI bus (10,11,12,13) with 10 as select
 also uses 4 as select for the SD card on the ethernet shield
 high deselects for the chip selects.  Reserve these incase the
 puzzle is ever connected to ClueControl via an Ethernet shield.
 
 General operation:
 
		
	RESET
		Called at startup and when reset button is activated
		Set all dials to off
		Set main output to on


	GAME PLAY
		Each input (SW) will toggle the state of a set of outputs:
		SW1		DL1,  DL2
		SW2		DL1,  DL2,  DL3, DL4
		SW3		DL1,  		DL3,      DL5
		SW4						 DL4
		SW5					DL3, DL4, DL5
				
		When all 5 outputs go true, the puzzle is solved and the main output is turned off.
		It will not turn back on until the game is reset, but the switches will continue	
		to toggle the dial outputs
 
 */


// Constant Values

#define DEBOUNCE_DLY 50  			// Number of mS before a switch state is accepted


// I/O Pin Naming

#define 	SW1		2			//four buttons for the laser sequence game
#define		SW2		3			//the code relies on these four buttons
#define		SW3		4			//being adjacent pin numbers
#define		SW4		5
#define		SW5		6

#define		RstButton	7		//reset button


#define		DL1	14
#define		DL2	15
#define		DL3	16
#define		DL4	17
#define		DL5	18


#define		MagOut		8		//maglock output


// Variables/Storage

bool	DL1State = false;			//booleans to store the current state of the outputs
bool	DL2State = false;
bool	DL3State = false;
bool	DL4State = false;
bool	DL5State = false;
bool RstState = false;

bool	SW1State = false;			//booleans to store switch/lever states and prevent re-triggers
bool	SW2State = false;
bool	SW3State = false;
bool	SW4State = false;
bool	SW5State = false;

bool NewVal;



void setup()
{
	// put your setup code here, to run once:
	
	// serial setup
	Serial.begin(9600);
	Serial.println("Serial interface started");
	
	//Pin Setup
	pinMode(SW1,INPUT_PULLUP);
	pinMode(SW2,INPUT_PULLUP);
	pinMode(SW3,INPUT_PULLUP);
	pinMode(SW4,INPUT_PULLUP);
	pinMode(SW5,INPUT_PULLUP);
	
	pinMode(DL1,OUTPUT);
	pinMode(DL2,OUTPUT);
	pinMode(DL3,OUTPUT);
	pinMode(DL4,OUTPUT);
	pinMode(DL5,OUTPUT);
	
	pinMode(RstButton,INPUT_PULLUP);
	
	pinMode(MagOut,OUTPUT);
	
	
	ResetGame();
	
	
}

void loop() {
	// put your main code here, to run repeatedly:
	
	NewVal = DebounceSW(SW1);
	if (NewVal != SW1State)
	{
		SW1State = !SW1State;
		if (SW1State)
		{
			DL1State = !DL1State;
			DL2State = !DL2State;
		}
	}
	
	NewVal = DebounceSW(SW2);
	if (NewVal != SW2State)
	{
		SW2State = !SW2State;
		if (SW2State)
		{
			DL1State = !DL1State;
			DL2State = !DL2State;
			DL3State = !DL3State;
			DL4State = !DL4State;
		}
	}
	
	NewVal = DebounceSW(SW3);
	if (NewVal != SW3State)
	{
		SW3State = !SW3State;
		if (SW3State)
		{
			DL1State = !DL1State;
			DL3State = !DL3State;
			DL5State = !DL5State;
		}
	}
	
	NewVal = DebounceSW(SW4);
	if (NewVal != SW4State)
	{
		SW4State = !SW4State;
		if (SW4State)
		{
			DL4State = !DL4State;
		}
	}
	
	NewVal = DebounceSW(SW5);
	if (NewVal != SW5State)
	{
		SW5State = !SW5State;
		if (SW5State)
		{
			DL3State = !DL3State;
			DL4State = !DL4State;
			DL5State = !DL5State;
		}
	}
	
	NewVal = DebounceSW(RstButton);
	if (NewVal != RstState)  
	{	
		RstState = !RstState;
		if (RstState)
		{
			ResetGame();
		}
	}

	digitalWrite(DL1,DL1State);
	digitalWrite(DL2,DL2State);
	digitalWrite(DL3,DL3State);
	digitalWrite(DL4,DL4State);
	digitalWrite(DL5,DL5State);

	if (DL1State && DL2State && DL3State && DL4State && DL5State)
	{
		digitalWrite(MagOut,LOW);			//turn off the maglock.  won't reset until reset button is pressed
	}


}	//end of loop


void ResetGame()
{
	//Reset the game and get ready for a new player

	digitalWrite(MagOut,HIGH);					//turn on the mag and laser

	DL1State = false;
	DL2State = false;
	DL3State = false;
	DL4State = false;
	DL5State = false;

	Serial.println("The game is reset, ready to play!");


} //end of ResetGame


bool DebounceSW(byte SWx)
{
	//read the passed switch twice and make sure both readings match
	//this prevents multiple triggers due to mechanical noise in 
	//the switch
	
	
	bool PossVal2;
	bool PossVal = !digitalRead(SWx);		//invert the reading due to the use of pullups

		
	while(true)
	{
		delay(DEBOUNCE_DLY);					//delay by the debounce amount
		PossVal2 = !digitalRead(SWx);			//re-read the switch
		
		if (PossVal == PossVal2)				//if the two reads are the same
		{
			return (PossVal);					//return the read value
		}
		
		//this code will only execute if the two reads did not match
		//Now read the pin again and look for a match.
		//If the button is cycling very fast, it is possible the code
		//will deadlock here.  This is a very slim possibility
		
		PossVal = !digitalRead(SWx);			//re-take the first reading
		//and loop back to the delay
	}
	
	return (PossVal); 	//this line is never executed, but makes the compiler happy.
	
}






