/*
 * Project Name :Audio Spectrometer
 * MCU used : Atmega328p
 * Description : This project reads the frequencies of an audio signal and graphically displays them on a OLED.jumpers have been provided
 to set different display modes
 * Audio_Spectrometer.c
 *
 * Created: 3/21/2018 6:32:28 AM
 * Author : Esskay
 */ 

# define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include "audio_spectrometer.h"
#include "macro.h"
#include "elektor_logo.h"
#include "name.h"


int main(void)
{

	DDRB |=(1 << OLED_SCL)|(1 << OLED_SDA)| (1 << OLED_DC)|(1 << OLED_RES);
	DDRC |=(1 << SA_RESET)|(1 << SA_STROBE);
	
	PORTC |= (1 << MODE_SEL0) | (1 << MODE_SEL1);
	
	ADCinit();
	
	util_BitClear(PORTB,OLED_SCL);
	
	// OLED RESET
	util_BitClear(PORTB,OLED_RES);
	_delay_ms(1);
	util_BitSet(PORTB,OLED_RES);
	_delay_ms(1);

	// OLED MODULE INITIALIZATION
	// SET OLED TO RECEIVE COMMAND
	util_BitClear(PORTB,OLED_DC); 
	send_oled_data(0xfd); // command lock
	send_oled_data(0x12);
	send_oled_data(0xaF); // display on
	send_oled_data(0xa4); // Normal Display mode
	send_oled_data(0x15); //set column address
	send_oled_data(0x00); //column address start 00
	send_oled_data(0x5f); //column address end 95
	send_oled_data(0x75); //set row address
	send_oled_data(0x00); //row address start 00
	send_oled_data(0x3f); //row address end 63
	send_oled_data(0x87); //master current control
	send_oled_data(0x06); //9/16(160uA)
	send_oled_data(0x83); //Set Contrast for Color R
	send_oled_data(0xc0);
	send_oled_data(0x82); //Set Contrast for Color G
	send_oled_data(0x65);
	send_oled_data(0x81); //Set Contrast for Color B
	send_oled_data(0x95);
	send_oled_data(0x8a);
	send_oled_data(0x61);
	send_oled_data(0x8b);
	send_oled_data(0x62);
	send_oled_data(0x8c);
	send_oled_data(0x63);
	send_oled_data(0xa0); //set re-map & data format
	send_oled_data(0x72); //Horizontal address increment
	send_oled_data(0xa1); //set display start line
	send_oled_data(0x00); //start 00 line
	send_oled_data(0xa2); //set display offset
	send_oled_data(0x00);
	send_oled_data(0xa8); //set multiplex ratio
	send_oled_data(0x3f); //64MUX
	send_oled_data(0xad);
	send_oled_data(0x8f);
	send_oled_data(0xb0); //set power save
	send_oled_data(0x1a); //
	send_oled_data(0xb1);
	send_oled_data(0xf1); // Phase 2 period Phase 1 period
	send_oled_data(0xb3); // Set Display Clock Divide Ratio/ Oscillator Frequency
	send_oled_data(0xd0); // 0.97MHZ
	send_oled_data(0xbb); // set pre-charge
	send_oled_data(0x3e); //
	send_oled_data(0xbe); //set Vcomh
	send_oled_data(0x3e); //0.83Vref
	send_oled_data(0x26); //Enable filling of color into rectangle in draw rectangle command
	send_oled_data(0xA1);
	

	
	

	display_full_white();
	_delay_ms(1000);
	util_BitClear(PORTB,OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x15); //set column address
	send_oled_data(00); //column address start 00
	send_oled_data(0x5f); //column address end 95
	send_oled_data(0x75); //set row address
	send_oled_data(8); //row address start 00
	send_oled_data(0x3f); //row address end 63
	display_elektor_logo();
	
	util_BitClear(PORTB,OLED_DC); // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x15); //set column address
	send_oled_data(0x00); //column address start 00
	send_oled_data(0x5f); //column address end 95
	send_oled_data(0x75); //set row address
	send_oled_data(0x00); //row address start 00
	send_oled_data(0x3f); //row address end 63
	
	_delay_ms(3000);   // SHOW LOGO FOR 3 SEC
	display_blank();
	_delay_ms(10);
	display_name();
	_delay_ms(3000);
	
	display_blank();
	_delay_ms(10);

	while(1)
	{

		read_sa();  // READ SPECTRUM ANALYZER
		if(util_IsBitCleared(PINC,MODE_SEL0)  && util_IsBitCleared(PINC,MODE_SEL1))
		{
			display_white_bars();
		}
		else if(util_IsBitCleared(PINC,MODE_SEL0)  && util_IsBitSet(PINC,MODE_SEL1))
		{
			display_green_yellow_red_bars();
		}
		else if(util_IsBitSet(PINC,MODE_SEL0)  && util_IsBitCleared(PINC,MODE_SEL1))
		{
			display_thin_blue_bars();
		}
		else if(util_IsBitSet(PINC,MODE_SEL0)  && util_IsBitSet(PINC,MODE_SEL1))
		{
			display_thin_red_bars (); 
		}
		_delay_ms(30);
	}
}


