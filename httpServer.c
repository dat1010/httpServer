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


char *getHtmlFile(char fileName[])
{
    FILE *fp;
    char *buffer;

    fp = fopen ( fileName , "rb" );
    if( !fp ) perror(fileName),exit(1);

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

char* getTime(){
    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return buf;
}

void headers(int client, const char *filename)
{
 char buf[1024];
 (void)filename;  /* could use filename to determine file type */

 strcpy(buf, "HTTP/1.0 200 OK\r\n");
 send(client, buf, strlen(buf), 0);
 //strcpy(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 strcpy(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
}


int main(int argc, char *argv[])
{
    int port = 8080;
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

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
            printf("Could not connect to Client.");
            return 1;
        }
        printf("Got connection\n");
        printf("%s\n",getTime());

        if (client_fd == -1) {
            perror("Can't accept");
            continue;
        }

        char line[5000];
        recv(client_fd,line,5000,0);
        char* token = strtok(line, " ");
        token = strtok(NULL," ");
        printf ("got from client: %s \n",token);
        char filep [5000];
        char cpytkn[5000];
        strcpy (cpytkn, token);
        memcpy (filep, &cpytkn[1], 4999);
        printf ("filep: %s \n", filep);
        if ( strcmp(filep, "") == 0){
          headers(client_fd,"index.html");
          send(client_fd, getHtmlFile("index.html"), lSize -1  , 0);
        }
        else {
          send(client_fd, getHtmlFile(filep), lSize -1  , 0);
        }
          close(client_fd);
    }

    return 1;
}
