/**************************************************************************
Souliss - Hello World for Expressif ESP8266

This is the basic example, create a software push-button on Android
using SoulissApp (get it from Play Store).

Load this code on ESP8266 board using the porting of the Arduino core
for this platform.

***************************************************************************/

// Configure the framework


#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/IPBroadcast.h"

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "Internetz"
#define WiFi_Password           "Sagidaned1"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>


/*** All configuration includes should be above this line ***/
#include "Souliss.h"
//#include "SoulissIrGateway.h"


// This identify the number of the LED logic
#define SLOT          0
#define SLOT_IR       1

// **** Define here the right pin for your ESP module **** 
#define	OUTPUTPIN	2


#define Gateway_address 3
#define myvNet_address  ip_address[3]   
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

uint8_t ip_address[4] = { 192, 168, 178, 100 };
uint8_t subnet_mask[4] = { 255, 255, 255, 0 };
uint8_t ip_gateway[4] = { 192, 168, 178, 1 };


#define RECV_PIN 4  //an IR detector/demodulator is connected to GPIO pin 4
#define SEND_PIN 5
IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN); //an IR led is connected to GPIO pin 5

decode_results  ir_data;

void setup()
{
	Initialize();

	// Connect to the WiFi network and get an address from DHCP
	//GetIPAddress();

	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway(myvNet_address);      // Set this node as gateway for SoulissApp 
									   // This is the vNet address for this node, used to communicate with other
									   // nodes in your Souliss network
	SetAddress(0xAB01, 0xFF00, 0x0000);
	SetAsPeerNode(0xAB02, 1);


	Set_T11(SLOT);
	//Set_T71(SLOT_IR);

	pinMode(OUTPUTPIN, OUTPUT);
	irrecv.enableIRIn();  // Start the receiver
	
	Serial.begin(9600);


}





void loop()
{
	// Here we start to play
	EXECUTEFAST()
	{
		UPDATEFAST();

		FAST_50ms()
		{
			Logic_T11(SLOT);
			DigOut(OUTPUTPIN, Souliss_T1n_Coil, SLOT);

						

			if (irrecv.decode(&ir_data))
			{  // Grab an IR code
				//dumpInfo(&ir_data);           // Output the results
											  //dumpRaw(&results);            // Output the results in RAW format
											  //Serial.println("");           // Blank line between entries
				Souliss_ir_control_remote_node(memory_map, 0xAB02, 0, Souliss_T1n_ToggleCmd, ir_data, NEC, 0x77E1BA0F);
				Souliss_ir_control_local_node(memory_map, 0, Souliss_T1n_ToggleCmd, ir_data, NEC, 0x77E1400F);
				
				if (ir_data.value == 0x77E17A0F)
				{
					delay(1000);
					//Souliss_ir_send_command(NEC, 32, 0x7E8154AB, 0);
				}

				delay(100);					  //delay(100);
				irrecv.resume();              // Prepare for the next value
			}
			
		}

		// Here we handle here the communication with Android
		FAST_GatewayComms();
	}

}


