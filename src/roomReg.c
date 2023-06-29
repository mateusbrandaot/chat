#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "menus.h"

typedef struct roomData
{
    char ip[16], name[22], nickOwner[15];
    int capacity, status;
} roomData;

int SZ = sizeof(roomData);

int insertArq(FILE *arq, int n, roomData* new){
    roomData *bufferRD = malloc(SZ);
    FILE *arq_temp = fopen("files/rooms_bkp", "ab+");
    rewind(arq);

    for (int i = 1; i < n; i++)
    {
        fread(bufferRD, SZ, 1, arq);
        fwrite(bufferRD, SZ, 1, arq_temp);
    }

    fseek(arq, SZ*1, SEEK_CUR);
    int ret = fwrite(new, SZ, 1, arq_temp);    

    if(ret){
        while (fread(bufferRD, SZ, 1, arq) != 0){
            fwrite(bufferRD, SZ, 1, arq_temp);
        }

        remove("files/rooms");
        rename("files/rooms_bkp", "files/rooms");

    }else remove("files/rooms_bkp");

    fclose(arq_temp);
    free(bufferRD);

return ret;
}

int registerRoom(int cp, char* owner, char* name, char* ip){

    FILE *arq = fopen("files/rooms", "ab+");

    roomData *new = malloc(SZ),
             *bufferRD = malloc(SZ);

    new->capacity = cp;
    new->status = 1;
    strcpy(new->name, name);
    strcpy(new->nickOwner, owner);
    
    int count = 1;
    while(fread(bufferRD, SZ, 1, arq) != 0){
        if(!bufferRD->status) break;
        count++;
    }

    char aux[3];
    sprintf(aux, "%d", count);

    strcat(ip, aux);

    strcpy(new->ip, ip);
    
    int ret = insertArq(arq, count, new);

    fclose(arq);
    free(new);

return ret;
}

char *listAndSelectRooms(){
    char *ret = malloc(16);

    while (1)
    {
        int sel = listOfRooms();

        if(!sel) {
            ret = "0";
            break;
        }

        FILE *arq = fopen("files/rooms", "rb");

        roomData *bufferRD = malloc(SZ);

        fseek(arq, (sel-1)*SZ, SEEK_SET);
        fread(bufferRD, SZ, 1, arq);

        fclose(arq);

        if(bufferRD == NULL){
            printf("\n-----------------------\n\n   Opção inválida...\n");
            sleep(1.4);
            continue;
        }
        //printf("%s\n", bufferRD->ip);
        strcpy(ret, (char*) &bufferRD->ip);
        free(bufferRD);
        break;
    }

return ret;    
}

void listAllRooms(){
    FILE *arq = fopen("files/rooms", "rb");

    int i = 0;

    if(arq != NULL){

        roomData *bufferRD = malloc(SZ);

        while (fread(bufferRD, SZ, 1, arq) != 0){
            if(!bufferRD->status) continue;
            printf("\n  [%d] - %s, Vagas: %d - Dono: @%s\n", i+1, bufferRD->name, bufferRD->capacity, bufferRD->nickOwner);
            i++;
        }

        fclose(arq);
        free(bufferRD);
    }

    if(!i) {
        printf("\n  Ops... Nenhuma sala disponível o.O\n");
    }
    else printf("\nDigite o número da sala que deseja entrar:\n\n");
}

int deactiveRoom(char *ipRoom){
    roomData *bufferRD = malloc(SZ);
    FILE *arq = fopen("files/rooms", "ab+");

    int count = 1;

    while (fread(bufferRD, SZ, 1, arq) != 0){
        if(strcmp(bufferRD->ip, ipRoom) == 0){
            bufferRD->status=0;
            break;
        }     
        count++;   
    }

    int ret = insertArq(arq, count, bufferRD);

    fclose(arq);
    free(bufferRD);

return ret;
}

int getVacations(char *addr){
    roomData *bufferRD = malloc(SZ);
    FILE *arq = fopen("files/rooms", "rb");

    while (fread(bufferRD, SZ, 1, arq) != 0){
        if(strcmp(bufferRD->ip, addr) == 0){
            break;
        }        
    }

    int ret = bufferRD->capacity;

    free(bufferRD);
    fclose(arq);

return ret;
}

void refreshVacations(char* addr, int number){
    roomData *bufferRD = malloc(SZ);
    FILE *arq = fopen("files/rooms", "ab+");

    int count = 1;

    while (fread(bufferRD, SZ, 1, arq) != 0){
        if(strcmp(bufferRD->ip, addr) == 0){
            bufferRD->capacity = number;
            break;
        }     
        count++;   
    }

    insertArq(arq, count, bufferRD);
    free(bufferRD);
    fclose(arq);
}