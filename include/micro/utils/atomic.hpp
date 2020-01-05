#pragma once

#include "storage.hpp"

#include <FreeRTOS.h>
#include <semphr.h>

#include <algorithm>
#include <type_traits>

namespace micro {

template <typename T>
class atomic {
public:
    typedef T underlying_type;

    template<typename ...Args>
    atomic(SemaphoreHandle_t hmutex, Args&&... args)
        : hmutex(hmutex) {
        this->data.emplace(std::forward<Args>(args)...);
    }

    void operator=(const T& value) volatile {
        this->data.construct(value);
    }

    void operator=(T&& value) volatile {
        this->data.construct(std::move(value));
    }

    void wait_copy(T& result) const volatile {
        while (!xSemaphoreTake(this->hmutex, 1)) {}
        result = *const_cast<T*>(this->data.value_ptr());
        xSemaphoreGive(this->hmutex);
    }

    void wait_set(const T& value) volatile {
        while (!xSemaphoreTake(this->hmutex, 1)) {}
        this->data.construct(value);
        xSemaphoreGive(this->hmutex);
    }

    volatile T* wait_ptr() volatile {
        while (!xSemaphoreTake(this->hmutex, 1)) {}
        return this->data.value_ptr();
    }

    volatile T* accept_ptr() volatile {
        return xSemaphoreTake(this->hmutex, 0) ? this->data.value_ptr() : nullptr;
    }

    void release_ptr() volatile {
        xSemaphoreGive(this->hmutex);
    }

    SemaphoreHandle_t getMutex() const volatile {
        return this->hmutex;
    }

private:
    SemaphoreHandle_t hmutex;
    volatile_storage_t<T> data;
};

template <typename T> struct is_atomic {
    enum { value = micro::is_base_of_template<atomic, T>::value };
};

template <typename T>
typename std::enable_if<is_base_of_template<atomic, T>::value, typename T::underlying_type>::type getValue(const T& value) {
    storage_t<typename T::underlying_type> result;
    value.wait_copy(*result.value_ptr());
    return result.value();
}

} // namespace micro
