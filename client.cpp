#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int  MAX_LEN=1024; // Maximum length of a message allowed in bytes.
using namespace std;

/**
 * @brief  Main function
 * 
 * @param argc Number of command line arguments
 * @param argv An array of command line parameters
 * @return int 
 */
int main(int argc, char* argv[])
{

    if (argc !=3) {
        cout << "Usage: client <server_ip> <server_port> " << endl;
        exit(1);
    }

    string server_ip = argv[1];
    int server_port = atoi(argv[2]);
    
    // Create TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr<<"Can't create TCP socket"<<endl;
        exit(1);
    }

    // Connect to server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_addr.sin_port = htons(server_port);
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr<<"Can't bind TCP socket"<<endl;
        exit(1);
    }

    // Send request (TCP)
    int message_type = 1;
    string req_message="Send UDP port";
    int message_length = req_message.length();
    send(sock, &message_type, sizeof(message_type), 0);
    send(sock, &message_length, sizeof(message_length), 0);
    send(sock, req_message.c_str(), message_length, 0);

    // Receive response (TCP)
    recv(sock, &message_type, sizeof(message_type), 0);
    int udp_port;
    recv(sock, &udp_port, sizeof(udp_port), 0);
    cout<<"Received udp port number= "<<udp_port<<endl;

    // Close TCP connection
    close(sock);

   int sockfd;
   struct sockaddr_in   servaddr;
 
 
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        cerr<<"Can't create socket"<<endl;
        exit(1);
    }   
    

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(udp_port);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    
    int num_of_bytes;
    socklen_t len;

    //Send UDP message
    char buffer[MAX_LEN];
    string hello;
    cout<<"Enter a message: ";
    getline(cin,hello);

    message_type=3;
    
    sendto(sockfd, &message_type,sizeof(message_type),MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));

    sendto(sockfd, hello.c_str(), hello.length(),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));


    //Receive UDP response
    recvfrom(sockfd,&message_type,sizeof(message_type),MSG_WAITALL, (struct sockaddr *)
            &servaddr, &len);

    num_of_bytes = recvfrom(sockfd, (char *)buffer, MAX_LEN,
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    buffer[num_of_bytes] = '\0';
    cout<<"Server[type-"<<message_type<<" ]: "<<buffer<<endl;



    // Close UDP connection
    close(sockfd);

    return 0;
}
    
