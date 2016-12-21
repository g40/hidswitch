/*******************************************************************************
* File Name: Main.c
* Version 1.0
*
* Description:
* This file contains the main function for USB HID Composite Device Application.
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
* This project uses a USB composite device configuration to implement a key pad
* which is used to input numerical values, and a quadrature rotary encoder to implement 
* volume control on a PC.
*************************************************************************************/


#include <device.h>

void Scan_Keypad (void);
void Keyboard_LEDs (void);
void Volume_Control(void);

/* External variable where OUT Report data is stored */
extern uint8 USBFS_1_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF[USBFS_1_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF_SIZE];
uint8 USB_IN_Volume_Data[1] = {0};
uint8 USB_IN_Keyboard_Data[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
uint8 USB_Out_Keyboard_Data[1] = {0};
int8 Volume_Data, Old_Volume_Data;

void main()
{
    CYGlobalIntEnable;
	QuadDec_1_Start();
	QuadDec_Clock_Start();
	QuadDec_ISR_Start();
	LCD_Char_1_Start();
	LCD_Char_1_Position(0,0);
	LCD_Char_1_PrintString("PSoC 3 Composite");
	/*Start USBFS Operation and Device 0 and with 5V operation*/ 
	USBFS_1_Start(0, USBFS_1_DWR_VDDD_OPERATION);  
	/*Waits for USB to enumerate*/ 
	while(!USBFS_1_bGetConfiguration());   
	/*Begins USB Traffic for Keyboard*/
	USBFS_1_LoadInEP(1, USB_IN_Keyboard_Data, 8);
	/*Begins USB Traffic for Volume Controller*/
	USBFS_1_LoadInEP(2, USB_IN_Volume_Data, 1);
	
    for(;;)
    {
       /*Checks for ACK from host for Keyboard Traffic*/
		if(USBFS_1_bGetEPAckState(1) != 0)
		{
			/*Function to scan keypad board */
			Scan_Keypad();
			
			/*Function to receive LED data from PC out EP2*/
			Keyboard_LEDs();
		}
		
		/*Checks for ACK from host for Volume Control Traffic*/
		if(USBFS_1_bGetEPAckState(2) != 0)
		{
			/*Function to control volume based on encoder status */
			Volume_Control();
			/*Loads EP3 to transfer data to PC */
			USBFS_1_LoadInEP(2, USB_IN_Volume_Data, 1);
		}
    }
}

void Scan_Keypad (void)
{
	
	/* Each button is scanned and if asserted, loads the proper scan code into USB_IN_Keyboard_Data */
	if (Pin_0_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x27; /* 0 key */
		while(Pin_0_Read() != 0);
	}
	
	else if (Pin_1_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x1E; /* 1 key */
		while(Pin_1_Read() != 0);
	}
	
	else if (Pin_2_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x1F; /* 2 key */
		while(Pin_2_Read() != 0);
	}
	
	else if (Pin_3_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x20; /* 3 key */
		while(Pin_3_Read() != 0);
	}
	
	else if (Pin_4_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x21; /* 4 key */
		while(Pin_4_Read() != 0);
	}
	
	else if (Pin_5_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x22; /* 5 key */
		while(Pin_5_Read() != 0);
	}
	
	else if (Pin_6_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x23; /* 6 key */
		while(Pin_6_Read() != 0);
	}
	
	else if (Pin_7_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x24; /* 7 key */
		while(Pin_7_Read() != 0);
	}
	
	else if (Pin_8_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x25; /* 8 key */
		while(Pin_8_Read() != 0);
	}
	
	else if (Pin_9_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x26; /* 9 key */
		while(Pin_9_Read() != 0);
	}
	
	else if (Divide_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x54; /* '/' key */
		while(Divide_Read() != 0);
	}
	
	else if (Multiply_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x55; /* '*' key */
		while(Multiply_Read() != 0);	
	}
	else if (Subtract_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x56; /* '-' key */
		while(Subtract_Read() != 0);
	}
	
	else if (Add_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x57; /* '+' key */
		while(Add_Read() != 0);
	}
	
	else if (Enter_Read() != 0)
	{
		USB_IN_Keyboard_Data[2] = 0x58; /* 'Return' key */
		while(Enter_Read() != 0);
	}
	
	else
	{
		USB_IN_Keyboard_Data[2] = 0x00;
	}
	
	/*Loads EP1 to transfer data to PC */
	USBFS_1_LoadInEP(1, USB_IN_Keyboard_Data, 8);
}

void Keyboard_LEDs (void)
{
	/*Reads the OUT Report data */
	USB_Out_Keyboard_Data[0] = USBFS_1_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF[0];
	
	/*If the Num Lock is enabled, display on LCD and LED*/
	if ((USB_Out_Keyboard_Data[0] & 0x01)!= 0)
	{
		LCD_Char_1_Position(1,0);
		LCD_Char_1_PrintString("NUM");
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
	if((USB_Out_Keyboard_Data[0] & 0x02)!= 0)
	{
		LCD_Char_1_Position(1,6);
		LCD_Char_1_PrintString("CAPS");
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
	if((USB_Out_Keyboard_Data[0] & 0x04)!= 0)
	{
		LCD_Char_1_Position(1,13);
		LCD_Char_1_PrintString("SCL");
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

void Volume_Control (void)
{

	/*Checks to see if Mute button was pressed */
	if(Mute_Button_Read() != 0)
	{
		while(Mute_Button_Read() != 0);
		USB_IN_Volume_Data[0] |= 0x04;
	}
	
	/*Checks to see if encoder has moved in the positive direction*/
	else if(Volume_Data > Old_Volume_Data)
	/*Stores value to increase volume*/
	USB_IN_Volume_Data[0] |= 0x01;
	/*Checks to see if encoder has moved in the negative direction*/
	else if(Volume_Data < Old_Volume_Data)
	/*Stores value to decrease volume*/
	USB_IN_Volume_Data[0] |= 0x02;
	else
	USB_IN_Volume_Data[0] &= 0x00;
	
	/*Updates comparison variable with current data*/
	Old_Volume_Data = Volume_Data;
}

/* End of File */


