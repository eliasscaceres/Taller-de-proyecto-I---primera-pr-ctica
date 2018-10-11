/*
 * lcd.c
 *
 *  Created on: 30/07/2013
 *      Author: pc2
 */
#define INACCURATE_TO_MS 20400
#include "lcd.h"

//#include "derivative.h" /* include peripheral declarations */ no sé si esta bien sacar esto.

union ubyte
{
	char _byte;
	struct
	{
		unsigned char b0 : 1;
		unsigned char b1 : 1;
		unsigned char b2 : 1;
		unsigned char b3 : 1;
		unsigned char b4 : 1;
		unsigned char b5 : 1;
		unsigned char b6 : 1;
		unsigned char b7 : 1;
	} bit;
};

// Display configuration global variable
static char lcd_mode;	


//**************************************************************************
//* A simple delay function (used by LCD functions)
//**************************************************************************
//* Calling arguments
//* unsigned char time: aproximate delay time in microseconds
//**************************************************************************
void LCD_delay_ms(uint64_t delay_ms) {
   volatile uint64_t i;
   volatile uint64_t delay;

   delay = INACCURATE_TO_MS * delay_ms;

   for( i=delay; i>0; i-- );
}


//void LCD_delay_ms (unsigned char time)
//{
//	unsigned int temp;
//	for(;time;time--) for(temp=(BUS_CLOCK/23);temp;temp--);
//}

//**************************************************************************
//* Send a nibble to the LCD
//**************************************************************************
//* Calling arguments
//* char data : data to be sent to the display
//**************************************************************************
void LCD_send_nibble(char data)
{
	union ubyte my_union;
	my_union._byte = data;
	// Output the four data bits  VA ESE ORDEN?? invertir o no invertir
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_D4_GPIO, LCD_D4_PIN, my_union.bit.b0); //envio el valor del LCD_D4 A pin db4
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_D3_GPIO, LCD_D3_PIN, my_union.bit.b1); //del LCD_D3 A pin DB5
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_D2_GPIO, LCD_D2_PIN, my_union.bit.b2); //DEL LCD D2 A PIN DB6
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_D1_GPIO, LCD_D1_PIN, my_union.bit.b3); //DEL LCD D1 A PIN DB7
	// pulse the LCD enable line
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_ENABLE_GPIO, LCD_ENABLE_PIN, 1);  //LCDEN en 1.
	for (data=255; data; data--); //delay ----
	//LCD_delay_ms(1);
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_ENABLE_GPIO, LCD_ENABLE_PIN, 0); //LCD EN en 1
}

//**************************************************************************
//* Write a byte into the LCD
//**************************************************************************
//* Calling arguments:
//* char address : 0 for instructions, 1 for data
//* char data : command or data to be written
//**************************************************************************
void LCD_send_byte(char address, char data)
{
  unsigned int temp;
	if(address==0)
		Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_RS_GPIO, LCD_RS_PIN, 0); // config the R/S line  0 en LCD_RS
	else
		Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_RS_GPIO, LCD_RS_PIN, 1); // lcd_rs en 1
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_ENABLE_GPIO, LCD_ENABLE_PIN, 0);// set LCD enable line to 0
	LCD_send_nibble(data >> 4);     // send the higher nibble
	LCD_send_nibble(data & 0x0f);   // send the lower nibble
	//for (temp=999999; temp; temp--); //modificar delay poner 1 ms
	LCD_delay_ms(1);
}

