#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include "common/typedef.h"

#include "ui/UI.h"
#include "ui/CLI.h"
#include "utils/ArgsParser.h"
#include "utils/Utils.h"
#include "model/ChatModel.h"
#include "controller/Controller.h"
#include "views/ChatView.h"
#include "services/NetworkService.h"

int main(int argc, char* argv[]) {
    AppContext context;
    ArgsParser argsParsr(context, argc, argv);
    
    //Determine UI type
    UIType uiType = argsParsr.getUIType();
    std::shared_ptr<UI> ui = nullptr;
    if(uiType == UIType::CLI){
        ui = std::make_shared<CLI>(context);
    }else if(uiType == UIType::QT){
        LOG_ERROR("QT UI is not implemented yet.");
        return 1;
    }else{
        LOG_ERROR("Only CLI UI is supported in this version.");
        return 1;
    }
    
    // Initialize MVC components
    ChatModel chatModel(context);
    ChatView chatView(context);
    Controller controller(context, chatModel, chatView);

    // Initialize services
    NetworkService networkService(context);

    // Connect event handlers
    UI* uiPtr = ui.get();
    Controller* ctrlPtr = &controller;
    
    //View to UI
    context.eventBus.on("ui::show-chat-menu", std::function<void(const std::vector<ChatOption>&)>([&ui](const std::vector<ChatOption>& options){
        ui->onChatView_ShowMainMenu(options);
    }));

    context.eventBus.on("ui::show-error", std::function<void(const char* options)>([&ui](const char* options){
        ui->onShowError(options);
    }));

    context.eventBus.on("ui::show-info", std::function<void(const char* options)>([&ui](const char* options){
        ui->onShowInfo(options);
    }));

    //UI to Controller
    context.eventBus.on("controller::user-input", std::function<void(const std::string& input, const std::vector<std::string> args)>([ctrlPtr](const std::string& input, const std::vector<std::string> args){
        ctrlPtr->dispatchUserCommand(input, args);
    }));
    
    // Start the application
    LOG_INFO("Starting ChatApp with UI type: %s", 
             (uiType == UIType::CLI ? "CLI" : "QT"));
    
    if (!ui) {
        LOG_ERROR("Failed to initialize UI");
        return 1;
    }

    chatView.show();
    
    std::thread networkThread([&networkService, &context]() {
        auto [code, msg] = networkService.startServer("127.0.0.1", 5000);
        if (code != 0) {
            LOG_ERROR("Failed to start server: %s", msg.c_str());
            context.eventBus.emit("ui::show-error", "Failed to start network server: " + msg);
        }
    });

    std::thread uiThread([&ui]() {
        ui->exec();
    });

    networkThread.join();
    uiThread.join();

    return 0;
}
