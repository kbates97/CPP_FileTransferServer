//
//  FTClient.hpp
//  FTClient
//
//  Created by Kyle Bates on 10/24/20.
//

#ifndef FTClient_hpp
#define FTClient_hpp

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using std::string;

namespace FTClientNS {
class FTClient{
private:
    string ftServerAddress;
    ushort ftServerPort;
    bool connected;
    int clientSocket;
    char buffer[BUFSIZ+1];
    char *bufPtr;
    
public:
    FTClient(string ftServerAddress, ushort ftServerPort);
    void Connect();
    void Disconnect();
    void GetDirectory(string directoryName);
    
private:
    void SendGet(string directoryName);
    void SendExit();
    void SendInvalidMessage();
    bool ReceiveFile(string directoryName);
    string ReadLine();
};
}

#endif /* FTClient_hpp */
