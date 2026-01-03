#pragma once

#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <any>

#include "utils/Utils.h"

// EventBus for decoupled communication between components
class EventBus
{
public:
    EventBus() = default;
    ~EventBus() = default;

    // Register an event handler with variadic arguments
    template<typename... Args>
    void on(const std::string& event, std::function<void(Args...)> callback) {
        auto wrapper = [callback](std::vector<std::any> args) {
            callWithArgs(callback, args, std::index_sequence_for<Args...>{});
        };
        events_[event].push_back(wrapper);
    }

    // Emit an event with variadic arguments
    template<typename... Args>
    void emit(const std::string& event, Args&&... args) {
        auto it = events_.find(event);
        if (it != events_.end()) {
            std::vector<std::any> argVec = {std::forward<Args>(args)...};
            for (auto& cb : it->second) {
                cb(argVec);
            }
        }
    }

private:
    // Helper to unpack std::any arguments and call the callback
    template<typename... Args, std::size_t... Is>
    static void callWithArgs(std::function<void(Args...)> callback, 
                           const std::vector<std::any>& args,
                           std::index_sequence<Is...>) {
        try{
            callback(std::any_cast<Args>(args[Is])...);
        } catch (const std::bad_any_cast& e) {
            LOG_ERROR("Failed to cast event arguments: %s", e.what());
        }
    }

    std::unordered_map<std::string, std::vector<std::function<void(std::vector<std::any>)>>> events_;
};

struct ChatOption { 
    std::string action; 
    std::string description; 
};

// Application state enum
enum class AppState : uint32_t {
    Running,
    Stopped
};

// Application context shared across components
struct AppContext {
    EventBus eventBus;
    AppState appState = AppState::Running;
};