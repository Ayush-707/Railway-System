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

// This function checks whether a string contains only numeric characters
int isNumber(char *s) {
    int i = 0;

    // Loop through each character in the string
    while (s[i] != '\0') {

        // If the character is not a digit, return 0 (false)
        if (!isdigit(s[i])) {
            return 0;
        }

        // Move on to the next character in the string
        i++;
    }

    // If all characters are digits, return 1 (true)
    return 1;
}


int main() {

                // Declare variables
            int sock, num_bytes;
            struct sockaddr_in servaddr;
            char buffer[BUFFER_SIZE];

            // Create socket
            sock = socket(AF_INET, SOCK_STREAM, 0); // create a socket with the given domain, type and protocol
            if (sock < 0) { // check if socket creation was unsuccessful
                printf("Error creating socket\n"); // print an error message
                return 1; // exit the program with an error code
            }

            // Fill in server address information
            memset(&servaddr, 0, sizeof(servaddr)); // fill the memory with 0s
            servaddr.sin_family = AF_INET; // set the address family to IPv4
            servaddr.sin_port = htons(SERVER_PORT); // set the server port number
            if (inet_pton(AF_INET, SERVER_ADDRESS, &servaddr.sin_addr) <= 0) { // convert the server address from text to binary
                printf("Invalid address or address not supported\n"); // print an error message if the conversion fails
                return 1; // exit the program with an error code
            }
            // Connect to server
            if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { // Try to connect to the server
                printf("Error connecting to server\n"); // If connection fails, print error message
                return 1; // Return 1 to indicate an error has occurred
            } else {
                printf("\n************Connected to the server************\n"); // If connection succeeds, print success message
            }

            char type[100],quantity[100];

            do {
                printf("\nEnter the type of tickets required:\n");
                printf("1. Second Sitting(2S)\n2. Air-Condition(AC)\n3. Sleeper(SL)\n");
                printf("Input ( e.g., Enter SL for Sleeper ): ");

                fgets(type, 100, stdin); // Get input from user for type of ticket required

                type[strcspn(type, "\n")] = '\0'; // Remove newline character from input

                if (strcmp(type, "SL") != 0 && strcmp(type, "AC") != 0 && strcmp(type, "2S") != 0) { // If input is not a valid ticket type, print error message
                    puts("\nInvalid Input, Please Try Again!");
                }
            } while (strcmp(type, "SL") != 0 && strcmp(type, "AC") != 0 && strcmp(type, "2S") != 0); // Repeat until valid input is received

            do {
                printf("\nEnter the number of tickets required: ");
                fgets(quantity,100,stdin); // Get input from user for number of tickets required
                quantity[strcspn(quantity, "\n")] = '\0'; // Remove newline character from input

            } while ( !isNumber(quantity)); // Repeat until valid input (a number) is received

            strcpy( buffer, type ); // Copy ticket type to buffer
            strcat( buffer, " "); // Add a space to buffer
            strcat( buffer, quantity); // Add ticket quantity to buffer

            printf("%s",buffer); // Print input data

            // Pass command and data to server
            printf("\nSending Input Data to the Server\n\n");

            num_bytes = send(sock, buffer, strlen(buffer), 0); // Send input data to server
            if (num_bytes < 0) {
                printf("Error sending data to server\n"); // If sending data fails, print error message
                return 1; // Return 1 to indicate an error has occurred
            }

            //Receiving data from the server
            char output_string[1024];
            int bytes = recv(sock, output_string, 1024, 0); // Receive data from server
            if (bytes < 0) {
                printf("Error receiving data from server\n"); // If receiving data fails, print error message
                return 1; // Return 1 to indicate an error has occurred
            }

            output_string[bytes] = '\0'; // Add null terminator to the end of the string
            printf("Received data: %s\n\n", output_string); // Print output data received from server

            // Close socket
            close(sock);

            return 0;
            }
