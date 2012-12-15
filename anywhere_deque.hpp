#ifndef ANYWHERE_DEQUE_HPP
#define ANYWHERE_DEQUE_HPP

#include "slidable_map.hpp"
#include <boost/iterator/iterator_facade.hpp>
#include <boost/operators.hpp>

namespace gununu {
template <class T, class Allocator>
class anywhere_deque;

namespace detail {
template <class Map, class Value, class Ref>
class iterator_base : public boost::iterator_facade<iterator_base<Map,Value,Ref>, Value, boost::random_access_traversal_tag, Ref> {
    friend class boost::iterator_core_access;
    template <class,class> friend class anywhere_deque;
    template <class,class,class> friend class iterator_base;
public:
    template <class M, class R>
    iterator_base(const iterator_base<M, Value,R>& other):map(other.map),index(other.index){}
    iterator_base():map(NULL),index(0){}
private:    
    iterator_base(Map* m, std::size_t n):map(m),index(n){}
    
    void increment() {
        assert(map);
        ++index;
    }
    void decrement() {
        assert(map);
        --index;
    }
    void advance(std::ptrdiff_t n) {
        assert(map);
        index += n;
    }
    Ref dereference() const {
        assert(map);
        return map->at(index);
    }
    template <class M, class R>
    bool equal(iterator_base<M,Value,R> rhs) const {
        assert(map && rhs.map);
        return this->index == rhs.index;
    }
    template <class M, class R>
    std::ptrdiff_t distance_to(iterator_base<M,Value,R> rhs) const {
        assert(map && rhs.map);
        return (std::ptrdiff_t)rhs.index - this->index;
    }

    Map* map;
    std::size_t index;
};
}

template <class T, class Allocator = std::allocator<T> >
class anywhere_deque : 
        private Allocator ,
        private boost::totally_ordered<anywhere_deque<T,Allocator> > {
public:
    typedef detail::iterator_base<anywhere_deque, T, T&> iterator;
    typedef detail::iterator_base<const anywhere_deque, T, const T&> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef typename Allocator::pointer pointer;
    typedef typename Allocator::const_pointer const_pointer;
    typedef Allocator allocator_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    
    explicit anywhere_deque(const Allocator& a = Allocator()):Allocator(a),map(a){}
    anywhere_deque(const anywhere_deque& r) : Allocator(r), map(r.map) {}
    anywhere_deque(const anywhere_deque& r, const Allocator& a) : Allocator(a), map(r.map, a) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
    anywhere_deque(anywhere_deque&& r) : Allocator(std::move(r)), map(std::move(r.map)){}
    anywhere_deque(anywhere_deque&& r, const Allocator& a) : Allocator(a), map(std::move(r.map), a){}
#endif
    anywhere_deque(size_type count, const value_type& val, const Allocator& a = Allocator()) : Allocator(a), map(a) {
            size_type i=size_type();
            for (; i < count; ++i)
                push_back(val);
    }
    explicit anywhere_deque(size_type count) {
        size_type i=size_type();
        for (; i < count; ++i)
            push_back(value_type());
    }

    template <class InputIt>
    anywhere_deque(InputIt first, InputIt last, const Allocator& a = Allocator()) : Allocator(a), map(a) {
        for (; first != last; ++first)
            push_back(*first);
    }

#ifndef BOOST_NO_UNIFIED_INITIALIZETION_SYNTAX
    anywhere_deque(std::initializer_list<T> list, const Allocator& a = Allocator()) : Allocator(a), map(a) {
        for (auto& v : list)
            push_back(v);
    }
#endif    
    void assign(size_type count, const value_type& val) {
        map_type tmp;
        size_type i = size_type();
        for (; i<count; ++i)
            tmp.push_back(val);
        swap(tmp);
    }
    template <class InputIt>
    void assign(InputIt first, InputIt last) {
        map_type tmp;
        for (;first != last; ++first)
            tmp.push_back(*first);
        swap(tmp);
    }

    void push_back(const value_type& val) {
        map.insert(std::make_pair(map.size(), val));
    }
#ifndef BOOST_NO_RVALUE_REFERENCES
    void push_back(value_type&& val) {
        map.insert(std::make_pair(map.size(), std::move(val)));
    }
#endif
    void push_front(const value_type& val) {
        map.slide_rightkeys(0, +1);
        try {
            map.insert(std::make_pair(0, val));
        } catch(...) {
            map.slide_rightkeys(0, -1);
            throw;
        }
    }
#ifndef BOOST_NO_RVALUE_REFERENCES
    void push_front(value_type&& val) {
        map.slide_rightkeys(0, +1);
        try {
            map.insert(std::make_pair(0, std::move(val)));
        } catch(...) {
            map.slide_rightkeys(0, -1);
            throw;
        }
    }
#endif
    void pop_back() {
        assert(!empty());
        map.erase(--map.end());
    }
    void pop_front() {
        assert(!empty());
        map.erase(map.begin());
        map.slide_rightkeys(1, -1);
    }
    iterator insert(const_iterator pos, const value_type& val) {
        assert(pos.map == this && pos.index <= size());
        map.slide_rightkeys(pos.index, +1);
        try {
            map.insert(std::make_pair(pos.index, val));
            return iterator(this,pos.index);
        } catch(...) {
            map.slide_rightkeys(pos.index, -1);
            throw;
        }
    }
#ifndef BOOST_NO_RVALUE_REFERENCES
    iterator insert(const_iterator pos, value_type&& val) {
        assert(pos.map == this && pos.index <= size());
        map.slide_rightkeys(pos.index, +1);
        try {
            map.insert(std::make_pair(pos.index, std::move(val)));
            return iterator(this,pos.index);
        } catch(...) {
            map.slide_rightkeys(pos.index, -1);
            throw;
        }
    }
#endif
    
    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        assert(pos.map == this && pos.index <= size());
        size_type r = pos.index;
        try {
            for (; first != last; ++first, ++pos)
                insert(pos, *first);
        } catch (...) {
            erase(iterator(this, r), pos);
            throw;
        }
        return iterator(this, r);
    }
    
#ifndef BOOST_NO_UNIFIED_INITIALIZETION_SYNTAX
    iterator insert(const_iterator pos, std::initializer_list<value_type> list) {
        return insert(pos, list.begin(), list.end());
    }
#endif
    iterator insert(const_iterator pos, size_type count, const value_type& val) {
        assert(pos.map == this && pos.index <= size());
        size_type r = pos.index;
        for (size_type i=0; i < count; ++i, ++pos)
            insert(pos, val);
        return iterator(this, r);
    }
    

