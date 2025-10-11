#pragma once
#include <iostream>
#include <mutex>

namespace base
{
    template <typename T>
    class Singleton {
        public:
            static T& getInstance() {
                std::call_once(initFlag_, []() {
                    std::cout << "Singleton instance created." << std::endl;
                    instance_ = new T();
                });
                return *instance_;
            }

            Singleton(const Singleton&) = delete;
            Singleton& operator=(const Singleton&) = delete;

        protected:
            Singleton() = default;
            virtual ~Singleton() = default;

        private:
            static std::once_flag initFlag_;
            static T* instance_;
    };

    template <typename T>
    std::once_flag Singleton<T>::initFlag_;

    template <typename T>
    T* Singleton<T>::instance_ = nullptr;
}

#define MAKE_SINGLETON(classname)                    \
        friend class base::Singleton<classname>;              \
    private:                                                \
        classname(const classname&);                        \
        classname& operator=(const classname);              \
    protected:                                              \
        classname() = default;                               \
        virtual ~classname();

            
