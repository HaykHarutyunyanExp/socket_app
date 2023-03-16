#include "headers/server.h"

int
main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        perror("Failed to create socket!\n");
        exit(1);
    }
    printf("Socket created...\n");

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    /// Binding
    if (bind(socket_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Failed to bind!");
        exit(2);
    }
    printf("Binding completed...\n");

    /// Listen
    if (listen(socket_fd, MAX_CLIENT) < 0) {
        perror("Failed to listen!");
        exit(3);
    }
    printf("Listening completed...\n");

    outputAdrressPortNumber();

    //Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    int c = sizeof(struct sockaddr_in);

    int client_sock;
    int* new_sock;
    struct sockaddr_in client;

    while ( (client_sock = accept(socket_fd, (struct sockaddr*)&client, (socklen_t*)&c)) ) {

        printf("Connection accepted\n");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0) {
            perror("Failed to create thread!");
            exit(4);
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        printf("Handler assigned\n");

    }
    
    if (client_sock < 0) {
        perror("Failed to accept!\n");
        exit(5);
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
            FILE* filePtr = popen(client_command, "r");
            memset(client_command, '\0', MAX_LENGTH);
            int n;

            while ( (n = fread(client_command, 1, MAX_LENGTH, filePtr) ) > 0) {
                client_command[n] = '\0';
            }

            write(sock, client_command, MAX_LENGTH);
            pclose(filePtr);
        }
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
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

