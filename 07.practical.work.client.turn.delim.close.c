#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> // for close

#define PORT 8784

int main(int argc, char **argv)
{
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

    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("Error connecting.\n");
        return 1;
    }
    char ip_addr[32];
    printf("Connected to %s\n", inet_ntoa(*(struct in_addr *)(host_info->h_addr)));

    char buff[2048];
    int isConnected = 1;
    while (isConnected)
    {
        printf("Type your message: ");
        do
        {
            fgets(buff, sizeof(buff), stdin);
            if (strcmp(buff, "/quit\n") == 0)
            {
                strcpy(buff, "Disconnect!\n");
                isConnected = 0;
            }
            send(sockfd, buff, strlen(buff) + 1, 0);

        } while (buff[strlen(buff) - 1] != '\n');
        if (!isConnected)
        {
            printf("Exiting.\n");
            break;
        }
        printf("Server message: ");
        do
        {
            recv(sockfd, buff, sizeof(buff), 0);
            printf("%s", buff);
        } while (buff[strlen(buff) - 1] != '\n');
    }
    close(sockfd);

    return 0;
}