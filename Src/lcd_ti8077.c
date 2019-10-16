#include "main.h"

/* show digit on LCD Display */
void lcd_digit(LCD_HandleTypeDef* lcd, uint8_t digit, uint8_t number, uint8_t mask) {
	/*
			a - bit1, com0
			b - bit2, com0
			f - bit0, com0
	
			c - bit2, com1
			e - bit0, com1
			g - bit1, com1
	
			d - bit1, com2
			h - bit0, com2
			p - bit2, com2 
	*****************************
		0 - abcdef
		1 - bc
		2 - abdeg
		3 - abcdg
		4 - bcfg
		5 - acdfg
		6 - acdefg
		7 - abc
		8 - abcdefgh
		9 - abcdfg */
	if (digit > 14) return;
	const uint8_t mat0[15] = {0x07, 0x04, 0x06, 0x06, 0x05, 0x03, 0x03, 0x06, 0x07, 0x07,
	0x00,	// point
	0x00, // galka
	0x07, // P
	0x00, // - 
	0x03};// C
	const uint8_t mat1[15] = {0x05, 0x04, 0x03, 0x06, 0x06, 0x06, 0x07, 0x04, 0x07, 0x06,
	0x00,
	0x00,
	0x03,
	0x02,
	0x01};
	const uint8_t mat2[15] = {0x02, 0x00, 0x02, 0x02, 0x00, 0x02, 0x02, 0x00, 0x02, 0x02,
	0x04,
	0x01,
	0x00,
	0x00,
	0x02};
	uint32_t tmp_digit;
	uint32_t tmp_mask;
	uint32_t tmp_sdvig = 0x07;

	tmp_digit = mat0[digit]<<(number*3);
	if(mask == LCD_OFF_MUSK) tmp_mask = ~(0x00000007<<(number*3));
	else tmp_mask = ~tmp_digit;
	HAL_LCD_Write(lcd, LCD_RAM_REGISTER0, tmp_mask, tmp_digit);

	tmp_digit = mat1[digit]<<(number*3);
	if(mask == LCD_OFF_MUSK) tmp_mask = ~(0x00000007<<(number*3));
	else tmp_mask = ~tmp_digit;
	HAL_LCD_Write(lcd, LCD_RAM_REGISTER2, tmp_mask, tmp_digit);

	tmp_digit = mat2[digit]<<(number*3);
	if(mask == LCD_OFF_MUSK) tmp_mask = ~(0x00000007<<(number*3));
	else tmp_mask = ~tmp_digit;
	HAL_LCD_Write(lcd, LCD_RAM_REGISTER4, tmp_mask, tmp_digit);
}
