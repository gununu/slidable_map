
#ifndef SLIDABLE_MAP_
#define SLIDABLE_MAP_

#include <algorithm>
#include <functional>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <boost/config.hpp>

namespace gununu {

namespace detail {
//for exception-safty
template <class T, size_t N>
class stack_pod_vector {
public:
    stack_pod_vector():num(0){}
    void push_back(T val) {
        assert(num < N);
        array[num++] = val;
    }
    void pop_back() {
        --num;
    }
    T& back() {
        assert(!empty());
        return array[num-1];
    }
    bool empty() const {
        return (num == 0);
    }
private:
    T array[N];
    size_t num;
};
}

template <class Key, class Diff, class Type, class Alloc = std::allocator<std::pair<const Key, Type> > >
class slidable_map
{
friend class const_iterator;
friend class iterator;
struct node;

public:
typedef Key key_type;
typedef Type mapped_type;
typedef std::pair<const Key, Type> value_type;
typedef Alloc allocator_type;
typedef typename Alloc::size_type size_type;
//typedef value_type& reference;
//typedef const value_type& const_reference;
//typedef typename Alloc::pointer pointer;
//typedef typename Alloc::const_pointer const_pointer;

class iterator_base : public std::iterator<std::bidirectional_iterator_tag, value_type >
{
protected:
    iterator_base() {
        wp.pnode = NULL;
        wp.container = NULL;
    }
    iterator_base(typename slidable_map::node* ptr, const slidable_map* container) {
        wp.pnode = ptr;
        wp.container = container;
    }

protected:
    struct wrapper_base {
        slidable_map::node* pnode;
        const slidable_map* container;
    };

    struct wrapper : public wrapper_base {
    private:
        /* NOTE: can not use 'auto' type */
        wrapper(const wrapper&);// = delete;
        wrapper& operator = (const wrapper&);// = delete;
    public:
        const Key first() const {
            assert(this->container && this->pnode);
            return slidable_map::getabkey(this->pnode);
        }
        
        Type& second() {
            assert(this->container && this->pnode);
            return this->pnode->val;
        }
        const Type& second() const {
            assert(this->container && this->pnode);
            return this->pnode->val;
        }

        operator std::pair<const Key, Type>() const
        {
            return std::make_pair(slidable_map::getabkey(this->pnode), this->pnode->val);
        }
    };
public:
    const wrapper& operator * ()  {
        return static_cast<wrapper&>(wp);
    }
    const wrapper* operator -> () {
        return static_cast<wrapper*>(&wp);
    }
protected:    
    void inc() {
        if (!wp.pnode) {
            wp.pnode = wp.container->leftmost;
        } else {
            wp.pnode = next(wp.pnode);
        }
    }
    void dec() {
        if (!wp.pnode) {
            wp.pnode = wp.container->rightmost;
        } else {
            wp.pnode = previous(wp.pnode);
        }
    }
    wrapper_base wp;
};
class const_iterator : public iterator_base
{
    friend class slidable_map;
protected:
    const_iterator(typename slidable_map::node* ptr, const slidable_map* container) : iterator_base(ptr, container) {}
public:
    const_iterator(): iterator_base(){}

    const_iterator& operator ++ ()
    {
        this->inc();
        return *this;
    }
    const_iterator& operator -- ()
    {
        this->dec();
        return *this;
    }
    const_iterator operator ++ (int)
    {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
    const_iterator operator -- (int)
    {
        const_iterator tmp = *this;
        --*this;
        return tmp;
    }
    friend bool operator == (const const_iterator& lhs, const const_iterator& rhs)
    {
        assert(lhs.wp.container == rhs.wp.container);
        return (lhs.wp.pnode == rhs.wp.pnode);
    }
    friend bool operator != (const const_iterator& lhs, const const_iterator& rhs)
    {
        return (!(lhs == rhs));
    }
};

class iterator : public const_iterator
{
    friend class slidable_map;
protected:
    iterator(typename slidable_map::node* ptr, const slidable_map* container) : const_iterator(ptr, container) {}
public:
    iterator(): const_iterator(){}

    typename iterator_base::wrapper& operator * ()  {
        return static_cast<typename iterator_base::wrapper&>(this->wp);
    }
    typename iterator_base::wrapper* operator -> () {
        return static_cast<typename iterator_base::wrapper*>(&(this->wp));
    }
    iterator& operator ++ ()
    {
        this->inc();
        return *this;
    }
    iterator& operator -- ()
    {
        this->dec();
        return *this;
    }
    iterator operator ++ (int)
    {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
    iterator operator -- (int)
    {
        iterator tmp = *this;
        --*this;
        return tmp;
    }
};

class const_reverse_iterator : public iterator_base
{
    friend class slidable_map;
protected:
    const_reverse_iterator(typename slidable_map::node* ptr, const slidable_map* container) : iterator_base(ptr, container) {}
public:
    const_reverse_iterator(): iterator_base(){}

    const_iterator base() {
        return ++const_iterator(this->wp.pnode, this->wp.container);
    }

