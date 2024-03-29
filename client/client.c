#include "headers/client.h"

int
main()
{
    while (1) {
        
        struct sockaddr_in server;
        
        //Create socket
        int sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1) {
            perror("Could not create socket");
            exit(1);
        }
        printf("\nSocket created...");
        printf("\nTo connect to a server.\n\tconnect <IP_ADDRESS> <PORT>");

        char server_reply[MAX_SERVER_REAPLY] = {0};
        char command[MAX_COMMAND_LENGTH] = {0};

        char* s[3]; /// array to store command, address, and port

        printf("\nClient> ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);

        /// get the first token
        char* token = strtok(command, " ");
        s[0] = token;

        for (int i = 0; token != NULL; ++i) {
            s[i] = token;
            token = strtok(NULL, " ");
        }

        if (strncmp(s[0], "exit", strlen("exit")) == 0) {
            exit(0);
        } else if (strncmp(s[0], "connect", strlen("connect")) == 0) {

            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(s[1]);
            int port = atoi(s[2]);
            server.sin_port = htons(port);

            //Connect to remote server
            if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
                perror("connect failed. Error");
                continue;
            } else {
                printf("Connected\n");
                //keep communicating with server
                while(1) {

                    printf("\nClient> ");
                    fgets(command, MAX_COMMAND_LENGTH, stdin);
                    if (strncmp(command, "\n", 1) == 0) { continue; }
                    
                    //Send some data
                    if (send(sock , command , strlen(command) , 0) < 0) {
                        perror("Send failed");
                        exit(2);
                    }
                    
                    //Receive a reply from the server
                    if (recv(sock , server_reply , MAX_SERVER_REAPLY, 0) < 0) {
                        perror("recv failed");
                        exit(3);
                    }
                    
                    printf("%s\n", server_reply);
                    if (strncmp(server_reply, "Disconnected", strlen("Disconnected")) == 0) break;

                    memset(command, '\0', MAX_COMMAND_LENGTH);
                    memset(server_reply, '\0', MAX_SERVER_REAPLY);
                }
            
            }

        } else {
            printf("\nCommand not found!");
        }

        close(sock);
        memset(command, '\0', MAX_COMMAND_LENGTH);

    }

    return 0;
}

