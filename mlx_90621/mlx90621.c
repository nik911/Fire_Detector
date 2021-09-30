#include "mlx90621.h"

#define DEBUG 0

float data_ir[64];
float data_ir1[64];
int event_ir1;


void readEEPROM(int fd, char* bytes);
void readConfig(int fd,  uint16_t* reg);
void readTrimOSC(int fd, char* byte);

void writeConfig(int fd);
void writeTrimOSC(int fd, char level);

void readPTAT(int fd, uint16_t* ptat);
void readCPIX(int fd, int16_t* cpix);
void readIR(int fd, int16_t** ir_matrix, int rows_count, int columns_count);

//calc
float calculateTA(char* eepromData, MLX90621_CONFIG_REG config, uint16_t ptat);
int calculatePixTemp(char* eepromData, MLX90621_CONFIG_REG config, uint16_t cpix, float Tambient, int16_t** ir_matrix, float** temperatures, int rows_count, int columns_count, float* Tmin, float* Tmax, int term_max);

void drawbmp (char * filename);

void IRService() {

    char EEPROM_bytes[256] = {0x00};
    MLX90621_CONFIG_REG EEPROM_config;
    char EEPROM_trim;
    uint16_t ptat = 0x0000;
    uint16_t cpix = 0x0000;

    int rows_count = 4;
    int columns_count = 16;
    int rows_counter = 0;

    int16_t **ir_matrix = NULL;
    int16_t **temp = (int16_t**) malloc(sizeof(int16_t*) * rows_count);
    if (temp == NULL) {
        if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
        goto error;
    } else {
        ir_matrix = temp;
    }
    while (rows_counter != rows_count) {
        int16_t *temp = (int16_t*) calloc(columns_count, sizeof(int16_t));
        if (temp == NULL) {
            if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
            goto error;
        } else {
            ir_matrix[rows_counter] = temp;
        }
        rows_counter++;
    }
    rows_counter = 0;
    float **temperatures = NULL;
    float **t_temp = (float**) malloc(sizeof(float*) * rows_count);
    if (t_temp == NULL) {
        if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
        goto error;
    } else {
        temperatures = t_temp;
    }
    while (rows_counter != rows_count) {
        float *t_temp = (float*) calloc(columns_count, sizeof(float));
        if (t_temp == NULL) {
            if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
            goto error;
        } else {
            temperatures[rows_counter] = t_temp;
        }
        rows_counter++;
    }

    float* ir_packet = (float*) malloc(sizeof(float)*rows_count*columns_count);
    if (ir_packet == NULL) {
        if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
        goto error;
    }

    float Tmin = 0.0;
    float Tmax = 0.0;

    int fd;
    char *filename = "/dev/i2c-1";
    if ((fd = open(filename, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
        if (DEBUG) fprintf(stderr, "Failed to open the i2c bus");
        goto error;
    }
    usleep(5000);
    readEEPROM(fd, EEPROM_bytes);

    writeTrimOSC(fd, EEPROM_bytes[OSC_TRIM_VALUE]);
    readTrimOSC(fd, &EEPROM_trim);

    writeConfig(fd);

    int loop = 5;
    generateBMP();
    generateGradient();

    //while (loop) {
        readConfig(fd, &EEPROM_config.VALUE);
        if (EEPROM_config.POR_MD == 0) {
            if (DEBUG) fprintf(stderr, "POR\n");
            //break;
        }else{
            if (EEPROM_config.IR_FLAG_RUN) {
                if (DEBUG) fprintf(stderr, "IR_FLAG_RUN\n");
                readPTAT(fd, &ptat);
                readCPIX(fd, &cpix);
                readIR(fd, ir_matrix, rows_count, columns_count);

                float Tambient = 0.0;

                Tambient = calculateTA(EEPROM_bytes, EEPROM_config, ptat);
                //if (DEBUG) fprintf (stderr, "Ambient temperature: %.1f\n", Tambient);

                int term_max = 60;
                loop = calculatePixTemp(EEPROM_bytes, EEPROM_config, cpix, Tambient, ir_matrix, temperatures, rows_count, columns_count, &Tmin, &Tmax, term_max);

                //fprintf(stderr, "size temp_pointer%d\n", sizeof(temp_pointer));


                //
                //if(DEBUG) generateFields(temperatures ,4, 16, 45, 45, 5);
            }
        }


        usleep(5000);
    //}

    uint32_t  	ir_data_length;
    uint8_t*	ir_data;

    float* temp_ir_packet_pointer = ir_packet;
    int k = 0;
    for(int i=0; i<rows_count; i++) {
        for(int j=0; j<columns_count; j++) {
            *((temp_ir_packet_pointer)+k) = temperatures[i][j];
            k++;
        }
    }

    ir_data_length = sizeof(float)*rows_count*columns_count;
    ir_data = (unsigned char*)ir_packet;

    int packet_size = 0;
    packet_size += sizeof(ir_data_length);
    packet_size += ir_data_length;

    // alloc
    unsigned char* udp_packet = (unsigned char*) calloc(packet_size, sizeof(unsigned char));
    // fill
    int length = 0;
    unsigned char* temp_pointer = udp_packet;

    ///float data_ir[64];
    memcpy (temp_pointer, ir_data, ir_data_length);
    for(int i=0; i<64; i++) {
        //fprintf(stderr, "%.1f  ", (((float*)(uint32_t)temp_pointer))[i] );
        data_ir1[i] = (((float*)(uint32_t)temp_pointer))[i];
    }

    /* for(int i=0; i<64; i++) {
         fprintf(stderr, "%.1f  ", data_ir[i]);
     }*/



    if(udp_packet) free(udp_packet);

    error:
    close(fd);
    rows_counter = rows_count;
    while (rows_counter--) {
        if(ir_matrix[rows_counter]) free(ir_matrix[rows_counter]);
        if(temperatures[rows_counter]) free(temperatures[rows_counter]);
    }
    if (ir_matrix) free(ir_matrix);
    if (temperatures) free(temperatures);
}



void IRService_vd() {

    char EEPROM_bytes[256] = {0x00};
    MLX90621_CONFIG_REG EEPROM_config;
    char EEPROM_trim;
    uint16_t ptat = 0x0000;
    uint16_t cpix = 0x0000;

    int rows_count = 4;
    int columns_count = 16;
    int rows_counter = 0;

    int16_t **ir_matrix = NULL;
    int16_t **temp = (int16_t**) malloc(sizeof(int16_t*) * rows_count);
    if (temp == NULL) {
        if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
        goto error;
    } else {
        ir_matrix = temp;
    }
    while (rows_counter != rows_count) {
        int16_t *temp = (int16_t*) calloc(columns_count, sizeof(int16_t));
        if (temp == NULL) {
            if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
            goto error;
        } else {
            ir_matrix[rows_counter] = temp;
        }
        rows_counter++;
    }
    rows_counter = 0;
    float **temperatures = NULL;
    float **t_temp = (float**) malloc(sizeof(float*) * rows_count);
    if (t_temp == NULL) {
        if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
        goto error;
    } else {
        temperatures = t_temp;
    }
    while (rows_counter != rows_count) {
        float *t_temp = (float*) calloc(columns_count, sizeof(float));
        if (t_temp == NULL) {
            if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
            goto error;
        } else {
            temperatures[rows_counter] = t_temp;
        }
        rows_counter++;
    }

    float* ir_packet = (float*) malloc(sizeof(float)*rows_count*columns_count);
    if (ir_packet == NULL) {
        if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
        goto error;
    }

    float Tmin = 0.0;
    float Tmax = 0.0;

    int fd;
    char *filename = "/dev/i2c-1";
    if ((fd = open(filename, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
        if (DEBUG) fprintf(stderr, "Failed to open the i2c bus");
        goto error;
    }
    usleep(5000);
    readEEPROM(fd, EEPROM_bytes);

    writeTrimOSC(fd, EEPROM_bytes[OSC_TRIM_VALUE]);
    readTrimOSC(fd, &EEPROM_trim);

    writeConfig(fd);

    int loop = 5;
    generateBMP();
    generateGradient();

    while (loop) {
        readConfig(fd, &EEPROM_config.VALUE);
        if (EEPROM_config.POR_MD == 0) {
            if (DEBUG) fprintf(stderr, "POR\n");
            break;
        }
        if (EEPROM_config.IR_FLAG_RUN) {
            if (DEBUG) fprintf(stderr, "IR_FLAG_RUN\n");
            readPTAT(fd, &ptat);
            readCPIX(fd, &cpix);
            readIR(fd, ir_matrix, rows_count, columns_count);

            float Tambient = 0.0;

            Tambient = calculateTA(EEPROM_bytes, EEPROM_config, ptat);
            //if (DEBUG) fprintf (stderr, "Ambient temperature: %.1f\n", Tambient);

            int term_max = 10;
            loop = calculatePixTemp(EEPROM_bytes, EEPROM_config, cpix, Tambient, ir_matrix, temperatures, rows_count, columns_count, &Tmin, &Tmax, term_max);

            //fprintf(stderr, "size temp_pointer%d\n", sizeof(temp_pointer));


            //
            //if(DEBUG) generateFields(temperatures ,4, 16, 45, 45, 5);
        }

        usleep(50000);
    }

    uint32_t  	ir_data_length;
    uint8_t*	ir_data;

    float* temp_ir_packet_pointer = ir_packet;
    int k = 0;
    for(int i=0; i<rows_count; i++) {
        for(int j=0; j<columns_count; j++) {
            *((temp_ir_packet_pointer)+k) = temperatures[i][j];
            k++;
        }
    }

    ir_data_length = sizeof(float)*rows_count*columns_count;
    ir_data = (unsigned char*)ir_packet;

    int packet_size = 0;
    packet_size += sizeof(ir_data_length);
    packet_size += ir_data_length;

    // alloc
    unsigned char* udp_packet = (unsigned char*) calloc(packet_size, sizeof(unsigned char));
    // fill
    int length = 0;
    unsigned char* temp_pointer = udp_packet;

    ///float data_ir[64];
    memcpy (temp_pointer, ir_data, ir_data_length);
    for(int i=0; i<64; i++) {
        //fprintf(stderr, "%.1f  ", (((float*)(uint32_t)temp_pointer))[i] );
        data_ir[i] = (((float*)(uint32_t)temp_pointer))[i];
    }

   /* for(int i=0; i<64; i++) {
        fprintf(stderr, "%.1f  ", data_ir[i]);
    }*/



    if(udp_packet) free(udp_packet);

    error:
    close(fd);
    rows_counter = rows_count;
    while (rows_counter--) {
        if(ir_matrix[rows_counter]) free(ir_matrix[rows_counter]);
        if(temperatures[rows_counter]) free(temperatures[rows_counter]);
    }
    if (ir_matrix) free(ir_matrix);
    if (temperatures) free(temperatures);
}



void readEEPROM(int fd, char* bytes) {
 //	char* err;
 // if (DEBUG) fprintf(stderr, "MLX EEPROM read\n");
 // if (ioctl(fd, I2C_SLAVE, MLX_ADDR_EEPROM) < 0) {
 //        if (DEBUG) fprintf(stderr, "Failed to acquire bus access and/or talk to EEPROM.\n");
 //        /* ERROR HANDLING; you can check errno to see what went wrong */
 //        //return 0;
 //    }

 //    if (read(fd, bytes, 256) != 256) {
 //    	/* ERROR HANDLING: i2c transaction failed */
 //        if (DEBUG) fprintf(stderr, "Failed to read from the i2c bus.\n");
 //        err = strerror(errno);
 //        if (DEBUG) fprintf(stderr, "%s\n", err);
 //        //return 0;
 //    } else {
 //    	int j = 0;
 //    		if (DEBUG) fprintf(stderr, "0x00 dAi:\n");
 //    	for (int i=0; i<256; i++) {

 //    		if (DEBUG) fprintf(stderr, "%02X ",bytes[i]);
 //    		if (i==0x3F) if (DEBUG) fprintf(stderr, "\n0x40 Bi:");
 //    		if (i==0x7F) if (DEBUG) fprintf(stderr, "\n0x80 dAlpha:");
 //    	    if (i==0xBF) if (DEBUG) fprintf(stderr, "\n0xC0 K:");
 //    	    if (i==0xCF) if (DEBUG) fprintf(stderr, "\n0xD0 CP:");
 //    	    if (i==0xDF) if (DEBUG) fprintf(stderr, "\n0xE0 Common:");

 //    		j++;
 // 			if (j==8) {
 // 				if (DEBUG) fprintf(stderr, "\n");
 // 				j=0;
 // 			}
 //    	}
 //    	if (DEBUG) fprintf(stderr, "\n");
 //    }
	char* err;

    char outbuf[1] = {0x00};
    char inbuf[256] = {0x00};

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg msgs[2];

    msgs[0].addr  = MLX_ADDR_EEPROM;
    msgs[0].flags = 0;
    msgs[0].len   = sizeof(outbuf);
    msgs[0].buf   = outbuf;

    msgs[1].addr  = MLX_ADDR_EEPROM;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len   = sizeof(inbuf);
    msgs[1].buf   = inbuf;

    packets.msgs      = msgs;
    packets.nmsgs     = 2;
    if(ioctl(fd, I2C_RDWR, &packets) < 0) {
    	if (DEBUG) fprintf(stderr, "Failed to read from the i2c bus.\n");
        err = strerror(errno);
        if (DEBUG) fprintf(stderr, "%s\n", err);
    } else {
    	int j = 0;
    	if (DEBUG) fprintf(stderr, "0x00 dAi:\n");
    	for (int i=0; i<256; i++) {
    		bytes[i] = inbuf[i];
    		if (DEBUG) fprintf(stderr, "%02X ",bytes[i]);
    		if (i==0x3F) if (DEBUG) fprintf(stderr, "\n0x40 Bi:");
    		if (i==0x7F) if (DEBUG) fprintf(stderr, "\n0x80 dAlpha:");
    	    if (i==0xBF) if (DEBUG) fprintf(stderr, "\n0xC0 K:");
    	    if (i==0xCF) if (DEBUG) fprintf(stderr, "\n0xD0 CP:");
    	    if (i==0xDF) if (DEBUG) fprintf(stderr, "\n0xE0 Common:");

    		j++;
 			if (j==8) {
 				if (DEBUG) fprintf(stderr, "\n");
 				j=0;
 			}
    	}
    	if (DEBUG) fprintf(stderr, "\n");
    }
}

void readConfig(int fd, uint16_t* reg) { // 0x60 0x02 0x92 0x00 0x01
	char* err;

    char outbuf[4] = {0x02, 0x92, 0x00, 0x01};
    char inbuf[2] = {0x00};

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg msgs[2];

    msgs[0].addr  = MLX_ADDR_CONFIG;
    msgs[0].flags = 0;
    msgs[0].len   = sizeof(outbuf);
    msgs[0].buf   = outbuf;

    msgs[1].addr  = MLX_ADDR_CONFIG;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len   = sizeof(inbuf);
    msgs[1].buf   = inbuf;

    packets.msgs      = msgs;
    packets.nmsgs     = 2;
    if(ioctl(fd, I2C_RDWR, &packets) < 0) {
    	if (DEBUG) fprintf(stderr, "Failed to read config register.\n");
        err = strerror(errno);
        if (DEBUG) fprintf(stderr, "%s\n", err);
    } else {
    	if (DEBUG) fprintf(stderr, "Configuration register: ");

    	*reg = inbuf[1] << 8 | inbuf[0];

    	if (DEBUG) fprintf(stderr, "%02X\n", *reg);
    }
}

void readTrimOSC(int fd, char* byte) { // 0x60 0x02 0x93 0x00 0x01
	char* err;

    char outbuf[4] = {0x02, 0x93, 0x00, 0x01};
    char inbuf[2] = {0x00};

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg msgs[2];

    msgs[0].addr  = MLX_ADDR_CONFIG;
    msgs[0].flags = 0;
    msgs[0].len   = sizeof(outbuf);
    msgs[0].buf   = outbuf;

    msgs[1].addr  = MLX_ADDR_CONFIG;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len   = sizeof(inbuf);
    msgs[1].buf   = inbuf;

    packets.msgs      = msgs;
    packets.nmsgs     = 2;
    if(ioctl(fd, I2C_RDWR, &packets) < 0) {
    	if (DEBUG) fprintf(stderr, "Failed to read config register.\n");
        err = strerror(errno);
        if (DEBUG) fprintf(stderr, "%s\n", err);
    } else {
    	*byte = inbuf[0];
    	if (DEBUG) fprintf(stderr, "Trim OSC register:  ");
    	if (DEBUG) fprintf(stderr, "%02X\n", *byte);
    }
}

void writeConfig(int fd) {
	char* err;

	MLX90621_CONFIG_REG config;

	config.VALUE = 0x0000;

	config.REFRESH_RATE	= 0x0E;
	config.ADC_RES		= 0x03;
	config.MEASURE_MODE	= 0;
	config.OP_MODE		= 0;
	config.POR_MD		= 1;
	config.FM_MODE		= 1;
	config.EEPROM_EN	= 0;
	config.ADC_REF		= 1;

	if (DEBUG) fprintf(stderr, "config to write %02X \n", config.VALUE);

	char outbuf[5] = {0x03, (uint8_t)config.VALUE - 0x55, (uint8_t)config.VALUE, (uint8_t)(config.VALUE >> 8) - 0x55, (uint8_t)(config.VALUE >> 8)};

	struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg msgs[1];

    msgs[0].addr  = MLX_ADDR_CONFIG;
    msgs[0].flags = 0;
    msgs[0].len   = sizeof(outbuf);
    msgs[0].buf   = outbuf;

    packets.msgs  = msgs;
    packets.nmsgs = 1;
    if(ioctl(fd, I2C_RDWR, &packets) < 0) {
        if (DEBUG) fprintf(stderr, "Failed to write config register.\n");
        err = strerror(errno);
        if (DEBUG) fprintf(stderr, "%s\n", err);
    }
}

void writeTrimOSC(int fd, char level) { // 0x60 0x04 level-0xAA level 0x56 0x00
	char* err;

	if (DEBUG) fprintf(stderr, "set trim osc level %02X \n", level);

	char outbuf[5] = {0x04, level - 0xAA, level, 0x56, 0x00};

	struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg msgs[1];

    msgs[0].addr  = MLX_ADDR_CONFIG;
    msgs[0].flags = 0;
    msgs[0].len   = sizeof(outbuf);
    msgs[0].buf   = outbuf;

    packets.msgs  = msgs;
    packets.nmsgs = 1;
    if(ioctl(fd, I2C_RDWR, &packets) < 0) {
        if (DEBUG) fprintf(stderr, "Failed to write trim osc register.\n");
        err = strerror(errno);
        if (DEBUG) fprintf(stderr, "%s\n", err);
    }
}

void readPTAT(int fd, uint16_t* ptat) { // 0x60 0x02 0x40 0x00 0x01
	char* err;

    char outbuf[4] = {0x02, 0x40, 0x00, 0x01};
    char inbuf[2] = {0x00};

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg msgs[2];

    msgs[0].addr  = MLX_ADDR_CONFIG;
    msgs[0].flags = 0;
    msgs[0].len   = sizeof(outbuf);
    msgs[0].buf   = outbuf;

    msgs[1].addr  = MLX_ADDR_CONFIG;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len   = sizeof(inbuf);
    msgs[1].buf   = inbuf;

    packets.msgs      = msgs;
    packets.nmsgs     = 2;
    if(ioctl(fd, I2C_RDWR, &packets) < 0) {
    	if (DEBUG) fprintf(stderr, "Failed to read config register.\n");
        err = strerror(errno);
        if (DEBUG) fprintf(stderr, "%s\n", err);
    } else {
    	*ptat = inbuf[1] << 8 | inbuf[0];

    	if (DEBUG) fprintf(stderr, "PTAT: %02X\n", *ptat);
    }
}

void readCPIX(int fd, int16_t* cpix) { // 0x60 0x02 0x41 0x00 0x01
	char* err;

    char outbuf[4] = {0x02, 0x41, 0x00, 0x01};
    char inbuf[2] = {0x00};

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg msgs[2];

    msgs[0].addr  = MLX_ADDR_CONFIG;
    msgs[0].flags = 0;
    msgs[0].len   = sizeof(outbuf);
    msgs[0].buf   = outbuf;

    msgs[1].addr  = MLX_ADDR_CONFIG;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len   = sizeof(inbuf);
    msgs[1].buf   = inbuf;

    packets.msgs      = msgs;
    packets.nmsgs     = 2;
    if(ioctl(fd, I2C_RDWR, &packets) < 0) {
    	if (DEBUG) fprintf(stderr, "Failed to read config register.\n");
        err = strerror(errno);
        if (DEBUG) fprintf(stderr, "%s\n", err);
    } else {
    	*cpix = (int16_t)(inbuf[1] << 8 | inbuf[0]);
    	if (*cpix >= 32768) *cpix -= 65536;
    	if (DEBUG) fprintf(stderr, "CPIX: %02X\n", *cpix);
    }
}

void readIR(int fd, int16_t** ir_matrix, int rows_count, int columns_count) { // 0x60 0x02 0x00 0x01 0x40
	char* err;

    char outbuf[4] = {0x02, 0x00, 0x01, 0x40};
    char inbuf[128] = {0x00};

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg msgs[2];

    msgs[0].addr  = MLX_ADDR_CONFIG;
    msgs[0].flags = 0;
    msgs[0].len   = sizeof(outbuf);
    msgs[0].buf   = outbuf;

    msgs[1].addr  = MLX_ADDR_CONFIG;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len   = sizeof(inbuf);
    msgs[1].buf   = inbuf;

    packets.msgs      = msgs;
    packets.nmsgs     = 2;
    if(ioctl(fd, I2C_RDWR, &packets) < 0) {
    	if (DEBUG) fprintf(stderr, "Failed to read config register.\n");
        err = strerror(errno);
        if (DEBUG) fprintf(stderr, "%s\n", err);
    } else {
    	if (DEBUG) fprintf(stderr, "IR_data:\n");
    	int index = 0;
    	for (int i = 0; i < rows_count; i++) {
    		for (int j = 0; j < columns_count; j++) {
    			ir_matrix[i][j] = (int16_t)(inbuf[index+1]<<8 | inbuf[index]);
    			index+=2;
    		}
    	}

    	for (int i = 0; i < rows_count; i++) {
    		for (int j = 0; j < columns_count; j++) {
    			if (DEBUG) fprintf(stderr, "%02X ",ir_matrix[i][j] );
    		}
    		if (DEBUG) fprintf(stderr, "\n");
    	}
    	if (DEBUG) fprintf(stderr, "\n");
    }
}

float calculateTA(char* eepromData, MLX90621_CONFIG_REG config, uint16_t ptat) {
	int16_t k_t1_scale = 0;
	int16_t k_t2_scale = 0;

	float v_th = 0.0;

	float k_t1 = 0.0;
	float k_t2 = 0.0;

	float Tambient = 0.0;

	k_t1_scale = (int16_t) (eepromData[KT_SCALE] & 0xF0) >> 4;
	k_t2_scale = (int16_t) (eepromData[KT_SCALE] & 0x0F) + 10;

	v_th = (float) 256 * eepromData[VTH_H] + eepromData[VTH_L];
	if (v_th >= 32768.0) v_th -= 65536.0;
	v_th = v_th / pow(2, (3 - config.ADC_RES));
	k_t1 = (float) 256 * eepromData[KT1_H] + eepromData[KT1_L];
	if (k_t1 >= 32768.0) k_t1 -= 65536.0;
	k_t1 /= (pow(2, k_t1_scale) * pow(2, (3 - config.ADC_RES)));
	k_t2 = (float) 256 * eepromData[KT2_H] + eepromData[KT2_L];
	if (k_t2 >= 32768.0) k_t2 -= 65536.0;
	k_t2 /= (pow(2, k_t2_scale) * pow(2, (3 - config.ADC_RES)));
	Tambient = ((-k_t1 + sqrt(pow(k_t1, 2) - (4 * k_t2 * (v_th - (float) ptat)))) / (2 * k_t2)) + 25.0;

	return Tambient;
}

int calculatePixTemp(char* eepromData, MLX90621_CONFIG_REG config, uint16_t cpix, float Tambient, int16_t** ir_matrix, float** temperatures, int rows_count, int columns_count, float* Tmin, float* Tmax, int term_max) {
	float a_ij[64], b_ij[64], alpha_ij[64];
	float emissivity, tgc, alpha_cp, a_cp, b_cp;
	int16_t a_common, a_i_scale, b_i_scale;

	emissivity = (256 * eepromData[CAL_EMIS_H] + eepromData[CAL_EMIS_L]) / 32768.0;

	if (DEBUG) fprintf(stderr, "Emissivity %.1f\n", emissivity);

	a_common = (int16_t) 256 * eepromData[CAL_ACOMMON_H] + eepromData[CAL_ACOMMON_L];
	if (a_common >= 32768) a_common -= 65536;
	alpha_cp = (256 * eepromData[CAL_alphaCP_H] + eepromData[CAL_alphaCP_L]) / (pow(2, CAL_A0_SCALE) * pow(2, (3 - config.ADC_RES)));
	a_i_scale = (int16_t) (eepromData[CAL_AI_SCALE] & 0xF0) >> 4;
	b_i_scale = (int16_t) eepromData[CAL_BI_SCALE] & 0x0F;
	a_cp = (float) 256 * eepromData[CAL_ACP_H] + eepromData[CAL_ACP_L];
	if (a_cp >= 32768.0) a_cp -= 65536.0;
	a_cp /= pow(2, (3 - config.ADC_RES));
	b_cp = (float) eepromData[CAL_BCP];
	if (b_cp > 127.0) b_cp -= 256.0;
	b_cp /= (pow(2, b_i_scale) * pow(2, (3 - config.ADC_RES)));
	tgc = (float) eepromData[CAL_TGC];
	if (tgc > 127.0)  tgc -= 256.0;
	tgc /= 32.0;
	float v_cp_off_comp = (float) cpix - (a_cp + b_cp * (Tambient - 25.0));
	float v_ir_off_comp, v_ir_tgc_comp, v_ir_norm, v_ir_comp;
	*Tmin = 1000;
	*Tmax = -1000;
	int index = 0;


	if (DEBUG) fprintf(stderr, "Temperatures: \n");
	for (int i = 0; i < rows_count; i++) {
		for (int j = 0; j < columns_count; j++) {

			a_ij[index] = ((float)a_common + eepromData[index] * pow(2, a_i_scale)) / pow(2, (3 - config.ADC_RES));
				//if (DEBUG) fprintf(stderr, "%.1f  ", a_ij[index]
				//((float)a_common + eepromData[index] * pow(2, a_i_scale)) / pow(2, (3 - config.ADC_RES)));
			b_ij[index] = eepromData[0x40 + index];
			if (b_ij[index] > 127) b_ij[index] -= 256;
				//if (DEBUG) fprintf(stderr, "%.1f  ", b_ij[index]);
			b_ij[index] /= (pow(2, b_i_scale) * pow(2, (3 - config.ADC_RES)));
				//if (DEBUG) fprintf(stderr, "%.4f  ", (pow(2, b_i_scale) * pow(2, (3 - config.ADC_RES))));
				//if (DEBUG) fprintf(stderr, "%.4f  ", b_ij[index]);
			v_ir_off_comp = ir_matrix[i][j] - (a_ij[index] + b_ij[index] * (Tambient - 25.0));
				//if (DEBUG) fprintf(stderr, "%.1f  ", v_ir_off_comp);
			v_ir_tgc_comp = v_ir_off_comp - tgc * v_cp_off_comp;
				//if (DEBUG) fprintf(stderr, "%.1f  ", v_ir_tgc_comp);
			alpha_ij[index] = ((256 * eepromData[CAL_A0_H] + eepromData[CAL_A0_L]) / pow(2, eepromData[CAL_A0_SCALE]));
				//if (DEBUG) fprintf(stderr, "%.1f  ", alpha_ij[index]);
			alpha_ij[index] = alpha_ij[index] + (eepromData[0x80 + index] / pow(2, eepromData[CAL_DELTA_A_SCALE]));
				//if (DEBUG) fprintf(stderr, "%.1f  ", alpha_ij[index]);
			alpha_ij[index] /= pow(2, 3 - config.ADC_RES);
				//if (DEBUG) fprintf(stderr, "%.1f  ", alpha_ij[index]);

			v_ir_norm = v_ir_tgc_comp / (alpha_ij[index] - tgc * alpha_cp);
				//if (DEBUG) fprintf(stderr, "%.1f  ", v_ir_norm);
			v_ir_comp = v_ir_norm / emissivity;
			 	//if (DEBUG) fprintf(stderr, "%.1f  ", v_ir_comp);

			temperatures[i][j] = sqrt(sqrt((v_ir_comp + pow((Tambient + 273.15), 4)))) - 273.15;

			if (temperatures[i][j] < *Tmin) *Tmin = temperatures[i][j];
			if (temperatures[i][j] > *Tmax) *Tmax = temperatures[i][j];
			index++;
			if (DEBUG) fprintf(stderr, "%.1f ",temperatures[i][j]);


		}
		if (DEBUG) fprintf(stderr, "\n");
	}
	if (DEBUG) fprintf(stderr, "Tmin: %.1f, Tmax %.1f \n", *Tmin, *Tmax);

	if (*Tmax > term_max) {
	    event_ir1 = 1;
	    fprintf(stderr, "IR: temperature level has exceeded %d°C: %.1f\n", term_max, *Tmax);
        if(!DEBUG) generateFields(temperatures ,4, 16, 45, 45, 5);
        return 0;
    }
	return 1;
}