void read_sa(void)
{
	uint16_t temp=0;
	// RESET PULSE
	util_BitSet(PORTC,SA_RESET);
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	util_BitClear(PORTC,SA_STROBE);
	_delay_ms(1);
	util_BitClear(PORTC,SA_RESET);
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	
	// read 63Hz
	util_BitClear(PORTC,SA_STROBE);
	_delay_ms(1);
	//noise cancellation loop
	temp = getADC(ADC_PIN);
	if(temp > OFFSET)
	{
		c_bar1 = (temp - OFFSET);
		if(c_bar1 > DIVIDE)
		c_bar1 /= DIVIDE;
		else
		c_bar1 = 0;
	}
	else
		c_bar1 = 0;
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	
	// read 160Hz
	util_BitClear(PORTC,SA_STROBE);
	_delay_ms(1);
	temp = getADC(ADC_PIN);
	//noise cancellation loop
	if(temp > OFFSET)
	{
		c_bar2 = (temp - OFFSET);
		if(c_bar2 > DIVIDE)
			c_bar2 /= DIVIDE;
		else
			c_bar2 = 0;
	}
	else
		c_bar2 = 0;
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	
	// read 400Hz
	util_BitClear(PORTC,SA_STROBE);
	_delay_ms(1);
	temp = getADC(ADC_PIN);
	//noise cancellation loop
	if(temp > OFFSET)
	{
		c_bar3 = (temp - OFFSET);
		if(c_bar3 > DIVIDE)
		c_bar3 /= DIVIDE;
		else
		c_bar3 = 0;
	}
	else
		c_bar3 = 0;
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	
	// read 1kHz
	util_BitClear(PORTC,SA_STROBE);
	_delay_ms(1);
	temp = getADC(ADC_PIN);
	//noise cancellation loop
	if(temp > OFFSET)
	{
		c_bar4 = (temp - OFFSET);
		if(c_bar4 > DIVIDE)
		c_bar4 /= DIVIDE;
		else
		c_bar4 = 0;
	}
	else
	c_bar4 = 0;
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	
	// read 2.5kHz
	util_BitClear(PORTC,SA_STROBE);
	_delay_ms(1);
	temp = getADC(ADC_PIN);
	//noise cancellation loop
	if(temp > OFFSET)
	{
		c_bar5 = (temp - OFFSET);
		if(c_bar5 > DIVIDE)
		c_bar5 /= DIVIDE;
		else
		c_bar5 = 0;
	}
	else
	c_bar5 = 0;
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	
	// read 6.25kHz
	util_BitClear(PORTC,SA_STROBE);
	_delay_ms(1);
	temp = getADC(ADC_PIN);
	//noise cancellation loop
	if(temp > OFFSET)
	{
		c_bar6 = (temp - OFFSET);
		if(c_bar6 > DIVIDE)
		c_bar6 /= DIVIDE;
		else
		c_bar6 = 0;
	}
	else
		c_bar6 = 0;
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	
	// read 16kHz
	util_BitClear(PORTC,SA_STROBE);
	_delay_ms(1);
	temp = getADC(ADC_PIN);
	//noise cancellation loop
	if(temp > OFFSET)
	{
		c_bar7 = (temp - OFFSET);
		if(c_bar7 > DIVIDE)
		c_bar7 /= DIVIDE;
		else
		c_bar7 = 0;
	}
	else
		c_bar7 = 0;
	util_BitSet(PORTC,SA_STROBE);
	_delay_ms(1);
	
}


