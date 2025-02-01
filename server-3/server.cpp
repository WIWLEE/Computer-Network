/*
The server side Chat source code.
Student Name : Jihyun Lee
Student Number : 14478965
Start Date : 9/29/2024
End date : 10/14/2024
- This source code implements the server side chat program including 4 functions : newuser, login, send, logout
*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include "winsock2.h"

#define SERVER_PORT   18965 //1 + last four student number
#define MAX_PENDING   5
#define MAX_LINE      256

/*
This function is for command "newuser".
It processes the server's response code and print an appropriate messages.
*/
int createNewUser(char* buf) {

    //open users.txt file
    FILE* userFile = fopen("../users.txt", "r");

    //seperate the buffer
    char* cmd = strtok(buf, " ");
    char* id = strtok(NULL, " ");
    char* password = strtok(NULL, "\n");

    //if the userfile doesn't exist, create new userfile.
    if (userFile == NULL) {
        userFile = fopen("../users.txt", "w");
        fclose(userFile);
        userFile = fopen("../users.txt", "r");
    }

    //if the length of id or password is out of bound, return error code
    if (strlen(id) < 3 || strlen(id) > 32 || strlen(password) < 4 || strlen(password) > 8) {
        printf("length of id or password wrong : %d %d\n", strlen(id), strlen(password));
        return 3;
    }
    //if buffer is accepted well, new user is added. 
    else {

        //check if there's same named user in user.txt
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), userFile) != NULL) {
            char checkId[MAX_LINE];
            char checkPassword[MAX_LINE];

            sscanf(line, "(%[^,],%[^)])\n", checkId, checkPassword);
            if (strcmp(checkId, id) == 0) {
                //if same name user exists in users.txt, return error code
                return 2;
            }
        }

        //write the new user into the file, and close the file 
        fclose(userFile);
        FILE* userFile = fopen("../users.txt", "a");
        fprintf(userFile, "\n(%s, %s)", id, password);
        printf("New user account created.\n");
        fclose(userFile);
    }
    return 0;
}

/*
This function is for command "login".
It processes the server's response code and print an appropriate messages.
*/
int login(char* buf, bool* login_status, char** login_name) {

    //open the file users.txt
    FILE* userFile = fopen("../users.txt", "r");

    //seperate the buffer
    char* cmd = strtok(buf, " ");
    char* id = strtok(NULL, " ");
    char* password = strtok(NULL, "\n");

    //check the file to find the corresponding user.
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), userFile) != NULL) {
        char checkId[MAX_LINE];
        char checkPassword[MAX_LINE];
        sscanf(line, "(%[^,], %[^)])\n", checkId, checkPassword);

        //if the corresponding user exists, return success code
        if (strcmp(checkId, id) == 0 && strcmp(checkPassword, password) == 0) {
            printf("%s login.\n", id);
            *login_status = true;
            *login_name = _strdup(id);
            fclose(userFile);
            return 0; //login successfully
        }
    }

    //if the corresponding user doesn't exist, return error code
    fclose(userFile);
    return 1; //login failed
}

/*
This function is for command "send".
It processes the server's response code and print an appropriate messages.
*/
char* send_message(char* buf, char* login_name) {

    //seperate the buffer
    char* cmd = strtok(buf, " ");
    char* msg = strtok(NULL, "\n");

    //if it recevied nothing as an input
    if (msg == NULL) {
        msg = NULL; //if length of message is 0 
    }

    printf("%s : %s\n", login_name, msg);
    return msg;
}

/*
This function is for command "logout".
It processes the server's response code. The code 4 means "logout requested"
*/
int logout(char* buf, char** login_name, bool* login_status) {

    //set the login status false
    printf("%s logout.\n", *login_name);
    *login_status = false;

    //deallocate the login_name space
    if (login_name != NULL) {
        free(*login_name);
        *login_name = NULL;
    }

    return 4;

}

void main() {

    // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("Error at WSAStartup()\n");
        return;
    }

    // Create a socket.
    SOCKET listenSocket;
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Bind the socket.
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; //use local address
    addr.sin_port = htons(SERVER_PORT);
    if (bind(listenSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("bind() failed.\n");
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Listen on the Socket.
    if (listen(listenSocket, MAX_PENDING) == SOCKET_ERROR) {
        printf("Error listening on socket.\n");
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    //server should not end.
    while (true) {

        // Accept connections.
        SOCKET s;

        printf("Waiting for a client to connect...\n");

        s = accept(listenSocket, NULL, NULL);
        if (s == SOCKET_ERROR) {
            printf("accept() error \n");
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        printf("Client Connected.\n");

        //Start comment
        printf("My chat room server. Version One.\n");

        //The varaibles storing login info.
        char* login_name = NULL;
        bool login_status = false;

        // Commands can be sent contiguously. 
        while (true) {

            //receive data from a client.
            char buf[MAX_LINE];
            int len = recv(s, buf, MAX_LINE, 0);
            buf[len] = 0; //make \n (By fgets) to \0

            //copy buffer data to use for strtok
            char temp_buf[MAX_LINE];
            strcpy_s(temp_buf, buf);
            //it identifies what the command is.
            char* cmd = strtok(temp_buf, " ");

            //error code zero = no problem, 4 = logout, otherwise yes.
            int error = 0;

            //It classifies the request to four commands
            //newuser, login, send, logout
            if (strncmp(cmd, "newuser", 7) == 0) {
                error = createNewUser(buf);
            }
            else if (strncmp(cmd, "login", 5) == 0) {
                error = login(buf, &login_status, &login_name);
            }
            else if (strncmp(cmd, "send", 4) == 0) {
                char* msg = send_message(buf, login_name);
                //if the msg is empty, return "EMPTY_MSG" as a code.
                if (msg == NULL) {
                    send(s, "EMPTY_MSG", strlen("EMPTY_MSG"), 0);
                }
                //else just send the msg.
                else {
                    send(s, msg, strlen(msg), 0);
                }
                //send doesn't have to treate any error code.
                continue;
            }
            else if (strncmp(cmd, "logout", 6) == 0) {
                error = logout(buf, &login_name, &login_status);
            }

            //process the error code. It returns one char as a code.
            if (error == 0) {
                send(s, "0", strlen("0"), 0);
            }
            else if (error == 1) {
                send(s, "1", strlen("1"), 0);
            }
            else if (error == 2) {
                send(s, "2", strlen("2"), 0);
            }
            else if (error == 3) {
                send(s, "3", strlen("3"), 0);
            }
            else if (error == 4) {
                send(s, "4", strlen("4"), 0);
                //the connected user logs out.
                closesocket(s);
                //and then make new socket to accept new user.
                break;
            }
        }

    }
}


