#pragma once

#include<inttypes.h>

typedef struct {
    unsigned char bits:4;
}nibble;

typedef struct{
    unsigned char var:8;
}byte;

typedef struct{
    unsigned short int vetor:8;
}short_int;

unsigned char bit_Code_to_char(nibble var);

nibble char_Code_to_bit(unsigned char var);

void encode16(unsigned char input, unsigned char* output);

unsigned char decode16(unsigned char* input);

void short_to_char(unsigned short int size, unsigned char* duo);

uint16_t checksum(void *data, unsigned int bytes);