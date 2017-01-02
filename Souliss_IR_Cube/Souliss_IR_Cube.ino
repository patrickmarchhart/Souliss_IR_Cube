/**************************************************************************

				Souliss - IR Gateway for Expressif ESP8266

***************************************************************************/

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/IPBroadcast.h"


// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "SSID"
#define WiFi_Password           "PASSWORD"


// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>


/*** All configuration includes should be above this line ***/
#include "Souliss.h"


#define SLOT_LAMP					0  //The Logical T11 uses Slot 0
#define SLOT_IR_SoundSystem_onoff	1  //IR command Sound System on/off uses Slot 1
#define SLOT_IR_SoundSystem_volup	2  //IR command Sound System on/off uses Slot 2
#define SLOT_IR_SoundSystem_voldo	3  //IR command Sound System on/off uses Slot 3
#define SLOT_IR_TV_onoff			4  //IR command TV on/off uses Slot 4
#define SLOT_IR_TV_chup				5  //IR command TV channel up uses Slot 5
#define SLOT_IR_TV_chdo				6  //IR command TV channel down uses Slot 6


#define	OUTPUTPIN	2	//Onboard LED is connected to GPIO pin 2
#define RECV_PIN	4	//an IR receiver is connected to GPIO pin 4
#define SEND_PIN	5	//an IR LED      is connected to GPIO pin 5 

#define Gateway_address 100
#define myvNet_address  ip_address[3]   
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

//IP Address Configuration
uint8_t ip_address[4]  = { 192, 168, 178, 100 };
uint8_t subnet_mask[4] = { 255, 255, 255, 0 };
uint8_t ip_gateway[4]  = { 192, 168, 178, 1 };


void setup()
{
	//Hardware Configuration
	pinMode(OUTPUTPIN, OUTPUT);
	Serial.begin(9600);
	
	//Souliss Configuration
	Initialize();

	//vNet Configuration
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway(myvNet_address);      					   
	SetAddress(0xAB01, 0xFF00, 0x0000);
	SetAsPeerNode(0xAB02, 1);

	//Activate Typicals
	Set_T11(SLOT_LAMP);
	Set_T14(SLOT_IR_SoundSystem_onoff);
	Set_T14(SLOT_IR_SoundSystem_volup);
	Set_T14(SLOT_IR_SoundSystem_voldo);
	Set_T14(SLOT_IR_TV_onoff);
	Set_T14(SLOT_IR_TV_chup);
	Set_T14(SLOT_IR_TV_chdo);

	//Souliss IR Configuration
	Souliss_IR_Setup(SEND_PIN, RECV_PIN);
	Souliss_IR_Debug(1);
}


void loop()
{
	// Here we start to play
	EXECUTEFAST()
	{
		UPDATEFAST();

		FAST_50ms()
		{
			Logic_T11(SLOT_LAMP);
			DigOut(OUTPUTPIN, Souliss_T1n_Coil, SLOT_LAMP);

			//Convert the Pushbuttons T14 into IR commands
			Logic_T14(SLOT_IR_SoundSystem_onoff);
			Souliss_IR_control_irdevice(Souliss_T14, SLOT_IR_SoundSystem_onoff, Souliss_T1n_ToggleCmd, NEC, 32, 0x7E8154AB);

			Logic_T14(SLOT_IR_SoundSystem_volup);
			Souliss_IR_control_irdevice(Souliss_T14, SLOT_IR_SoundSystem_volup, Souliss_T1n_ToggleCmd, NEC, 32, 0x5EA158A7);

			Logic_T14(SLOT_IR_SoundSystem_voldo);
			Souliss_IR_control_irdevice(Souliss_T14, SLOT_IR_SoundSystem_voldo, Souliss_T1n_ToggleCmd, NEC, 32, 0x5EA1D827);

			Logic_T14(SLOT_IR_TV_onoff);
			Souliss_IR_control_irdevice(Souliss_T14, SLOT_IR_TV_onoff, Souliss_T1n_ToggleCmd, RC6, 20, 0x1000C);

			Logic_T14(SLOT_IR_TV_chup);
			Souliss_IR_control_irdevice(Souliss_T14, SLOT_IR_TV_chup, Souliss_T1n_ToggleCmd, RC6, 20, 0x10020);

			Logic_T14(SLOT_IR_TV_chdo);
			Souliss_IR_control_irdevice(Souliss_T14, SLOT_IR_TV_chdo, Souliss_T1n_ToggleCmd, RC6, 20, 0x10021);

			//Toggle the onboard LED on a Souliss node with the address 0xAB02 and Slot-ID 0
			Souliss_IR_control_remote_node(0xAB02, 0, Souliss_T1n_ToggleCmd, NEC, 0x77E1BA0F);
			
			//Toggle the onboard LED of the Souliss IR Cube
			Souliss_IR_control_local_node(SLOT_LAMP, Souliss_T1n_ToggleCmd, NEC, 0x77E1400F);			
		}

		// Here we handle here the communication with Android
		FAST_GatewayComms();
	}
}