    const_reverse_iterator& operator ++ ()
    {
        this->dec();
        return *this;
    }
    const_reverse_iterator& operator -- ()
    {
        this->inc();
        return *this;
    }
    const_reverse_iterator operator ++ (int)
    {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
    const_reverse_iterator operator -- (int)
    {
        const_iterator tmp = *this;
        --*this;
        return tmp;
    }
    friend bool operator == (const const_reverse_iterator& lhs, const const_reverse_iterator& rhs)
    {
        assert(lhs.wp.container == rhs.wp.container);
        return (lhs.wp.pnode == rhs.wp.pnode);
    }
    friend bool operator != (const const_reverse_iterator& lhs, const const_reverse_iterator& rhs)
    {
        return (!(lhs == rhs));
    }
};
class reverse_iterator : public const_reverse_iterator
{
    friend class slidable_map;
protected:
    reverse_iterator(typename slidable_map::node* ptr, const slidable_map* container) : const_reverse_iterator(ptr, container) {}
public:
    reverse_iterator(): const_reverse_iterator(){}

    iterator base() {
        return ++iterator(this->wp.pnode, this->wp.container);
    }

    typename iterator_base::wrapper& operator * ()  {
        return static_cast<typename iterator_base::wrapper&>(this->wp);
    }
    typename iterator_base::wrapper* operator -> () {
        return static_cast<typename iterator_base::wrapper*>(&(this->wp));
    }
    reverse_iterator& operator ++ ()
    {
        this->dec();
        return *this;
    }
    reverse_iterator& operator -- ()
    {
        this->inc();
        return *this;
    }
    reverse_iterator operator ++ (int)
    {
        reverse_iterator tmp = *this;
        ++*this;
        return tmp;
    }
    reverse_iterator operator -- (int)
    {
        reverse_iterator tmp = *this;
        --*this;
        return tmp;
    }
};

public:
    slidable_map(void) : root(NULL), rightmost(NULL), leftmost(NULL), mysize(0) {}
    slidable_map(const slidable_map& rhs) { *this = rhs; }
    ~slidable_map(void) { clear(); }

#ifndef BOOST_NO_RVALUE_REFERENCES
    slidable_map(slidable_map&& rhs) : root(NULL), rightmost(NULL), leftmost(NULL), mysize(0) { swap(rhs); }
    slidable_map& operator = (slidable_map&& rhs) {
        assert(this != &rhs);
        swap(rhs);
        rhs.clear();
        return *this;
    }

    template <class P>
    std::pair<iterator, bool> insert(P&& kv)
    {
        auto ret =insertnode(kv.first, std::move(kv.second));
        return std::pair<iterator, bool>(iterator(ret.first, this), ret.second);
    }
#endif
    
#ifndef BOOST_NO_UNIFIED_INITIALIZETION_SYNTAX
    void insert(std::initializer_list<value_type> list) {
        for (auto& p : list) {
            insert(p);
        }
    }
#endif

    std::pair<iterator, bool> insert(const value_type& kv)
    {
        std::pair<node*,bool> ret = insertnode(kv.first, kv.second);
        return std::pair<iterator, bool>(iterator(ret.first, this), ret.second);
    }

//#ifndef BOOST_NO_RVALUE_REFERENCES
//    template <class T>
//    std::pair<iterator, bool> insert_by(const_iterator where, const Key& whereidx, const Key& key, T&& val)
//#else
//    std::pair<iterator, bool> insert_by(const_iterator where, const Key& whereidx, const Key& key, const Type& val)
//#endif
//    {
//        assert(where.wp.pnode && where.wp.container == this);
//        assert(whereidx < key);
//        //assert(!(where->first() < whereidx || whereidx < where->first()));

//        node* self = where.wp.pnode;
//        Diff pos = key-whereidx;
//        if (self->right) {
//            self = self->right;
//            while(true) {
//                pos -= self->key;
//                if (!self->left)
//                    break;
//                self = self->left;
//            }
//        } 

//        node* child = alnod.allocate(1);
//        try {
//#ifndef BOOST_NO_RVALUE_REFERENCES
//            new ((void*)child) node(self, NULL, NULL, Red, pos, std::forward<T>(val));
//#else
//            new ((void*)child) node(self, NULL, NULL, Red, pos, val);
//#endif
//        } catch (...) {
//            alnod.deallocate(child, 1);
//            throw;
//        }

//        if (pos < Diff())
//            self->left = child;
//        else
//            self->right = child;
        
//        if (ISRED(self)) {
//            insert_balance(child);
//        }
//        assert(ISBLACK(root));
        
//        ++mysize;
//        return std::pair<iterator, bool>(iterator(child, this), true);
//    }

    Type& operator [] (const Key& key)
    {
        return insertnode(key, Type()).first->val;
    }

    const Type& at(const Key& key) const {
        return const_cast<slidable_map*>(this)->at(key);
    }
    Type& at(const Key& key) {
        if (node* p = findnode(key)) {
            return p->val;
        } else {
            throw std::out_of_range("");
        }
    }
    
    slidable_map& operator = (const slidable_map& rhs)
    {
        if (this != &rhs) {
            root = copynodes(NULL, rhs.root);
            leftmost = getleftmost(root);
            rightmost = getrightmost(root);
            mysize = rhs.mysize;
        }
        return *this;
    }

    iterator erase(const_iterator where)
    {
        assert (where.wp.pnode && where.wp.container == this);
        iterator f(where.wp.pnode, where.wp.container);
        ++f;
        erasenode(where.wp.pnode);
        return f;
    }

