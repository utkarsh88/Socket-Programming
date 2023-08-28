#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

const int MAX_LEN=1024; // Maximum length of a message allowed in bytes.
const int NUM_PORTS=65536; //Total number of ports 

/**
 * @brief A function to handle TCP and UDP connection on the server side
 * 
 * @param sock_client  socket to communicate with the client
 * @param addr_of_client Address of the client
 */
void connection_handling(int sock_client, struct sockaddr_in addr_of_client){
    int type_of_msg, len_of_msg;
    char msg[MAX_LEN];

    recv(sock_client, &type_of_msg, sizeof(type_of_msg), MSG_WAITALL);
    recv(sock_client, &len_of_msg, sizeof(len_of_msg), MSG_WAITALL);

    //handling bufferoverflow
    if(len_of_msg>=MAX_LEN)
    {
        cerr<<"Out of bound"<<endl;
        return;

    }
    recv(sock_client, msg, len_of_msg, 0);
    msg[len_of_msg] = '\0';

    if (type_of_msg == 1) {
        cout << "Request received from- " << inet_ntoa(addr_of_client.sin_addr) << ":" << ntohs(addr_of_client.sin_port) << endl;
        cout << "Received message: " << msg << endl;

        type_of_msg = 2;
        int uPort ;

        //Generate a random port number
        srand(time(NULL));
        uPort=rand() % NUM_PORTS;
        send(sock_client, &type_of_msg, sizeof(type_of_msg), MSG_CONFIRM );
        send(sock_client, &uPort, sizeof(uPort), MSG_CONFIRM);

        //Close TCP connection
        close(sock_client);

        int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_sock < 0) {
            cerr<<"Can't create UDP socket"<<endl;
            return;
        }

        struct sockaddr_in udp_addr;
        udp_addr.sin_family = AF_INET;
        udp_addr.sin_addr.s_addr = INADDR_ANY;
        udp_addr.sin_port = htons(uPort);
        if (bind(udp_sock, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) {
            cerr<<"Can't bind UDP socket"<<endl;
            return;
        }

        socklen_t len;
        int  num_of_bytes;
        char buffer[MAX_LEN];
     string hello = "Happy to serve";


    len = sizeof(udp_addr); //len is value/result

    //Receive UDP message
    recvfrom(udp_sock,&type_of_msg, sizeof(type_of_msg), MSG_WAITALL, (struct sockaddr *) 
            &udp_addr, &len);
    num_of_bytes = recvfrom(udp_sock, (char *)buffer, MAX_LEN,
                MSG_WAITALL, ( struct sockaddr *) &udp_addr,
                &len);
    buffer[num_of_bytes] = '\0';
    cout<<"Client[type-"<<type_of_msg<<"]: "<<buffer<<endl;

    //Send UDP response
    type_of_msg=4;
    sendto(udp_sock,&type_of_msg,sizeof(type_of_msg), MSG_CONFIRM, (const struct sockaddr *) 
            &udp_addr,len);
    sendto(udp_sock,hello.c_str(), hello.length(),
        MSG_CONFIRM, (const struct sockaddr *) &udp_addr,
            len);
    cout<<"Served!"<<endl;



        close(udp_sock);
    }
}

/**
 * @brief  Main function
 * 
 * @param argc Number of command line arguments including the name of program
 * @param argv An array of command line parameters
 * @return int 
 */
int main(int argc, char* argv[])
{
    if (argc != 2) {
        cout << "Usage: server <port>" << endl;
        exit(1);
    }
    cout<<"Server has started..."<<endl;

    int server_sock, sock_client;
    struct sockaddr_in server_addr, addr_of_client;
    int port = atoi(argv[1]); //byte to integer conversion

    // Create TCP socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        cerr<<"Error creating socket"<<endl;
        exit(1);
    }

    // Bind socket to IP address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr<<"Error binding socket"<<endl;
        exit(1);
    }

    // Listen for incoming connections
    listen(server_sock, 5);

    while (true) {
        // Accept incoming connection
        socklen_t client_len = sizeof(addr_of_client);
        sock_client = accept(server_sock, (struct sockaddr*)&addr_of_client, &client_len);
        if (sock_client < 0) {
            cerr<<"Error accepting connection"<<endl;
            continue;
        }
        pid_t pid=fork();
        if (pid == 0)
        {
            //In child process
            cout<<"Forking..."<<endl;
            connection_handling(sock_client, addr_of_client);
            return 0;
        }
        else 
            //parent process
            close(sock_client);

    }

    return 0;
}


