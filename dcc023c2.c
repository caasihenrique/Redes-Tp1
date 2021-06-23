#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include "common.h"
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

int main(int argc, char **argv) {
    
    if (argc < 3) {
     usage(argc, argv);
    }

    char identificador1[2] = "-c";
    char identificador2[2] = "-s";
    

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
        printf("Menssege >>%s\n", buf );

        //fgets(buf, BUFSZ-1, stdin);
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