#include <iostream>
#include <vector>
#include <chrono>
#include <boost/random.hpp>
#include "anywhere_deque.hpp"
using namespace std;
using namespace gununu;

#ifndef GUNUNU_CHECK
#define GUNUNU_CHECK(ex) do {if (!(ex)) {cout << "check fail at: " << __func__ << " file: \"" << __FILE__ << "\" line:" << __LINE__ << " error:" << #ex << endl; abort();} } while(false)
#endif

void ad_interface() {
    typedef anywhere_deque<int> que;
    que q,p{0,1,2,3};
    que r(1);
    que s(1u,0);
    que t(q);
    que u = std::move(t);
    u.clear();
    int ary[3] = {0,1,2};
    que w(&ary[0],&ary[3]);
    
    q.clear();
    q.size();
    q.max_size();
    q.get_allocator();
    q.push_back(1);
    q.push_front(2);
    q.erase(q.begin()+1);
    q.insert(q.begin()+0, 4);
    int val=5;
    q.insert(q.end(), val);
    q.insert(q.begin(), 1u, 0);
    q.insert(q.end(), begin(ary), end(ary));
    q.insert(q.begin()+3, {1,2,3});
    q.erase(q.begin()+0);
    q.erase(q.begin(), q.begin()+1);
    q.empty();
    q[1];
    q.at(0);
    q.front();
    q.back();
    
    //que::iterator x = que::const_iterator();
    que::iterator x = q.begin(),y = q.end();
    x = y;
    x == y;
    x != y;
    ++x;
    x++;
    x + 1;
    x += 2;
    x -= 1;
    x - 1;
    x - y;
    que::const_iterator a = q.begin();
    int v = *a;
    *q.begin() = 1;
    *q.cbegin();
    q.end();
    q.cend();
    que::const_reverse_iterator b = q.rbegin();
    q.crbegin();
    q.rend();
    q.crend();
    q.pop_front();
    q.pop_back();
    q.swap(p);
    std::swap(q,p);
    q = p;
    q = anywhere_deque<int>();
    q = std::move(u);
    q == p;
    q != p;
    q < p;
    q > p;
    q <= p;
    q >= p;
}

void ad_random_insertion(boost::random::mt19937& mt) {
    anywhere_deque<int> q;
    vector<int> v;
    v.reserve(10000);
    
    boost::random::uniform_int_distribution<> rn;
    for (int i=0; i<10000; ++i) {
        boost::random::uniform_int_distribution<> ud(0, q.size());
        int n = ud(mt);
        int e = rn(mt);
        q.insert(q.begin()+n, e);
        v.insert(v.begin()+n, e);
    }
    GUNUNU_CHECK(q.size() == v.size());
    GUNUNU_CHECK(std::equal(q.begin(),q.end(),v.begin()));
    GUNUNU_CHECK(q.front() == v.front());
    GUNUNU_CHECK(q.back() == v.back());
    GUNUNU_CHECK(std::equal(q.rbegin(),q.rend(),v.rbegin()));
    
    anywhere_deque<int> s;
    s = q;
    GUNUNU_CHECK(s == q);
    
    GUNUNU_CHECK(!q.empty());
    q.clear();
    GUNUNU_CHECK(q.empty());
}

void ad_random_insert_range(boost::random::mt19937& mt) {
    anywhere_deque<int> q;
    vector<int> v;
    v.reserve(10000);
    
    boost::random::uniform_int_distribution<> rn;
    for (int i=0; i<10000; ++i) {
        boost::random::uniform_int_distribution<> ud(0, q.size());
        int n = ud(mt);
        int e = rn(mt);
        std::vector<int> tmp;
        for (int loop = boost::random::uniform_int_distribution<>(0, 5)(mt); loop > 0; --loop)
            tmp.push_back(e + loop);
        q.insert(q.begin()+n, tmp.begin(), tmp.end());
        v.insert(v.begin()+n, tmp.begin(), tmp.end());
    }
    GUNUNU_CHECK(q.size() == v.size());
    GUNUNU_CHECK(std::equal(q.begin(),q.end(),v.begin()));
    GUNUNU_CHECK(q.front() == v.front());
    GUNUNU_CHECK(q.back() == v.back());
    GUNUNU_CHECK(std::equal(q.rbegin(),q.rend(),v.rbegin()));
    
    anywhere_deque<int> s;
    s = q;
    GUNUNU_CHECK(s == q);
    
    GUNUNU_CHECK(!q.empty());
    q.clear();
    GUNUNU_CHECK(q.empty());
}

void ad_random_erase(boost::random::mt19937& mt) {
    anywhere_deque<int> q;
    vector<int> v;
    v.reserve(20000);
    
    for (int i=0; i<20000; ++i) {
        q.push_back(i);
        v.push_back(i);
    }
    
    for (int i=0; i<10000; ++i) {
        boost::random::uniform_int_distribution<> ud(0, q.size()-1);
        int n = ud(mt);
        q.erase(q.begin()+n);
        v.erase(v.begin()+n);
    }
    GUNUNU_CHECK(q.size() == v.size());
    GUNUNU_CHECK(std::equal(q.begin(),q.end(),v.begin()));
}

