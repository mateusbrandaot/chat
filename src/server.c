#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "roomReg.h"

#define STDIN 0

int PORT = 5200, CAPACITY, TRUE = 1, SD_S, ADDRLEN, FDMAX;
char IP_ADR[16] = "127.0.0.", NAME[22], NICKOWNER[15], BUF[256];
fd_set MASTER, READ_FDS;
struct sockaddr_in 	MYADDR, REMOTEADDR;

typedef struct participant{
    char name[15];
    int fd;
}participant;

void insertVet(int tam, int i, participant *part){

    for (int j=0; j<tam; j++){
        if (part[j].fd == -1){
            strcpy(part[j].name, BUF);
            part[j].fd = i;
            break;
        }
    }
}

void removeVet(int tam, int i, participant *part){
    for (int j=0; j<tam; j++){
        if (part[j].fd == i){
            strcpy(part[j].name, "\0");
            part[j].fd = -1;
        }
    }
}

int serverComand(){
    char c = BUF[0];

    if(c == 'q') return 1;
    else if(c == 'e') return 2;
    else if(c == 'l') return 3;
    else if(c == '#') return 4;

return 0;
}

void send_msg(int arg, int n) {
    for (int i = 0; i <= FDMAX; i++) {
        if (FD_ISSET(i, &MASTER)){
                if ((i != arg) && (i != SD_S)) send(i, BUF, n, 0);
        }
    }
}

void closeServer(){
    memset (&BUF, 0 , sizeof (BUF));
    strcpy(BUF, "[SERVER]: A sala está sendo fechada, até a proxima! bye...\n");
    send_msg(SD_S, sizeof(BUF));
    sleep(2);

    memset(BUF, 0, sizeof(BUF));
    BUF[0] = 'e';
    send_msg(SD_S, 1);  
    deactiveRoom(IP_ADR);
    exit(0);
}

void listParts(int tam, participant *parts){
    memset (&BUF, 0 , sizeof (BUF));
    strcpy(BUF, "[SERVER]: lista de participantes");
    for(int i=0; i<tam; i++){
        if(parts[i].fd != -1) {
            strcat(BUF, "\n     @");
            strcat(BUF, parts[i].name);
        }
    }
}

void launchRoom(){
    FD_ZERO(&MASTER); 
    FD_ZERO(&READ_FDS);
    
    SD_S = socket(AF_INET, SOCK_STREAM, 0);
    
    if (SD_S < 0){
        fprintf(stderr, "Falha ao criar socket do server da sala %s!\n", NAME);
        exit(1);
    }

    setsockopt(SD_S, SOL_SOCKET, SO_REUSEADDR, &TRUE, sizeof(int));

    if(!registerRoom(CAPACITY, NICKOWNER, NAME, IP_ADR)){
        fprintf(stderr, "Falha ao registrar a sala %s!\n", NAME);
        exit(1);
    }
    
    MYADDR.sin_family = AF_INET;
    MYADDR.sin_addr.s_addr = inet_addr(IP_ADR);
    MYADDR.sin_port = htons(PORT);

    memset(&(MYADDR.sin_zero), '\0',8); 
    ADDRLEN = sizeof(REMOTEADDR);
    if(bind(SD_S, (struct sockaddr *)&MYADDR, sizeof(MYADDR)) < 0){
        fprintf(stderr, "Ligacao Falhou!\n");
        exit(1);
    }

    listen(SD_S, 10); 
    FD_SET(SD_S, &MASTER);
    FDMAX = SD_S;
    
    participant *parts = malloc(CAPACITY*sizeof(participant));
    int tam = CAPACITY;
    for (int i =0; i<tam; i++) parts[i].fd = -1;

    while (1){
       READ_FDS = MASTER;
       
       select(FDMAX+1, &READ_FDS, NULL, NULL, NULL);

       for (int i = 0; i <= FDMAX; i++) {
	        if (FD_ISSET(i, &READ_FDS)) {
	            if (i == SD_S) {
                    if(CAPACITY){
                        int newfd = accept(SD_S, (struct sockaddr *)&REMOTEADDR, (socklen_t*)&ADDRLEN);
	                    FD_SET(newfd, &MASTER);
	                    if (newfd > FDMAX) FDMAX = newfd;

                        memset (&BUF, 0 , sizeof(BUF));
                        recv(newfd, BUF, sizeof(BUF), 0);
                        insertVet(tam, newfd, parts);

                        CAPACITY--;
                        refreshVacations(IP_ADR, CAPACITY);
                        //to do send a wellcome msg
                        //to do broadcast the new member
                    }
                    else continue;                    
	            } 
                else {
		        	memset (&BUF, 0 , sizeof (BUF));
	        		int n = recv(i, BUF, sizeof(BUF), 0);

                    if(BUF[1]=='#'){
                        int comand = serverComand();
                        if (comand == 1) {
                            removeVet(tam, i, parts);
                            FD_CLR(i, &MASTER);
                            CAPACITY++;                            
                            refreshVacations(IP_ADR, CAPACITY);
                            continue;
                        }
                        else if(comand == 2){
                            char nick[15];
                            strcpy(nick, BUF+2);

                            if(strcmp(nick, NICKOWNER) == 0) closeServer();
                            else{
                                memset(BUF, 0, sizeof(BUF));
                                strcpy(BUF, "[SERVER]: Permissão negada!\0");
                                send(i, BUF, sizeof(BUF), 0);
                            } 
                        }
                        else if(comand == 3){
                            listParts(tam, parts);
                            send(i, BUF, sizeof(BUF), 0);
                        }
                        else if(comand == 4){
                            continue;
                            //to do implementação de mensagens diretas
                        }
                        else {
                            strcpy(BUF, "Comando inválido...");
                            send(i, BUF, sizeof(BUF), 0);
                        }
                    } else send_msg(i, n); 
                } 
            }
  	    }  
	}    
}

int createRoom(char* nick, char* ladoServ){

    strcpy(NICKOWNER, nick);

    while (1)
    {
        system("clear");
        printf("\n-------------------------------------\n");
        printf("--------- Criação de sala -----------\n\n");
        printf("Digite um nome para a sua sala (max 20 caracteres):\n [0] Cancelar\n\n>> ");
        int tam = scanf(" %[^\n]", NAME);

        if(strlen(NAME) == 1 && NAME[0] == '0') return 1;
        else if(tam > 20) {
            printf("Nome muito longo! Max 20 caracteres...\n");
            sleep(2);
            continue;
        }
        else break;        
    }

    while (1)
    {
        system("clear");
        printf("\n-------------------------------------\n");
        printf("-------- Criação de sala ----------\n\n");
        printf("Digite a capacidade da sua sala (max 20 participantes):\n [0] Cancelar\n\n>> ");
        
        scanf(" %d", &CAPACITY);

        if(CAPACITY == 0) return 1;

        else if(CAPACITY > 20) {
            printf("O número máximo de participantes é 20!\n");
            sleep(2);
            continue;
        }
        else break;
    }

    if(!fork()){
        if(!fork()) launchRoom();
        exit(0);
    } 
    sleep(2);    

    char *ip_r = inet_ntoa(MYADDR.sin_addr);
    
    memcpy(ladoServ, &ip_r, sizeof(ip_r));

return 0;
}