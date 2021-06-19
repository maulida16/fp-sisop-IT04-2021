## Kelompok IT-04
* Maulida Meirisdiana 05311940000005
* Dava Aditya Jauhar 05311940000030
* Gerry Putra Fresnando 05311940000031

### Tugas: Membuat sistem database menggunakan sistem server-client

##### Pembahasan Client-side
* Full code program dapat diakses di [sini](https://github.com/maulida16/fp-sisop-IT04-2021/blob/main/client.c)
* Program client memiliki pointer `*dynamicstring` dan fungsi `main`
     
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

* Pointer `*dynamicstring` ini digunakan untuk mengirimkan karakter yang diinputkan oleh client ke server.

* Fungsi `main` diambil dari template yang telah disediakan. Dalam program ini, hal pertama yang dilakukan adalah memastikan bahwa client dan server tersambung. Setelah itu, maka dilanjutkan dengan fungsi sbb:

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

                  if (msg[strlen(msg)-1] != ';') {printf("[CLIENT] msg Incorrect [;]\n"); exit(0);};
                  msg[strlen(msg)-1] = '\0';
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

* Fungsi di atas merupakan proses dari client untnuk mengirimkan pesan ke server. Ketika pesan dikirimkan oleh user yang berhak mengirimkan, maka dapat dilanjutkan.
* Jika client ingin mengakhiri program dengan mengetikkan `break` atau `shutdown`, maka akan mendapat pesan `[CLIENT] Auf wiedersehen!\n`
* Jika client mengirimkan command yang diakhiri delimiter `;`, maka pesan akan terkirim.
* Jika client mengirimkan command yang tidak sesuai, maka akan tampil output `[CLIENT] Please provide message!\n`

##### Pembahasan Server-side

    char *kepala[] = {"CREATE", "USE", "GRANT", "DROP", "INSERT", "UPDATE", "DELETE", "SELECT", "WHERE"};
    char *path[] = {"dir_server", "databases", "root"};
    char gudang[16][64];
    char db_session[MAXCHAR/4] = {0};

    int file_created = 0, admin = 0;
    char session[8][64];
    char *trade, kosongan[512];

    char *dynamicstring();
    int initialization(), file_reader(), create_user(), grant_user(), create_database(), delete_reader();

* Di awal program terdapat beberapa deklarasi array pointer bernama `kepala` yang digunakan untuk membuat pointer fungsi yang diminta pada soal seperti `CREATE`, `USE`, `GRANT`, dll.
* Matriks `gudang` digunakan untuk menyimpan data-data ke dalam database nantinya.
* Di akhir juga diberi deklarasi beberapa fungsi untuk menjalankan fungsi yang diminta.

Kode lengkap server side ada di [sini](https://github.com/maulida16/fp-sisop-IT04-2021/blob/main/server.c)

* Fungsi main pada server side diambil dari template server socket pada modul lalu dimodifikasi di beberapa bagian.
* Setelah socket server berhasil terkoneksi dengan client, 
