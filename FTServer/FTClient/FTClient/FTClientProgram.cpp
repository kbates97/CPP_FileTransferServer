//
//  main.cpp
//  FTClient
//
//  Created by Kyle Bates on 10/24/20.
//

#include <iostream>
#include "PRSClient.hpp"
#include "FTClient.hpp"

using namespace PRSLib;
using namespace FTClientNS;
using std::cout;
using std::endl;
using std::string;

namespace FTClientNS {
class FTClientProgram
{
public:
    static void Usage(){
        cout << "Usage: FTClient -d <directory> [-prs <PRS IP>:<PRS Port>] [-s <FT Server IP>]" << endl;
    }
};
}

int main(int argc, const char * argv[]) {
    //defaults
    string PRSSERVER_IPADDRESS = "127.0.0.1";
    ushort PRSSERVER_PORT = 30000;
    string FTSERVICE_NAME = "FT Server";
    string FTSERVER_IPADDRESS = "127.0.0.1";
    ushort FTSERVER_PORT = 40000;
    string DIRECTORY_NAME = "";
    
    try {
        for (int i = 1; i < argc; i++){
            if ((string)argv[i] == "-d"){
                DIRECTORY_NAME = string(argv[++i]);
            }
            else if ((string)argv[i] == "-prs"){
                string arg = string(argv[++i]);
                size_t pos = arg.find(":");
                PRSSERVER_IPADDRESS = arg.substr(0, pos);
                PRSSERVER_PORT = std::stoi(arg.substr(pos+1, arg.npos));
            }
            else if ((string)argv[i] == "-s"){
                FTSERVER_IPADDRESS = string(argv[++i]);
            }
            else {
                cout << "Invalid argument: " << argv[i] << endl;
                FTClientProgram::Usage();
                return -1;
            }
        }
    } catch (std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    if (DIRECTORY_NAME == ""){
        cout << "Missing directory name." << endl;
        FTClientProgram::Usage();
        return -1;
    }
    
    cout << "PRS Address: " << PRSSERVER_IPADDRESS << endl;
    cout << "PRS Port: " << PRSSERVER_PORT << endl;
    cout << "FT Server Address: " << FTSERVER_IPADDRESS << endl;
    cout << "Directory: " << DIRECTORY_NAME << endl;
    
    try {
        //contact PRS and lookup port for "FT Server"
        PRSClient prs = PRSClient(PRSSERVER_IPADDRESS, PRSSERVER_PORT, FTSERVICE_NAME);
        FTSERVER_PORT = prs.LookupPort();
        
        //create an FTClient and connect it to the server
        FTClient ft = FTClient(FTSERVER_IPADDRESS, FTSERVER_PORT);
        ft.Connect();
        
        //get the contents of the specified directory
        ft.GetDirectory(DIRECTORY_NAME);
        
        //disconnect from the server
        ft.Disconnect();
        
    } catch (std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    return 0;
}
