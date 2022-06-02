#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    int sockfd, clientfd;
    socklen_t clen;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        return 1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("Error binding");
        return 1;
    }

    if (listen(sockfd, 5) < 0)
    {
        perror("Error listening");
        return 1;
    }
    printf("Listening on port %d...\n", port);

    clen = sizeof(caddr);
    if ((clientfd = accept(sockfd, (struct sockaddr *)&caddr, &clen)) < 0)
    {
        perror("Error accepting connection");
        return 1;
    }

    printf("A client IP address is %s connected\n", inet_ntoa(caddr.sin_addr));
    char sBuff[2048] = "";
    char rBuff[2048];

    while (1)
    {
        recv(clientfd, sBuff, sizeof(sBuff), 0);
        printf("From client: %s\n", sBuff);
        printf("> ");
        fgets(rBuff, sizeof(rBuff), stdin);
        send(clientfd, rBuff, sizeof(rBuff), 0);
    }

    return 0;
}