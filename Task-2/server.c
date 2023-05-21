#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define PORT_NO 9001
#define CLIENTS 5
#define AC
#define SL

// Declaration of function handle_input
char* handle_input ( char *data);

// Declaration of function update_seat_availability
void update_seat_availability(const char* seat_type, int remaining_seats);

// Declaration of two mutex locks for file access synchronization
pthread_mutex_t lock_file = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_file2 = PTHREAD_MUTEX_INITIALIZER;

// Definition of a struct to hold connection data
typedef struct {

    struct sockaddr_in adres;
    int address_length;
    int bind_result;

} connect__data;

// Definition of a struct to hold arguments for a function
struct args {
    int arg1;
    int arg2;
};

// Definition of function handle_client, which will be executed in a separate thread for each client
void *handle_client(void *arg) {

    // Convert void pointer argument to a struct of two integers
    struct args *my_args = (struct args *) arg;

    // Extract the two integers from the struct
    int i = my_args ->arg1; // i is the client number
    int new_socket = my_args -> arg2; // new_socket is the socket descriptor for the client connection

    /* --------------------------------------------------------------------------------*/

    // Receive the client request using recv() system call

    char buffer[1024];
    int bytes_received = recv(new_socket, buffer, 1024, 0);
    if (bytes_received < 0) {
        printf("Error receiving client request\n");
        printf("Error: %d - %s\n", errno, strerror(errno));
        return NULL;
    } else {
        printf("Received data from Client-%d\n",i);
        
    }

    buffer[bytes_received] = '\0';

    // Close the socket

    // Dynamically allocate memory for a message to be sent back to the client
    char* message = malloc(2048*sizeof(char));
    
    // Call the handle_input function to process the client request and generate a response message
    message = handle_input(buffer);

    if ( message != NULL) {

        // Send the message to the client
        int bytes_sent = send(new_socket, message, strlen(message), 0);
        if (bytes_sent < 0) {
            printf("Error sending message to client\n");
        }

        close(new_socket);

        // Mutex lock to protect the critical section
        static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&lock);

        // Access the shared resources
        FILE* file = fopen("server_records.txt", "a");

        if (file != NULL) {
            fprintf(file, "[Client-%d] Request: %s\nServer Response: %s\n\n", i, buffer, message);
            fclose(file);
        } 

        pthread_mutex_unlock(&lock);
    } 
    
    
    return NULL;
}

char* handle_input(char* data) {
    int i = 1; 
    int k = 0; 
    char seat_type[50]; // buffer for storing seat type
    char number_of_seats[50]; // buffer for storing number of seats

    // Read the seat type from the input data
    sscanf(data, "%s", seat_type);

    // Move the pointer to the number of seats
    char* ptr = data + strlen(seat_type);
    while ( *ptr == ' ' ) ptr++; // Skip over whitespace

    // Copy the number of seats to a buffer
    strcpy (number_of_seats, ptr);

    // Allocate memory for the output message and file message
    char* msg = malloc(1024*sizeof(char));
    char* file_msg = malloc(1024 * sizeof(char));

    // Lock access to the file to prevent multiple threads from writing to it simultaneously
    pthread_mutex_lock(&lock_file);

    // Open the seat availability file
    FILE* file = fopen("Seat_Availability.txt", "r+");

    if (file != NULL) {

        int found = 0;

        char line[500], avl_seats[50];

        // Read each line of the file
        while (fgets(line, sizeof(line), file) != NULL) {

            // Extract the seat type and available seats from the line
            char* token = strtok(line, " ");

            if (strcmp(token, seat_type) == 0) {

                // The requested seat type was found
                token = strtok(NULL, " ");
                strcpy(avl_seats, token);
                found = 1;
                break;
            }
        }

        if (found) {

            // Convert the number of required and available seats to integers
            int required_seats = atoi(number_of_seats);
            int available_seats = atoi(avl_seats);

            int remaining_seats = 0;

            char new_data[500];

            if (required_seats > available_seats) {

                // Not enough seats are available
                strcpy(msg,"Required Number of Seats Not Available");

            } else {

                // Update the number of available seats in the file
                remaining_seats = available_seats - required_seats;
                sprintf(new_data, "%s %d", seat_type, remaining_seats);
                update_seat_availability(seat_type, remaining_seats);

                // Generate a message indicating that the seats were booked successfully
                sprintf(msg, "%d Seats have been booked for type %s", required_seats, seat_type);
            }

        } else {

            // The requested seat type was not found in the file
            strcpy(msg,"Seat type not found!");
        }

        // Close the file
        fclose(file);
    }

    // Unlock access to the file
    pthread_mutex_unlock(&lock_file);

    // Return the output message
    return msg;
}

    
   
