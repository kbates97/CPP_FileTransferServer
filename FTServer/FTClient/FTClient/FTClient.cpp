//
//  FTClient.cpp
//  FTClient
//
//  Created by Kyle Bates on 10/24/20.
//

#include "FTClient.hpp"
#include <sys/stat.h>
#include <fstream>

using namespace FTClientNS;
using std::cout;
using std::endl;

FTClient::FTClient(std::string ftServerAddress, ushort ftServerPort) {
    //save server address/port
    this->ftServerAddress = ftServerAddress;
    this->ftServerPort = ftServerPort;
    
    //initialize to not connected to server
    this->connected = false;
    
    this->clientSocket = NULL;
}

void FTClient::Connect() {
    if (!connected){
        //create a client socket and connect to the FT Server's IP address and port
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0){
            throw std::runtime_error("FTCient::Connect(): Error creating client socket");
        }
        
        sockaddr_in remoteEP;
        remoteEP.sin_addr.s_addr = inet_addr(ftServerAddress.data());
        remoteEP.sin_port = htons(ftServerPort);
        remoteEP.sin_family = AF_INET;
        
        if(connect(clientSocket, (sockaddr *) &remoteEP, sizeof(remoteEP)) < 0){
            throw std::runtime_error("FTClient::Connect(): Error connecting to FT Server");
        }
        
        // now connected
        this->connected = true;
        
        cout << "FT Client Connected!" << endl;
    }
}

void FTClient::Disconnect() {
    if (connected){
        //send exit to FT Server
        SendExit();
        
        //disconnect and close socket
        shutdown(clientSocket, 2);
        close(clientSocket);
        
        //now disconnected
        this->connected = false;
        
        clientSocket = NULL;
        
        cout << "FT Client disconnected." << endl;
    }
}

void FTClient::GetDirectory(std::string directoryName) {
    //send get to the server for the specified directory and receive files
    if (connected){
        //send get command for the directory
        SendGet(directoryName);
        
        // Receive stream
        bzero(&buffer, sizeof(buffer));
        read(clientSocket, buffer, BUFSIZ);
        bufPtr = buffer;
        
        //receive and process files
        while(ReceiveFile(directoryName)){
            cout << "Found a file" << endl;
        }
    }
}

/*   -------Implementation------   */


void FTClient::SendGet(std::string directoryName) {
    //send get message for the directory
    string message = "get\n" + directoryName + "\n";
    write(clientSocket, message.data(), message.length());
    cout << "Sent Get" << endl;
}

void FTClient::SendExit() {
    //send exit message
    string message = "exit\n";
    write(clientSocket, message.data(), message.length());
}

void FTClient::SendInvalidMessage() {
    //allows for testing of server's error handling code
    write(clientSocket, "Invalid", 7);
    
}

string FTClient::ReadLine(){
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

bool FTClient::ReceiveFile(std::string directoryName) {
    //receive a single file from the server and save it locally in the speciefied directory
    
    // get response stream from TCP socket
    
    //expect file name from server
    string fileName = ReadLine();
    
    //when the server sends "done", then there are no more files!
    if (fileName == "done"){
        cout << "No more files found" << endl;
        return false;
    }
    
    //handle error messages from the server
    if (fileName == "error") {
        string errorMessage = ReadLine();
        cout << "Error: " << errorMessage << endl;
        return false;
    }
    
    // received a file name
    //receive file length from server
    string fileLength = ReadLine();
    int charsToRead = std::stoi(fileLength);
    
    //receive file contents
    string contents = "";
    
    // Careful to only grab file contents
    char * end;
    if (strlen(bufPtr) > charsToRead)
        end = bufPtr + charsToRead;
    else
        end = &buffer[BUFSIZ];
    char c = *end;
    *end = '\0';
    contents += bufPtr;
    charsToRead -= strlen(bufPtr);
    *end = c;
    
    // move buffer pointer to the next command
    bufPtr = end;
    
    //loop until all of the file contents are received
    while (charsToRead > 0)
    {
        // receive as many characters from the server as available
        bzero(&buffer, sizeof(buffer));
        if (charsToRead < BUFSIZ)
            charsToRead -= read(clientSocket, buffer, charsToRead);
        else
            charsToRead -= read(clientSocket, buffer, BUFSIZ);
        // accumulate bytes read into the contents
        contents += buffer;
    }
    
    // create the local directory if needed
    struct stat s;
    if (stat(directoryName.data(), &s) == -1) {
        mkdir(directoryName.data(), 0700);
    }
    
    // save the file locally on the disk
    std::ofstream os;
    os.open(directoryName + "/" + fileName, std::ios::trunc);
    os << contents;
    os.close();
    
    return true;
}
