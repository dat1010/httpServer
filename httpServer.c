#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <string.h>
#include <time.h>

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
    
    listen(sock, 5);
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
        
        write(client_fd, getHtmlFile("index.html"), lSize-1);
        close(client_fd);
    }
    
    return 1;
}
