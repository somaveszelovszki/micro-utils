#pragma once

#include <functional>
#include <initializer_list>

#include <etl/flat_set.h>

#include <micro/container/aligned_storage.hpp>

namespace micro {

template <typename T, size_t N, typename Compare = std::less<T>>
class set : public etl::flat_set_ext<T> {
    using base = etl::flat_set_ext<T>;
public:
    set() : base(lookupBuffer_, storageBuffer_, N) {}

    set(const set& other) : base(lookupBuffer_, storageBuffer_, N) {
        this->assign(other.begin(), other.end());
    }

    set(set&& other) : base(lookupBuffer_, storageBuffer_, N) {
        if (&other != this)
        {
          this->move_container(std::move(other));
        }
    }

    template <typename Iterator>
    set(Iterator first, Iterator last) : base(lookupBuffer_, storageBuffer_, N) {
        this->assign(first, last);
    }

    set(std::initializer_list<T> init) : base(lookupBuffer_, storageBuffer_, N) {
        this->assign(init.begin(), init.end());
    }

    ~set() = default;

    set& operator=(const set& other)
    {
        static_cast<base&>(*this) = other;
        return *this;
    }

    set& operator=(set&& other)
    {
        static_cast<base&>(*this) = std::move(other);
        return *this;
    }

private:
    typename std::conditional<sizeof(T) < 8, aligned_storage<T>, T>::type storageBuffer_[N];
    typename base::node_ptr_t lookupBuffer_[N];
};

} // namespace micro
