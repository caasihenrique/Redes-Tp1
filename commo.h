#pragma once

#include<inttypes.h>

typedef struct {
    unsigned char bits:4;
}nibble;

typedef struct{
    unsigned char var:8;
}byte;

unsigned char bit_Code_to_char(nibble var);

nibble char_Code_to_bit(unsigned char var);

void encode16(unsigned char input, unsigned char* output);

unsigned char decode16(unsigned char* input);

