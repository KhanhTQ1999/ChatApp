#include <iostream>
#include <string>
#include <getopt.h>
#include <memory>

#include "utils/Utils.h"
#include "model/ChatModel.h"
#include "controller/Controller.h"
#include "views/ChatView.h"

void printUsage(const char *argv){
    std::cout << "Use: " << argv << std::endl;
    std::cout << " [-h] [--help]: Print usage" << std::endl;
    std::cout << " [-u] [--ui] <view type>: View type" << std::endl;
    std::cout << "                   CLI: Commandline View." << std::endl;
    std::cout << "                   WEB: WEB View." << std::endl;
}

int main(int argc, char* argv[]) {
    // Setup logger
    // LOG_SET_LEVEL(LogLevel::INFO);
    // LOG_SET_QUIET(false);

    // Parse command line arguments
    std::string uiType = "CLI"; // Default UI type
    int opt;

    static struct option longOpt[] = {
        {"help", no_argument, 0, 'h'},
        {"ui", required_argument, 0, 'u'},
        {0, 0, 0, 0}
    };
    
    int longIdx = 0;
    while ((opt = getopt_long(argc, argv, "hu:", longOpt, &longIdx)) != -1) {
        switch (opt) {
            case 'h':
                printUsage(argv[0]);
                return 0;
            case 'u':
                uiType = optarg;
                break;
            default:
                LOG_ERROR("Try %s --help for more information", argv[0]);
                return 1;
        }
    }

    //Initialize components
    ChatModel chatModel;
    ChatView chatView;
    Controller controller(chatModel, chatView);

    //Start application
    chatView.show();
    return 0;
}