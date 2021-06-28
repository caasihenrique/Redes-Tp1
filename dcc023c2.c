#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include "commo.h"
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void logexit(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage) {
    if (addrstr == NULL || portstr == NULL) {
        return -1;
    }

    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

    struct in_addr inaddr4; // 32-bit IP address
    if (inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; // 128-bit IPv6 address
    if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        // addr6->sin6_addr = inaddr6
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize) {
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;

    if (addr->sa_family == AF_INET) {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr,
                       INET6_ADDRSTRLEN + 1)) {
            logexit("ntop");
        }
        port = ntohs(addr4->sin_port); // network to host short
    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr,
                       INET6_ADDRSTRLEN + 1)) {
            logexit("ntop");
        }
        port = ntohs(addr6->sin6_port); // network to host short
    } else {
        logexit("unknown protocol family.");
    }
    if (str) {
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
    }
}

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage) {
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(proto, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    }
}


//main

void usage(int argc, char **argv) {
    printf("usage: %s <server IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

#define BUFSZ 1024
#define MAX 65536

typedef struct{
   int unsigned SYNC_1;
   int unsigned SYNC_2;
   unsigned short int length;
   unsigned short int checksum;
   unsigned char ID;
   unsigned char flags;
   char *dados;
}quadro;

uint16_t checksum(void *data, unsigned int bytes){

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

typedef unsigned short u_short;
typedef unsigned char u_char;

//			 return checksum in low-order 16 bits
int	in_cksum(ptr, nbytes)
register u_short	*ptr;
register int		nbytes;
{
	register long		sum;		// assumes long == 32 bits
	u_short			oddbyte;
	register u_short	answer;		// assumes u_short == 16 bits 

	 // Our algorithm is simple, using a 32-bit accumulator (sum),
	 // we add sequential 16-bit words to it, and at the end, fold back
	 // all the carry bits from the top 16 bits into the lower 16 bits.
	 

	sum = 0;
	while (nbytes > 1)  {
		sum += *ptr++;
		nbytes -= 2;
	}

				// mop up an odd byte, if necessary
	if (nbytes == 1) {
		oddbyte = 0;		// make sure top half is zero 
		*((u_char *) &oddbyte) = *(u_char *)ptr;   // one byte only 
		sum += oddbyte;
	}

	
	// * Add back carry outs from top 16 bits to low 16 bits.

	sum  = (sum >> 16) + (sum & 0xffff);	// add high-16 to low-16
	sum += (sum >> 16);			// add carry 
	answer = ~sum;		// ones-complement, then truncate to 16 bits
	return(answer);
}

int main(int argc, char **argv) {
    /*
    if (argc < 3) {
     usage(argc, argv);
    }
    */

    char identificador1[2] = "-c";
    char identificador2[2] = "-s";

    quadro DCCNET;
    DCCNET.SYNC_1 = 0xdcc023c2;
    DCCNET.SYNC_2 = 0xdcc023c2;
    DCCNET.checksum = 0;
    DCCNET.ID = 0;
    DCCNET.flags = 0x00;//pode assumir 0x80,0x40

    if(strncmp(identificador1,argv[1],2)==0) {//Cliente 

        struct sockaddr_storage storage;
        if (0 != addrparse(argv[2], argv[3], &storage)) {
            usage(argc, argv);
        }

        int s;
        s = socket(storage.ss_family, SOCK_STREAM, 0);
        if (s == -1) {
            logexit("socket");
        }
        struct sockaddr *addr = (struct sockaddr *)(&storage);
        if (0 != connect(s, addr, sizeof(storage))) {
            logexit("connect");
        }

        char addrstr[BUFSZ];
        addrtostr(addr, addrstr, BUFSZ);

        printf("connected to %s\n", addrstr);

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        
        FILE *arq;
        arq = fopen(argv[4], "rt");
        if (arq == NULL) {
            printf("Arquivo Nulo >\n");
            return 0;
        }   
    
        while(!feof(arq)) {
            fgets(buf, BUFSZ-1, arq);
        }

        DCCNET.length = (unsigned short int)strlen(buf);
        printf("tamanho da mensagem: %d\n", DCCNET.length);
        DCCNET.dados = malloc(DCCNET.length*sizeof(char));

        memcpy(DCCNET.dados,buf,(int)DCCNET.length);
        DCCNET.length = htons(DCCNET.length);
        
        printf("Menssege >>%s\n", DCCNET.dados );
        printf("Valor do length %d\n",DCCNET.length);
        printf("Tamanho do DCCNET %ld\n",sizeof(DCCNET));
        printf("Tamanho do SYNC_1 %ld\n",sizeof(DCCNET.SYNC_1));
        printf("Tamanho do SYNC_2 %ld\n",sizeof(DCCNET.SYNC_2));
        printf("Tamanho do length %ld\n",sizeof(DCCNET.length));
        printf("Tamanho do checksum %ld\n",sizeof(DCCNET.checksum));
        printf("Tamanho do ID %ld\n",sizeof(DCCNET.ID));
        printf("Tamanho do flags %ld\n",sizeof(DCCNET.flags));
        printf("Tamanho do dados %ld\n",sizeof(DCCNET.dados));

        DCCNET.checksum = (unsigned short int)checksum(&DCCNET,sizeof(DCCNET));
        printf("Primeira vez Ckesum = %d\n", DCCNET.checksum );

        DCCNET.checksum = (unsigned short int)checksum(&DCCNET,sizeof(DCCNET));
        printf("Segunda vez Ckesum = %d\n", DCCNET.checksum );
        //trocar o sinal e imprimir

        short int retorno = in_cksum(DCCNET, sizeof(DCCNET));
        printf("Primeira vez in_cksum = %d\n", retorno );

        size_t count = send(s, buf, strlen(buf)+1, 0);
        if (count != strlen(buf)+1) {
            logexit("send");
        }

        memset(buf, 0, BUFSZ);
        unsigned total = 0;
        while(1) {
            count = recv(s, buf + total, BUFSZ - total, 0);
            if (count == 0) {
                // Connection terminated.
                break;
            }
            total += count;
        }
        close(s);

        printf("received %u bytes\n", total);
        puts(buf);

        exit(EXIT_SUCCESS);
    }
    if(strncmp(identificador2,argv[1],2)==0){//servidor
        struct sockaddr_storage storage;
        if (0 != server_sockaddr_init(argv[2], argv[3], &storage)) {
            usage(argc, argv);
        }

        int s;
        s = socket(storage.ss_family, SOCK_STREAM, 0);
        if (s == -1) {
            logexit("socket");
        }

        int enable = 1;
        if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
            logexit("setsockopt");
        }

        struct sockaddr *addr = (struct sockaddr *)(&storage);
        if (0 != bind(s, addr, sizeof(storage))) {
            logexit("bind");
        }

        if (0 != listen(s, 10)) {
            logexit("listen");
        }

        char addrstr[BUFSZ];
        addrtostr(addr, addrstr, BUFSZ);
        printf("bound to %s, waiting connections\n", addrstr);

        while (1) {
            struct sockaddr_storage cstorage;
            struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
            socklen_t caddrlen = sizeof(cstorage);

            int csock = accept(s, caddr, &caddrlen);
            if (csock == -1) {
                logexit("accept");
            }

            char caddrstr[BUFSZ];
            addrtostr(caddr, caddrstr, BUFSZ);
            printf("[log] connection from %s\n", caddrstr);

            char buf[BUFSZ];
            memset(buf, 0, BUFSZ);
            size_t count = recv(csock, buf, BUFSZ - 1, 0);
            printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

            sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
            count = send(csock, buf, strlen(buf) + 1, 0);
            if (count != strlen(buf) + 1) {
                logexit("send");
            }
            close(csock);
        }

        exit(EXIT_SUCCESS);
                
            } 
}