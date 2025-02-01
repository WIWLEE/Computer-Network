/*
The client side Chat source code.
Student Name : Jihyun Lee
Student Number : 14478965
Start Date : 9/29/2024
End date : 10/14/2024
- This source code implements the client side chat program including 4 functions : newuser, login, send, logout
*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include "winsock2.h"

#define SERVER_PORT   18965 //1 + last four student number
#define MAX_LINE      256

/*
This function is for command "newuser".
It processes the received code from a server and print an appropriate messages.
The code means :
- 0 : New user acount created well.
- 1 : Something goes wrong.
- 2 : User account already exists.
- 3 : length of id or password wrong.
*/
void createNewUser(SOCKET s) {

    //receive server's response code
    char code[2];
    int len = recv(s, code, MAX_LINE, 0);
    code[1] = '\0';

    //process the received code.
    if (strcmp(code, "0") == 0) {
        printf("New user account created. Please login.\n");
    }
    else if (strcmp(code, "1") == 0) {
        printf("Something goes wrong.\n");
    }
    else if (strcmp(code, "3") == 0) {
        printf("length of id or password wrong.\n");
    }
    else {
        printf("Denied. User account already exists.\n");
    }
}

/*
This function is for command "login".
It processes the received code from a server and print an appropriate messages.
The code means :
- 0 : Login successfully.
- 1 : Can't find the user. User name or password incorrect.
- else : Something goes wrong.
*/
void login(SOCKET s, bool* login_status, char** login_name, char* NAME) {
    //receive server's response code
    char code[2];
    int len = recv(s, code, MAX_LINE, 0);
    code[1] = '\0';

    //process the received code
    if (strcmp(code, "0") == 0) {
        *login_status = true;
        *login_name = _strdup(NAME); //allocate space not a point
        printf("login confirmed\n");
    }
    else if (strcmp(code, "1") == 0)
    {
        printf("Denied. User name or password incorrect\n");
    }
    else {
        printf("Somthing goes wrong.\n");
    }
}

/*
This function is for command "send".
It processes the received code from a server and print an appropriate messages.
The code means :
- "EMPTY_MSG : msg with no content arrived.
- else : any msg arrived.
*/
void send_message(SOCKET s, char login_name[]) {

    //receive server's response msg
    char code[MAX_LINE] = { 0 };
    int len = recv(s, code, MAX_LINE, 0);
    code[len] = '\0';

    //process the messages
    if (strncmp(code, "EMPTY_MSG", 9) == 0) {
        printf("%s :\n", login_name);
    }
    else {
        printf("%s : %s\n", login_name, code);
    }
}

/*
This function is for command "logout".
It processes the received code from a server and print an appropriate messages.
The code means :
- 0 : log out accepted.
- else : somethign goes wrong.
*/
void logout(SOCKET s, char** login_name, bool* login_status) {

    //receive server's response code.
    char code[MAX_LINE];
    int len = recv(s, code, MAX_LINE - 1, 0);
    code[len] = '\0';

    //process the code
    if (strcmp(code, "4") == 0) {
        printf("%s left.\n", *login_name);
        if (*login_name != NULL) {
            free(*login_name); //deallocation the login_name space
            *login_name = NULL;
        }
        *login_status = false;
    }
    else {
        printf("something goes wrong.");
    }

}

void main(int argc, char** argv) {

    if (argc < 2) {
        printf("\nUseage: client serverName\n");
        return;
    }

    // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("Error at WSAStartup()\n");
        return;
    }

    //translate the server name or IP address (128.90.54.1) to resolved IP address
    unsigned int ipaddr;
    // If the user input is an alpha name for the host, use gethostbyname()
    // If not, get host by addr (assume IPv4)
    if (isalpha(argv[1][0])) {   // host address is a name  
        hostent* remoteHost = gethostbyname(argv[1]);
        if (remoteHost == NULL) {
            printf("Host not found\n");
            WSACleanup();
            return;
        }
        ipaddr = *((unsigned long*)remoteHost->h_addr);
    }
    else //"128.90.54.1"
        ipaddr = inet_addr(argv[1]);


    // Create a socket.
    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Connect to a server.
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ipaddr;
    addr.sin_port = htons(SERVER_PORT);
    if (connect(s, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("Failed to connect.\n");
        WSACleanup();
        return;
    }

    //Start comment
    printf("My chat room client. Version One.\n");

    //The varaibles storing login info
    bool login_status = false; //0 = log out, 1= log in
    char* login_name = NULL;

    // Commands can be sent contiguously.
    while (true) {

        //receive data from a client.
        char buf[MAX_LINE];
        fgets(buf, sizeof(buf), stdin);

        //copy buffer data to use for strtok
        char stored_buf[MAX_LINE];
        strcpy(stored_buf, buf);

        // belongs to command
        char* str = strtok(buf, " ");
        char* next_str = strtok(NULL, " ");
        char* next_next_str = strtok(NULL, "\n");

        //It classifies the request to four commands
        //newuser, login, send, logout
        if (strncmp(str, "newuser", 7) == 0) {

            //if login status is true, newuser command is not allowed.
            if (login_status == true) {
                printf("you cannot create new user while you are logged in.\n");
            }
            //if the length of string is wrong, the command is not allowed.
            else if (strlen(next_str) < 3 || strlen(next_str) > 32 || strlen(next_next_str) < 4 || strlen(next_next_str) > 8) {
                printf("length of id or password wrong\n");
            }
            //else, invoke createNewUser
            else {
                send(s, stored_buf, strlen(stored_buf), 0);
                createNewUser(s);
            }

        }
        else if (strncmp(str, "login", 5) == 0) {
            //if login status is true, login command is not allowed.
            if (login_status == true) {
                printf("you cannot login another user while you are logged in.\n");
            }
            //else, invoke login
            else {
                send(s, stored_buf, strlen(stored_buf), 0);
                login(s, &login_status, &login_name, next_str);
            }
        }
        else if (strncmp(str, "send", 4) == 0) {
            //if login status is false, send command is not allowed.
            if (login_status == false) {
                printf("Denied. Please login first.\n");
            }
            //if msg is empty, it is treated as the other route.
            else if (next_str == NULL) {
                send(s, "send\n", strlen("send\n"), 0);
                send_message(s, login_name);
            }
            //if the length of string is wrong, send command is not allowed.
            else if (strlen(next_str) < 1 || strlen(next_str) > 256) {
                printf("Message size should be between 1 and 256.\n");
            }
            //else, invoke send message
            else {
                send(s, stored_buf, strlen(stored_buf), 0);
                send_message(s, login_name);
            }
        }
        else if (strncmp(str, "logout", 6) == 0) {
            //if login status is false, the logout command is not allowed.
            if (login_status == false) {
                printf("Denied. Please login first.\n");
            }
            //else, invoke logout and close the socket.
            else {
                send(s, stored_buf, strlen(stored_buf), 0);
                logout(s, &login_name, &login_status);
                closesocket(s);
                break;
            }
        }
        //The other command doesn't exist.
        else {
            printf("wrong command\n");
        }
    }

}
