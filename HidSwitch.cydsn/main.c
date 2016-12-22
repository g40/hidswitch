/*

    HidSwitch: Based on a Cypress application note for a USB keyboard HID

    Device presents as a HID with up to 64 bits of switch data and 



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