    size_type erase(const Key& key)
    {
        node* tmp = findnode(key);
        if (!tmp)
            return 0;
        erasenode(tmp);
        return 1;
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        assert(first.wp.container == this && last.wp.container == this);

        if (first == begin() && last == end()) {
            clear();
        } else {
            while(first != last) {
                first = erase(first);
            }
        }
        return iterator(last.wp.pnode, last.wp.container);
    }
    
    void clear()
    {
        recursive_erase(root);
        root = leftmost = rightmost = NULL;
        mysize = 0;
    }

    void slide_rightkeys(const Key& bgn, const Diff& qty)
    {
        node* p = root;
        Diff rlbgn = bgn - Key();
        while(1) {
            if (!p) return;
            while(p->key < rlbgn) {
                rlbgn -= p->key;
                p = p->right;
                if (!p) return;    
            }
            rlbgn -= p->key;
            p->key += qty;
            p = p->left;
            if (!p) return;

            while(!(p->key < rlbgn)) {
                rlbgn -= p->key;
                p = p->left;
                if (!p) return;    
            }
            rlbgn -= p->key;
            p->key -= qty;
            p = p->right;
        }
    }
    
    void slide_leftkeys(const Key& bgn, const Diff& qty)
    {
        node* p = root;
        Diff rlbgn = bgn - Key();
        while(1) {
            if (!p) return;
            while(rlbgn < p->key) {
                rlbgn -= p->key;
                p = p->left;
                if (!p) return;    
            }
            rlbgn -= p->key;
            p->key += qty;
            p = p->right;
            if (!p) return;

            while(!(rlbgn < p->key)) {
                rlbgn -= p->key;
                p = p->right;
                if (!p) return;    
            }
            rlbgn -= p->key;
            p->key -= qty;
            p = p->left;
        }
    }
    
    iterator         begin() { return iterator(leftmost, this); }
    const_iterator   begin() const { return const_cast<slidable_map*>(this)->begin();    }
    const_iterator  cbegin() const { return begin(); }
    iterator         end() { return iterator(NULL, this); }
    const_iterator   end() const { return const_cast<slidable_map*>(this)->end(); }
    const_iterator  cend() const { return end(); }
    reverse_iterator         rbegin() { return reverse_iterator(rightmost, this); }
    const_reverse_iterator   rbegin() const { return const_cast<slidable_map*>(this)->rbegin(); }
    const_reverse_iterator  crbegin() const { return rbegin(); }
    reverse_iterator         rend() { return reverse_iterator(NULL, this); }
    const_reverse_iterator   rend() const { return const_cast<slidable_map*>(this)->rend(); }
    const_reverse_iterator  crend() const { return rend(); }

    iterator        find(const Key& key) { return iterator(findnode(key), this); }
    const_iterator  find(const Key& key) const { return const_iterator(findnode(key), this); }

    size_type count(const key_type& key) const { return (find(key) != end()) ? 1 : 0; }

    iterator lower_bound(const Key& key)
    {
        node* p = root;
        if (!p)
            return iterator(NULL, this);

        Diff rlkey = key - Key();
        while(1) {
            if (p->key < rlkey) {
                if (!p->right) {
                    return iterator(next(p), this);
                }
                rlkey -= p->key;
                p = p->right;
            } else if (rlkey < p->key) {
                if (!p->left) {
                    return iterator(p, this);
                }
                rlkey -= p->key;
                p = p->left;
            } else {
                return iterator(p, this);
            }
        }
    }
    const_iterator lower_bound(const Key& key) const
    {
        return const_cast<slidable_map*>(this)->lower_bound(key);
    }

    iterator upper_bound(const Key& key)
    {
        node* p = root;
        if (!p)
            return iterator(NULL, this);

        Diff rlkey = key - Key();
        while(1) {
            if (p->key < rlkey) {
                if (!p->right) {
                    return iterator(next(p), this);
                }
                rlkey -= p->key;
                p = p->right;
            } else if (rlkey < p->key) {
                if (!p->left) {
                    return iterator(p, this);
                }
                rlkey -= p->key;
                p = p->left;
            } else {
                return iterator(next(p), this);
            }
        }
    }
    const_iterator upper_bound(const Key& key) const
    {
        return const_cast<slidable_map*>(this)->upper_bound(key);
    }

    iterator rlower_bound(const Key& key)
    {
        node* p = root;
        node* leftnode = NULL;
        if (!p) 
            return iterator(NULL, this);
        
        Diff rlkey = key - Key();
        while(true) {
            if (p->key < rlkey) {
                if (!p->right) {
                    return iterator(p, this);
                }
                leftnode = p;
                rlkey -= p->key;
                p = p->right;
            } else if (rlkey < p->key) {
                if (!p->left) {
                    //return iterator(previous(p));
                    return iterator(leftnode, this);
                }
                rlkey -= p->key;
                p = p->left;
            } else {
                return iterator(p, this);
            }
        }
    }
    const_iterator rlower_bound(const Key& key) const
    {
        return const_cast<slidable_map*>(this)->rlower_bound(key);
    }
    
