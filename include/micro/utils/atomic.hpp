#pragma once

#include <micro/utils/storage.hpp>
#include <micro/bsp/mutex.hpp>

#include <algorithm>
#include <type_traits>

namespace micro {

template <typename T>
class atomic {
public:
    template<typename ...Args>
    atomic(mutex_handle_t *_hmutex, Args&&... args)
        : hmutex(_hmutex) {
        this->data.emplace(std::forward<Args>(args)...);
    }

    void operator=(const T& _value) volatile {
        this->data.construct(_value);
    }

    void operator=(T&& _value) volatile {
        this->data.construct(std::move(_value));
    }

    void wait_copy(T& result) const volatile {
        while (!isOk(micro::mutexTake(this->hmutex, millisecond_t(1)))) {}
        result = *const_cast<T*>(this->data.value_ptr());
        micro::mutexRelease(this->hmutex);
    }

    void wait_set(const T& value) volatile {
        while (!isOk(micro::mutexTake(this->hmutex, millisecond_t(1)))) {}
        this->data.construct(value);
        micro::mutexRelease(this->hmutex);
    }

    volatile T* wait_ptr() volatile {
        while (!isOk(micro::mutexTake(this->hmutex, millisecond_t(1)))) {}
        return this->data.value_ptr();
    }

    volatile T* accept_ptr() volatile {
        return isOk(micro::mutexTake_ISR(this->hmutex)) ? this->data.value_ptr() : nullptr;
    }

    void release_ptr() volatile {
        micro::mutexRelease(this->hmutex);
    }

    mutex_handle_t* getMutex() {
        return this->hmutex;
    }

private:
    mutex_handle_t * const hmutex;
    volatile_storage_t<T> data;
};

template <typename T> struct is_atomic {
    enum { value = micro::is_base_of_template<atomic, T>::value };
};

} // namespace micro
