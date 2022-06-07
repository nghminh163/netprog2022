#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> // for close
#include <fcntl.h>
#include <poll.h>

#define PORT 8784

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    struct sockaddr_in saddr;
    int sockfd;

    char input[255];
    if (argc > 1)
    {
        strcpy(input, argv[1]);
    }
    else
    {
        printf("Enter a domain name: ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0';
    }

    struct hostent *host_info;
    if ((host_info = gethostbyname(input)) == NULL)
    {
        perror("Error Resolving.\n");
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error creating socket.\n");
        return 1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *)&saddr.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
    saddr.sin_port = htons(PORT);

    setsockopt(sockfd, SOL_SOCKET,
               SO_REUSEADDR, &(int){1},
               sizeof(int));

    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("Error connecting.\n");
        return 1;
    }

    int sockfl = fcntl(sockfd, F_GETFL, 0);
    sockfl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, sockfl);

    struct pollfd _pollfd;
    _pollfd.fd = fileno(stdin);
    _pollfd.events = POLLIN;

    char ip_addr[32];
    printf("Connected to %s\n", inet_ntoa(*(struct in_addr *)(host_info->h_addr)));

    char buff[2048];
    int isConnected = 1;
    printf("Type: ");

    while (isConnected)
    {
        if (recv(sockfd, buff, sizeof(buff), 0) > 0)
        {
            printf("\rServer message: ");
            printf("%s", buff);
            printf("Type: ");
        }
        if (poll(&_pollfd, 1, 0) > 0)
        {
            if (_pollfd.revents & POLLIN)
            {
                fgets(buff, sizeof(buff), stdin);
                send(sockfd, buff, strlen(buff) + 1, 0);
                printf("Type: ");
            }
        }
    }
    close(sockfd);

    return 0;
}