    iterator rupper_bound(const Key& key)
    {
        node* p = root;
        
        if (!p) {
            return iterator(NULL, this);
        }
        Diff rlkey = key - Key();
        while(1) {
            if (p->key < rlkey) {
                if (!p->right) {
                    return iterator(p, this);
                }
                rlkey -= p->key;
                p = p->right;
            } else if (rlkey < p->key) {
                if (!p->left) {
                    return iterator(previous(p), this);
                }
                rlkey -= p->key;
                p = p->left;
            } else {
                return iterator(previous(p), this);
            }
        }
    }
    const_iterator rupper_bound(const Key& key) const
    {
        return const_cast<slidable_map*>(this)->rupper_bound(key);
    }

    std::pair<iterator, Key> lower_bound2(const Key& key)
    {
        typedef std::pair<iterator, Key> rettype;
        node* p = root;
        if (!p)
            return rettype(iterator(NULL, this), Key());

        const Key& orgkey = key;
        Diff rlkey = key - Key();
        while(1) {
            if (p->key < rlkey) {
                rlkey -= p->key;
                if (!p->right) {
                    node* old;
                    do {
                        old = p;
                        rlkey += p->key;
                        p = Parent(p);
                    } while(p && p->right == old);
                    return rettype(iterator(p, this), orgkey-rlkey);
                }
                p = p->right;
            } else if (rlkey < p->key) {
                rlkey -= p->key;
                if (!p->left) {
                    return rettype(iterator(p, this), orgkey-rlkey);
                }
                p = p->left;
            } else {
                return rettype(iterator(p, this), orgkey);
            }
        }
    }
    std::pair<const_iterator, Key> lower_bound2(const Key& key) const
    {
        std::pair<iterator, Key> ret = const_cast<slidable_map*>(this)->lower_bound2(key);
        return std::pair<const_iterator, Key>(ret.first, ret.second);
    }
    std::pair<iterator, Key> rlower_bound2(const Key& key)
    {
        typedef std::pair<iterator, Key> rettype;
        node* p = root;
        if (!p)
            return rettype(iterator(NULL, this), Key());

        const Key& orgkey = key;
        Diff rlkey = key - Key();
        while(true) {
            if (p->key < rlkey) {
                rlkey -= p->key;
                if (!p->right) {
                    return rettype(iterator(p, this), orgkey-rlkey);
                }
                p = p->right;
            } else if (rlkey < p->key) {
                rlkey -= p->key;
                if (!p->left) {
                    node* old;
                    do {
                        old = p;
                        rlkey += p->key;
                        p = Parent(p);
                    } while(p && p->left == old);
                    return rettype(iterator(p, this), orgkey-rlkey);
                }
                p = p->left;
            } else {
                return rettype(iterator(p, this), orgkey);
            }
        }
    }

    std::pair<const_iterator, Key> rlower_bound2(const Key& key) const
    {
        std::pair<iterator, Key> ret = const_cast<slidable_map*>(this)->rlower_bound2(key);
        return std::pair<const_iterator, Key>(ret.first, ret.second);
    }
    
    void swap(slidable_map& rhs)
    {
        if (alval == rhs.alval) {
            std::swap(this->root, rhs.root);
            std::swap(this->rightmost, rhs.rightmost);
            std::swap(this->leftmost, rhs.leftmost);
        } else {
            node *copied_for_rhs,
                 *copied_for_lhs;

            copied_for_rhs = rhs.copynodes(NULL, this->root);
            try {
                copied_for_lhs  = this->copynodes(NULL, rhs.root);
            } catch (...) {
                rhs.recursive_erase(copied_for_rhs);
                throw;
            }

            this->recursive_erase(this->root);
            rhs.recursive_erase(rhs.root);

            rhs.root = copied_for_rhs;
            this->root = copied_for_lhs;
            rhs.leftmost = getleftmost(copied_for_rhs);
            rhs.rightmost = getrightmost(copied_for_rhs);
            this->leftmost = getleftmost(copied_for_lhs);
            this->rightmost = getrightmost(copied_for_lhs);
        }
        std::swap(this->mysize, rhs.mysize);
    }

    bool        empty() const { return (root == NULL); }    
    friend void swap(slidable_map& left, slidable_map& right) { left.swap(right);    }
    size_type   size() const { return mysize;    }
    size_type   max_size() const { return alval.max_size(); }
    Alloc       get_allocator() const { return alval; }

    friend bool operator == (const slidable_map& lhs, const slidable_map& rhs) {
        if (lhs.size() != rhs.size())
            return false;

        if (lhs.empty())
            return true;
        const_iterator p = lhs.begin(), e = lhs.end();
        const_iterator q = rhs.begin();
        Key pk = p->first();
        Key qk = q->first();
        for (; p != e; ) {
            if (pk < qk || qk < pk || p->second() < q->second() || q->second() < p->second())
                return false;
            p = lhs.next(p, pk);
            q = rhs.next(q, qk);
        }
        return true;
    }
    friend bool operator != (const slidable_map& lhs, const slidable_map& rhs) {
        return !(lhs == rhs);
    }
    friend bool operator < (const slidable_map& lhs, const slidable_map& rhs) {
        if (lhs.empty() || rhs.empty())
            return false;
        const_iterator p = lhs.begin(), e = lhs.end();
        const_iterator q = rhs.begin(), f = rhs.end();
        Key pk = p->first();
        Key qk = q->first();
        for (; (p != e) && (q != f); ) {
            if (pk < qk)
                return true;
            if (qk < pk)
                return false;
            if (p->second() < q->second())
                return true;
            if (q->second() < p->second())
                return false;
            p = lhs.next(p, pk);
            q = rhs.next(q, qk);
        }
        return (p == e) && (q != f);
    }
    friend bool operator > (const slidable_map& lhs, const slidable_map& rhs) {
        return (rhs < lhs);
    }
    friend bool operator >= (const slidable_map& lhs, const slidable_map& rhs) {
        return !(lhs < rhs);
    }
    friend bool operator <= (const slidable_map& lhs, const slidable_map& rhs) {
        return !(lhs > rhs);
    }

