#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAXCHAR 1024
#define PORT 8080

char *kepala[] = {"CREATE", "USE", "GRANT", "DROP", "INSERT", "UPDATE", "DELETE", "SELECT", "WHERE"};
char *path[] = {"dir_server", "databases", "root"};
char gudang[16][64];
char db_session[MAXCHAR/4] = {0};

int file_created = 0, admin = 0;
char session[8][64];
char *trade, kosongan[512];

char *dynamicstring();
int initialization(), file_reader(), create_user(), grant_user(), create_database(), delete_reader();

int main(int argc, char const *argv[])
{
    umask(0);
    initialization();
    int server_fd, new_socket, valread, status = 1;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0}, buffer2[1024] = {0};
    char *hello = "[SERVER] You are connected\n";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("[SERVER] socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("[SERVER] setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("[SERVER] bind failed");
        exit(EXIT_FAILURE);
    }

    int listening = listen(server_fd, 3);
    if (listening < 0)
    {
        perror("[SERVER] listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {

        printf("[SERVER] status: connected\n");

        if (!strcmp(session[1], "0"))
            printf("[SERVER] Loginnya kosong lur!\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            // exit(EXIT_FAILURE);
        }
        else
        {
            // send(new_socket , hello , strlen(hello) , 0);
            printf("[SERVER] client found (%d)\n", new_socket);
            int aa = read(new_socket, buffer2, 1024);
            // strcpy(session[1], "0");
            char *reply = "[SERVER] Server received your message!\n";
            char *reply2[] = {"SUCCESS!\n", "FAILED!\n"};
            if (aa > 0)
            {
                int tok = 0;
                printf("[SERVER] client login: (%s)\n", buffer2);
                strcpy(session[tok], buffer2);
                tok++;
                // memset(buffer2, 0, sizeof(buffer2));
                char *session_token = strtok(buffer2, "\t");
                while (session_token != NULL)
                {
                    strcpy(session[tok], session_token);
                    // printf("%d. session: %s\n", tok, session[tok]);
                    session_token = strtok(NULL, "\t");
                    tok++;
                }
                // printf("[SERVER] session: %s|||%s\n", session[2], session[1]);
                // int login = ;
                // if (create_user(session[1], session[2], 1) != 1) strcpy(session[0], "0");
            }
            while (1)
            {
                if (status < 2)
                {
                    if (create_user(session[1], session[2], 1) == 0)
                    {
                        memset(session, 0, sizeof(session));
                        strcpy(session[0], "0");
                        strcpy(session[1], "0");
                        printf("[SERVER] client %d login error\n", new_socket);
                        send(new_socket, reply2[1], sizeof(reply2[1]), 0);
                        close(new_socket);
                        status = 0;
                        break;
                    }
                    else
                    {
                        send(new_socket, reply2[0], sizeof(reply2[0]), 0);
                        status = 2;
                        if (!strcmp(session[1], "root")) admin = 1;
                        printf("[SERVER] SESSION <%d>: %s\n", admin, session[0]);
                    }
                }
                printf("[SERVER] LISTENING: %d\n\n", listening);
                read(new_socket, buffer, 1024);
                send(new_socket, reply, strlen(reply), 0);

                if (strcmp(buffer, "break") == 0)
                {
                    close(new_socket);
                    // memset(session, 0, sizeof(session));
                    strcpy(session[0], "0");
                    strcpy(session[1], "0");
                    printf("[SERVER] client %d has been disconnected\n", new_socket);
                    status = 0;
                    break;
                }

                else if (strcmp(buffer, "shutdown") == 0)
                {
                    close(new_socket);
                    printf("[SERVER] client %d has been requested shutdown\n", new_socket);
                    status = -1;
                    break;
                }

                else
                {
                    printf("BUFFER: %s\n", buffer);
                    char *token = strtok(buffer, " ");
                    int i = 0;
                    while (token != NULL)
                    {
                        
                        strcpy(gudang[i], token);
                        // printf("TOKEN: %s -> %s\n", token, gudang[i]);
                        // printf("token: %s\nstring now: %s\ngudang[%d]: %s\n", token, buffer, i, gudang[i]);
                        token = strtok(NULL, " ");
                        i++;
                        // printf("---%d\n", i);
                    }
                    memset(buffer, 0, sizeof(buffer));
                    //sytax list
                    if (strcmp(gudang[0], kepala[0]) == 0)
                    {
                        printf("[%s] COMMAND\n", gudang[0]);

                        if (!strcmp(gudang[1], "USER"))
                        {

                            printf("[%s]\n", gudang[1]);
                            //CREATE USER [name] IDENTIFIED BY [password];
                            printf("username: %s\npassword: %s\n", gudang[2], gudang[5]);
                            printf("CREATE USER [%s]\n", create_user(gudang[2], gudang[5], 0) ? "SUCCESS" : "FAILED");
                        }

                        else if (!strcmp(gudang[1], "DATABASE"))
                        {
                            printf("[%s]\n", gudang[1]);

                            //CREATE DATABASE [db]
                            printf("INPUT: %s\n", gudang[2]);
                            printf("CREATE DATABASE [%s]\n", create_database(gudang[2], session[1]) ? "SUCCESS" : "FAILED");
                        }

                        //CREATE TABLE [table] ([column1] [type1], ...)
                        else if (!strcmp(gudang[1], "TABLE"))
                        {
                            printf("[%s]\n", gudang[1]);
                            
                            int n = 0;
                            while(gudang[n] != ";" || gudang[n] != NULL){
                                printf("STRCHR: \n", strchr(gudang[n], "("))
                                printf("Gudang %d: %s\n", n, gudang[n]);
                                n++;
                            }
                            printf("INPUT: %s\n", gudang[2]);
                            // if (db_session != "0"){
                            //     // while (buffer[strlen(buffer)-1] != ';')
                            //     sprintf(kalem, "%s|%s")
                            //     printf("CREATE TABLE [%s]", table_maker(db_session, kalem) ? "SUCCESS" : "FAILED");

                            // }
                            // else continue;
                            // printf("CREATE TABLE [%s]\n", create_database(gudang[2], login) ? "SUCCESS" : "FAILED");
                        }
                        else
                            continue;
                    }
                    else if (strcmp(gudang[0], kepala[1]) == 0)
                    {
                        int nemu = 0;
                        printf("[%s] COMMAND\n", kepala[1]);
                        //USE DATABASE [db] -> use db path, save on session?
                        // printf("INPUT: %s\n", gudang[2]);
                        nemu = grant_user(gudang[2], session[1], 0);
                            printf("NEMU [%d]\n", nemu);
                        // printf("USE DATABASE [%s]\n", nemu ? "SUCCESS" : "FAILED");
                        if (nemu){
                            strcpy(db_session, gudang[2]);
                            printf("USE DATABASE [%s]\n", db_session);
                        }
                        else {
                            strcpy(db_session, "0");
                            // printf("USE DATABASE [%s]\n", "2222");
                            continue;
                        }
                        printf("[SERVER] SESSION DB: %s\n", db_session);
                    }
                    else if (strcmp(gudang[0], kepala[2]) == 0)
                    {

                        printf("[%s] COMMAND\n", kepala[2]);
                        //GRANT PERMISSION [db] INTO [user] -> grating permission into a user
                        printf("INPUT: %s\n", gudang[1]);
                        printf("GRANT PERMISSION [%s]\n", grant_user(gudang[2], gudang[4], 1) ? "SUCCESS" : "FAILED");
                    }
                    else if (strcmp(gudang[0], kepala[3]) == 0)
                    {

                        // if(!strcmp(gudang[1], "DATABASE")){}
                        // else if(!strcmp(gudang[1], "COLUMN")){}
                        // else continue;
                        printf("[%s] COMMAND\n", kepala[3]);
                        //DROP DATABASE [db]-> drop a db
                        printf("INPUT: %s\n", gudang[2]);

                        delete_reader(gudang[2], 0);
                    }
                    else if (strcmp(gudang[0], kepala[4]) == 0)
                    {

                        printf("[%s] COMMAND\n", kepala[4]);
                    }
                    else if (strcmp(gudang[0], kepala[5]) == 0)
                    {

                        printf("[%s] COMMAND\n", kepala[5]);
                    }
                    else if (strcmp(gudang[0], kepala[6]) == 0)
                    {

                        printf("[%s] COMMAND\n", kepala[6]);
                    }
                    else if (strcmp(gudang[0], kepala[7]) == 0)
                    {

                        printf("[%s] COMMAND\n", kepala[7]);
                    }
                    else if (!strcmp(gudang[0], "exit"))
                        break;
                }
                // free(buffer);
                // printf("[SERVER] Message: %s", buffer);
                printf("\n");
                memset(buffer, 0, sizeof(buffer));
            }
            if (status < 0)
                break;
        }
    }
}

