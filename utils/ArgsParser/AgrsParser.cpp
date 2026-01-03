#include <getopt.h>
#include "services/ArgsParser.h"
#include "utils/Utils.h"

ArgsParser::ArgsParser(AppContext& context, int argc, char* argv[])
    : context_(context), uiType_(UIType::CLI)
{
    parseArgs(argc, argv);
}

void ArgsParser::parseArgs(int argc, char* argv[])
{
    std::string uiTypeStr = "CLI"; // Default UI type
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
                exit(0);
            case 'u':
                uiTypeStr = optarg;
                break;
            default:
                LOG_ERROR("Try %s --help for more information", argv[0]);
                exit(1);
        }
    }

    // Convert string to UIType enum
    if(uiTypeStr == "CLI"){
        uiType_ = UIType::CLI;
    } else if(uiTypeStr == "WEB"){
        uiType_ = UIType::WEB;
    } else if(uiTypeStr == "QT" || uiTypeStr == "Qt" || uiTypeStr == "qt"){
        uiType_ = UIType::QT;
    } else {
        LOG_WARN("Unknown UI type '%s', defaulting to CLI", uiTypeStr.c_str());
        uiType_ = UIType::CLI;
    }
}

void ArgsParser::printUsage(const char *argv){
    std::cout << "Use: " << argv << std::endl;
    std::cout << " [-h] [--help]: Print usage" << std::endl;
    std::cout << " [-u] [--ui] <view type>: View type" << std::endl;
    std::cout << "                   CLI: Commandline View (default)." << std::endl;
    std::cout << "                   WEB: WEB View." << std::endl;
    std::cout << "                   QT:  Qt GUI View." << std::endl;
}

ArgsParser::~ArgsParser()
{
}

UIType ArgsParser::getUIType() const {
    return uiType_;
}
