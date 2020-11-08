//
//  FtConnectedClient.hpp
//  FTServer
//
//  Created by Kyle Bates on 10/22/20.
//

#ifndef FtConnectedClient_hpp
#define FtConnectedClient_hpp

#include <iostream>
#include <sstream>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>

using std::string;
using std::istringstream;

namespace FTServe {
class FTConnectedClient
{
private:
    int clientSocket;
    char buffer[BUFSIZ+1];
    char *bufPtr;
    pthread_t clientThread;
    
public:
    FTConnectedClient(int clientSocket);
    void Start();
    
private:
    static void* ThreadProc(void* param);
    void Run();
    void SendFileName(string fileName, int fileLength);
    void SendFileContents(string fileContents);
    void SendDone();
    void SendError(string errorMessage);
    string ReadLine();
};
}

#endif /* FtConnectedClient_hpp */
