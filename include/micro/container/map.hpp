#pragma once

#include <micro/container/vec.hpp>

#include <utility>

namespace micro {

/* @brief Stores key-value pairs.
 * @tparam K The key type.
 * @tparam V The value type.
 * @tparam capacity The map capacity.
 **/
template <typename K, typename V, uint32_t capacity_>
class map_base {
private:
    typedef vec<std::pair<K, V>, capacity_> vec_type;

public:
    typedef typename vec_type::iterator iterator;
    typedef typename vec_type::const_iterator const_iterator;
    typedef typename vec_type::entry_type entry_type;
    uint32_t size() const {
        return this->values_.size();
    }

    uint32_t capacity() const {
        return this->values_.capacity();
    }

    /* @brief Puts a key-value pair into the map.
     * @param key The key.
     * @param Value The value.
     **/
    virtual void put(const K& key, const V& value) = 0;

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    virtual const V* get(const K& key) const = 0;

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    virtual V* get(const K& key) = 0;

    /* @brief Clears vector.
     **/
    void clear() {
        this->values_.clear();
    }

    iterator begin() {
        return this->values_.begin();
    }

    const_iterator begin() const {
        return this->values_.begin();
    }

    iterator end() {
        return this->values_.end();
    }

    const_iterator end() const {
        return this->values_.end();
    }

    virtual ~map_base() {}

protected:
    vec<std::pair<K, V>, capacity_> values_;  // Vector of the values.
};


/* @brief Stores key-value pairs.
 * @tparam K The key type.
 * @tparam V The value type.
 * @tparam capacity The map capacity.
 **/
template <typename K, typename V, uint32_t capacity_>
class sorted_map : public map_base<K, V, capacity_> {
private:
    typedef map_base<K, V, capacity_> base_type;

public:
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::entry_type entry_type;

    /* @brief Puts a key-value pair into the map.
     * @param key The key.
     * @param Value The value.
     **/
    void put(const K& key, const V& value) override {
        if (this->values_.size() < this->values_.capacity()) {
            if (!this->size()) {
                this->values_.emplace_back(key, value);
            } else {
                for (iterator it = this->begin(); it != this->end(); ++it) {
                    if (key < it->first) {
                        this->values_.emplace(it, key, value);
                        break;
                    }
                }
            }
        }
    }

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    const V* get(const K& key) const override {
        const V *res = nullptr;

        if (this->size() > 0) {
            uint32_t first = 0;
            uint32_t last = this->size() - 1;

            do {

                uint32_t mid = micro::avg(first, last);
                const std::pair<K, V>& entry = this->values_[mid];

                if (key < entry.first) {
                    last = mid - 1;
                } else if (key > entry.first) {
                    first = mid + 1;
                } else {
                    res = &entry.second;
                }

            } while (!res && first <= last);
        }

        return res;
    }

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    V* get(const K& key) override {
        return const_cast<V*>(const_cast<const sorted_map<K, V, capacity_>*>(this)->get(key));
    }
};

/* @brief Stores unsorted key-value pairs.
 * @tparam K The key type.
 * @tparam V The value type.
 * @tparam capacity The map capacity.
  **/
template <typename K, typename V, uint32_t capacity_>
class unsorted_map : public map_base<K, V, capacity_> {
private:
    typedef map_base<K, V, capacity_> base_type;

public:
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::entry_type entry_type;

    /* @brief Puts a key-value pair into the map.
     * @param key The key.
     * @param Value The value.
     **/
    void put(const K& key, const V& value) override {
        if (this->values_.size() < this->values_.capacity()) {
            this->values_.emplace_back(key, value);
        }
    }

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    const V* get(const K& key) const override {
        const V *res = nullptr;

        for (const std::pair<K, V>& entry : *this) {
            if (entry.first == key) {
                res = &entry.second;
                break;
            }
        }

        return res;
    }

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    V* get(const K& key) override {
        return const_cast<V*>(const_cast<const unsorted_map<K, V, capacity_>*>(this)->get(key));
    }
};
} // namespace micro
