/*******************************************************************************
* File Name: Main.c
* Version 1.0
*
* Description:
* This file contains the main function for the USB HID Keyboard application.
*
* Note:
*
********************************************************************************
* Copyright (2009), Cypress Semiconductor Corporation. All rights reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign),United States copyright laws and international treaty provisions. 
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating 
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement. 
*******************************************************************************/

/*************************************************************************************
                                THEORY OF OPERATION
* This project is a keyboard that when a user presses a button, will print a pre-defined 
* string into a text editor. The program is also constantly display the status of num lock,
* caps lock, and scroll lock on both LEDs and an LCD.
*
*************************************************************************************/

#include <device.h>

#define LSHIFT 0x02 
#define ENTER 0x28 
#define CAPS 0x39

void In_EP (void);
void Out_EP (void);
void CapsLock (void);

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
        Keyboard_Data[0] = Pin_Btn_Read();
        
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
    Keyboard_Data[0] = Pin_Btn_Read();
	/*Loads 0x28 (Enter/Return) into keycode 0 to move to next line*/
	Keyboard_Data[2] = ENTER;
	/*Loads EP1 for a IN transfer to PC*/
	USBFS_1_LoadInEP(1, Keyboard_Data, 8);
	/*Waits for ACK from PC*/
	while(!USBFS_1_bGetEPAckState(1))
    {
        /* NOP */;
    }
	/*Resets keycode 0 to 0x00*/
	Keyboard_Data[2] = 0x00;
	/*Loads EP1 for a IN transfer to PC. This simulates the buttons being released.*/
	USBFS_1_LoadInEP(1, Keyboard_Data, 8);
	/*Waits for ACK from PC*/
	while(!USBFS_1_bGetEPAckState(1))
    {
        /* NOP */;
    }
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