    std::pair<iterator, iterator> equal_range(const key_type& key)
    {
        iterator first = find(key);
        iterator last = first;
        if (first != end())
            ++last;
        return std::pair<iterator, iterator>(first, last);
    }

    void movekey(const_iterator where, const Diff& qty)
    {
        assert(where.wp.pnode && where.wp.container == this);
        node* node = where.wp.pnode;
        node->key += qty;
        if (node->right) node->right->key -= qty;
        if (node->left) node->left->key -= qty;
    }
    
private:
    static node* Parent(const node* p)  { return p->parent; }
    static void SetParent(node* target, node* newparent)  { target->parent = newparent; }

    bool ISLEFT     (const node* n) const   { return (n == Parent(n)->left); }
    bool ISSAMESIDE (const node* n1, const node* n2) const { return (ISLEFT(n1) == ISLEFT(n2)); }
    node* GETSIBLING(const node* n) const   { return ((ISLEFT(n)) ? Parent(n)->right : Parent(n)->left); }
    bool ISBLACK    (const node* p) const   { return p->col == Black; }
    bool ISRED      (const node* p) const   { return !ISBLACK(p); }
    bool SAFE_ISRED (const node* p) const   { return p && ISRED(p); }
    bool SAFE_ISBLACK(const node* p) const  { return !SAFE_ISRED(p); }
    bool ISNIL      (const node* p) const   { return !p; }

    static node* getleftmost(node* p)
    {
        if (p)
            while(p->left) p = p->left;
        return p;
    }
    static node* getrightmost(node* p)
    {
        if (p)
            while(p->right) p = p->right;
        return p;
    }

    static inline node* previous(node* base)
    {
        assert(base);

        node* p;
        if (base->left) {
            p = base->left;
            while (p->right) p = p->right;
        } else {
            node* old = base;
            p = Parent(base);
            while(p && p->left == old) {
                old = p;
                p = Parent(p);
            }
        }
        return p;
    }

    static inline node* next(node* base)
    {
        assert (base);

        node* p;
        if (base->right) {
            p = base->right;
            while (p->left) p = p->left;
        } else {
            node* old = base;
            p = Parent(base);
            while(p && p->right == old) {
                old = p;
                p = Parent(p);
            }
        }
        return p;
    }
    static inline node* next(node* base, Key& nextkey)
    {
        assert (base);

        node* p;
        if (base->right) {
            p = base->right;
            nextkey += p->key;
            while (p->left) {
                p = p->left;
                nextkey += p->key;
            }
        } else {
            node* old = base;
            p = Parent(base);
            nextkey += -base->key;
            while(p && p->right == old) {
                old = p;
                nextkey += -p->key;
                p = Parent(p);
            }
        }
        return p;
    }
    static inline iterator next(const_iterator base, Key& nextkey)
    {
        return iterator(next(base.wp.pnode, nextkey), base.wp.container);
    }

    static inline Key getabkey(const node* base)
    {
        Key ret = Key();
        do {
            ret += base->key;
            base = Parent(base);
        } while(base);

        return ret;
    }

    void recursive_erase(node* target)
    {
        node* p = target;
        if (!p) 
            return;

        detail::stack_pod_vector<node*, 64*2> childstack;
        while(true) {
            node* leftchild = p->left;

            if (p->right)
                childstack.push_back(p->right);

            alnod.destroy(p);
            alnod.deallocate(p, 1);

            if (leftchild) {    
                p = leftchild;
            } else {
                if (childstack.empty())
                    break;
                p = childstack.back();
                childstack.pop_back();
            }
        }
    }

    node* copynodes(node* parent, const node* org)
    {
        if (!org)
            return NULL;
        detail::stack_pod_vector<std::pair<const node*,node*>, 64*2> child;
        node* const top = alnod.allocate(1);
        node* p = top;
        try {
            while(true) {
                try {
                    new ((void*)p) node (*org);
                    p->left = p->right = NULL;
                } catch (...) {
                    alnod.deallocate(p, 1);
                    throw;
                }
    
                if (Parent(org)) {
                    if (ISLEFT(org))
                        link2left(parent, p);
                    else
                        link2right(parent, p);
                } else {
                    SetParent(p, NULL);
                }
                
                if (org->right)
                    child.push_back(std::make_pair(org->right, p));
                if (org->left) {
                    org  = org->left;
                    parent = p;
                } else {
                    if (child.empty())
                        break;
                    org = child.back().first;
                    parent = child.back().second;
                    child.pop_back();
                }
                p = alnod.allocate(1);
            }
        } catch (...) {
            recursive_erase(top);
            throw;
        }

        return top;
    }

