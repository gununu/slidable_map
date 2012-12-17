## anywhere_deque
anywhere_dequeはどの位置にもO(logN)で挿入が可能でstd::deque互換のインターフェイスを備えた配列です。  

push_back, push_front, insert, erase, ランダムアクセスなどの操作はO(logN)で完了します。  
全てのメンバ関数についてnothrowまたはstrongな例外安全性を提供します。  

### Usage
`namespace gununu`にanywhere_dequeが定義されています。
std::dequeとほとんど同じように使用できます。

    #include "anywhere_deque.hpp"
    using namespace gununu;
    void test() {
        anywhere_deque<int> ad{0,1,2,3,4,5,6};
        ad.insert(ad.begin()+2, 7);
        std::cout << ad[2] << std::endl;
    }

### 利用可能なiteratorの条件
要素の挿入や削除を行うとそれより後方のイテレータは無効になります。
swapやoperator=の操作でも無効になります。

### Performance
std::vector&lt;int&gt;とanywhere_deque&lt;int&gt;でランダム挿入テストを行うと1000要素を超えたあたりからanywhere_dequeの方が高速になるようです。  

### Member 
    explicit anywhere_deque(const Allocator& a = Allocator())
    anywhere_deque(const anywhere_deque& r)
    anywhere_deque(const anywhere_deque& r, const Allocator& a)
    anywhere_deque(anywhere_deque&& r)
    anywhere_deque(anywhere_deque&& r, const Allocator& a)
    anywhere_deque(size_type count, const value_type& val, const Allocator& a = Allocator())
    explicit anywhere_deque(size_type count)
    template <class InputIt>
    anywhere_deque(InputIt first, InputIt last, const Allocator& a = Allocator())
    anywhere_deque(std::initializer_list<T> list, const Allocator& a = Allocator())
    void assign(size_type count, const value_type& val) 
    template <class InputIt>
    void assign(InputIt first, InputIt last)
-

    void push_back(const value_type& val)
    void push_back(value_type&& val)
    void push_front(const value_type& val)
    void push_front(value_type&& val)
Complexity: O(logN)  
Exception Safety: Strong  

    void pop_back()
    void pop_front()
Complexity: O(logN)  
Exception Safety: Nothrow  

    iterator insert(const_iterator pos, const value_type& val)
    iterator insert(const_iterator pos, value_type&& val)
Complexity: O(logN)  
Exception Safety: Strong  

    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    iterator insert(const_iterator pos, std::initializer_list<value_type> list)
    iterator insert(const_iterator pos, size_type count, const value_type& val)
Complexity: O(logN) * (last-first or list.size() or count)  
Exception Safety: Strong  

    iterator erase(const_iterator pos) 
Complexity: O(logN)  
Exception Safety: Nothrow  

    iterator erase(const_iterator first, const_iterator last)
    anywhere_deque& operator = (const anywhere_deque& rhs)
    anywhere_deque& operator = (anywhere_deque&& rhs)
-

    reference operator [] (size_type index)
    const_reference operator [] (size_type index) const
Complexity: O(logN)  
Exception Safety: Nothrow  

    reference at(size_type index)
    const_reference at(size_type index) const
Complexity: O(logN)  
Exception Safety: Strong  

    reference front()
    const_reference front() const
    reference back()
    const_reference back() const
Complexity: Constant  
Exception Safety: Nothrow  

    iterator begin()
    const_iterator begin() const
    const_iterator cbegin() const
    iterator end()
    const_iterator end() const
    const_iterator cend() const 
    reverse_iterator rbegin() 
    const_reverse_iterator rbegin() const
    const_reverse_iterator crbegin() const
    reverse_iterator rend()
    const_reverse_iterator rend() const
    const_reverse_iterator crend() const
Complexity: Constant  
Exception Safety: Nothrow  

    void clear()
Complexity: O(N)  
Exception Safety: Nothrow  

    size_type size() const
    bool empty() const
Complexity: Constant  
Exception Safety: Nothrow  

    size_type max_size() const
    allocator_type get_allocator() const
    void swap(anywhere_deque& other) 
    friend bool operator == (const anywhere_deque& lhs, const anywhere_deque& rhs)
    friend bool operator != (const anywhere_deque& lhs, const anywhere_deque& rhs)
    friend bool operator < (const anywhere_deque& lhs, const anywhere_deque& rhs)
    friend bool operator >= (const anywhere_deque& lhs, const anywhere_deque& rhs)
    friend bool operator > (const anywhere_deque& lhs, const anywhere_deque& rhs)
    friend bool operator <= (const anywhere_deque& lhs, const anywhere_deque& rhs)
    void std::swap(anywhere_deque& lhs, anywhere_deque& rhs)
