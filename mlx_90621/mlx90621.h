#ifndef MLX90621_H
#define MLX90621_H

#include <linux/i2c-dev.h>
#include <linux/i2c.h>


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stddef.h>

#include <math.h>

#include "mlx90621.h"

#include "bmp.h"

typedef union {
	struct {
		uint16_t REFRESH_RATE	: 4;  // 0 - 3
		uint16_t ADC_RES		: 2;  // 4 - 5
		uint16_t MEASURE_MODE	: 1;  // 6
		uint16_t OP_MODE		: 1;  // 7
		uint16_t NA				: 1;  // 8
		uint16_t IR_FLAG_RUN	: 1;  // 9
		uint16_t POR_MD			: 1;  // 10 
		uint16_t FM_MODE		: 1;  // 11
		uint16_t EEPROM_EN		: 1;  // 12
		uint16_t RESERVED_1		: 1;  // 13
		uint16_t ADC_REF		: 1;  // 14
		uint16_t RESERVED_2		: 1;  // 15
	};
	uint16_t VALUE;
} MLX90621_CONFIG_REG;

#define MLX_ADDR_EEPROM		0x50
#define MLX_ADDR_CONFIG		0x60

#define VTH_L 				0xDA
#define VTH_H 				0xDB

#define KT1_L 				0xDC
#define KT1_H 				0xDD
#define KT2_L 				0xDE
#define KT2_H 				0xDF

#define CAL_A0_L 			0xE0
#define CAL_A0_H 			0xE1
#define CAL_A0_SCALE 		0xE2
#define CAL_DELTA_A_SCALE	0xE3
#define CAL_EMIS_L 			0xE4
#define CAL_EMIS_H 			0xE5

#define KT_SCALE 			0xD2
#define OSC_TRIM_VALUE 		0xF7

#define CAL_ACOMMON_L 		0xD0
#define CAL_ACOMMON_H 		0xD1
#define CAL_ACP_L 			0xD3
#define CAL_ACP_H 			0xD4
#define CAL_BCP 			0xD5
#define CAL_alphaCP_L 		0xD6
#define CAL_alphaCP_H 		0xD7
#define CAL_TGC 			0xD8
#define CAL_AI_SCALE 		0xD9
#define CAL_BI_SCALE 		0xD9

#define T_MAX_LEVEL			150

extern float data_ir[64];

void IRService();
void IRService_vd();


#endif /* MLX90621_H */