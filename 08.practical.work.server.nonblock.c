#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define PORT 8784
int main()
{
    setbuf(stdout, NULL);
    int sockfd, clientfd;
    socklen_t clen;
    struct sockaddr_in saddr, caddr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        return 1;
    }

    setsockopt(sockfd, SOL_SOCKET,
               SO_REUSEADDR, &(int){1},
               sizeof(int));

    int sockfl = fcntl(sockfd, F_GETFL, 0);
    sockfl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, sockfl);

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
    char buff[2048];
    struct pollfd _pollfd;
    _pollfd.fd = fileno(stdin);
    _pollfd.events = POLLIN;

    for (;;)
    {
        clientfd = accept(sockfd, (struct sockaddr *)&caddr, &clen);
        if (clientfd > 0)
        {
            printf("A client connected!\n");
            printf("Type: ");

            int clientfl = fcntl(clientfd, F_GETFL, 0);
            clientfl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, clientfl);
            int isConnected = 1;
            while (isConnected)
            {
                if (recv(clientfd, buff, sizeof(buff), 0) > 0)
                {
                    printf("\rClient message: ");
                    printf("%s", buff);
                    printf("Type: ");
                }
                if (poll(&_pollfd, 1, 0) > 0)
                {
                    if (_pollfd.revents & POLLIN)
                    {
                        fgets(buff, sizeof(buff), stdin);
                        send(clientfd, buff, strlen(buff) + 1, 0);
                        printf("Type: ");
                    }
                }
            }

            close(clientfd);
        }
    }
    close(sockfd);
    return 0;
}