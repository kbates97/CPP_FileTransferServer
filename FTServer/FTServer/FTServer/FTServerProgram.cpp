//
//  main.cpp
//  FTServer
//
//  Created by Kyle Bates on 10/22/20.
//

#include <iostream>
#include "PRSClient.hpp"
#include "FTServe.hpp"

using std::string;
using std::cout;
using std::endl;
using namespace PRSLib;
using namespace FTServe;

namespace FTServe {
class FTServerProgram{
public:
    static void Usage(){
        std::cout << "Usage: FTServer -prs <PRS IP address>:<PRS port>" << std::endl;
    }
};
}

int main(int argc, const char * argv[]) {//defaults
    ushort FTSERVER_PORT = 40000;
    int CLIENT_BACKLOG = 5;
    string PRS_ADDRESS = "127.0.0.1";
    ushort PRS_PORT = 30000;
    string SERVICE_NAME = "FT Server";
    
    try {
        for (int i = 1; i < argc; i++) {
            if ((string)argv[i] == "-prs"){
                string arg = string(argv[++i]);
                size_t pos = arg.find(":");
                PRS_ADDRESS = arg.substr(0, pos);
                PRS_PORT = std::stoi(arg.substr(pos+1, arg.npos));
            } else {
                cout << "Invalid argument: " << argv[i] << endl;
                FTServerProgram::Usage();
                return -1;
            }
        }
    } catch (std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    std::cout << "PRS Address: " << PRS_ADDRESS << std::endl;
    std::cout << "PRS Port: " << PRS_PORT << std::endl;
    
    try {
        //contact PRS and request port for FT server. Keep it alive
        PRSClient prs = PRSClient(PRS_ADDRESS, PRS_PORT, SERVICE_NAME);
        FTSERVER_PORT = prs.RequestPort();
        prs.KeepPortAlive();
        
        // instantiate and start FT server
        FTServer ft = FTServer(FTSERVER_PORT, CLIENT_BACKLOG);
        ft.Start();
        
        //give PRS port back
        prs.ClosePort();
        
    } catch (std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
