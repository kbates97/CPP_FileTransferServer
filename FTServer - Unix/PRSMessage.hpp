//
//  PRSMessage.hpp
//  PRSLib
//
//  Created by Kyle Bates on 10/16/20.
//

#ifndef PRSMessage_hpp
#define PRSMessage_hpp

#include <stdio.h>

#include <string>
#include <sys/socket.h>
#include <iostream>

static const std::string MESSAGE_TYPE_STRING[] = { "REQUEST_PORT", "LOOKUP_PORT", "KEEP_ALIVE", "CLOSE_PORT", "STOP", "RESPONSE"};
static const std::string STATUS_STRING[] = { "SUCCESS", "SERVICE_IN_USE", "SERVICE_NOT_FOUND", "ALL_PORTS_BUSY", "INVALID_ARG", "UNDEFINED_ERROR"};

namespace PRSLib {
class PRSMessage {
public:
    enum MESSAGE_TYPE
    {
        REQUEST_PORT = 0,
        LOOKUP_PORT = 1,
        KEEP_ALIVE = 2,
        CLOSE_PORT = 3,
        STOP = 4,
        RESPONSE = 5
    };
    
    enum STATUS
    {
        SUCCESS = 0,
        SERVICE_IN_USE = 1,
        SERVICE_NOT_FOUND = 2,
        ALL_PORTS_BUSY = 3,
        INVALID_ARG = 4,
        UNDEFINED_ERROR = 5
    };
    
private:
    static const int MSG_SIZE = 54;
    
    MESSAGE_TYPE msg_type;
    std::string service_name;
    ushort port;
    STATUS status;
    
public:
    PRSMessage();
    PRSMessage (MESSAGE_TYPE msg_type, std::string service_name, ushort port, STATUS status);
    MESSAGE_TYPE MsgType();
    std::string ServiceName();
    ushort Port ();
    STATUS Status();
    std::string ToString();
    void SendMessage(int sockFD, sockaddr* toAddress);
    static inline PRSMessage ReceiveMessage(int sockFD, sockaddr* fromAddress)
    {
        unsigned char buffer[MSG_SIZE];
        bzero(buffer, MSG_SIZE);
        socklen_t length = sizeof(*fromAddress);
        int result = (int) recvfrom(sockFD, buffer, MSG_SIZE, 0, fromAddress, &length);
        PRSMessage msg = PRSMessage(buffer);
        std::cout << "Received " << result << " bytes: " + msg.ToString() << std::endl;
        return msg;
    }
    
private:
    PRSMessage (unsigned char bytes[]);
    unsigned char* GetBytes(unsigned char* result);
};
}

#endif /* PRSMessage_hpp */
