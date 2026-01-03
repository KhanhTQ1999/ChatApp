#include <iostream>
#include <string>
#include <memory>

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
    std::unique_ptr<UI> ui = nullptr;
    if(uiType == UIType::CLI){
        ui = std::make_unique<CLI>();
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
    context.eventBus.on<std::vector<ChatOption>>("ui::show-chat-menu", [uiPtr](const std::vector<ChatOption>& options){
        uiPtr->onChatView_ShowMainMenu(options);
    });

    context.eventBus.on<std::string>("ui::show-error", [uiPtr](const std::string& options){
        uiPtr->onShowError(options);
    });

    //UI to Controller
    context.eventBus.on<std::string>("controller::user-input", [ctrlPtr](const std::string& input){
        ctrlPtr->dispatchUserCommand(input);
    });
    
    // Start the application
    LOG_INFO("Starting ChatApp with UI type: %s", 
             (uiType == UIType::CLI ? "CLI" : "QT"));
    
    if (!ui) {
        LOG_ERROR("Failed to initialize UI");
        return 1;
    }

    chatView.show();
    
    std::thread networkThread([&networkService, &context]() {
        auto [code, msg] = networkService.startServer();
        if (code != 0) {
            LOG_ERROR("Failed to start server: %s", msg.c_str());
            context.eventBus.emit("ui::show-error", "Failed to start network server: " + msg);
        }else{
            LOG_INFO("Network server started successfully");
            context.eventBus.emit("ui::show-info", "Network server started successfully");
        }
    });

    std::thread uiThread([&ui]() {
        ui->exec();
    });

    networkThread.join();
    uiThread.join();

    return 0;
}