    void swaplink_with_rightchild(node* p)
    {
        node* rc = p->right;

        node* pp = Parent(p);
        if (pp) {
            //if (ISLEFT(p))
            if (p == pp->left)
                pp->left = rc;
            else
                pp->right = rc;
        } else {
            root = rc;
        }

        SetParent(p->left, rc);

        node* ar = rc->right;
        if (rc->right)
            SetParent(rc->right, p);

        SetParent(rc, pp);
        rc->right = p;
        SetParent(p, rc);
        p->right = ar;

        rc->left = p->left;
        p->left = NULL;
    }

    void swaplink_with_right_single_child(node* p)
    {
        node* rc = p->right;
        node* pp = Parent(p);
        if (pp) {
            //if (ISLEFT(p))
            if (p == pp->left)
                pp->left = rc;
            else
                pp->right = rc;
        } else {
            root = rc;
        }

        SetParent(rc, pp);
        rc->right = p;
        SetParent(p, rc);
    }

    void swaplink_with_left_single_child(node* p)
    {
        node* lc = p->left;
        node* pp = Parent(p);
        if (pp) {
            //if (ISLEFT(p))
            if (p == pp->left)
                pp->left = lc;
            else
                pp->right = lc;
        } else {
            root = lc;
        }

        SetParent(lc, pp);
        lc->left = p;
        SetParent(p, lc);
    }

    // Precondition: descendant is right side of ancestor
    void swaplink_with_farnode(node* ancestor, node* descendant)
    {
        assert(ancestor->right != descendant && ancestor->left != descendant);
        const bool leftb = true;

        node* ap = Parent(ancestor);
        if (ap) {
            //if (ISLEFT(ancestor))
            if (ancestor == ap->left)
                ap->left = descendant;
            else
                ap->right = descendant;
        } else {
            root = descendant;
        }

        SetParent(ancestor->left, descendant);
        SetParent(ancestor->right, descendant);

        node* dp = Parent(descendant);
        if (leftb)
            dp->left = ancestor;
        else
            dp->right = ancestor;

        if (descendant->right)
            SetParent(descendant->right, ancestor);

        SetParent(ancestor, dp);
        SetParent(descendant, ap);

        std::swap(ancestor->right, descendant->right);

        descendant->left = ancestor->left;
        ancestor->left = NULL;
    }

    void swap2endleaf(node* const p)
    {
        assert(p);

        if (p->left) {
            if (p->right) {
                node* descendant = p->right;
                Diff diff = descendant->key;
                
                while(descendant->left) {
                    descendant = descendant->left;
                    diff += descendant->key;
                }
                             
                Diff nearkey = descendant->key;
                bool bchildswap = (p->right == descendant);
                if (bchildswap) {    //child
                    swaplink_with_rightchild(p);// descendant > p
                    
                    descendant->key = p->key + diff;
                    p->key = -diff;
                } else {            //grandchild and far
                    swaplink_with_farnode(p, descendant);// descendant > p
                    
                    descendant->key = p->key + diff;
                    descendant->right->key -= diff;
                    p->key = nearkey-diff;
                }
                descendant->left->key -= diff;
                
                if (node* bottom = p->right) {
                    assert(ISBLACK(descendant));
                    assert(ISRED(bottom));
                    descendant->col = p->col;
                    // descendant > bottom > p
                    swaplink_with_right_single_child(p);

                    bottom->col = Black;
                    p->col = Red;

                    //bottom->key += p->key + diff;
                    if (!bchildswap)
                        bottom->key += nearkey;
                } else {
                    std::swap(descendant->col, p->col);
                }
            } else {        //p is black and child is red
                assert(ISBLACK(p));
                assert(ISRED(p->left));
                node* child = p->left;
                swaplink_with_left_single_child(p);
                child->col = Black;
                p->col = Red;

                child->key += p->key;
            }
        } else {
            if (p->right) { //p is black and child is red
                assert(ISBLACK(p));
                assert(ISRED(p->right));
                node* child = p->right;
                swaplink_with_right_single_child(p);

                child->col = Black;
                p->col = Red;

                child->key += p->key;
            }
        }
    }

    void erase_balance(node* enode)
    {
        node* eparent = Parent(enode);
        assert(eparent);

        while(true) {
            node* sibling;
            //sibling = GETSIBLING(enode);
            //bool bleft = ISLEFT(enode);
            const bool bleft = (enode == eparent->left);
            sibling = bleft ? eparent->right : eparent->left;
            assert(sibling);
            if (ISRED(sibling)) {
                eparent->col = Red;
                sibling->col = Black;
                
                if (bleft/*ISLEFT(enode)*/) {
                    rotate_left(eparent);
                } else {
                    rotate_right(eparent);
                }
                continue;
            } else if (SAFE_ISBLACK(sibling->left) && SAFE_ISBLACK(sibling->right)) {
                if (ISBLACK(eparent)) {
                    sibling->col = Red;
                    node* egrandparent = Parent(eparent);
                    if (!egrandparent)
                        break;
                    enode = eparent;
                    eparent = egrandparent;
                    continue;
                } else {
                    eparent->col = Black;
                    sibling->col = Red;
                }
            } else {
                if (bleft/*!ISLEFT(sibling)*/ == SAFE_ISRED(sibling->left)) {
                    sibling->col = Red;

                    if (bleft/*ISLEFT(enode)*/) {
                        sibling->left->col = Black;
                        sibling = rotate_right(sibling);
                    } else {
                        sibling->right->col = Black;
                        sibling = rotate_left(sibling);
                    }
                }

                sibling->col = eparent->col;
                eparent->col = Black;

                if (bleft/*ISLEFT(enode)*/) {
                    sibling->right->col = Black;
                    rotate_left(eparent);
                } else {
                    sibling->left->col = Black;
                    rotate_right(eparent);
                }
            }
            break;
        }
    }
    
