#ifndef MBED_PINNAMES_H
#define MBED_PINNAMES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	P20, P21, P22, P23, P24, P25, P32, P33, PC2, PC3, PC4, PC5, PC6, PC7,
	P50, P52, PA0, PA1, PA2, PA6, PA7,
	AN0, AN1, AN2, AN3, AN4, AN5, AN6, AN7,

	D0 = P21,
	D1 = P20,
	D2 = P22,
	D3 = P23,
	D4 = P24,
	D5 = P25,
	D6 = P32,
	D7 = P33,
	D8 = PC2,
	D9 = PC3,
	D10 = PC4,
	D11 = PC6,
	D12 = PC7,
	D13 = PC5,

	A0 = AN0,
	A1 = AN1,
	A2 = AN2,
	A3 = AN3,
	A4 = AN4,
	A5 = AN5,
	A6 = AN6,
	A7 = AN7,

	I2C_SCL = P52,
	I2C_SDA = P50,

	PIN_LED0 = PA0,
	PIN_LED1 = PA1,
	PIN_LED2 = PA2,
	PIN_LED3 = PA6,
	PIN_SW = PA7,

	NC = (int)0xFFFFFFFF
} PinName;

#define LED_RED			PIN_LED0
#define LED_GREEN		PIN_LED1
#define LED_BLUE		PIN_LED2
#define LED_USER		PIN_LED3
#define USER_BUTTON0	PIN_SW

typedef enum {
	PIN_INPUT,
	PIN_OUTPUT
} PinDirection;

typedef enum {
	PullUp = 2,
	PullDown = 3,
	PullNone = 0,
	OpenDrain = 4,
	PullDefault = PullNone
} PinMode;

#ifdef __cplusplus
}
#endif

#endif
