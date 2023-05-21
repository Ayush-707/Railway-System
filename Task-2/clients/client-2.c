#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 9001
#define BUFFER_SIZE 1024


int isNumber(char *s) {
    int i = 0;
    while (s[i] != '\0') {
        if (!isdigit(s[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}

int main() {
    int sock, num_bytes;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Error creating socket\n");
        return 1;
    }

    // Fill in server address information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &servaddr.sin_addr) <= 0) {
        printf("Invalid address or address not supported\n");
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("Error connecting to server\n");
        return 1;
    } else {

        printf("\n************Connected to the server************\n");

    }
    char type[100],quantity[100];

    do {
        printf("\nEnter the type of tickets required:\n");
        printf("1. Second Sitting(2S)\n2. Air-Condition(AC)\n3. Sleeper(SL)\n");
        printf("Input ( e.g., Enter SL for Sleeper ): ");

        fgets(type, 100, stdin);

        type[strcspn(type, "\n")] = '\0';

        if (strcmp(type, "SL") != 0 && strcmp(type, "AC") != 0 && strcmp(type, "2S") != 0) {
            puts("\nInvalid Input, Please Try Again!");
        }
    } while (strcmp(type, "SL") != 0 && strcmp(type, "AC") != 0 && strcmp(type, "2S") != 0);

    do {
        printf("\nEnter the number of tickets required: ");
        fgets(quantity,100,stdin);
        quantity[strcspn(quantity, "\n")] = '\0';

    } while ( !isNumber(quantity)); 


    strcpy( buffer, type );
    strcat( buffer, " ");
    strcat( buffer, quantity);

    printf("%s",buffer);

    // Pass command and data to server
    printf("\nSending Input Data to the Server\n\n");

    
    num_bytes = send(sock, buffer, strlen(buffer), 0);
    if (num_bytes < 0) {
        printf("Error sending data to server\n");
        return 1;
    }


    //Receiving data from the server
    char output_string[1024];
    int bytes = recv(sock, output_string, 1024, 0);
    if (bytes < 0) {
        printf("Error receiving data from server\n");
        return 1;
    }

    output_string[bytes] = '\0'; // add null terminator to the end of the string
    printf("Received data: %s\n\n", output_string);


    // Close socket
    close(sock);

    return 0;
}
