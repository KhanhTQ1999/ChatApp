#pragma once
#include <functional>
#include <optional>
#include <string>
#include "../utils/Utils.h"

namespace pattern
{
    template<typename Func>
    inline void tryCatch(const std::string& context, Func&& func) {
        try {
            std::forward<Func>(func)();
        } catch (const std::exception& ex) {
            LOG_ERROR("%s failed: %s", context, ex.what());
        } catch (...) {
            LOG_ERROR("%s failed: Unknown error", context);
        }
    }

    template<typename Func>
    inline auto tryCatchWithReturn(const std::string& context, Func&& func) -> std::optional<decltype(func())> {
        try {
            return std::forward<Func>(func)();
        } catch (const std::exception& ex) {
            LOG_ERROR("%s failed: %s", context, ex.what());
        } catch (...) {
            LOG_ERROR("%s failed: Unknown error", context);
        }
        return std::nullopt;
    }

    template<typename Func>
    inline std::optional<std::string> tryCatchWithError(const std::string& context, Func&& func) {
        try {
            std::forward<Func>(func)();
            return std::nullopt;
        } catch (const std::exception& ex) {
            return context + " failed: " + std::string(ex.what());
        } catch (...) {
            return context + " failed: Unknown error";
        }
    }

    template<typename Func>
    inline auto tryCatchWithPair(const std::string& context, Func&& func)
        -> std::pair<std::optional<decltype(func())>, std::optional<std::string>>
    {
        try {
            return { std::forward<Func>(func)(), std::nullopt };
        } catch (const std::exception& ex) {
            return { std::nullopt, context + " failed: " + std::string(ex.what()) };
        } catch (...) {
            return { std::nullopt, context + " failed: Unknown error" };
        }
    }
}