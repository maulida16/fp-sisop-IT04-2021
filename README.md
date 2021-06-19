## Kelompok IT-04
* Maulida Meirisdiana 05311940000005
* Dava Aditya Jauhar 05311940000030
* Gerry Putra Fresnando 05311940000031

### Tugas: Membuat sistem database menggunakan sistem server-client

#### Pembahasan Client-side
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

#### Pembahasan Server-side

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
* Matriks `session` digunakan untuk menyimpan user yang telah login ke database.
* Di akhir juga diberi deklarasi beberapa fungsi untuk menjalankan fungsi yang diminta.
* Fungsi main pada server side diambil dari template server socket pada modul lalu dimodifikasi di beberapa bagian. Kode lengkap server side ada di [sini](https://github.com/maulida16/fp-sisop-IT04-2021/blob/main/server.c)
* Dalam program server side, terdapat beberapa fungsi yaitu:

1. Membuat data user pada fungsi `create_user`

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
 * Fungsi ini digunakan untuk membuat user baru dan untuk mengecek apakah data user sudah terdapat dalam database atau tidak. Karena jika sudah terdapat pada database, program akan menolak pertambahan user.
 
 2. Inisialisasi pada fungsi `initialization`

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
* Fungsi ini dibuat sebagai format awal untuk mengakses database.

3. Membuat tabel baru menggunakan fungsi `table_maker`

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

* Fungsi ini digunakan untuk membuat tabel baru pada database.

4. Fungsi selanjutnya adalah grant user dengan fungsi `grant_user`

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
 
 * Fungsi `grant_user` digunakan untuk memberi akses bagi user yang ingin mengakses databasenya. User harus mengikuti format command yang telah diberikan untuk dicek dari root.

5. Fungsi selanjutnya digunakan untuk membuat database, fungsi ini bernama `create_database`

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
 
 * User hanya bisa membuat database jika sudah mendapat akses dari root.

6. Fungsi selanjutnya untuk menghapus data pada database. Fungsi ini bernama `delete_reader`

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
              
* Fungsi ini akan melakukan iterasi baris dan kolom pada database untuk mendapatkan data yang ingin dihapus oleh user.