void display_white_bars (void)
{
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar1_start_row);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar1_end_row);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar1);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar1_start_row);   //Set the starting row coordinates
	send_oled_data(63 - c_bar1);   //Set the starting column coordinates,
	send_oled_data(bar1_end_row);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	_delay_ms(2);
	//===========================
	
	PORTB &=~(OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar2_start_row);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar2_end_row);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar2);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar2_start_row);   //Set the starting row coordinates
	send_oled_data(63 - c_bar2);   //Set the starting column coordinates,
	send_oled_data(bar2_end_row);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	_delay_ms(2);
	//===============================
	
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar3_start_row);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar3_end_row);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar3);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar3_start_row);   //Set the starting row coordinates
	send_oled_data(63 - c_bar3);   //Set the starting column coordinates,
	send_oled_data(bar3_end_row);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar4_start_row);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar4_end_row);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar4);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar4_start_row);   //Set the starting row coordinates
	send_oled_data(63 - c_bar4);   //Set the starting column coordinates,
	send_oled_data(bar4_end_row);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar5_start_row);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar5_end_row);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar5);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar5_start_row);   //Set the starting row coordinates
	send_oled_data(63 - c_bar5);   //Set the starting column coordinates,
	send_oled_data(bar5_end_row);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar6_start_row);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar6_end_row);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar6);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar6_start_row);   //Set the starting row coordinates
	send_oled_data(63 - c_bar6);   //Set the starting column coordinates,
	send_oled_data(bar6_end_row);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar7_start_row);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar7_end_row);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar7);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar7_start_row);   //Set the starting row coordinates
	send_oled_data(63 - c_bar7);   //Set the starting column coordinates,
	send_oled_data(bar7_end_row);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
}

void display_full_white (void)
{
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(0);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(95);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0xff);
	send_oled_data(0xff);
}

void display_blank (void)
{
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	
	send_oled_data(0x25); // Enter the "clear mode" by execute the command 25h
	send_oled_data(0);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(95);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	_delay_ms(2);
}



//====================================================================
void send_oled_data (unsigned char c_oled_data)
{

	if(CHECK_BIT(c_oled_data,7))    // CHECK IF 7TH BIT IS 1
	{
		util_BitSet(PORTB,OLED_SDA); // SET LCD DATA LINE HIGH
	}
	else
	{
		util_BitClear(PORTB,OLED_SDA);  // SET LCD DATA LINE LOW
	}
	util_BitSet(PORTB,OLED_SCL);
	util_BitClear(PORTB,OLED_SCL);   // provide clock pulse
	
	if(CHECK_BIT(c_oled_data,6))    // CHECK IF 6TH BIT IS 1
	{
		util_BitSet(PORTB,OLED_SDA);   // SET LCD DATA LINE HIGH
	}
	else
	{
		PORTB &= ~(1 << OLED_SDA);     // SET LCD DATA LINE LOW
	}
	util_BitSet(PORTB,OLED_SCL);
	util_BitClear(PORTB,OLED_SCL);   // provide clock pulse
	
	if(CHECK_BIT(c_oled_data,5))    // CHECK IF 5TH BIT IS 1
	{
		util_BitSet(PORTB,OLED_SDA);   // SET LCD DATA LINE HIGH
	}
	else
	{
		util_BitClear(PORTB,OLED_SDA);     // SET LCD DATA LINE LOW
	}
	util_BitSet(PORTB,OLED_SCL);
	util_BitClear(PORTB,OLED_SCL);
	
	if(CHECK_BIT(c_oled_data,4))    // CHECK IF 4TH BIT IS 1
	{
		util_BitSet(PORTB,OLED_SDA);   // SET LCD DATA LINE HIGH
	}
	else
	{
		util_BitClear(PORTB,OLED_SDA);     // SET LCD DATA LINE LOW
	}
	util_BitSet(PORTB,OLED_SCL);
	util_BitClear(PORTB,OLED_SCL);
	
	if(CHECK_BIT(c_oled_data,3))    // CHECK IF 4TH BIT IS 1
	{
		util_BitSet(PORTB,OLED_SDA);   // SET LCD DATA LINE HIGH
	}
	else
	{
		util_BitClear(PORTB,OLED_SDA);      // SET LCD DATA LINE LOW
	}
	util_BitSet(PORTB,OLED_SCL);
	util_BitClear(PORTB,OLED_SCL);
	
	if(CHECK_BIT(c_oled_data,2))    // CHECK IF 4TH BIT IS 1
	{
		util_BitSet(PORTB,OLED_SDA);   // SET LCD DATA LINE HIGH
	}
	else
	{
		util_BitClear(PORTB,OLED_SDA);      // SET LCD DATA LINE LOW
	}
	util_BitSet(PORTB,OLED_SCL);
	util_BitClear(PORTB,OLED_SCL);
	
	if(CHECK_BIT(c_oled_data,1))    // CHECK IF 4TH BIT IS 1
	{
		util_BitSet(PORTB,OLED_SDA);   // SET LCD DATA LINE HIGH
	}
	else
	{
		util_BitClear(PORTB,OLED_SDA);     // SET LCD DATA LINE LOW
	}
	util_BitSet(PORTB,OLED_SCL);
	util_BitClear(PORTB,OLED_SCL);
	
	if(CHECK_BIT(c_oled_data,0))    // CHECK IF 4TH BIT IS 1
	{
		util_BitSet(PORTB,OLED_SDA);   // SET LCD DATA LINE HIGH
	}
	else
	{
		util_BitClear(PORTB,OLED_SDA);      // SET LCD DATA LINE LOW
	}
	util_BitSet(PORTB,OLED_SCL);
	util_BitClear(PORTB,OLED_SCL);
}



