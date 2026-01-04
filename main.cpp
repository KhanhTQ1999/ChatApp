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

    // Connect MVC -> Services event to handlers
    context.eventBus.on("network::connect-to-peer", std::function<void(std::string, int)>([&networkService](std::string ip, int port){
        networkService.connectToPeer(ip, port);
    }));

    context.eventBus.on("network::send-to-peer", std::function<void(int, const std::string&)>([&networkService](int connectionId, const std::string& message){
        networkService.sendMessageToPeer(connectionId, message);
    }));

    context.eventBus.on("network::disconnect-from-peer", std::function<void(int)>([&networkService](int connectionId){
        networkService.disconnectFromPeer(connectionId);
    }));

    context.eventBus.on("network::list-active-connections", std::function<void()>([&networkService, &chatView](){
        std::vector<int> sfdList = networkService.getActiveConnections();
        chatView.displayConnections(sfdList);
    }));

    // Connect View -> UI event to handlers
    context.eventBus.on("ui::show-chat-menu", std::function<void(const std::vector<ChatOption>&)>([&ui](const std::vector<ChatOption>& options){
        ui->onChatView_ShowMainMenu(options);
    }));

    context.eventBus.on("ui::show-all-connections", std::function<void(const std::vector<int>&)>([&ui](const std::vector<int>& connectionIds){
        ui->onChatView_ShowAllConnections(connectionIds);
    }));

    // Connect anyone -> UI event to handlers
    context.eventBus.on("ui::show-error", std::function<void(const char* options)>([&ui](const char* options){
        ui->onShowError(options);
    }));

    context.eventBus.on("ui::show-info", std::function<void(const char* options)>([&ui](const char* options){
        ui->onShowInfo(options);
    }));

    // Connect UI -> Controller event to handlers
    context.eventBus.on("controller::user-input", std::function<void(const std::string& input, const std::vector<std::string>& args)>([&controller](const std::string& input, const std::vector<std::string>& args){
        controller.dispatchUserCommand(input, args);
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
            context.eventBus.emit("ui::show-error", ("Failed to start network server: " + msg).c_str());
        }
    });

    std::thread uiThread([&ui]() {
        ui->exec();
    });

    networkThread.join();
    uiThread.join();

    return 0;
}
