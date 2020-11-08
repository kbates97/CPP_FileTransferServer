//
//  PRSClient.hpp
//  PRSLib
//
//  Created by Kyle Bates on 10/24/20.
//

#ifndef PRSClient_hpp
#define PRSClient_hpp

#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "PRSMessage.hpp"

using std::string;

namespace PRSLib {
class PRSClient
{
private:
    int clientSocket;
    sockaddr_in endPt;
    string serviceName;
    ushort portNumber;
    pthread_t keepAliveThread;
    bool keepAliveRunning;
    int keepAliveTimeout;
    
public:
    PRSClient(string prsAddress, ushort prsPort, string serviceName);
    ushort RequestPort();
    void KeepPortAlive(int timeout = 300);
    static void* KeepAliveThreadProc(void* param);
    void ClosePort();
    ushort LookupPort();
    
private:
    void SendMessage(PRSMessage msg);
    PRSMessage ReceiveResponse();
};
}

#endif /* PRSClient_hpp */
