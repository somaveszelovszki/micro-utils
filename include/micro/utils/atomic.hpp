#pragma once

#include <micro/utils/storage.hpp>
#include <micro/bsp/mutex.hpp>

#include <algorithm>
#include <type_traits>

namespace micro {

template <typename T>
class atomic {
public:
    typedef T underlying_type;

    template<typename ...Args>
    atomic(mutex_handle_t _hmutex, Args&&... args)
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
        while (!isOk(micro::mutexTake(this->getMutex(), millisecond_t(1)))) {}
        result = *const_cast<T*>(this->data.value_ptr());
        micro::mutexRelease(this->getMutex());
    }

    void wait_set(const T& value) volatile {
        while (!isOk(micro::mutexTake(this->hmutex, millisecond_t(1)))) {}
        this->data.construct(value);
        micro::mutexRelease(this->hmutex);
    }

    volatile T* wait_ptr() volatile {
        while (!isOk(micro::mutexTake(this->getMutex(), millisecond_t(1)))) {}
        return this->data.value_ptr();
    }

    volatile T* accept_ptr() volatile {
        return isOk(micro::mutexTake_ISR(this->getMutex())) ? this->data.value_ptr() : nullptr;
    }

    void release_ptr() volatile {
        micro::mutexRelease(this->getMutex());
    }

    mutex_handle_t getMutex() const volatile {
        return { this->hmutex.ptr };
    }

private:
    mutex_handle_t hmutex;
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
