//
//  FTServer.cpp
//  FTServer
//
//  Created by Kyle Bates on 10/22/20.
//

#include "FTServe.hpp"
#include "FtConnectedClient.hpp"

using namespace FTServe;
using std::cout;
using std::endl;

using std::chrono::seconds;
using std::this_thread::sleep_for;

void FTServer::Start() {
    // create a listening socket for clients to connect
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    // bind to the FT Server port
    sockaddr_in hostEP;
    bzero(&hostEP, sizeof(hostEP));
    hostEP.sin_addr.s_addr = INADDR_ANY;
    hostEP.sin_port = htons(listeningPort);
    hostEP.sin_family = AF_INET;
    bind(listeningSocket, (sockaddr *) &hostEP, sizeof(hostEP));
    
    // set the socket to listen
    listen(listeningSocket, clientBacklog);
    
    bool done = false;
    while (!done)
    {
        try {
            sockaddr_in clientEP;
            socklen_t clientEPLength = sizeof(clientEP);
            bzero(&clientEP, clientEPLength);
            
            // accept a client connection
            int clientSocket = accept(listeningSocket, (sockaddr *) &clientEP, &clientEPLength);
            
            // instantiate connected client to process messages
            FTConnectedClient client = FTConnectedClient(clientSocket);
            client.Start();
            
        } catch (std::exception& e) {
            cout << "Error while accepting and starting client: " << e.what() << endl;
            cout << "Waiting for 5 seconds and trying again..." << endl;
            sleep_for(seconds(5));
        }
    }
    // close socket and quit
    shutdown(listeningSocket, 2);
}
