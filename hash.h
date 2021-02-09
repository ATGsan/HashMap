#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include <initializer_list>
#include <stdexcept>

const size_t INIT_SIZE = 2;
const size_t CONST_REHASH = 2;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
public:
    typedef typename std::pair<const KeyType, ValueType> KeyValue;

    typedef typename std::list<KeyValue>::iterator iterator;

    typedef typename std::list<KeyValue>::const_iterator const_iterator;

    HashMap(Hash h = Hash()) : hash(h) {
        hash_size = INIT_SIZE;
        hash_table.resize(hash_size);
    };

    template<class Iter>
    HashMap(Iter begin, Iter end, Hash h = Hash()) : hash(h) {
        hash_size = INIT_SIZE;
        hash_table.resize(hash_size);
        for (; begin != end; ++begin)
            insert(*begin);
    }

    HashMap(const std::initializer_list<KeyValue> &data, Hash h = Hash()) : hash(h) {
        hash_size = INIT_SIZE;
        hash_table.resize(hash_size);
        for (auto &el : data)
            insert(el);
    }

    HashMap(const std::initializer_list<KeyValue> &&data, Hash h = Hash()) : hash(h) {
        hash_size = INIT_SIZE;
        hash_table.resize(hash_size);
        for (auto &el : data)
            insert(el);
    }

    size_t size() const;

    bool empty() const;

    Hash hash_function() const;

    void insert(const KeyValue &el);

    void erase(const KeyType &key);

    ValueType &operator[](const KeyType &key);

    const ValueType &at(const KeyType &key) const;

    void clear();

    iterator find(const KeyType &key) {
        size_t hash_index = hash(key) % hash_size;
        for (auto &it : hash_table[hash_index]) {
            if (key == it->first) {
                return it;
            }
        }
        return elem_value.end();
    }

    const_iterator find(const KeyType &key) const {
        size_t hash_index = hash(key) % hash_size;
        for (auto &it : hash_table[hash_index]) {
            if (key == it->first) {
                return it;
            }
        }
        return elem_value.end();
    }

    iterator begin() {
        return elem_value.begin();
    }

    const_iterator begin() const {
        return elem_value.cbegin();
    }

    iterator end() {
        return elem_value.end();
    }

    const_iterator end() const {
        return elem_value.cend();
    }

    HashMap &operator=(HashMap const &other);


private:
    std::vector<std::list<iterator>> hash_table;
    std::list<KeyValue> elem_value;
    Hash hash;
    size_t elem_size = 0;
    size_t hash_size = 0;

    void rehash();
};

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash> &HashMap<KeyType, ValueType, Hash>::operator=(
        HashMap<KeyType, ValueType, Hash> const &other) {
    if (&other == this) {
        return *this;
    }
    for (auto &el : elem_value) {
        size_t hash_index =
                hash(el.first) % hash_size;
        hash_table[hash_index].clear();
    }
    elem_value.clear();
    hash_size = INIT_SIZE;
    hash = other.hash_function();
    hash_table.resize(other.hash_size);
    for (auto &other_el : other) {
        insert(other_el);
    }
    return *this;
}

template<class KeyType, class ValueType, class Hash>
size_t HashMap<KeyType, ValueType, Hash>::size() const {
    return elem_size;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::empty() const {
    return elem_size == 0;
}

template<class KeyType, class ValueType, class Hash>
Hash HashMap<KeyType, ValueType, Hash>::hash_function() const {
    return hash;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert(const HashMap::KeyValue &el) {
    if (find(el.first) != elem_value.end())
        return;
    size_t hash_index = hash(el.first) % hash_size;
    ++elem_size;
    elem_value.push_front(el);
    hash_table[hash_index].push_front(elem_value.begin());
    if (hash_size < elem_size * CONST_REHASH) {
        rehash();
    }
}



template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(const KeyType &key) {
    size_t hash_index = hash(key) % hash_size;
    for (auto it = hash_table[hash_index].begin(); it != hash_table[hash_index].end(); ++it) {
        if (key == (*it)->first) {
            elem_size--;
            elem_value.erase(*it);
            hash_table[hash_index].erase(it);
            return;
        }
    }
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::rehash() {
    hash_table.clear();
    hash_size *= CONST_REHASH;
    hash_table.resize(hash_size);
    for (auto it = elem_value.begin(); it != elem_value.end(); ++it) {
        size_t hash_index = hash(it->first) % hash_size;
        hash_table[hash_index].push_back(it);
    }
}

template<class KeyType, class ValueType, class Hash>
ValueType &HashMap<KeyType, ValueType, Hash>::operator[](const KeyType &key) {
    auto it = find(key);
    if (it == elem_value.end()) {
        insert({key, ValueType()});
        it = find(key);
    }
    return it->second;
}

template<class KeyType, class ValueType, class Hash>
const ValueType &HashMap<KeyType, ValueType, Hash>::at(const KeyType &key) const {
    auto it = find(key);
    if (it == elem_value.end()) {
        throw std::out_of_range("");
    }
    return it->second;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::clear() {
    for (auto &el : elem_value) {
        size_t hash_index = hash(el.first) % hash_size;
        hash_table[hash_index].clear();
    }
    elem_value.clear();
    elem_size = 0;
}