void display_elektor_logo (void)
{
	int16_t i_counter = 0;
	
	util_BitSet(PORTB,OLED_DC);   // SET OLED TO RECEIVE DATA
	while(i_counter < 9216)
	{
		send_oled_data(pgm_read_byte(&gImage_elektor_logo[i_counter]));
		i_counter++;
	}
}

void display_name (void)
{
	int16_t i_counter = 0;
	
	util_BitSet(PORTB,OLED_DC);    // SET OLED TO RECEIVE DATA
	while(i_counter < 11904)
	{
		send_oled_data(pgm_read_byte(&gImage_name[i_counter]));
		i_counter++;
	}
}

void ADCinit(){
	ADCSRA=0b0111           ; // predivider 128
	ADCSRA |= ( 1 << ADEN)  ; // enable
	// ADMUX =0b00000001 ; // Vref=Aref external , input at ADC1 pin 24
	ADMUX =0b01000001 ; // Vref=AVcc , input at ADC1 pin 24
}

//Read Analog value from the channel
uint16_t getADC(uint8_t Channel){
	ADMUX &=0b11110000 ;
	ADMUX |=Channel ;
	ADCSRA |= ( 1 << ADSC)  ;
	while ( (ADCSRA & (1 << ADIF ))==0 ){ } ;
	ADCSRA |= (1 << ADIF ) ;
	return ADC ;
}

void display_green_yellow_red_bars (void)
{
	generate_colour_bar(bar1_start_row, bar1_end_row, c_bar1);
	generate_colour_bar(bar2_start_row, bar2_end_row, c_bar2);
	generate_colour_bar(bar3_start_row, bar3_end_row, c_bar3);
	generate_colour_bar(bar4_start_row, bar4_end_row, c_bar4);
	generate_colour_bar(bar5_start_row, bar5_end_row, c_bar5);
	generate_colour_bar(bar6_start_row, bar6_end_row, c_bar6);
	generate_colour_bar(bar7_start_row, bar7_end_row, c_bar7);
}

// creating colour bar from provided adc data
// THIS IS COMMON ENGINE USED TO GENERATE ALL 7 BARS
void generate_colour_bar (int8_t c_start_row, int8_t c_end_row, int8_t c_bar)
{
	if(c_bar > 63)
	{
		c_bar = 63;
	}
	util_BitClear(PORTB,OLED_DC); // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(c_start_row);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(c_end_row);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	if(c_bar <= 35)
	{
		send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
		send_oled_data(c_start_row);   //Set the starting row coordinates
		send_oled_data(63 - c_bar);   //Set the starting column coordinates,
		send_oled_data(c_end_row);  //Set the finishing row coordinates
		send_oled_data(63);  //Set the finishing column coordinates
		send_oled_data(0);   //Set the outline color C, B and A
		send_oled_data(0xff);
		send_oled_data(0);
		send_oled_data(0);   //Set the filled color C, B and A
		send_oled_data(0xff);
		send_oled_data(0);
		_delay_ms(2);
	}
	else
	{
		
		// MAKE LOWER BAR GREEN FROM ROW 63 TO 29
		send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
		send_oled_data(c_start_row);   //Set the starting row coordinates
		send_oled_data(63 - 35);   //Set the starting column coordinates,
		send_oled_data(c_end_row);  //Set the finishing row coordinates
		send_oled_data(63);  //Set the finishing column coordinates
		send_oled_data(0);   //Set the outline color C, B and A
		send_oled_data(0xff);
		send_oled_data(0);
		send_oled_data(0);   //Set the filled color C, B and A
		send_oled_data(0xff);
		send_oled_data(0);
		_delay_ms(2);
		
		// MAKE MIDDLE PORTION OF BAR YELLOW FROM ROW 29 TO  10
		send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
		send_oled_data(c_start_row);   //Set the starting row coordinates
		send_oled_data(63 - c_bar);   //Set the starting column coordinates,
		send_oled_data(c_end_row);  //Set the finishing row coordinates
		send_oled_data(29);  //Set the finishing column coordinates
		send_oled_data(0xff);   //Set the outline color C, B and A
		send_oled_data(0xff);
		send_oled_data(0);
		send_oled_data(0xff);   //Set the filled color C, B and A
		send_oled_data(0xff);
		send_oled_data(0);
		_delay_ms(2);
		
		if(c_bar > 54)  // it is above 54 so show green yellow & red all three colours
		{
			// MAKE TOP PORTION OF BAR RED FROM ROW 54 TO 64
			send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
			send_oled_data(c_start_row);   //Set the starting row coordinates
			send_oled_data(63 - c_bar);   //Set the starting column coordinates,
			send_oled_data(c_end_row);  //Set the finishing row coordinates
			send_oled_data(10);  //Set the finishing column coordinates
			send_oled_data(0xff);   //Set the outline color C, B and A
			send_oled_data(0);
			send_oled_data(0);
			send_oled_data(0xff);   //Set the filled color C, B and A
			send_oled_data(0);
			send_oled_data(0);
			_delay_ms(2);
		}
		
		
		
	}
}

