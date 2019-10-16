#pragma O0

#include "bmp085.h"
#include "main.h"
#include <math.h>

struct bmp085_result resul1;
int32_t B5;
int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
uint16_t UT, AC4, AC5, AC6;
int32_t X1, X2, X3, B3, B6, UP, p, T;
uint32_t B4, B7;

int32_t get_temperature_convert(uint8_t* i2c_buffer, uint8_t* temperature) {
	uint8_t convert[2];
	convert[0] = temperature[1];
	convert[1] = temperature[0];
	UT = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[1];
	convert[1] = i2c_buffer[0];
	AC1 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[3];
	convert[1] = i2c_buffer[2];
	AC2 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[5];
	convert[1] = i2c_buffer[4];
	AC3 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[7];
	convert[1] = i2c_buffer[6];
	AC4 = *((uint16_t *)&convert[0]);
	convert[0] = i2c_buffer[9];
	convert[1] = i2c_buffer[8];
	AC5 = *((uint16_t *)&convert[0]);
	convert[0] = i2c_buffer[11];
	convert[1] = i2c_buffer[10];
	AC6 = *((uint16_t *)&convert[0]);
	convert[0] = i2c_buffer[13];
	convert[1] = i2c_buffer[12];
	B1 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[15];
	convert[1] = i2c_buffer[14];
	B2 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[17];
	convert[1] = i2c_buffer[16];
	MB = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[19];
	convert[1] = i2c_buffer[18];
	MC = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[21];
	convert[1] = i2c_buffer[20];
	MD = *((int16_t *)&convert[0]);

	X1 = (UT - AC6) * AC5 / 32768;
	X2 = MC * 2048 / (X1 + MD);
	B5 = X1 + X2;
	T = (B5 + 8) / 16;
	return T;
}

int32_t get_pressure_convert(uint8_t* i2c_buffer, uint8_t* pressure) {
	uint8_t convert[4];
	float p_rt_st;
	convert[0] = pressure[2];
	convert[1] = pressure[1];
	convert[2] = pressure[0];
	convert[3] = 0x00;
	UP = ( *((int32_t *)&convert[0])) >> (8 - OSS);
	convert[0] = i2c_buffer[1];
	convert[1] = i2c_buffer[0];
	AC1 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[3];
	convert[1] = i2c_buffer[2];
	AC2 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[5];
	convert[1] = i2c_buffer[4];
	AC3 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[7];
	convert[1] = i2c_buffer[6];
	AC4 = *((uint16_t *)&convert[0]);
	convert[0] = i2c_buffer[9];
	convert[1] = i2c_buffer[8];
	AC5 = *((uint16_t *)&convert[0]);
	convert[0] = i2c_buffer[11];
	convert[1] = i2c_buffer[10];
	AC6 = *((uint16_t *)&convert[0]);
	convert[0] = i2c_buffer[13];
	convert[1] = i2c_buffer[12];
	B1 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[15];
	convert[1] = i2c_buffer[14];
	B2 = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[17];
	convert[1] = i2c_buffer[16];
	MB = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[19];
	convert[1] = i2c_buffer[18];
	MC = *((int16_t *)&convert[0]);
	convert[0] = i2c_buffer[21];
	convert[1] = i2c_buffer[20];
	MD = *((int16_t *)&convert[0]);
	
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6 / 4096)) /  2048;
	X2 = AC2 * B6 / 2048;
	X3 = X1 + X2;
	B3 = (((AC1 * 4 + X3) << OSS) + 2) / 4;
	X1 = AC3 * B6 / 8192;
	X2 = (B1 * (B6 * B6 / 4096)) / 65536;
	X3 = ((X1 + X2) + 2) / 4;
	B4 = AC4 * (uint32_t)(X3 + 32768) / 32768;
	B7 = ((uint32_t)UP - B3) * (50000 >> OSS);
	if (B7 < 0x80000000) p = (B7 * 2)/ B4;
		else p = (B7 / B4 ) * 2;
	X1 = (p / 256) * ( p / 256);
	X1 = ( X1 * 3038) / 65536;
	X2 = ( -7357 * p) / 65536;
	p = p + (X1 + X2 + 3791) / 16;
	p_rt_st = p * 0.00750062;
	return p;
}

uint8_t bmp085_get_data(I2C_HandleTypeDef *hi2c, struct bmp085_result *result) {

	uint8_t bmp085_callibrate_f[24], i2c_write_f[10], i2c_temperature_f[2], i2c_pressure_f[3];
	int32_t pressure_f;
	
	/* read callibrate data */
	HAL_I2C_Mem_Read(hi2c, BMP085_I2C_ADDRESS, BMP085_CALLIBRATION_ADDRESS, I2C_MEMADD_SIZE_8BIT, bmp085_callibrate_f, 22, HAL_MAX_DELAY);	
	/* read temperature */
	i2c_write_f[0] = 0x2E;
	HAL_I2C_Mem_Write(hi2c, BMP085_I2C_ADDRESS, BMP085_COMMAND_ADDRESS, I2C_MEMADD_SIZE_8BIT, i2c_write_f, 1, HAL_MAX_DELAY);
	//delay_timer(5);
	for(uint32_t in = 0;in<4500;in++); // wait 4.5 ms
	HAL_I2C_Mem_Read(hi2c, BMP085_I2C_ADDRESS, BMP085_DATA_ADDRESS, I2C_MEMADD_SIZE_8BIT, i2c_temperature_f, 2, HAL_MAX_DELAY);
	result->t_cel = get_temperature_convert(bmp085_callibrate_f, i2c_temperature_f);
	/* read pressure */
	i2c_write_f[0] = 0x34 + (OSS << 6);
	HAL_I2C_Mem_Write(hi2c, BMP085_I2C_ADDRESS, BMP085_COMMAND_ADDRESS, I2C_MEMADD_SIZE_8BIT, i2c_write_f, 1, HAL_MAX_DELAY);
	//delay_timer(5);
	for(uint32_t in = 0;in<4500;in++); // wait 4.5 ms
	HAL_I2C_Mem_Read(hi2c, BMP085_I2C_ADDRESS, BMP085_DATA_ADDRESS, I2C_MEMADD_SIZE_8BIT, i2c_pressure_f, 3, HAL_MAX_DELAY);
	pressure_f = get_pressure_convert(bmp085_callibrate_f, i2c_pressure_f);
	result->p_mm_rt_st = pressure_f * 0.00750062;
	result->p0_mm_rt_st = result->p_mm_rt_st / 0.98094479;
	return 0;
}