    void insert_balance(node* rednode)
    {
        node* rp = Parent(rednode);
        node* grandparent;
        bool rpisleft;
        node* uncle;
        while(true) {
            //node* uncle = GETSIBLING(rp);
            grandparent = Parent(rp);
            rpisleft = (rp == grandparent->left);
            uncle = (rpisleft ? grandparent->right : grandparent->left);

            if (!uncle || ISBLACK(uncle)) {
                grandparent->col = Red;
                //if (ISSAMESIDE(rednode, rp)) {
                if ((rednode == rp->left) == rpisleft) {
                    rp->col = Black;
                    if(rpisleft) {
                        rotate_right(grandparent);
                    } else {
                        rotate_left(grandparent);
                    }
                } else {
                    rednode->col = Black;
                    if (rpisleft) {
                        rotate_left(rp);
                        rotate_right(grandparent);
                    } else {
                        rotate_right(rp);
                        rotate_left(grandparent);
                    }
                }
            } else {
                rp->col    = Black;
                uncle->col = Black;

                node* great_grandparent = Parent(grandparent);
                if (great_grandparent) {
                    grandparent->col = Red;
                    if (ISRED(great_grandparent)) {
                        rednode = grandparent;
                        rp = great_grandparent;
                        continue;
                    }
                }
            }
            break;
        }
    }
        
    void link2left(node* target, node* left)
    {
        assert(target && left);
        target->left = left;
        SetParent(left, target);
    }

    void link2right(node* target, node* right)
    {
        assert(target && right);
        target->right = right;
        SetParent(right, target);
    }

    void link2nullleft(node* target)
    {
        assert(target);
        target->left = NULL;
    }

    void link2nullright(node* target)
    {
        assert(target);
        target->right = NULL;
    }

    node* rotate_left(node* base)
    {
        assert(base);
        node *ntmp = base->right;
        assert(ntmp);
        
        node* bp = Parent(base);
        if (bp) {
            if (ISLEFT(base))
                link2left(bp, ntmp);
            else
                link2right(bp, ntmp);
        } else {
            SetParent(ntmp, NULL);
            root = ntmp;
        }

        Diff tmpkey = ntmp->key;
        ntmp->key += base->key;
        base->key = -tmpkey;
        if (ntmp->left) {
            node* ntmpleft = ntmp->left;
            ntmpleft->key += tmpkey;
            link2right(base, ntmpleft);
        } else {
            link2nullright(base);
        }
        link2left(ntmp, base);
        return ntmp;
    }

    node* rotate_right(node* base)
    {
        assert(base);
        node *ntmp = base->left;
        assert(ntmp);
        node* bp = Parent(base);
        if (bp) {
            if (ISLEFT(base))
                link2left(bp, ntmp);
            else
                link2right(bp, ntmp);
        } else {
            SetParent(ntmp, NULL);
            root = ntmp;
        }
        Diff tmpkey = ntmp->key;
        ntmp->key += base->key;
        base->key = -tmpkey;
        if (ntmp->right) {
            node* ntmpright = ntmp->right;
            ntmpright->key += tmpkey;

            link2left(base, ntmpright);
        } else {
            link2nullleft(base);
        }
        link2right(ntmp, base);
        return ntmp;
    }

    std::pair<node*,bool> getinsertnode(const Diff& key, node* begin, Diff& pos) const
    {
        node* p = begin;
        Diff diff = key;
        assert(p);
        while(true) {
            if (p->key < diff) {
                diff -= p->key;
                if (!p->right)
                    break;
                p = p->right;
            } else if (diff < p->key){
                diff -= p->key;
                if (!p->left)
                    break;
                p = p->left;
            } else {
                return std::make_pair(p, false);
            }
        }
#ifndef BOOST_NO_RVALUE_REFERENCES
        pos = std::move(diff);
#else
        pos = diff;
#endif
        return std::make_pair(p, true);
    }
    
#ifndef BOOST_NO_RVALUE_REFERENCES
    template <class T>
    std::pair<node*,bool> insertnode(const Key& key, T&& value)
#else
    std::pair<node*,bool> insertnode(const Key& key, const Type& value)
#endif
    {
        if (!root) {
            node* tmp = alnod.allocate(1);
            try {
#ifndef BOOST_NO_RVALUE_REFERENCES
                new ((void*)tmp) node(NULL, NULL, NULL, Black, key-Key(), std::forward<T>(value));
#else
                new ((void*)tmp) node(NULL, NULL, NULL, Black, key-Key(), value);
#endif
            } catch (...) {
                alnod.deallocate(tmp, 1);
                throw;
            }
            root = leftmost = rightmost = tmp;            
            ++mysize;
            return std::make_pair(root, true);
        } else {
            Diff pos;
            std::pair<node*,bool> ret = getinsertnode(key-Key(), root, pos);
            if (!ret.second) 
                return ret;

            node* self = ret.first;
            node* child = alnod.allocate(1);
            try {
#ifndef BOOST_NO_RVALUE_REFERENCES
                new ((void*)child) node(self, NULL, NULL, Red, pos, std::forward<T>(value));
#else
                new ((void*)child) node(self, NULL, NULL, Red, pos, value);
#endif
            } catch (...) {
                alnod.deallocate(child, 1);
                throw;
            }
            
            if (Diff() < pos) {
                self->right = child;
                if (self == rightmost)
                    rightmost = child;
            } else {
                self->left = child;
                if (self == leftmost)
                    leftmost = child;
            }

            if (ISRED(self)) {
                insert_balance(child);
            }
            assert(ISBLACK(root));

            ++mysize;
            return std::make_pair(child, true);
        }
    }

