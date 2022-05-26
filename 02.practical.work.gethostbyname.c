#include <netinet/in.h> // in_addr
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h> // hostent

int main(int argc, char *argv[])
{
    struct hostent *host_info;

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

    host_info = gethostbyname(input);
    if (host_info == NULL)
    {
        printf("No such host\n");
        exit(1);
    }

    for (int i = 0; host_info->h_addr_list[i] != NULL; i++)
    {
        struct in_addr *address;
        address = (struct in_addr *)(host_info->h_addr_list[i]);

        printf("%s has address %s\n", input, inet_ntoa(*address));
    }
    return 0;
}
