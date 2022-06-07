#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8784
int main()
{
    int sockfd, clientfd;
    socklen_t clen;
    struct sockaddr_in saddr, caddr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        return 1;
    }
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(PORT);

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
    printf("Listening on port %d...\n", PORT);
    clen = sizeof(caddr);
    if ((clientfd = accept(sockfd, (struct sockaddr *)&caddr, &clen)) < 0)
    {
        perror("Error accepting connection");
        return 1;
    }

    printf("A client IP address is %s connected\n", inet_ntoa(caddr.sin_addr));
    char buff[2048];

    while (1)
    {
        printf("Client message: ");
        do
        {
            int n = recv(clientfd, buff, sizeof(buff), 0);
            if (n <= 0)
            {
                printf("Something went wrong when receive");
                return 1;
            }
            else
            {
                printf("%s", buff);
            }
        } while (buff[strlen(buff) - 1] != '\n');
        printf("Type your message: ");
        do
        {
            fgets(buff, 1024, stdin);
            send(clientfd, buff, strlen(buff) + 1, 0);
        } while (buff[strlen(buff) - 1] != '\n');
    }
    return 0;
}