//**************************************************************************
//* LCD initialization
//**************************************************************************
//* Calling arguments:
//* char mode1 : display mode (number of lines and character size)
//* char mode2 : display mode (cursor and display state)
//**************************************************************************
void LCD_init(char mode1, char mode2)
{
	char aux;
		// Configure the pins as outputs
		   /* Config EDU-CIAA-NXP Led Pins as GPIOs */
	   	Chip_SCU_PinMux(LCD_ENABLE_P, LCD_ENABLE_P_, MD_PUP, FUNC4);  //TODOS EN FUNCION GPIO!!
	   	Chip_SCU_PinMux(LCD_RS_P, LCD_RS_P_, MD_PUP, FUNC4);
	   	Chip_SCU_PinMux(LCD_D4_P, LCD_D4_P_, MD_PUP, FUNC4);
		Chip_SCU_PinMux(LCD_D3_P, LCD_D3_P_, MD_PUP, FUNC0);
	   	Chip_SCU_PinMux(LCD_D2_P, LCD_D2_P_, MD_PUP, FUNC0);
	   	Chip_SCU_PinMux(LCD_D1_P, LCD_D1_P_, MD_PUP, FUNC0);


	   /* Config EDU-CIAA-NXP Led Pins as Outputs */
	    Chip_GPIO_SetDir(LPC_GPIO_PORT, LCD_ENABLE_GPIO, (1<<LCD_ENABLE_PIN), OUTPUT); //SE SETEA LA DIRECCION DE TODOS LOS BITS ANTERIORES COMO SALIDA
	    Chip_GPIO_SetDir(LPC_GPIO_PORT, LCD_RS_GPIO, (1<<LCD_RS_PIN), OUTPUT);
	    Chip_GPIO_SetDir(LPC_GPIO_PORT, LCD_D4_GPIO, (1<<LCD_D4_PIN), OUTPUT);
	    Chip_GPIO_SetDir(LPC_GPIO_PORT, LCD_D3_GPIO, (1<<LCD_D3_PIN), OUTPUT);
	    Chip_GPIO_SetDir(LPC_GPIO_PORT, LCD_D2_GPIO, (1<<LCD_D2_PIN), OUTPUT);
	    Chip_GPIO_SetDir(LPC_GPIO_PORT, LCD_D1_GPIO, (1<<LCD_D1_PIN), OUTPUT);
	// Set the LCD data pins to zero
	    // Set the LCD data pins to zero

	    Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_ENABLE_GPIO, LCD_ENABLE_PIN, 0); // LCD enable = 0
	    Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_RS_GPIO, LCD_RS_PIN, 0); // LCD_RS EN 0
	    Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_D4_GPIO, LCD_D4_PIN, 0); //LCD_D4 EN 0 .. ETC
	    Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_D3_GPIO, LCD_D3_PIN, 0);
	    Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_D2_GPIO, LCD_D2_PIN, 0);
	    Chip_GPIO_SetPinState( LPC_GPIO_PORT, LCD_D1_GPIO, LCD_D1_PIN, 0);

	LCD_delay_ms(15);
	// LCD 4-bit mode initialization sequence
	// send three times 0x03 and then 0x02 to finish configuring the LCD
	for(aux=0;aux<3;++aux)
	{		
	  LCD_send_nibble(3);
	  LCD_delay_ms(5);
	}
	LCD_send_nibble(2);
	// Now send the LCD configuration data
	LCD_send_byte(0,0x20 | mode1);
	LCD_send_byte(0,0x08 | mode2);
	lcd_mode = 0x08 | mode2;
	
	LCD_send_byte(0,1);
	LCD_delay_ms(5);
	LCD_send_byte(0,6); //entry mode set I/D=1 S=0
}

//**************************************************************************
//* LCD cursor position set
//**************************************************************************
//* Calling arguments:
//* char x : column (starting by 0)
//* char y : line (0 or 1)
//**************************************************************************
void LCD_pos_xy(char x, char y)
{
  char address;
  if (y) address = LCD_SEC_LINE; else address = 0;
  address += x;
  LCD_send_byte(0,0x80|address);
}

//**************************************************************************
//* Write a character on the display
//**************************************************************************
//* Calling arguments:
//* char c : character to be written
//**************************************************************************
//* Notes :
//* \f clear the display
//* \n and \r return the cursor to line 1 column 0
//**************************************************************************
void LCD_write_char(char c)
{
  switch (c)
	{
	  case '\f' :	
	    LCD_send_byte(0,1);
	    LCD_delay_ms(5);
	    break;
	  case '\n' :
	  case '\r' :	
	    LCD_pos_xy(0,1);
	    break;
	  default:
	    LCD_send_byte(1,c);
   }
}

//**************************************************************************
//* Write a string on the display
//**************************************************************************
//* Calling arguments:
//* char *c : pointer to the string
//**************************************************************************
void LCD_write_string (char *c)
{
	while (*c)
	{
	  LCD_write_char(*c);
	  c++;
	}
}

//**************************************************************************
//* Turn the display on
//**************************************************************************
void LCD_display_on(void)
{
	lcd_mode |= 4;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the display off
//**************************************************************************
void LCD_display_off(void)
{
	lcd_mode &= 0b11111011;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the cursor on
//**************************************************************************
void LCD_cursor_on(void)
{
  lcd_mode |= 2;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn the cursor off
//**************************************************************************
void LCD_cursor_off(void)
{
	lcd_mode &= 0b11111101;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn on the cursor blink function
//**************************************************************************
void LCD_cursor_blink_on(void)
{
	lcd_mode |= 1;
	LCD_send_byte (0,lcd_mode);
}

//**************************************************************************
//* Turn off the cursor blink function
//**************************************************************************
void LCD_cursor_blink_off(void)
{
	lcd_mode &= 0b11111110;
	LCD_send_byte (0,lcd_mode);
}
/* a�adiMmos funcionabilidades al display*/

void LCD_reset(void) { // Resetea ambas lineas del display
	LCD_pos_xy(0, 0);
	LCD_write_string("                ");
	LCD_pos_xy(0, 1);
	LCD_write_string("                ");
}
void LCD_reset_0(void) { // Resetea la primera linea del display
	LCD_pos_xy(0, 0);
	LCD_write_string("                ");
}
void LCD_reset_1(void) { // Resetea la segunda linea del display
	LCD_pos_xy(0, 1);
	LCD_write_string("                ");
}

