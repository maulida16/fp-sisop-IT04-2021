#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

char* dynamicstring();
int send_file();

int main(int argc, char const *argv[]) {
    char credential[128];
    printf("[CLIENT] Welcome to our socket\n");
    sprintf(credential, "%s\t%s", argv[2], argv[4]);
    // printf("User pas: %s\n", credential);
    struct sockaddr_in address;
    int sock = 0, valread, count = 0;
    struct sockaddr_in serv_addr;
    char *msg;
    // char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n[CLIENT] Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n[CLIENT] Invalid address/ Address not supported \n");
        return -1;
    }
  
    int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (connected < 0) {
        printf("\n[CLIENT] Connection Failed \n");
        return -1;
    }

    while (connected == 0) {
        if(count == 0){
            printf("[CLIENT] Connected to server!\n");
            send(sock, credential, strlen(credential)+1, 0);
            read(sock , buffer, 1024);
            printf("[CLIENT] Reply: %s\n",buffer );
            if (!strncmp(buffer, "FAILED!", 7)) return 0;
            memset(buffer, 0, sizeof(buffer));
            count++;
        }

        printf("[CLIENT] Enter Message: ");
        msg = dynamicstring();

        if((strcmp(msg,"break") == 0) || strcmp(msg, "shutdown") == 0){
            send(sock , msg , strlen(msg) , 0 );
            printf("[CLIENT] Auf wiedersehen!\n");
            close(sock);
            break;
        }

        else if (strlen(msg) > 0) {

            send(sock , msg , strlen(msg) , 0 );
            printf("[CLIENT] Message sent!\n");

            read(sock , buffer, 1024);
            
            free(msg);
            printf("[CLIENT] Reply: %s\t size: %ld\n", buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));

        } else printf("[CLIENT] Please provide message!\n");
    }
    return 0;
} 

char* dynamicstring() {
    char *str = NULL;
    int i = 0, j = 2, c;
    str = (char*)malloc(sizeof(char));
    //error checking
    if (str == NULL) {
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }

    while((c = getc(stdin)) && c != '\n')
    {
        str[i] = c;
        str = realloc(str,j*sizeof(char));
        //error checking
        if (str == NULL) {
            printf("Error allocating memory\n");
            free(str);
            exit(EXIT_FAILURE);
        }

        i++;
        j++;
    }
    str[i] = '\0';
    return str;
}
