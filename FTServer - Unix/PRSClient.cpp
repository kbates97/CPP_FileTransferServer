//
//  PRSClient.cpp
//  PRSLib
//
//  Created by Kyle Bates on 10/24/20.
//

#include "PRSClient.hpp"
#include <chrono>
#include <thread>

using namespace PRSLib;
using std::cout;
using std::endl;

PRSClient::PRSClient(std::string prsAddress, ushort prsPort, std::string serviceName) {
    this->serviceName = serviceName;
    portNumber = 0;
    
    //create the socket for sending messages to the server
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    
    endPt.sin_family = AF_INET;
    endPt.sin_port = htons(prsPort);
    inet_pton(AF_INET, prsAddress.data(), &endPt.sin_addr);
    
    // no keep alive thread initially
    keepAliveThread = NULL;
    keepAliveRunning = false;
    keepAliveTimeout = 0;
}

ushort PRSClient::RequestPort() {
    // send request to server for our service name
    SendMessage(PRSMessage(PRSMessage::REQUEST_PORT, serviceName, 0, PRSMessage::SUCCESS));
    
    // receive server's response and retreive the port
    PRSMessage response = ReceiveResponse();
    if (response.Status() == PRSMessage::SUCCESS)
    {
        portNumber = response.Port();
        return portNumber;
    }
    throw "Failed to request port for service " + serviceName + ", error " + STATUS_STRING[response.Status()];
}

struct thread_info {
    long pid;
};

void PRSClient::KeepPortAlive(int timeout) {
    // Create a thread to periodically send KEEP_ALIVE to the PRS for this port
    keepAliveTimeout = timeout;
    pthread_create(&keepAliveThread, NULL, &KeepAliveThreadProc, this);
}

void * PRSClient::KeepAliveThreadProc(void *param) {
    // this method is called on the KeepAliveThread
    PRSClient *prs = (PRSClient *)param;
    prs->keepAliveRunning = true;
    while(prs->keepAliveRunning)
    {
        // send the keep alive to the PRS for this client's port
        prs->SendMessage(PRSMessage(PRSMessage::KEEP_ALIVE, prs->serviceName, prs->portNumber, PRSMessage::SUCCESS));
        PRSMessage response = prs->ReceiveResponse();
        
        //if we receive a failure to keep this port alive, assume there's a serious failure and stop
        if(response.Status() != PRSMessage::SUCCESS)
        {
            prs->keepAliveRunning = false;
            prs->keepAliveThread = NULL;
            prs->keepAliveTimeout = 0;
            cout << "Failed to keep port alive for service " << prs->serviceName << ", error " << STATUS_STRING[response.Status()] << endl;
            return NULL;
        }
        
        // sleep until half the timout passes, before sending again
        try {
            std::this_thread::sleep_for(std::chrono::seconds(prs->keepAliveTimeout)/2);
        } catch (std::exception e) {
            //nothing to do, expect to receive when ClosePort() is called
        }
    }
    return NULL;
}

void PRSClient::ClosePort() {
    //if keep alive thread is running, then stop it before closing the port
    if (keepAliveRunning){
        keepAliveRunning = false;
        if(pthread_cancel(keepAliveThread) == 0){
            keepAliveThread = NULL;
        } else {
            throw "Error cancelling thread.";
        }
        keepAliveTimeout = 0;
    }
    
    // send close port message to server for our service name and port number
    SendMessage(PRSMessage(PRSMessage::CLOSE_PORT, serviceName, portNumber, PRSMessage::SUCCESS));
    
    //expect success
    PRSMessage response = ReceiveResponse();
    if (response.Status() != PRSMessage::SUCCESS){
        throw "Failed to close port " + std::to_string(portNumber) + " for service " + serviceName;
    }
}

ushort PRSClient::LookupPort() {
    //send lookup to server for our service name
    SendMessage(PRSMessage(PRSMessage::LOOKUP_PORT, serviceName, 0, PRSMessage::SUCCESS));
    
    // receive server's response and retrieve the port
    PRSMessage response = ReceiveResponse();
    if (response.Status() == PRSMessage::SUCCESS){
        return response.Port();
    } else {
        throw "Failed to lookup port for service " + serviceName + STATUS_STRING[response.Status()];
    }
}



void PRSClient::SendMessage(PRSLib::PRSMessage msg) { 
    msg.SendMessage(clientSocket, (sockaddr *) &endPt);
}

PRSLib::PRSMessage PRSClient::ReceiveResponse() { 
    // NOTE: ignoring server's endpoint because we know who we sent it to
    sockaddr_in remoteEP;
    remoteEP.sin_addr.s_addr = INADDR_ANY;
    remoteEP.sin_family = AF_INET;
    remoteEP.sin_port = 0;
    
    return PRSMessage::ReceiveMessage(clientSocket, (sockaddr *) &remoteEP);
}