    void erasenode(node* target)
    {
        assert(target);
        assert(mysize > 0);
        if (mysize == 1) {
            assert(target == root);
            assert(target == leftmost);
            assert(target == rightmost);
            root = leftmost = rightmost = NULL;
        } else {
            if (target == leftmost) {
                //leftmost = next(target);
                leftmost = (target->right) ? target->right : target->parent;
            } else if (target == rightmost) {
                //rightmost = previous(target);
                rightmost = (target->left) ? target->left : target->parent;
            }
            swap2endleaf(target);
            if (ISBLACK(target)) {
                erase_balance(target);
            }   
        
            node* tp = Parent(target);
            if (ISLEFT(target)) {
                tp->left = NULL;
            } else {
                tp->right = NULL;
            }
            assert(!next(rightmost));
            assert(!previous(leftmost));
        }
        alnod.destroy(target);
        alnod.deallocate(target, 1);
        --mysize;

        assert(SAFE_ISBLACK(root));
    }

    inline node* findnode(const Key& key) const
    {
        node* p = root;
        Diff rlkey = key - Key();
        while(p) {
            if (p->key < rlkey) {
                rlkey -= p->key;
                p = p->right;
            } else if (rlkey < p->key) {
                rlkey -= p->key;
                p = p->left;
            } else {
                return p;
            }
        }
        return NULL;
    }
    
public:
    bool check_structure() const
    {
        if (mysize == 0) {
            return (root == NULL && leftmost == NULL && rightmost == NULL);
        } else {
            if (root == NULL)
                return false;
            if (root->parent)
                return false;
            if (leftmost == NULL || rightmost == NULL)
                return false;
            if (previous(leftmost))
                return false;
            if (next(rightmost))
                return false;
            if (!ISBLACK(root))
                return false;
            
            size_t count = 1;
            size_t depthmax = 1;
            size_t depthmin = std::numeric_limits<size_t>::max();
            if (!check_structure_sub(root->left, root, count, 1, depthmax, depthmin))
                return false;
            if (!check_structure_sub(root->right, root, count, 1, depthmax, depthmin))
                return false;
            if (count != mysize)
                return false;
            if (depthmax > depthmin*2)
                return false;
            
            return true;
        }
    }
protected:
     bool check_structure_sub(const node* p, const node* parent, size_t& count, size_t depth, size_t& depthmax, size_t& depthmin) const {
         if (!p) {
             if (depth < depthmin)
                 depthmin = depth;
             if (depth > depthmax)
                 depthmax = depth;
             return true;
         }
         if (ISRED(parent) && ISRED(p))
             return false;
         if (p->parent != parent)
             return false;
         count++;
         if (!check_structure_sub(p->left, p, count, depth +1, depthmax, depthmin))
             return false;
         if (!check_structure_sub(p->right, p, count, depth +1, depthmax, depthmin))
             return false;
         return true;
     }
        
private:
    typedef unsigned char color;

    static const color Black = 0;
    static const color Red = 1;

    struct node {
        node(node* p, node* l, node* r, color c, const Diff& k, const Type& t)
            :left(l), right(r), parent(p), col(c), key(k), val(t){}
#ifndef BOOST_NO_RVALUE_REFERENCES
        node(node* p, node* l, node* r, color c, const Diff& k, Type&& t)
            :left(l), right(r), parent(p), col(c), key(k), val(std::move(t)){}
#endif
        node(const node& rhs) : col(rhs.col), key(rhs.key), val(rhs.val) {}

        node* left;
        node* right;
        node* parent;
        color col;

        Diff key;
        Type val;
    };

    node* root;
    node* rightmost;
    node* leftmost;
    size_type mysize;

    typename Alloc::template rebind<node>::other alnod;
    typename Alloc::template rebind<value_type>::other alval;

};

} //namespace

namespace std {

template <class K, class D, class T, class A>
void swap(gununu::slidable_map<K,D,T,A>& lhs, gununu::slidable_map<K,D,T,A>& rhs) {
    lhs.swap(rhs);
}

} //namespace std

#endif /* SLIDABLE_MAP_ */