char *dynamicstring()
{
    char *str = NULL;
    int i = 0, j = 2, c;
    str = (char *)malloc(sizeof(char));
    //error checking
    if (str == NULL)
    {
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }

    while ((c = getc(stdin)) && c != '\n')
    {
        str[i] = c;
        str = realloc(str, j * sizeof(char));
        //error checking
        if (str == NULL)
        {
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

int file_reader(char *path, char *letter, int mode)
{
    //mode 0 = entire line, mode 1 = per column

    FILE *fp;
    char str[1024];
    int ketemu = 0;

    fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s\n", path);
        return 1;
    }

    while (fgets(str, MAXCHAR, fp) != NULL)
    {

        // printf("%s", str);
        if (mode == 0)
        {
            if (!strcmp(letter, str))
            {
                printf("Ketemu!\n");
                ketemu++;
                break;
            };
        }
        else
        {
            char *token = strtok(str, "\t");
            while (token != NULL)
            {

                // printf("Token: %s\n", token);
                if (!strcmp(letter, token))
                {
                    printf("Ketemu!\n");
                    ketemu++;
                    break;
                };
                token = strtok(NULL, "\t");
            }
        }
        if (ketemu > 0)
            break;
    }

    fclose(fp);
    return ketemu;
}

int create_user(char *username, char *password, int mode)
{
    //mode 0 = register, mode 1 = login

    // printf("creating user for [%s] with password [%s]\n", username, password);
    char wadah[MAXCHAR];
    char PATHWAY[MAXCHAR];
    sprintf(PATHWAY, "%s/%s/%s", path[0], path[1], path[2]);
    sprintf(wadah, "%s/%s", PATHWAY, "credentials.tsv");
    // printf("----%s\n", wadah);

    FILE *target;
    target = fopen(wadah, "a+");

    if (mode == 1)
    {
        char baskom[MAXCHAR / 2];
        sprintf(baskom, "%s\t%s\n", username, password);
        // printf("BASKOM: %s\n", baskom);
        // fprintf(target, "%s\n", baskom);
        printf("WADAH: %s\n", wadah);
        return (file_reader(wadah, baskom, 0) > 0) ? 1 : 0;
        // return 0;
    }

    if (mode == 0 && file_reader(wadah, username, 1))
    {
        printf("Username [%s] already exist!", username);
        return 0;
    }

    else
    {
        fprintf(target, "%s\t%s\n", username, password);
    }

    memset(wadah, 0, sizeof(wadah));
    fclose(target);

    return 1;
}

int initialization()
{

    char wadah[MAXCHAR], PATHWAY[MAXCHAR];
    char *mulai;
    sprintf(PATHWAY, "%s/%s/%s", path[0], path[1], path[2]);
    FILE *target;

    for (int i = 0; i < 2; i++)
    {

        if (i == 0)
        {
            sprintf(wadah, "%s/%s", PATHWAY, "credentials.tsv");
            mulai = "user|string\tpassword|string\nroot\troot\n";
        }
        else if (i = 1)
        {
            sprintf(wadah, "%s/%s", PATHWAY, "access.tsv");
            mulai = "user|string\tdatabase|string\n";
        }

        target = fopen(wadah, "r");

        if (!target)
        {
            target = fopen(wadah, "a+");
            fputs(mulai, target);
            printf("PREPARING [%s]\n", wadah);
        }

        fclose(target);
    }
    return 1;
}

int table_maker(char *name, char *kolom)
{

    char wadah[MAXCHAR], PATHWAY[MAXCHAR];
    char *mulai;
    sprintf(PATHWAY, "%s/%s/%s", path[0], path[1], db_session);
    FILE *target;

    sprintf(wadah, "%s/%s", PATHWAY, name);

    target = fopen(wadah, "r");

    if (!target)
    {
        target = fopen(wadah, "a+");
        fputs(kolom, target);
        printf("CREATING TABLE [%s]\n", wadah);
    }

    fclose(target);
 
    return 1;
}

int grant_user(char *dabase, char *username, int mode)
{
    //mode 0 = check only, mode 1 = create/write
    // char *FILES= "dir_server/databases/root/access.tsv";
    int credentials_i, access_i, access_a;
    char wadah[MAXCHAR];
    char PATHWAY[MAXCHAR];
    char access_tsv[MAXCHAR / 4];

    sprintf(PATHWAY, "%s/%s/%s", path[0], path[1], path[2]);
    sprintf(wadah, "%s/%s", PATHWAY, "credentials.tsv");
    printf("1. ----%s\n", wadah);
    credentials_i = file_reader(wadah, username, 1);
    printf("cred: %d\n", file_reader(wadah, username, 1));

    memset(wadah, 0, sizeof(wadah));
    sprintf(wadah, "%s/%s", PATHWAY, "access.tsv");

    FILE *target_c;
    target_c = fopen(wadah, "a+");

    printf("2. ----%s\n", wadah);
    sprintf(access_tsv, "%s\t%s\n", username, dabase);
    printf("2.5. ----%s\n", access_tsv);
    // exit(EXIT_SUCCESS);
    access_i = file_reader(wadah, access_tsv, 0);
    // access_a = file_reader(wadah, dabase, 1);

    //credential not found, dabase not found, already have access => failed
    printf("%d <> %d\n", credentials_i, access_i);
    if (credentials_i != 1 || access_i != 0)
    {
        if (mode == 0){
            printf("ACCESS: %d\n", access_i);
            return access_i;
        }
        else {
        printf("OR NOT FULFILLED\n");
        return 0;
        }
    }
    else{

        // target_c = freopen(wadah, "a+", target_c);
        if (mode == 1){
        
            printf("FD2: %s--%s\n", username, dabase);
            fprintf(target_c, "%s\t%s\n", username, dabase);
            // memset(wadah, 0, sizeof(wadah));
            fclose(target_c);
            printf("FD3: %s--%s\n", username, dabase);
            return 1;

        } else return 0;

    }

}

int create_database(char *db_name, char *user)
{

    char wadah[MAXCHAR];
    sprintf(wadah, "%s/%s/%s", path[0], path[1], db_name);
    printf("WADAH: %s\n", wadah);

    if (access(wadah, F_OK) < 0 && grant_user(db_name, user, 1))
        mkdir(wadah, 0777);
    else
        return 0;

    return 1;
}

int delete_reader(char *letter, int mode)
{

    printf("LETTER: %s\n", letter);
    FILE *fp;
    char str[MAXCHAR], jalan[MAXCHAR / 2], judul[MAXCHAR / 2], mangkok[MAXCHAR / 8][MAXCHAR / 4];
    int ketemu = 0;
    sprintf(jalan, "%s/%s/%s/%s", path[0], path[1], path[2], "access.tsv");
    printf("JALAN: %s\n", jalan);

    fp = fopen(jalan, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s\n", jalan);
        return 1;
    }

    int z = 0;
    while (fgets(str, MAXCHAR, fp) != NULL)
    {

        strcpy(judul, str);
        printf("STR: %s\n", str);

        if (mode == 0)
        {
            //COLUMN COUNTER

            char *token = strtok(judul, "\t");
            while (token != NULL && (z += 1))
            {

                strcpy(mangkok[z - 1], token);
                printf("Token[%d]: %s\n", z - 1, mangkok[z - 1]);
                if (!strncmp(token, letter, strlen(letter)))
                {
                    printf("Ketemu!\n");
                    ketemu++;
                    break;
                };
                token = strtok(NULL, "\t");
                // ;
            }
            break;
            // if (token == NULL) {mode = -1; break;}
            return z - 1;
        }

        else if (mode == 1)
        {
            //COLUMN DATA FINDER

            char *token = strtok(str, "\t");
            while (token != NULL && (z += 1))
            {

                strcpy(mangkok[z - 1], token);
                printf("Token[%d]: %s\n", z, mangkok[z - 1]);
                if (!strncmp(token, letter, strlen(letter)))
                {
                    printf("Ketemu!\n");
                    ketemu++;
                    break;
                };
                token = strtok(NULL, "\t");
                // ;
            }
            break;
            // if (token == NULL) {mode = -1; break;}
        }

        else if (mode == 2)
        {
            //ROW COUNTER

            // if (!strcmp(letter, token)) {printf("Ketemu!\n"); ketemu++; break;};
            strcpy(mangkok[z], str);
            printf("ROW [%d]: %s\n", z, mangkok[z]);
            z++;
        }
        else
            break;
    }
    printf("TOTAL: %d %s\n", z, mode > 0 ? "ROW" : "COLUMN");

    fclose(fp);
    return ketemu;
}
