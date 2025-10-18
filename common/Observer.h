#pragma once
#include <functional>
#include <vector>

namespace pattern
{
    template<typename T>
    class Observer {
    public:
        using ObserverFunc = std::function<void(const T&)>;

        void set(const T& value) {
            data = value;
            notify();
        }

        const T& get() const {
            return data;
        }

        void subscribe(ObserverFunc observer) {
            observers.push_back(observer);
        }

    private:
        T data;
        std::vector<ObserverFunc> observers;

        void notify() {
            for (auto& obs : observers) {
                obs(data);
            }
        }
    };
}
