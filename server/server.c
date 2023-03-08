#include "headers/server.h"

int
main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        perror("Failed to create socket!\n");
        return 1;
    }
    puts("Socket created...");

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    /// Binding
    if (bind(socket_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Failed to bind!");
        return 2;
    }
    puts("Binding completed...");

    /// Listen
    if (listen(socket_fd, MAX_CLIENT) < 0) {
        perror("Failed to listen!");
        return 3;
    }
    puts("Listening completed...");

    outputAdrressPortNumber();

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    int c = sizeof(struct sockaddr_in);

    int client_sock;
    int* new_sock;
    struct sockaddr_in client;

    while ( (client_sock = accept(socket_fd, (struct sockaddr*)&client, (socklen_t*)&c)) ) {

        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0) {
            perror("Failed to create thread!");
            return 4;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");

    }
    
    if (client_sock < 0) {
        perror("Failed to accept!");
        return 5;
    }

    return 0;
}

/*
 * This will handle connection for each client
 * */
void*
connection_handler(void* socket_desc)
{

    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;

    //Send some messages to the client
    char client_command[MAX_LENGTH] = {0};

    //Receive a message from client
    while ( (read_size = recv(sock, client_command, MAX_LENGTH, 0)) > 0 ) {

        // Remove the newline character from the end of the command string
        client_command[strcspn(client_command, "\n")] = '\0';

        // if disconnect command is entered
        if (strncmp(client_command, "disconnect", 10) == 0) {

            read_size = 0; break;

        } else if (strncmp(client_command, "rm", 2) == 0 ||
                   strncmp(client_command, "cp", 2) == 0 ||
                   strncmp(client_command, "mv", 2) == 0 ||
                   strncmp(client_command, "exit", 4) == 0) {

            write(sock, "Unsafe command!", 16); continue;

        } else {

            FILE* filePtr = fopen("output.txt", "w");

            char command_with_redirect[MAX_LENGTH] = {0};
            snprintf(command_with_redirect, sizeof(command_with_redirect), "%s > output.txt 2>&1", client_command);
            system(command_with_redirect);

            memset(client_command, '\0', MAX_LENGTH);

            filePtr = fopen("output.txt", "r");
            
            write(sock , filePtr, 100);

            fclose(filePtr);

        }
    }

    if (read_size == 0) {
        puts("Client disconnected");
        write(sock , "Disconnected", 12);
        fflush(stdout);
    } else if (read_size == -1) {
        perror("recv failed");
    }
            
    //Free the socket pointer
    free(socket_desc);
    
    return 0;
}

void
outputAdrressPortNumber()
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }

            printf("IPv4 address for %s: %s\n", ifa->ifa_name, host);
        }
    }
    printf("Port number: %d\n", PORT);

    freeifaddrs(ifaddr);
}

