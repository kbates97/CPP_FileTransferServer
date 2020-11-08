//
//  FTServer.hpp
//  FTServer
//
//  Created by Kyle Bates on 10/22/20.
//

#ifndef FTServer_hpp
#define FTServer_hpp

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <chrono>
#include <thread>

namespace FTServe {
class FTServer {
private:
    ushort listeningPort;
    int clientBacklog;
    
public:
    FTServer(ushort listeningPort, int clientBacklog){
        this->listeningPort = listeningPort;
        this->clientBacklog = clientBacklog;
    }
    void Start();
};
}

#endif /* FTServer_hpp */
