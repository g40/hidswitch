/*

    HidSwitch: Based on a Cypress application note for a USB keyboard HID

    Device presents as a HID with up to 64 bits of switch data and 8 bits of LED.

    This is work in progress.

	Copyright (c) Jerry Evans, 2016

	visit https://github.com/g40

	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
	STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
	IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.

*/


#include <device.h>

void In_EP (void);
void Out_EP (void);

/* External variable where OUT Report data is stored */
extern uint8 USBFS_1_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF[USBFS_1_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF_SIZE];
						   
/* Array of Keycode information to send to PC */
static unsigned char Keyboard_Data[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
/* Array of information pertaining to LEDS */
static unsigned char LED_Data[1] = {0 };

static uint8_t btn = 0;


int main()
{
    CYGlobalIntEnable; 

    /*Start USBFS Operation and Device 0 and with 5V operation*/ 
	USBFS_1_Start(0, USBFS_1_DWR_VDDD_OPERATION);
	/*Enables OUT EP*/
	USBFS_1_EnableOutEP(2);
	/*Waits for USB to enumerate*/ 
	while(!USBFS_1_bGetConfiguration()); 		

    /*Begins USB Traffic*/
	USBFS_1_LoadInEP(1, Keyboard_Data, 8);    
	
    reg_led_Write(1);

    reg_led_Write(0);

    reg_led_Write(1);
    
    reg_led_Write(0);

    for(;;)
    {
        //
        Keyboard_Data[0] = (Pin_Btn_Read() == 0 ? 0xFF : 0x00);
        
		/*Checks for ACK from host*/
		if(USBFS_1_bGetEPAckState(1)) 
		{
			/*Function to Send Data to PC*/
			In_EP(); 	

			/*Function to recieve data from PC*/	
			Out_EP();

		}
    }
    
    return 0;
}

//
void In_EP (void)
{
    Keyboard_Data[0] = (Pin_Btn_Read() == 0 ? 0xFF : 0x00);
	/*Loads EP1 for a IN transfer to PC*/
	USBFS_1_LoadInEP(1, Keyboard_Data, 8);
	/*Waits for ACK from PC*/
	while(!USBFS_1_bGetEPAckState(1))
    {
        /* NOP */;
    }
#if 0
    /*Loads EP1 for a IN transfer to PC. This simulates the buttons being released.*/
	USBFS_1_LoadInEP(1, Keyboard_Data, 8);
	/*Waits for ACK from PC*/
	while(!USBFS_1_bGetEPAckState(1))
    {
        /* NOP */;
    }
#endif    
}
	
//
void Out_EP (void)
{
	/*Reads the OUT Report data */
	LED_Data[0] = USBFS_1_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF[0];

	if (LED_Data[0] == 0xFF)
	{
        reg_led_Write(1);
	}
	else if (LED_Data[0] == 0x00)
    {
        reg_led_Write(0);
    }
}

/* End of File */