void display_thin_blue_bars (void)
{
	
	util_BitClear(PORTB,OLED_DC); // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar1_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar1_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar1);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar1_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar1);   //Set the starting column coordinates,
	send_oled_data(bar1_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	_delay_ms(2);
	//===========================
	
	util_BitClear(PORTB,OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar2_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar2_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar2);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar2_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar2);   //Set the starting column coordinates,
	send_oled_data(bar2_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	_delay_ms(2);
	//===============================
	
	util_BitClear(PORTB,OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar3_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar3_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar3);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar3_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar3);   //Set the starting column coordinates,
	send_oled_data(bar3_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar4_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar4_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar4);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar4_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar4);   //Set the starting column coordinates,
	send_oled_data(bar4_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC); // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar5_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar5_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar5);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar5_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar5);   //Set the starting column coordinates,
	send_oled_data(bar5_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar6_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar6_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar6);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar6_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar6);   //Set the starting column coordinates,
	send_oled_data(bar6_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar7_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar7_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar7);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar7_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar7);   //Set the starting column coordinates,
	send_oled_data(bar7_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0xff);
	_delay_ms(2);
	//==========================
}


//=====================================================================================

void display_thin_red_bars (void)
{
	util_BitClear(PORTB,OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar1_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar1_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar1);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar1_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar1);   //Set the starting column coordinates,
	send_oled_data(bar1_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	//===========================

	util_BitClear(PORTB,OLED_DC);   // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar2_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar2_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar2);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar2_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar2);   //Set the starting column coordinates,
	send_oled_data(bar2_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	//===============================
	
	util_BitClear(PORTB,OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar3_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar3_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar3);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar3_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar3);   //Set the starting column coordinates,
	send_oled_data(bar3_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC); // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar4_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar4_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar4);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar4_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar4);   //Set the starting column coordinates,
	send_oled_data(bar4_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC);  // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar5_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar5_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar5);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar5_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar5);   //Set the starting column coordinates,
	send_oled_data(bar5_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC); // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar6_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar6_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar6);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar6_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar6);   //Set the starting column coordinates,
	send_oled_data(bar6_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	//==========================
	
	util_BitClear(PORTB,OLED_DC); // SET OLED TO RECEIVE COMMAND
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar7_start_row+3);   //Set the starting row coordinates
	send_oled_data(0);   //Set the starting column coordinates,
	send_oled_data(bar7_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63 - c_bar7);  //Set the finishing column coordinates
	send_oled_data(0);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	
	send_oled_data(0x22); // Enter the "draw rectangle mode" by execute the command 22h
	send_oled_data(bar7_start_row+3);   //Set the starting row coordinates
	send_oled_data(63 - c_bar7);   //Set the starting column coordinates,
	send_oled_data(bar7_end_row-3);  //Set the finishing row coordinates
	send_oled_data(63);  //Set the finishing column coordinates
	send_oled_data(0xff);   //Set the outline color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	send_oled_data(0xff);   //Set the filled color C, B and A
	send_oled_data(0);
	send_oled_data(0);
	_delay_ms(2);
	//==========================
}
//

