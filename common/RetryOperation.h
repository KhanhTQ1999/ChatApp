#pragma once
#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include <stdexcept>

/**
 * @brief Executes an operation with automatic retry logic.
 *
 * This function attempts to execute the provided operation. If the operation throws
 * an exception, it will retry after a specified delay, up to a maximum number of retries.
 *
 * @tparam T Return type of the operation.
 * @param operation A function that takes the number of retries left and returns T.
 * @param delay_ms Delay in milliseconds between retries.
 * @param retries Number of retry attempts allowed.
 * @return T The result of the successful operation.
 * @throws std::exception If all retries fail, the last exception is rethrown.
 */
template<typename T>
T retryOperation(std::function<T(int32_t)> operation, int32_t delay_ms, int32_t retries) {
    try {
        return operation(retries);
    } catch (const std::exception& e) {
        if (retries > 0) {
            std::cerr << "Retrying in " << delay_ms << "ms... (" << retries << " retries left)\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
            return retryOperation<T>(operation, delay_ms, retries - 1);
        } else {
            std::cerr << "All retries exhausted. Final error: " << e.what() << "\n";
            throw;
        }
    }
}