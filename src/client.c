#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include "server.h"
#include "menus.h"
#include "dialog.h"
#include "roomReg.h"

char *NICK;
int SD;

void enableSocket(){
    SD = socket(AF_INET, SOCK_STREAM, 0);
	if (SD < 0)
	{
		fprintf(stderr, "Criacao do socket falhou!\n");
		exit(1);
	}
}

int main(){
    
    wellcome();

    NICK = identify();  

    while (1)
    {
        int enter = home(NICK);

        if(enter == 1){
            char *addr_s;           
            
            if(createRoom(NICK, &addr_s)) continue;

            if(!getVacations(addr_s)){
                printf("\n------------------------------------\n");
                printf("\n         Não há vagas! :(\n");
                sleep(2);
                continue;
            }

            enableSocket();

			if(connectServer(addr_s, SD, NICK)) {
                close(SD);
                continue;
            }

            startDialogProccess(SD, NICK);

            close(SD);            
        }
        else if(enter == 2){
            char *addr_room = listAndSelectRooms();

            if (!strcmp(addr_room, "0")) continue;

            if(!getVacations(addr_room)){
                printf("\n------------------------------------\n");
                printf("\n         Não há vagas! :(\n");
                sleep(2);
                continue;
            }

            enableSocket();

            if(connectServer(addr_room, SD, NICK)){
                close(SD);
                continue;
            }

            startDialogProccess(SD, NICK);

            close(SD);
        }
        else {
            printf("encerrando...\n");
            sleep(1.5);
            break;
        }
    }
return 0;
}