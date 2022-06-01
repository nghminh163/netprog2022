#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    // char input[255];
    // if (argc > 1)
    // {
    //     strcpy(input, argv[1]);
    // }
    // else
    // {
    //     printf("Enter a domain name: ");
    //     fgets(input, sizeof(input), stdin);
    //     input[strlen(input) - 1] = '\0';
    // }
    struct hostent *host_info;
    // if ((host_info = gethostbyname(input)) == NULL)
    if ((host_info = gethostbyname("localhost")) == NULL)

    {
        printf("No such host\n");
        return 1;
    }
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
    }
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *)&saddr.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
    saddr.sin_port = htons(8784);
    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("Error connecting.\n");
        return 1;
    }

    printf("Connected to %s\n", inet_ntoa(*(struct in_addr *)(host_info->h_addr)));
    char rBuff[2048] = "";
    char sBuff[2048];

    while (1)
    {
        printf("> ");
        scanf("%s", sBuff);
        send(sockfd, sBuff, sizeof(sBuff), 0);
        recv(sockfd, rBuff, sizeof(rBuff), 0);
        printf("Server: %s\n", rBuff);
    }

    return 0;
}