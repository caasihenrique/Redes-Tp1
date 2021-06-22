#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>
#include<string.h>

typedef struct {
    unsigned char bits:4;
}nibble;

typedef struct{
    unsigned char var:8;
}byte;

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

typedef struct{
    unsigned short int vetor:8;
}short_int;


void short_to_char(unsigned short int size, unsigned char* duo){
    short_int var;

    var.vetor = (size & 0x00FF);

    //unsigned char duo[2];
    duo[0] = (unsigned char) var.vetor;

    var.vetor = (size & 0xFF00)>>8;
    duo[1] = (unsigned char) var.vetor;

}

uint16_t checksum(void *data, unsigned int bytes){
    //uint16_t checksum16(void *data, unsigned int bytes){
    uint16_t *data_pointer = (uint16_t *) data;
    uint32_t total_sum;

    while(bytes > 1){
        total_sum += *data_pointer++;
        //If it overflows to the MSBs add it straight away
        if(total_sum >> 16){
            total_sum = (total_sum >> 16) + (total_sum & 0x0000FFFF);
        }
        bytes -= 2; //Consumed 2 bytes
    }
    if(1 == bytes){
        //Add the last byte
        total_sum += *(((uint8_t *) data_pointer) + 1);
        //If it overflows to the MSBs add it straight away
        if(total_sum >> 16){
            total_sum = (total_sum >> 16) + (total_sum & 0x0000FFFF);
        }
        bytes -= 1;
    }

    return (~((uint16_t) total_sum));
}
//}

unsigned short NetIpChecksum(unsigned short const ipHeader[], int nWords){
    unsigned long  sum = 0;
    /*      * IP headers always contain an even number of bytes.      */     
    while (nWords-- > 0){
        sum += *(ipHeader++);
        }
    /*      * Use carries to compute 1's complement sum.      */     
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += sum >> 16;
    /*      * Return the inverted 16-bit result.      */     
    return ((unsigned short) ~sum);
}

/*
 * Checksum routine for Internet Protocol family headers (C Version).
 *
 * Refer to "Computing the Internet Checksum" by R. Braden, D. Borman and
 * C. Partridge, Computer Communication Review, Vol. 19, No. 2, April 1989,
 * pp. 86-101, for additional details on computing this checksum.
 */

typedef unsigned short u_short;
typedef unsigned char u_char;

			/* return checksum in low-order 16 bits */
int	in_cksum(ptr, nbytes)
register u_short	*ptr;
register int		nbytes;
{
	register long		sum;		/* assumes long == 32 bits */
	u_short			oddbyte;
	register u_short	answer;		/* assumes u_short == 16 bits */

	/*
	 * Our algorithm is simple, using a 32-bit accumulator (sum),
	 * we add sequential 16-bit words to it, and at the end, fold back
	 * all the carry bits from the top 16 bits into the lower 16 bits.
	 */

	sum = 0;
	while (nbytes > 1)  {
		sum += *ptr++;
		nbytes -= 2;
	}

				/* mop up an odd byte, if necessary */
	if (nbytes == 1) {
		oddbyte = 0;		/* make sure top half is zero */
		*((u_char *) &oddbyte) = *(u_char *)ptr;   /* one byte only */
		sum += oddbyte;
	}

	/*
	 * Add back carry outs from top 16 bits to low 16 bits.
	 */

	sum  = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;		/* ones-complement, then truncate to 16 bits */
	return(answer);
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

    char teste[] = {"uma frase bem grande"};
    int tam;
    tam = strlen(teste);
    printf("Tamanho da frase: %d\n",tam);

    unsigned short int size;//uint16_t size;
    size = (unsigned short int)checksum(teste,tam);
    printf("Conteudo da memoria %x\n",size);
    
    //unsigned char *B = malloc(sizeof(unsigned char));
    byte B;
    unsigned char* caracter = (unsigned char *)malloc(sizeof(uint16_t));
    printf("Tamanhos\n>unsigned char: %ld\n>uint16_t: %ld\n", sizeof(unsigned char), sizeof(uint16_t));
    
    B.var = (unsigned char)(size & 0x00FF);

    printf("O valore em B.var: %c\n",B.var);
    //*caracter = checksum(teste,tam);
    short_to_char((unsigned short int)checksum(teste,tam), caracter);

    printf("Conversao em string de size: %s\n", caracter);
    printf("Primeira conversao: %c\n",caracter[0]);
    printf("Segunda conversao: %c\n",caracter[1]);
    B.var = (unsigned char)((size & 0xFF00)>>8);
    printf("O valor segundo em B.var: %c\n",B.var);
    //caracter[1] = B.var;
    
    //unsigned short NetIpChecksum(unsigned short const ipHeader[], int nWords)
    //unsigned short int resposta;
    //resposta = NetIpChecksum((unsigned short const int)checksum(teste,tam), tam);
    //short_to_char(resposta, caracter);
    //printf("Primeira conversao: %c\n",caracter[0]);
    //printf("Segunda conversao: %c\n",caracter[1]);
    return 0;
}