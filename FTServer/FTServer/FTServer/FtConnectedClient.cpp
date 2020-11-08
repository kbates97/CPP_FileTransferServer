//
//  FtConnectedClient.cpp
//  FTServer
//
//  Created by Kyle Bates on 10/22/20.
//

#include "FtConnectedClient.hpp"

using namespace FTServe;
using std::cout;
using std::endl;

FTConnectedClient::FTConnectedClient(int clientSocket) {
    //save the client's socket
    this->clientSocket = clientSocket;
    
    //at this time, there is no stream, reader, write, or thread
    clientThread = NULL;
}

void FTConnectedClient::Start() {
    cout << "FTConnectedClient.Start()" << endl;
    //called by main thread to start the clientThread and process messages for the client
    //create and start the clientThread, pass in a reference to this class instance as a parameter
    pthread_create(&clientThread, NULL, &ThreadProc, this);
}



void* FTConnectedClient::ThreadProc(void* param) {
    //the procedure for the clientThread
    //when this method returns, the clientThread will exit
    
    //the param is a FTConnectedClient instance
    //start processing messages with the Run() method
    FTConnectedClient *client = (FTConnectedClient *) param;
    client->Run();
    return NULL;
}

string FTConnectedClient::ReadLine(){
    string message = "";
    char * remainder = strchr(bufPtr, '\n');
    while (remainder == nullptr) {
        message += bufPtr;
        bzero(this->buffer, sizeof(this->buffer));
        read(clientSocket, this->buffer, BUFSIZ);
        bufPtr = this->buffer;
        remainder = strchr(bufPtr, '\n');
    }
    *remainder = '\0';
    message += bufPtr;
    bufPtr = remainder + 1;
    return message;
}

void FTConnectedClient::Run() {
    //this method is executed on the clientThread
    
    try {
        //process client requests
        bool done = false;
        while (!done) {
            // get network stream over the socket
            bzero(&buffer, sizeof(buffer));
            read(clientSocket, buffer, 1024);
            bufPtr = buffer;
            
            // receive a message from the client
            string msg = ReadLine();
            
            //handle the message
            if(msg == "get")
            {
                //get directoryName
                string dirName = ReadLine();
                
                // retrieve directory contents and send all the files
                DIR *dir = opendir(dirName.data());
                
                //if directory does not exist! send an error!
                if (dir == nullptr){
                    SendError("Directory does not exist: " + dirName);
                } else {
                    //directory exists, send each file to the client
                    //for each file...
                    dirent* entry = nullptr;
                    while (dir) {
                        entry = readdir(dir);
                        
                        if (entry){
                            if (entry->d_type == DT_REG) {
                                //get the file's name
                                string fileName = string(entry->d_name);
                                
                                //make sure it's a txt file
                                size_t pos = fileName.find_last_of(".");
                                if((string(entry->d_name)).substr(pos) == ".txt"){
                                    cout << "Found a file: " << fileName << endl;
                                    //get the file contents
                                    string path = dirName + "/" + fileName;
                                    std::ifstream file(path);
                                    std::ostringstream ostream;
                                    ostream << file.rdbuf();
                                    string contents = ostream.str();
                                    
                                    // send file name and length
                                    SendFileName(fileName, (int)contents.length());
                                    
                                    //send a file to the client
                                    SendFileContents(contents);
                                }
                            }
                        } else {
                            // no more, close dir
                            closedir(dir);
                            dir = nullptr;
                        }
                    }
                    //send done after  last file
                    SendDone();
                }
            }else if (msg == "exit") {
                // client is done, disconnect it's socket and quit the thread
                shutdown(clientSocket, 2);
                done = true;
                cout << "FTConnectedClient()::Run(): Exit" << endl;
            } else {
                //error handling for an invalid message
                
                //this client is too broken to waste our time on!
                //quit processing messages and disconnect
                SendError("Unrecognized message: " + msg);
                shutdown(clientSocket, 2);
                done = true;
            }
        }
    } catch (std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    //close the clients socket
    close(clientSocket);
}

void FTConnectedClient::SendFileName(std::string fileName, int fileLength) {
    //send file name and file length message
    fileName += "\n" + std::to_string(fileLength) + "\n";
    write(clientSocket, fileName.data(), fileName.length());
}

void FTConnectedClient::SendFileContents(std::string fileContents) {
    //send file contents only
    //NOTE: no \n at end of contents
    write(clientSocket, fileContents.data(), fileContents.size());
}

void FTConnectedClient::SendDone() {
    //send done message
    string message = "done\n";
    write(clientSocket, message.data(), message.length());
    cout << "Sent Done" << endl;
}

void FTConnectedClient::SendError(std::string errorMessage) {
    //send error message
    string message = "error\n" + errorMessage + "\n";
    write(clientSocket, message.data(), message.length());
}


