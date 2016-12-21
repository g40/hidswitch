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

//Creats a Scan Code Look Up Table for the various ASCII values
const uint8 aASCII_ToScanCode[] = {0x2C, 0x1E, 0x34, 0x20, 0x21, 0x22, 0x24, 0x34, 0x26, 0x27, 0x25, 0x2E, 0x36, 
								   0x2D, 0x37, 0x38, 0x27, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 
								   0x33, 0x33, 0x36, 0x2E, 0x37, 0x38, 0x1F, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
								   0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15 ,0x16, 
								   0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x2F, 0x31, 0x30, 0x23, 0x2D, 0x35, 
								   0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 
								   0x11, 0x12, 0x13, 0x14, 0x15 ,0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 
								   0x2F, 0x31, 0x30, 0x35, 0x4C};
								   
/* Array of Keycode information to send to PC */
static unsigned char Keyboard_Data[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
/* Array of information pertaining to LEDS */
static unsigned char Out_Data[1] = {0};
/* String that will be printed when Type_Input (SW2) is pressed */
static const char StringStorage[] = {"Cypress PSoC USB HID: Stay Connected My Friends"};
static uint8 i;
static uint8 key;


int main()
{
    CYGlobalIntEnable; 
	LCD_Char_1_Start();
	LCD_Char_1_Position(0,0);
	LCD_Char_1_PrintString("PSoC 3 Keyboard");
	/*Start USBFS Operation and Device 0 and with 5V operation*/ 
	USBFS_1_Start(0, USBFS_1_DWR_VDDD_OPERATION);
	/*Enables OUT EP*/
	USBFS_1_EnableOutEP(2);
	/*Waits for USB to enumerate*/ 
	while(!USBFS_1_bGetConfiguration()); 		
	/*Begins USB Traffic*/
	USBFS_1_LoadInEP(1, Keyboard_Data, 8);    
	

    for(;;)
    {
		/*Function to check for Caps Lock button press on DVK*/
		CapsLock();
		
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

void In_EP (void)
{
	/*Checks for Type_Input(SW1) to be pressed. This ensures that the text is only typed when button is pressed*/
	if(Type_Input_Read() == 0) 
	{
		/*Waits for button to be released. This is a de-bounce implimentation*/
		while(Type_Input_Read() == 0);   
			/*Checks to be sure a null is not present*/
			while(StringStorage[i] != 0) 
			{
				/*Ensures that all ASCII values in the string are in range*/
				if((StringStorage[i] >= 0x20) && (StringStorage[i] <= 0x7F))
				{
					/*Removes 0x20 offset to have key presses begin at 0. This is not required but used to make the example easier to understand.*/
					key = StringStorage[i] - 0x20; 
					if(((key >= 1) && (key <= 6)) || ((key >= 8) && (key <=11)) || (key == 26) || (key == 28) || ((key >= 30) && (key <=58)) || (key == 62) || (key == 63) || ((key >= 91) && (key <= 94)))
					{
						/*Applies Shift key is any keys that require the shift button is pressed. The buttons include '!', '@', 'B', etc.*/
						Keyboard_Data[0] = LSHIFT; 
					}
					else
					{
						/*If the shift is not required, then the modifier byte remains set to 0x00*/
						Keyboard_Data[0] = 0x00; 
					}
				
					/*Loads the converted Scan Code using LUT into the fist Key Code array location*/
					Keyboard_Data[2] = aASCII_ToScanCode[key]; 
				}
				
					/*Loads EP1 for a IN transfer to PC*/
					USBFS_1_LoadInEP(1, Keyboard_Data, 8);
					/*Waits for ACK from PC*/
					while(!USBFS_1_bGetEPAckState(1));
					/*Resets modifer byte to 0x00*/
					Keyboard_Data[0] = 0x00; 
					/*Resets keycode 0 to 0x00*/
					Keyboard_Data[2] = 0x00; 
					/*Loads EP1 for a IN transfer to PC. This simulates the buttons being released.*/
					USBFS_1_LoadInEP(1, Keyboard_Data, 8);
					/*Waits for ACK from PC*/
					while(!USBFS_1_bGetEPAckState(1));
					/*Advances to the next ASCII Character*/
					i++;		
			}	
	
		/*Loads 0x28 (Enter/Return) into keycode 0 to move to next line*/
		Keyboard_Data[2] = ENTER;
		/*Loads EP1 for a IN transfer to PC*/
		USBFS_1_LoadInEP(1, Keyboard_Data, 8);
		/*Waits for ACK from PC*/
		while(!USBFS_1_bGetEPAckState(1));
		/*Resets keycode 0 to 0x00*/
		Keyboard_Data[2] = 0x00;
		/*Loads EP1 for a IN transfer to PC. This simulates the buttons being released.*/
		USBFS_1_LoadInEP(1, Keyboard_Data, 8);
		/*Waits for ACK from PC*/
		while(!USBFS_1_bGetEPAckState(1));
		/*Resets string position counter to 0 to type string again*/
		i=0;
	}
}
	

void Out_EP (void)
{
	/*Reads the OUT Report data */
	Out_Data[0] = USBFS_1_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF[0];


	/*If the Num Lock is enabled, display on LCD and LED*/
	if ((Out_Data[0] & 0x01)!= 0)
	{
		LCD_Char_1_Position(1,0);
		LCD_Char_1_PrintString("NUM");
		Keyboard_Data[2] = 0x00;
		USBFS_1_LoadInEP(1, Keyboard_Data, 8);
		Num_Lock_Write(1);
	}
	
	/*If the Num Lock not is enabled, clear the LCD and LED*/
	else
	{
		LCD_Char_1_Position(1,0);
		LCD_Char_1_PrintString("   ");
		Num_Lock_Write(0);
	}
		
	/*If the Caps Lock is enabled, display on LCD and LED*/
	if((Out_Data[0] & 0x02)!= 0)
	{
		LCD_Char_1_Position(1,6);
		LCD_Char_1_PrintString("CAPS");
		Keyboard_Data[2] = 0x00;
		USBFS_1_LoadInEP(1, Keyboard_Data, 8);
		Caps_Lock_Write(1);
	}
	
	/*If the Caps Lock not is enabled, clear the LCD and LED*/
	else
	{
		LCD_Char_1_Position(1,6);
		LCD_Char_1_PrintString("    ");
		Caps_Lock_Write(0);
	}
			
	
	/*If the Scroll Lock is enabled, display on LCD and LED*/				
	if((Out_Data[0] & 0x04)!= 0)
	{
		LCD_Char_1_Position(1,13);
		LCD_Char_1_PrintString("SCL");
		Keyboard_Data[2] = 0x00;
		USBFS_1_LoadInEP(1, Keyboard_Data, 8);
		Scroll_Lock_Write(1);
	} 
	/*If the Scroll Lock not is enabled, clear the LCD and LED*/
	else
	{
		LCD_Char_1_Position(1,13);
		LCD_Char_1_PrintString("   ");
		Scroll_Lock_Write(0);
	}
}

void CapsLock (void)
{
	/*Checks if Caps Lock (SW2) has been pressed. If so, loads the Caps Lock scan code into the key code array*/
	if(Caps_Lock_Key_Read() == 0)
	{
		while(Caps_Lock_Key_Read() == 0);
		Keyboard_Data[2] = CAPS;
		USBFS_1_LoadInEP(1, Keyboard_Data, 8);
		while(!USBFS_1_bGetConfiguration());
	}
}
/* End of File */

