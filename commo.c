#include<stdio.h>
#include<stdlib.h>

typedef struct {
    unsigned char bits:4;
}nibble;

 unsigned char bit_Code_to_char(nibble var){
    unsigned char output;
    switch (var.bits)
    {
    case 0:
        output = '0';
        break;
    case 1:
        output = '1';
        break;
    case 2:
        output = '2';
        break;
    case 3:
        output = '3';
        break;
    case 4:
        output = '4';
        break;
    case 5:
        output = '5';
        break;
    case 6:
        output = '6';
        break;
    case 7:
        output = '7';
        break;
    case 8:
        output = '8';
        break;
    case 9:
        output = '9';
        break;
    case 10:
        output = 'A';
        break;
    case 11:
        output = 'B';
        break;
    case 12:
        output = 'C';
        break;
    case 13:
        output = 'D';
        break;
    case 14:
        output = 'E';
        break;
    case 15:
        output = 'F';
        break;                                                                                                
    default:
        break;
    }
    return output;
}

 nibble char_Code_to_bit(unsigned char var){
    nibble output;
    switch (var)
    {
    case '0':
        output.bits = 0;
        break;
    case '1':
        output.bits = 1;
        break;
    case '2':
        output.bits = 2;
        break;
    case '3':
        output.bits = 3;
        break;
    case '4':
        output.bits = 4;
        break;
    case '5':
        output.bits = 5;
        break;
    case '6':
        output.bits = 6;
        break;
    case '7':
        output.bits = 7;
        break;
    case '8':
        output.bits = 8;
        break;
    case '9':
        output.bits = 9;
        break;
    case 'A':
        output.bits = 10;
        break;
    case 'B':
        output.bits = 11;
        break;
    case 'C':
        output.bits = 12;
        break;
    case 'D':
        output.bits = 13;
        break;
    case 'E':
        output.bits = 14;
        break;
    case 'F':
        output.bits = 15;
        break;                                                                                                
    default:
        break;
    }
    return output;
}

void encode16(unsigned char input, unsigned char* output){
    nibble dir, esq;
    dir.bits = (input & 0x0F);
    esq.bits = (input & 0xF0)>>4;
    printf("valor a esquerda do byte: %d\n",esq.bits);
    printf("valor a direita do byte: %d\n",dir.bits);
     
    output[0] = bit_Code_to_char(esq);
    output[1] = bit_Code_to_char(dir);
    
}

unsigned char decode16(unsigned char* input){
    unsigned char output;
    nibble aux1, aux2;
    aux1 = char_Code_to_bit(input[0]);
    aux2 = char_Code_to_bit(input[1]);;
    output = (aux1.bits << 4) | aux2.bits;
    
    return output;
}

void checksum(){
    
}

int main(int argc, const char** argv){
    
    //char temp = *(argv[2]);
    unsigned char* variavel =  malloc( 2 * sizeof(char) );//(char*)
    encode16(*(argv[1]),variavel);
    //printf("tamanho do espaco alocado: %ld bits\n", sizeof(variavel)*8);
    printf("valor do primeiro caracter: %c\n",variavel[0]);
    printf("valor do segundo caracter: %c\n",variavel[1]);

    unsigned char retorno;
    retorno = decode16(variavel);
    printf("O caracter da entrada: %c\n", retorno);
    free(variavel);

    return 0;
}