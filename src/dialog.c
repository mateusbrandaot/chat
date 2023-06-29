#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "menus.h"

int SD_C;

void *receiveAndShowMsg(){
    while (1){        
		char res[256];
        
		memset(&res, 0x0, sizeof(res));
        
		recv(SD_C, &res, sizeof(res), 0);

        if(res[0] == 'e'){
            printf("\nUtilize o comando /q para continuar!\n");
            printf("------------------------------------\n");
            pthread_exit(NULL);
        }

        fprintf(stdout, "\n%s\n> ", res);
		fflush(stdout);  		
	}
}

void startDialogProccess(int sd, char* nick){
    SD_C = sd;

    send(sd, nick, sizeof(nick), 0);

    int sz = strlen(nick);
    char *identtify = calloc(sz+5, sizeof(char));

    identtify[0] = '[';    
    strncat(identtify, nick, sz);
    strcat(identtify, "]: \0");

    headerChat();
    
    pthread_t rec_th;
    pthread_create(&rec_th, NULL, receiveAndShowMsg, NULL);

    while (1){   
        char msg[276], bufout[256];
        memset(&msg, 0x0, sizeof(msg));

        strcpy(msg, identtify);

        fflush(stdin);
        printf("> ");
        fscanf(stdin, " %[^\n]", bufout);
		 
		if (strncmp(bufout, "/q", 2) == 0){
            char com[2] = "q#";
            send(SD_C, com, strlen(com), 0);
            printf("------- encerrando conexao com o servidor -----\n");
            break;
        }
        else if (strncmp(bufout, "/e", 2) == 0){
            char com[18] = "e#";
            strcat(com, nick);
            send(SD_C, com, strlen(com), 0);
            continue;
        }
        else if (strncmp(bufout, "/l", 2) == 0){
            char com[18] = "l#";
            send(SD_C, com, strlen(com), 0);
            continue;
        }
        else if (bufout[0]== '@'){           
            char com[256] = "##";
            strcat(msg, bufout);
            strcat(com, msg);
            send(SD_C, com, strlen(com), 0);
            continue;
        }

        strcat(msg, bufout);
        send(SD_C, &msg, strlen(msg), 0);
	} 

        pthread_cancel(rec_th);
        pthread_join(rec_th, NULL);
        printf("------- Chat encerrado! -----\n");
}

int connectServer(char* addr, int sd, char* nick){

    struct sockaddr_in room;
	room.sin_family = AF_INET;
    room.sin_addr.s_addr = inet_addr(addr);
   	room.sin_port = htons(5200);
   	memset(&(room.sin_zero), '\0', 8); 

    if (connect(sd, (struct sockaddr *)&room, sizeof(room)) < 0){
	    fprintf(stderr, "\nTentativa de conexao falhou!\n");
        sleep(2);
	    return 1;
    }
return 0;
}