    iterator erase(const_iterator pos) {
        assert(pos.map == this && pos.index < size());
        map.erase(pos.index);
        map.slide_rightkeys(pos.index, -1);
        return iterator(this,pos.index);
    }
    iterator erase(const_iterator first, const_iterator last) {
        assert(first.map == this && last.map == this && first.index <= size() && last.index <= size());
        for (size_type i=first.index; i<last.index; ++i)
            map.erase(i);
        map.slide_rightkeys(first.index, first-last);
        return iterator(this,first.index);
    }
    
    anywhere_deque& operator = (const anywhere_deque& rhs) {
        *static_cast<Allocator*>(this) = rhs;
        map = rhs.map;
        return *this;
    }
    
#ifndef BOOST_NO_RVALUE_REFERENCES
    anywhere_deque& operator = (anywhere_deque&& rhs) {
        *static_cast<Allocator*>(this) = std::move(rhs);
        map = std::move(rhs.map);
        return *this;
    }
#endif
    
    reference operator [] (size_type index) {
        assert(index < size());
        return map.at(index);
    }
    const_reference operator [] (size_type index) const {
        assert(index < size());
        return map.at(index);
    }
    reference at(size_type index) {
        return map.at(index);
    }
    const_reference at(size_type index) const {
        return map.at(index);
    }

    reference front() {
        assert(!empty());
        return map.at(0);
    }
    const_reference front() const {
        assert(!empty());
        return map.at(0);
    }
    reference back() {
        assert(!empty());
        return map.at(map.size()-1);
    }
    const_reference back() const {
        assert(!empty());
        return map.at(map.size()-1);
    }
    
    iterator begin() {
        return iterator(this,0);
    }
    const_iterator begin() const {
        return const_iterator(this,0);
    }
    const_iterator cbegin() const {
        return begin();
    }
    iterator end() {
        return iterator(this, size());
    }
    const_iterator end() const {
        return const_iterator(this, size());
    }
    const_iterator cend() const {
        return end();
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const {
        return rbegin();
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(cbegin());
    }
    const_reverse_iterator crend() const {
        return rend();
    }
    
    void clear() {
        map.clear();
    }

    size_type size() const {
        return map.size();
    }
    bool empty() const {
        return map.empty();
    }
    size_type max_size() const {
        return Allocator::max_size();
    }
    allocator_type get_allocator() const {
        return *static_cast<const Allocator*>(this);
    }
    
    void swap(anywhere_deque& other) {
        map.swap(other.map);
    }
    
    friend bool operator == (const anywhere_deque& lhs, const anywhere_deque& rhs) {
        return lhs.map == rhs.map;
    }
    friend bool operator < (const anywhere_deque& lhs, const anywhere_deque& rhs) {
        return lhs.map < rhs.map;
    }

private:
    typedef slidable_map<size_type, difference_type, value_type, Allocator> map_type;
    map_type map;
};

} //namespace gununu

namespace std {

template <class T, class A>
void swap(gununu::anywhere_deque<T,A>& lhs, gununu::anywhere_deque<T,A>& rhs) {
    lhs.swap(rhs);
}

} //namespace std

#endif // ANYWHERE_DEQUE_HPP
