#pragma once
#include <functional>
#include <optional>
#include <string>

namespace pattern
{
    template<typename Func>
    inline void tryCatch(const std::string& context, Func&& func) {
        try {
            std::forward<Func>(func)();
        } catch (const std::exception& ex) {
            LOG_ERROR(context + " failed: " + ex.what());
        } catch (...) {
            LOG_ERROR(context + " failed: Unknown error");
        }
    }

    template<typename Func>
    inline auto tryCatchWithReturn(const std::string& context, Func&& func) -> std::optional<decltype(func())> {
        try {
            return std::forward<Func>(func)();
        } catch (const std::exception& ex) {
            LOG_ERROR(context + " failed: " + ex.what());
        } catch (...) {
            LOG_ERROR(context + " failed: Unknown error");
        }
        return std::nullopt;
    }

    template<typename Func>
    inline auto tryCatchWithTuple(const std::string& context, Func&& func)
        -> std::tuple<std::optional<decltype(func())>, std::optional<std::string>>
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