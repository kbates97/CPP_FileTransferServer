//
//  PRSMessage.cpp
//  PRSLib
//
//  Created by Kyle Bates on 10/16/20.
//

#include "PRSMessage.hpp"

using namespace PRSLib;
    
    /*  Expected format for a message sent to the PRS server
        typedef struct
        {
            int8_t msg_type;            //  1 byte,  from 0 to 0
            char service_name[50];      // 50 bytes, from 1 to 50
            uint16_t port;              //  2 bytes, from 51 to 52
            int8_t status;              //  1 byte,  from 53 to 53
        } request_t;                    // 54 bytes total
    */

PRSMessage::PRSMessage(){}

PRSMessage::PRSMessage (MESSAGE_TYPE msg_type, std::string service_name, ushort port, STATUS status){
    this -> msg_type = msg_type;
    this -> service_name = service_name;
    this -> port = port;
    this -> status = status;
}
    
PRSMessage::MESSAGE_TYPE PRSMessage::MsgType() { return msg_type; }
std::string PRSMessage::ServiceName() { return service_name;}
ushort PRSMessage::Port () { return port;}
PRSMessage::STATUS PRSMessage::Status() { return status;}
    
std::string PRSMessage::ToString()
{
    std::string result = "";
    
    result += "{";
    result += MESSAGE_TYPE_STRING[msg_type];
    result += ", ";
    result += service_name;
    result += ", ";
    result += std::to_string(port);
    result += ", ";
    result += STATUS_STRING[status];
    result += "}";
    
    return result;
}
    
void PRSMessage::SendMessage(int sockFD, sockaddr* toAddress)
{
    unsigned char message[MSG_SIZE];
    int result = (int) sendto(sockFD, GetBytes(message), MSG_SIZE, 0, toAddress, sizeof(*toAddress));
    std::cout << "Sent " << result << " bytes: " << this -> ToString() << std::endl;
}
    
     /* -------- implementation --------- */

PRSMessage::PRSMessage (unsigned char bytes[])
{
    unsigned char serv_name[50];
    memcpy(serv_name, bytes+1, 50);
    ushort port_bytes = (bytes[51] << 8) | bytes[52];
    serv_name[49] = '\0';
    this -> msg_type = (MESSAGE_TYPE)bytes[0];
    this -> service_name = (reinterpret_cast<char const *>(serv_name));
    this -> port = ntohs(port_bytes);
    this -> status = (STATUS)bytes[53];
}
    
unsigned char* PRSMessage::GetBytes(unsigned char* result)
{
    result[0] = (unsigned char) msg_type;
    memcpy(result+1, service_name.data(), service_name.length());
    result[service_name.length()+1] = '\0';
    ushort port_number = htons(port);
    result[51] = port_number >> 8;
    result[52] = port_number & 0xFF;
    result[53] = (unsigned char)status;
    return result;
}
