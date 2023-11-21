#pragma once

#include <functional>
#include <initializer_list>

#include <etl/flat_map.h>

#include <micro/container/aligned_storage.hpp>
#include <micro/utils/types.hpp>

namespace micro {

template <typename Key, typename Value, size_t N, typename Compare = std::less<Key>>
class map : public etl::flat_map_ext<Key, Value> {
    using base = etl::flat_map_ext<Key, Value>;
public:
    map() : base(lookupBuffer_, storageBuffer_, N) {}

    map(const map& other) : base(lookupBuffer_, storageBuffer_, N) {
        this->assign(other.begin(), other.end());
    }

    map(map&& other) : base(lookupBuffer_, storageBuffer_, N) {
        if (&other != this)
        {
          this->move_container(std::move(other));
        }
    }

    template <typename Iterator>
    map(Iterator first, Iterator last) : base(lookupBuffer_, storageBuffer_, N) {
        this->assign(first, last);
    }

    map(std::initializer_list<typename base::node_t> init) : base(lookupBuffer_, storageBuffer_, N) {
        this->assign(init.begin(), init.end());
    }

    ~map() = default;

    map& operator=(const map& other)
    {
        static_cast<base&>(*this) = other;
        return *this;
    }

    map& operator=(map&& other)
    {
        static_cast<base&>(*this) = std::move(other);
        return *this;
    }

private:
    aligned_storage_t<remove_const_key_t<typename base::node_t>> storageBuffer_[N];
    typename base::node_ptr_t lookupBuffer_[N];
};

} // namespace micro
