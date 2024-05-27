/*
 * audio_spectrometer.h
 *
 * Created: 3/21/2018 6:42:04 AM
 *  Author: Slaney
 */ 


#ifndef AUDIO_SPECTROMETER_H_
#define AUDIO_SPECTROMETER_H_

#define ADC_PIN		0

#define OLED_SCL	PB5
#define OLED_SDA	PB3
#define OLED_RES	PB1
#define OLED_DC		PB0

#define SA_STROBE   PC1
#define SA_RESET	PC2

#define MODE_SWITCH    PC3

#define OFFSET       192
#define DIVIDE        13
#define DELAY		1000

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define bar1_start_row 0
#define bar1_end_row 10
#define bar2_start_row 14
#define bar2_end_row 24
#define bar3_start_row 28
#define bar3_end_row 38
#define bar4_start_row 42
#define bar4_end_row 52
#define bar5_start_row 56
#define bar5_end_row 66
#define bar6_start_row 70
#define bar6_end_row 80
#define bar7_start_row 84
#define bar7_end_row 94

void read_sa(void);
void read_adc_avg(void);
void display_white_bars (void);
void display_full_white (void);
void display_blank (void);
void send_oled_data (unsigned char c_oled_data);
void display_elektor_logo (void);
void display_name (void);
void ADCinit();
uint16_t getADC(uint8_t Channel);
void display_green_yellow_red_bars (void);
void generate_colour_bar (int8_t c_start_row, int8_t c_end_row, int8_t c_bar);
void display_thin_blue_bars (void);
void display_thin_red_bars (void);



int16_t c_temp;
int16_t c_bar1;
int16_t c_bar2;
int16_t c_bar3;
int16_t c_bar4;
int16_t c_bar5;
int16_t c_bar6;
int16_t c_bar7;

uint8_t c_adc_count;
int16_t i_temp;
int mode = 1;
int button_pressed = 0;




#endif /* AUDIO SPECTROMETER_H_ */