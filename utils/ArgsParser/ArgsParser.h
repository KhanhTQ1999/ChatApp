#pragma once

#include "common/typedef.h"
#include "ui/UI.h"
#include <memory>

class IView;

class ArgsParser
{
public:
    ArgsParser(AppContext& context, int argc, char* argv[]);
    ~ArgsParser();

    void exec();
    void printUsage(const char *argv);
    void parseArgs(int argc, char* argv[]);
    UIType getUIType() const;
    
private:
    AppContext& context_;
    UIType uiType_;
};