void ad_push_front() {
    anywhere_deque<int> q;
    std::vector<int> rv;
    rv.reserve(10000);
    
    for (int i=0; i< 10000; ++i) {
        q.push_front(i);
        rv.push_back(i);
    }
    GUNUNU_CHECK(std::equal(rv.rbegin(),rv.rend(),q.begin()));
    GUNUNU_CHECK(q.front() == rv.back());
    GUNUNU_CHECK(q.back() == rv.front());
}

void ad_push_back() {
    anywhere_deque<int> q;
    std::vector<int> v;
    v.reserve(10000);
    
    for (int i=0; i< 10000; ++i) {
        q.push_back(i);
        v.push_back(i);
    }
    GUNUNU_CHECK(std::equal(v.begin(),v.end(),q.begin()));
    GUNUNU_CHECK(q.front() == v.front());
    GUNUNU_CHECK(q.back() == v.back());
    for (size_t i=0; i < v.size(); ++i) {
        GUNUNU_CHECK(v[i] == q[i]);
    }
}

void ad_swap(boost::random::mt19937& mt) {
    anywhere_deque<int> q, s;

    for (int i=0; i<10000; ++i) {
        boost::random::uniform_int_distribution<> ud(0, q.size());
        int n = ud(mt);
        q.insert(q.begin()+n, n);
    }
    for (int i=0; i<5001; ++i) {
        boost::random::uniform_int_distribution<> ud(0, s.size());
        int n = ud(mt);
        s.insert(s.begin()+n, n);
    }
    
    vector<int> t(q.begin(), q.end());
    vector<int> u(s.begin(), s.end());
    swap(q,s);
    GUNUNU_CHECK(t.size() == s.size());
    GUNUNU_CHECK(u.size() == q.size());
    GUNUNU_CHECK(std::equal(t.begin(),t.end(),s.begin()));
    GUNUNU_CHECK(std::equal(u.begin(),u.end(),q.begin()));
}

void ad_pop_back(boost::random::mt19937& mt) {
    anywhere_deque<int> q;

    for (int i=0; i< 10000; ++i) {
        boost::random::uniform_int_distribution<> ud(0, q.size());
        int n = ud(mt);
        q.insert(q.begin()+n, n);
    }
    std::vector<int> v(q.begin(), q.end());
    
    while (!v.empty()) {
        GUNUNU_CHECK(q.back() == v.back());
        q.pop_back();
        v.pop_back();
    }
    GUNUNU_CHECK(q.empty());
}

void ad_pop_front(boost::random::mt19937& mt) {
    anywhere_deque<int> q;

    for (int i=0; i< 10000; ++i) {
        boost::random::uniform_int_distribution<> ud(0, q.size());
        int n = ud(mt);
        q.insert(q.begin()+n, n);
    }
    std::vector<int> v(q.rbegin(), q.rend());
    
    while (!v.empty()) {
        GUNUNU_CHECK(q.front() == v.back());
        q.pop_front();
        v.pop_back();
    }
    GUNUNU_CHECK(q.empty());
}

void ad_random_erase_range(boost::random::mt19937& mt) {
    anywhere_deque<int> q;
    vector<int> v;
    v.reserve(20000);
    
    for (int i=0; i<20000; ++i) {
        q.push_back(i);
        v.push_back(i);
    }
    
    for (int i=0; i<5000; ++i) {
        if(v.empty())
            break;
        boost::random::uniform_int_distribution<> ud(0, q.size()-1);
        int n = ud(mt);
        boost::random::uniform_int_distribution<> r(0, 5);
        int m = r(mt);
        auto a = q.begin() + n;
        m = (std::min)(q.end()-a, m);
        auto b = v.begin() + n;
        int o = (std::min)(v.end()-b, m);
        q.erase(a, a + m);
        v.erase(b, b + o);
    }
    GUNUNU_CHECK(q.size() == v.size());
    GUNUNU_CHECK(std::equal(q.begin(),q.end(),v.begin()));
}

#ifndef GUNUNU_TEST 
int main()
#else
int test_anywhere_deque()
#endif

{
    cout << "testing: test_anywhere_deque\n";
    boost::random::mt19937 mt;
    mt.seed(std::chrono::system_clock::now().time_since_epoch().count());
    ad_interface();
    ad_random_insertion(mt);
    ad_random_insert_range(mt);
    ad_random_erase(mt);
    ad_random_erase_range(mt);
    ad_push_front();
    ad_push_back();
    ad_swap(mt);
    ad_pop_back(mt);
    ad_pop_front(mt);
    cout << "passed: test_anywhere_deque\n";
    return 0;
}