void update_seat_availability(const char* seat_type, int remaining_seats) {
    pthread_mutex_lock(&lock_file2);

    FILE* file = fopen("Seat_Availability.txt", "r+");
    if (file == NULL) {
        perror("Failed to open Seat_Availability.txt");
        pthread_mutex_unlock(&lock_file2);
        return;
    }

    // Find the line for the specified seat type
    char line[500];
    int line_num = -1; // initialize to an invalid value
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        // Remove the newline character from the end of the line
        strtok(line, "\n");
        if (strstr(line, seat_type) != NULL) {
            // Update the remaining seats for this seat type
            int pos = ftell(file);
            fseek(file, pos - strlen(line)-1, SEEK_SET);
            fprintf(file, "%-2s %5d\n", seat_type, remaining_seats);
            break;
        }
    }
    
    fclose(file);
    pthread_mutex_unlock(&lock_file2);
}


void handle_agent_login () {

    // Set the correct user ID and password
    char* user_id = "Agent123";
    char* password = "123456";

    // Initialize variables to store user input
    char input_id[100];
    char input_pass[100];

    do {

        // Prompt the user to enter their login credentials
        puts("Agent Login:");
        printf("Enter User ID: ");

        // Store the user's input for their user ID
        fgets(input_id,100,stdin);

        // Remove the newline character at the end of the input string
        input_id[strcspn(input_id, "\n")] = '\0';

        printf("Enter Password: ");

        // Store the user's input for their password
        fgets(input_pass, 100, stdin);

        // Remove the newline character at the end of the input string
        input_pass[strcspn(input_pass, "\n")] = '\0';

        // Check if the user's login credentials are valid
        if ( strcmp(input_id,user_id) != 0 || strcmp(input_pass,password) != 0) {

            // If the login credentials are invalid, display an error message and prompt the user to try again
            puts("Invalid Login Credentials, Please Try Again!\n");
        }

    } while ( strcmp(input_id,user_id) != 0 || strcmp(input_pass,password) != 0);

    // If the login credentials are valid, return control to the calling function
    return;
}

// This function creates a socket and returns its file descriptor
int create_socket () {

    // Create a TCP/IP socket
    int my_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Check if socket creation was successful
    if ( my_socket == -1 ) {
        printf("Socket Error!\n");
        return -1;
    } else {
        puts("Socket creation successful!");
    }

    // Return the socket file descriptor
    return my_socket;

}

connect__data bind_port(int sock) {

    connect__data temp;

    struct sockaddr_in address;
    int adlength = sizeof(address);

    memset(&address, 0, sizeof(address)); // Initialize address structure with zeros

    address.sin_family = AF_INET; // Set the address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
    address.sin_port = htons(PORT_NO); // Convert the port number to network byte order

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        printf("Error setting socket options!\n");
        exit(0);
    }

    int bind_res = bind(sock, (struct sockaddr*)&address, sizeof(address)); // Bind the socket to the specified address and port

    if (bind_res == -1) {
        printf("Bind Error!\n");
        exit(0);
    } else {
        puts("Bind Successful!");
    }

    temp.adres = address; // Store the address structure in the connect__data object
    temp.address_length = adlength; // Store the length of the address structure
    temp.bind_result = bind_res; // Store the result of the bind operation

    return temp;
}

int main() {

    // Call a function to handle agent login
    handle_agent_login();

    // Declare variables for socket creation and binding
    int i = 1;
    struct sockaddr_in address;
    int adlength;
    
    // Create a socket using socket() system call
    int sock = create_socket();

    if (sock == -1) {
        // If there was an error creating the socket, print an error message
        printf("\n*************Socket Error!*******************\n");
    }

    // Bind server’s address and port using bind() system call
    connect__data result = bind_port(sock);

    // Get the address and address length from the connect_data struct
    address = result.adres;
    adlength = result.address_length;

    // Get the bind result from the connect_data struct
    int bind_res = result.bind_result;

    // Convert the socket into a listening socket using listen() system call
    if (listen(sock, CLIENTS) < 0) {
        // If there was an error setting the socket to listen, print an error message
        printf("Listen Error!\n");
        return 1;
    } 

    // Accept client connections using accept() system call
    printf("\nServer is listening on port %d....\n\n", PORT_NO);

    while (1) {

       int new_socket = accept(sock, (struct sockaddr*)&address, (socklen_t*)&adlength);

        if (new_socket < 0) {
            // If there was an error accepting the client connection, print an error message
            printf("Accept Error!\n");
            return 1;
        }

        printf("Client-%d connected\n",i);

        // Create a new thread to handle the client request
        pthread_t client_thread;

        // Create a struct to hold the arguments for the client thread
        struct args my_args = {i,new_socket};
        
        // Create the client thread and pass it the struct of arguments
        if (pthread_create(&client_thread, NULL, handle_client, (void*) &my_args) < 0) {
            // If there was an error creating the client thread, print an error message
            printf("Error creating thread for client\n");
            return 1;
        }

        i++;
    }

    // Close the socket
    close(sock);

    return 0;
}
