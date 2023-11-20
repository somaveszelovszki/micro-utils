#pragma once

#include <initializer_list>
#include <type_traits>

#include <etl/vector.h>

namespace micro {

template <typename T, size_t N>
class vector : public etl::vector_ext<T> {
    using base = etl::vector_ext<T>;
public:
    vector() : base(buffer_, N)
    {
        this->initialise();
    }

    explicit vector(size_t initial_size) : base(buffer_, N)
    {
        this->initialise();
        this->resize(initial_size);
    }

    vector(size_t initial_size, typename base::parameter_t value) : base(buffer_, N)
    {
        this->initialise();
        this->resize(initial_size, value);
    }

    template <typename Iterator>
    vector(Iterator first, Iterator last, typename std::enable_if_t<!std::is_integral_v<Iterator>, int> = 0) : base(buffer_, N)
    {
        this->assign(first, last);
    }

    vector(std::initializer_list<T> init) : base(buffer_, N)
    {
        this->assign(init.begin(), init.end());
    }

    vector(const vector& other) : base(buffer_, N)
    {
        this->assign(other.begin(), other.end());
    }

    vector(vector&& other) : base(buffer_, N)
    {
        if (this != &other)
        {
        this->initialise();

        auto itr = other.begin();
        while (itr != other.end())
        {
            this->push_back(std::move(*itr));
            ++itr;
        }

        other.initialise();
        }
    }

    ~vector() = default;

    vector& operator=(const vector& other)
    {
        static_cast<base&>(*this) = other;
        return *this;
    }

    vector& operator = (vector&& other)
    {
        static_cast<base&>(*this) = std::move(other);
        return *this;
    }

private:
    T buffer_[N];
};

} // namespace micro
