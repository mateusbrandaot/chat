#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include"roomReg.h"

char nickname[15];

void wellcome(){
    system("clear");
    printf("-------------- Chatroom --------------\n");
    printf("\nBem vindo ao sistema de salas de FRC!\n");
}

char* identify(){
    printf("\nDigite o seu apelido\n>> ");
    
    scanf(" %[^\n]", nickname);

    for(int i=0; i<15; i++){
        if(nickname[i]==' ') nickname[i]='_';
    }

    return nickname;
}

int home(char* nick){
    int ret;

    while(1){
        system("clear");
        printf("\n-------------------------------------\n");
        printf("   [ @%s ] online\n", nick);
        printf("\nMenu:\n\n [1] Criar uma sala\n [2] Entrar em uma sala\n [3] Sair\n\n>> ");
        
        char enter;
        if(scanf(" %[^\n]", &enter) != 1) ret = -1;
        else ret = atoi(&enter);

        if(ret == 1 || ret == 2 || ret == 3) break;
        else {
            
            printf("\nOpção invalida! Selecione as disponíveis no Menu...\n");
            sleep(2.2);
        }
    }
    
    return ret;
}

int listOfRooms(){
    system("clear");
    printf("\n-------------------------------------\n");
    printf("-------- Salas disponíveis ----------\n");
    printf("   [0] Voltar\n");

    listAllRooms();
    
    printf("\n>> ");

    int ret;
    scanf(" %d", &ret);

return ret;
}

void headerChat(){
    system("clear");
    printf("\n-------------------------------------\n");
    printf("--------------------------------------\n");
    printf("  Bem vindo! Diga ola 0/\n\n");
}