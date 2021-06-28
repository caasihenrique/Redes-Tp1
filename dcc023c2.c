#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>

#define BUFSZ 1024
#define SYNC 0xdcc023c2

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
    //printf("valor a esquerda do byte: %d\n",esq.bits);
    //printf("valor a direita do byte: %d\n",dir.bits);
     
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

typedef enum {false, true} bool;

typedef struct frame_t//Estrutura do "quadro"
{
	uint32_t sync1;
	uint32_t sync2;
	uint16_t chksum;
	uint16_t length;
	uint8_t id;
	uint8_t flags;
	char data[BUFSZ];
} frame;

int argtest (int argc, char* argv[])//Função para teste de argumentos
{
	if(argc < 2)
	{
		fprintf(stderr, "Erro passagem de argumentos.\n");
		exit(1);
	}
	else if(argc < 5)
	{
		if(strcmp(argv[1], "-s") == 0)
		{
			fprintf(stderr, "Erro! Use ./dcc023c2 -s <port> <INPUT.txt> <OUTPUT.txt>\n");
			exit(1);
		}
		else if(strcmp(argv[1], "-c") == 0)
		{
			fprintf(stderr, "Erro! Use ./dcc023c2 -c <IPPAS> <port> <INPUT.txt> <OUTPUT.txt>\n");
			exit(1);
		}
	}
	return 1;
}

unsigned short checksum(unsigned short *ptr, int bytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while(bytes > 1)
    {
        sum += *ptr++;
        bytes -= 2;
    }
    if(bytes == 1)
    {
        oddbyte = 0;
        *((u_char*)&oddbyte) = *(u_char*)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return(answer);
}

int main(int argc, char* argv[])
{
	if(argtest(argc, argv) != 1)//Verifica argumentos 
		return 0;

	//Declaração das variáveis
	int s, s_use, s_new;//sockets
	int port;
	int tam;
	
	struct sockaddr_in l_addr, r_addr;//endereços de envio e recebimento
	char *input;
	char *output;
	uint8_t buffer[BUFSZ];
	frame frame_send;//, frame_recv, ack;//quadros enviados, recebidos e ack
	int l_id;//,r_id;
	l_id = 1;
	//r_id = 1;
	//uint16_t l_chksum = 0;

	s = socket(AF_INET, SOCK_STREAM, 0);

	if(strcmp(argv[1], "-s") == 0) //Inicia Servidor/Receptor
	{
		input = argv[3];
		output = argv[4];
		port = atoi(argv[2]);
		memset((char*)&l_addr, '\0', sizeof(l_addr));
		l_addr.sin_family = AF_INET;
		l_addr.sin_port = htons(port);
		l_addr.sin_addr.s_addr = INADDR_ANY;

		if(bind(s, (struct sockaddr *) &l_addr, sizeof(struct sockaddr)) < 0)
		{
			printf("Erro Bind\n");
			exit(1);
		}
		printf("Aguardando conexão\n");
		if(listen(s, 10) < 0)
		{
			printf("Erro Listen\n");
			exit(1);
		}
		socklen_t sock_size = sizeof(struct sockaddr_in);
		s_new = accept(s, (struct sockaddr *) &r_addr, &sock_size);
		if(s_new < 0)
		{
			printf("Erro accept\n");
			exit(1);
		}
		printf("Conectado\n");
		
		s_use = s_new;
	}
	if(strcmp(argv[1], "-c") == 0)//Inicia Cliente/Transmissor
	{
		
		input = argv[4];
		output = argv[5];
		char *ip; //*port_num;
		ip = argv[2];
		port = atoi(argv[3]);
		memset((char*)&l_addr,'\0', sizeof(struct sockaddr));
		l_addr.sin_family =AF_INET;
		l_addr.sin_port = htons(port);
		inet_pton(AF_INET, ip, &l_addr.sin_addr);

		if(connect(s, (struct sockaddr *) &l_addr,sizeof(struct sockaddr)) <0)
		{
			printf("Erro ao conectar");
			exit(1);
		}
		s_use = s;
	}
	
	FILE *f_send = fopen(input, "rt");//Abertura dos .txt's
	if(f_send == NULL)
	{
		printf("Erro ao abrir arquivo input.txt.\n");
		fclose(f_send);
		exit(1);
	}

	FILE *f_recv = fopen(output, "w");
	if(f_recv == NULL)
	{
		printf("Erro ao abrir arquivo output.txt\n");
		fclose(f_recv);
		exit(1);
	}
	while((tam = fread(buffer, sizeof(uint8_t), BUFSZ, f_send)) > 0 )//Inicializa os quadros
	{
		if(tam >= 0)
		{
			memset(&frame_send, '\0', sizeof(frame));
			frame_send.sync1 = htonl(SYNC);
			frame_send.sync2 = htonl(SYNC);
			frame_send.chksum = htons(0);
			frame_send.length = htonl(tam);
			l_id == 1 ? (frame_send.id = 0) : (frame_send.id = 1);
			tam == BUFSZ ? (frame_send.flags = 0) : (frame_send.flags = 64);
			memcpy(frame_send.data, &buffer, tam);
			frame_send.chksum = htons((uint16_t)checksum((unsigned short *) &frame_send, sizeof(frame)));

            int size = sizeof(frame_send);
            unsigned char *texto_send = malloc(2*size);
            
            int i;
            for(i = 0; i < size; i++ ){
                encode16((unsigned char)frame_send.data[i], &texto_send[i]);
            }         

			if(send(s_use, (frame *) &texto_send, size, 0) < 0)
			{
				printf("Erro envio\n");
				exit(1);
			}
			puts(frame_send.data);
            free(texto_send);
		}
	}
	fclose(f_send);
	fclose(f_recv);
		

	return 0;
}
