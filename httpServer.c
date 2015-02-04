#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

int lSize = 0;
void notFound(int);


char *getHtmlFile(char fileName[])
{
    FILE *fp = NULL;
    char *buffer;

    fp = fopen ( fileName , "rb" );
    if( fp == NULL) {
        printf ("null");
      //404 Error
      return NULL;
    }

    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    buffer = calloc( 1, lSize+1 );
    if( !buffer ) fclose(fp),fputs("Memory alloc fails",stderr),exit(1);

    /* copy the file into the buffer */
    if( 1!=fread( buffer , lSize, 1 , fp) )
        fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);
    fclose(fp);
    return buffer;
}


void notFound(int client)
{
 char buf[1024];
 lSize = 0;
 sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 send(client, getHtmlFile("404Error.html"), lSize -1  , 0);
}

void headers(int client, const char *filename)
{
 char buf[1024];
 //(void)filename;  /* could use filename to determine file type */

 strcpy(buf, "HTTP/1.0 200 OK\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 strcpy(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
}

char* getTime()
{
    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return buf;
}



int main(int argc, char *argv[])
{
    int port = 8080;
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);
    char * root;
    int root_given = 0;
    char * logfile;
    int log_f = 0;
    FILE *f;

    if ( argc > 2){
        int i = 1;
        while(i < argc - 1){
            if (strcmp(argv[i], "-p") == 0){
                port = atoi(argv[i+1]);
            }
            else if (strcmp(argv[i], "-docroot") == 0){
                root_given = 1;
                root = argv[i+1];
            }
            else if (strcmp(argv[i], "-logfile") == 0){
                log_f = 1;
                logfile = argv[i+1];
                f = fopen(logfile, "a");
                if (f == NULL){
                    printf ("File not found.");
                    return 1;
                }
            }
            i++;
        }
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        err(1, "Can't open socket");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));


    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
        close(sock);
        err(1, "Can't bind");
    }

    listen(sock, 10);
    while (1) {
        client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);

        if (client_fd < 0) {
            if ( log_f == 1){
                fprintf(f,"Could not connect to Client.");
            }
            else{
                printf("Could not connect to Client.");
            }
            return 1;
        }
        if ( log_f == 1){
            fprintf(f,"Got connection\n");
            fprintf(f,"%s\n",getTime());
        }
        else{
            printf("Got connection\n");
            printf("%s\n",getTime());
        }

        if (client_fd == -1) {
            if ( log_f == 1){
                fprintf (f,"Can't accept");
            }
            else {
                perror("Can't accept");
            }
            continue;
        }


        char line[5000];
        recv(client_fd,line,5000,0);

        if(strncmp(line, "GET ", 4) != 0)
        {
			headers(client_fd,"");
			send(client_fd, getHtmlFile("501Error.html"), lSize -1  , 0);

		}
        char* token = strtok(line, " ");
        token = strtok(NULL," ");

        if ( log_f == 1){
            fprintf (f,"got from client: %s \n",token);
        }
        else{
            printf ("got from client: %s \n",token);
        }
        char filep [5000];
        char cpytkn[5000];
        if(token != NULL)
        {
          strcpy (cpytkn, token);
          memcpy (filep, &cpytkn[1], 4999);
        }

        if ( strcmp(filep, "") == 0)
        {
          headers(client_fd,"index.html");
          send(client_fd, getHtmlFile("index.html"), lSize -1  , 0);

        }
        else
        {
          char* fileString;
          printf ("here2");
          if (root_given == 1){
              if (log_f == 1){
                  fprintf (f,"the path: %s \n",strcat(strcat(root,"/"),filep));
              }
              else {
                  printf ("the path: %s \n",strcat(strcat(root,"/"),filep));
              }
              fileString = getHtmlFile(strcat(strcat(root,"/"),filep));
          }
          else {
              fileString = getHtmlFile(filep);
          }

          if (fileString != NULL)
          {
			  int f_block_sz;
              char * dot = strrchr(filep, '.');
              printf ("dot: %s \n", dot);
              if ( strcmp(dot, ".html")==0 ||strcmp(dot, ".js")==0||strcmp(dot, ".css")==0||strcmp(filep, "darknight.jpeg") == 0){
                  headers(client_fd,fileString);
                  send(client_fd, fileString, lSize -1  , 0);
			  }
              else{
				  FILE* fp = fopen (filep, "r");
				  char sdbuf[1024*1024];
			      bzero (sdbuf, 1024*1024);
				  f_block_sz = fread (sdbuf,sizeof(char),1024 * 1024,fp);
					send ( client_fd, sdbuf, f_block_sz, 0);

			}

          }else{
              if (log_f == 1){
                  fprintf(f,"404\n");
              }else{
                  printf("404\n");
              }
              notFound(client_fd);
          }
        }
        if (log_f == 1){
            fclose(f);
        }
        close(client_fd);
    }

    return 1;
}
