#pragma once

#include <micro/container/vec.hpp>

#include <utility>

namespace micro {

template <typename K, typename V>
struct PairKeyComparator {
    constexpr bool operator()(const std::pair<K, V>& a, const std::pair<K, V>& b) const { return a.first < b.first; }
    constexpr bool operator()(const K& key, const std::pair<K, V>& entry) const         { return key < entry.first; }
    constexpr bool operator()(const std::pair<K, V>& entry, const K& key) const         { return entry.first < key; }
};

/* @brief Stores key-value pairs.
 * @tparam K The key type.
 * @tparam V The value type.
 * @tparam capacity The map capacity.
 **/
template <typename K, typename V, uint32_t capacity_, typename vec_type>
class map_base {
public:
    static_assert(std::is_base_of<vec_base<std::pair<K, V>, capacity_>, vec_type>::value, "Invalid vector type");

    typedef typename vec_type::iterator iterator;
    typedef typename vec_type::const_iterator const_iterator;
    typedef typename vec_type::entry_type entry_type;

    map_base() {}

    map_base(const map_base& other)
        : values_(other.values_) {}

    uint32_t size() const {
        return this->values_.size();
    }

    uint32_t capacity() const {
        return this->values_.capacity();
    }

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

    iterator back() {
        return this->values_.back();
    }

    const_iterator back() const {
        return this->values_.back();
    }

    iterator erase(iterator iter) {
        return this->values_.erase(iter);
    }

    const_iterator erase(const_iterator iter) {
        return this->values_.erase(iter);
    }

    virtual ~map_base() {}

protected:
    vec_type values_;  // Vector of the values.
};

/* @brief Stores unsorted key-value pairs.
 * @tparam K The key type.
 * @tparam V The value type.
 * @tparam capacity The map capacity.
  **/
template <typename K, typename V, uint32_t capacity_>
class unsorted_map : public map_base<K, V, capacity_, vec<std::pair<K, V>, capacity_>> {
private:
    typedef map_base<K, V, capacity_, vec<std::pair<K, V>, capacity_>> base_type;

public:
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::entry_type entry_type;

    unsorted_map() {}

    unsorted_map(const unsorted_map& other) : base_type(other) {}

    unsorted_map(const std::initializer_list<entry_type>& values) {
        for (const entry_type& entry : values) {
            this->push_back(entry);
        }
    }

    /* @brief Puts a key-value pair into the map.
     * @param key The key.
     * @param Value The value.
     **/
    void emplace(const K& key, const V& value) {
        V * const v = this->at(key);
        if (v) {
            *v = value;
        } else {
            this->values_.emplace_back(key, value);
        }
    }

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    const V* at(const K& key) const {
        const const_iterator it = std::find_if(this->begin(), this->end(), [&key] (const entry_type& entry) { return entry.first == key; });
        return it != this->end() ? &it->second : nullptr;
    }

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    V* at(const K& key) {
        return const_cast<V*>(const_cast<const unsorted_map*>(this)->at(key));
    }

    ~unsorted_map() {}

private:
    void push_back(const entry_type& entry) {
        V * const v = this->at(entry.first);
        if (v) {
            *v = entry.second;
        } else {
            this->values_.push_back(entry);
        }
    }
};

/* @brief Stores key-value pairs.
 * @tparam K The key type.
 * @tparam V The value type.
 * @tparam capacity The map capacity.
 **/
template <typename K, typename V, uint32_t capacity_>
class sorted_map : public map_base<K, V, capacity_, sorted_vec<std::pair<K, V>, capacity_, PairKeyComparator<K, V>>> {
private:
    typedef map_base<K, V, capacity_, sorted_vec<std::pair<K, V>, capacity_, PairKeyComparator<K, V>>> base_type;

public:
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::entry_type entry_type;

    sorted_map() {}

    sorted_map(const sorted_map& other) : base_type(other) {}

    sorted_map(const std::initializer_list<entry_type>& values) {
        for (const entry_type& entry : values) {
            this->insert(entry);
        }
    }

    /* @brief Puts a key-value pair into the map.
     * @param key The key.
     * @param Value The value.
     **/
    void emplace(const K& key, const V& value) {
        this->insert({ key, value });
    }

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    const V* at(const K& key) const {
        const const_iterator it = std::lower_bound(this->begin(), this->end(), key, PairKeyComparator<K, V>{});
        return it != this->end() && it->first == key ? &it->second : nullptr;
    }

    /* @brief Gets value by key.
     * @param key The key.
     * @returns Pointer to the value or nullptr if key not found.
     **/
    V* at(const K& key) {
        return const_cast<V*>(const_cast<const sorted_map*>(this)->at(key));
    }

    V lerp(const K& key) {
        V result { 0 };
        if (this->size() > 0) {
            const_iterator lower = std::lower_bound(this->begin(), this->end(), key, PairKeyComparator<K, V>{});
            if (lower != this->begin()) {
                lower = std::prev(lower);
            }

            const_iterator upper = std::upper_bound(this->begin(), this->end(), key, PairKeyComparator<K, V>{});
            if (upper == this->end()) {
                upper = this->back();
            }

            result = map(key, lower->first, upper->first, lower->second, upper->second);
        }

        return result;
    }

    ~sorted_map() {}

private:
    void insert(const entry_type& entry) {
        V * const v = this->at(entry.first);
        if (v) {
            *v = entry.second;
        } else {
            this->values_.insert(entry);
        }
    }
};

} // namespace micro
