##slidable_map
  
slidable_mapは保持しているKeyを一括して高速(O(log N))に増減が可能なstd::mapライクなコンテナです。  
slidable_map is std::map like C++ container, but this can increase and decrease a lump of keys in O(log N).
  
    template <class Key, class Diff, class Type, class Alloc = std::allocator<std::pair<Key, Type> > >
    class slidable_map
  
Keyはインデックスに使用する型を表し、DiffはKeyの差分を表す型です。  
Key is type of indexing and Diff is type that represent difference of Key.
  
    slidable_map<uint32_t, int64_t, std::string> m =  
        {{0,"a"},{1,"b"},{2,"c"},{3,"d"},{4,"e"},{5,"f"},{6,"g"},{7,"h"},{8,"i"},{9,"j"}};  

Keyとして 0,1,2,3,4,5,6,7,8,9 が格納されています。
ここで `m.slide_rightkeys(3, +10);`
とすると3以降のKeyは+10ずらされて 0,1,2,13,14,15,16,17,18,19となります。  
container has 0,1,2,3,4,5,6,7,8,9 as Key.
if you call 'm.slide_rightkeys(3, +10);' here,
after keys of 3 will slide by +10, and then keys will be 0,1,2,13,14,15,16,17,18,19.
                                                        
例えばchronoのtime_pointとdurationを利用して自由に編集可能なタイムラインを作ることも可能です。
事前に使用する型が例外安全性の要件を満たしているか確認しておいてください。(特にdurationがnothrowであること)  
For example. By using time_point,duration of chrono, can make freely editable timeline.
On ahead, confilm fullfil requirement of exception safety. (especially, duration shall be nothrow.)

    slidable_map<time_point, duration, std::string> timeline;
    timeline.insert({tp("00:00"), "opening"});
    timeline.insert({tp("01:00"), "greeting"});
    timeline.insert({tp("02:00"), "meeting"});
    timeline.insert({tp("06:00"), "closing"});

ここで'meeting'の開始時刻を3分ほど後にずらしたい場合  
here, if you want to delay time of "meeting" by 3 minuites. you can do following:

    timeline.slide_rightkeys(tp("2:00"), dr("+03:00"));

    00:00, "opening"
    01:00, "greeting"
    05:00, "meeting"
    09:00, "closing"

ランダムアクセスと途中への要素の挿入がO(logN)で可能な配列としても利用できます。これをラップしたものが[anywhere_deque](ANYWHERE_DEQUE.md)です。  
you can also serve as array of random accessible and insertable in O(log N). 'anywhere_deque' is wrapping this function.
                                                                            
    slidable_map<unsigned, int64_t, double> m;
    m.insert({{0,"a"},{1,"b"},{2,"c"},{3,"d"},{4,"e"},{5,"f"},{6,"g"},{7,"h"},{8,"i"},{9,"j"}});
    m[6] = "G";
    m.slide_rightkeys(4,+1);
    m.insert({4, "DD"});
とすると  
result as

    {0,"a"},{1,"b"},{2,"c"},{3,"d"},{4,"DD"},{5,"e"},{6,"f"},{7,"G"},{8,"h"},{9,"i"},{10,"j"}

###使い方 (Usage)
`#include "slidable_map.hpp"`  
`namespace gununu`にslidable_mapクラスが定義してあります。  
  
###Keyに必須の要件 (Key requirement)  
    default constractible 且つ そのまま演算可能なこと  
    copy constractible
    Key operator + (const Key& lhs, const Diff& rhs)  
    Key& operator += (const Key& lhs, const Diff& rhs)  
    Diff operator - (const Key& lhs, const Key& rhs)  
    Key& operator -= (const Key& lhs, const Diff& rhs)
    bool operator < (const Key& lhs, const Key& rhs)  
    上記のすべての操作に於いて例外安全がnothrowまたはStrong  
    all above operation shall be nothrow or Strong exception safety.
    destructorがnothrowであること  
###Diffに必須の要件 (Diff requirement)  
    default constractible 且つ そのまま演算可能なこと  
    copy constractible
    copy assignable or move assignable
    bool operator < (Diff lhs, Diff rhs)  
    Diff operator - (Diff lhs, Diff rhs)  
    Diff operator + (Diff lhs, Diff rhs)  
    Diff& operator -= (Diff lhs, Diff rhs)  
    Diff& operator += (Diff lhs, Diff rhs)  
    Diff operator -()  
    上記のすべての操作に於いてnothrow (insertやerase等の操作においてStrongまたはBasicな例外安全を保証するために必要)  
    all above operation shall be nothrow. (in insert, erase, etc... operation needs to have Strong or Basic exceptions safety.)  
    destructorがnothrowであること  
###Typeに必須の要件(Type requirement)  
    slidable_mapの比較関数を使用する場合には operator < 
    if you use slidable_map's comparative operation, must have operator <
    destructorがnothrowであること  
                              
###Keyの制限 (Key restriction)
Keyに利用できる値は Keyのデフォルトコンストラクトした初期値+Diffで表現できる値で尚且つ
slidable_mapに格納される最小値と最大値は双方からDiffで表現できなければなりません。  
value of Key shall be represented by initial value of defaul constructed Key + value of Diff.
and then maximum and minimum value in slidable_map shall be represented each direction by value of Diff.

具体例として `slidable_map<uint8_t, int8_t, element>` は 0から+127までの範囲しか使えません。  
`slidable_map<int8_t, int8_t, element>`は 0と-128の値などを同時に格納できません。  
0から-128の距離は-128でDiffで表現できますが逆からの距離は+128でDiffで表現できないからです。  
For example, `slidable_map<uint8_t, int8_t, element>` is only available range in 0 to +127.
distance of 0 to -128 is -128 that can represented by Diff, but distance from opposite is +128 and this value can't represented by Diff.

###利用可能なiteratorの条件 (requirement of valid iterator)
有効な要素を指し示しているiteratorはそれ以外の要素への変更があっても利用可能なままです。  
end(),cend(),rend(),crend(), 空のコンテナのbegin(),cbegin(),rbegin(),crbeginのiteratorは有効な要素を指していません。  
swap,operator = などで全体の要素が変更された場合iteratorは無効になります。  
iterator pointing to valid element is still avaliable.
if iterator equivalent to end(),cend(),rend(),crend(), empty container's begin(),cbegin(),rbegin(),crbegin, iterator isn't point to valid element.
if you change whole elements by using swap, operator=,etc.. then iterator will be invalid.

###関数リファレンス (Function reference)
  
                
    implimentation-defined-wrapper& iterator::operator * ()  
    implimentation-defined-wrapper* iterator::operator -> ()                 
std::pair<const Key, Type>に近いインターフェイスを提供します。firstとsecondはfirst(),second()のように関数呼び出し形式を使用してください。  
                                                                                                  
    iterator it = ...;  
    (*it).first(); //Keyを取得  
    it->first();  
Complexity:　O(logN)  
Exception safety: Strong  
                  
    (*it).second();//Typeへの参照を取得  
    it->second();  
Complexity: Constant  
Exception safety: nothrow  

    std::pair<const Key, Type> ret = *it;//pairを取得  
    //auto ret2 = *it; // Error: autoは使用できません。  
Complexity: O(logN)  
Exception safety: Strong  


    std::pair<iterator, bool> insert(value_type&& kv)  
    std::pair<iterator, bool> insert(const value_type& kv)  
kvを挿入します。戻り値のfirstは挿入された要素または元々存在していた要素へのiteratorです。
secondは挿入できたことを表すbool値です。falseの場合は既に要素が存在していたことを示します。  
Complexity: O(logN)  
Exception safety: Diffがすべての操作に於いてnothrowならば Strong そうでなければ Unsafe  

    template <class T>
    std::pair<iterator, bool> insert_by(const_iterator hint, const Diff& diff, T&& val)
hintからdiffの距離だけ離れた位置へvalを挿入します。だだしdiffは既存要素の位置より短い距離でなければなりません。insertを使用するよりも高速化が期待できます。  
Complexity: O(logN)  
Exception safety: Diffがすべての操作に於いてnothrowならば Strong そうでなければ Unsafe  


    void slide_rightkeys(const Key& bgn, const Diff& qty)  
bgn以降のKey全てをqtyだけずらします。  
移動した結果として既存のKeyの順序が入れ替わったり同じ値になったりしてはいけません。  
また移動した結果としてKeyが表現できる値を超えないように注意してください。  
Complexity: O(logN)  
Exception safety: Diffがすべての操作に於いてnothrowならば Strong そうでなければ Unsafe  
  
  
    void slide_leftkeys(const Key& bgn, const Diff& qty)
bgn以前のKey全てをqtyだけずらします。  
移動した結果として既存のKeyの順序が入れ替わったり同じ値になったりしてはいけません。  
また移動した結果としてKeyが表現できる値を超えないように注意してください。  
Complexity: O(logN)  
Exception safety: Diffがすべての操作に於いてnothrowならば Strong そうでなければ Unsafe 

    void slide_all(const Diff& qty)
全てのKeyをqtyだけずらします。移動した結果としてKeyが表現できる値を超えないように注意してください。  
Complexity: Constant  
Exception safety: Diffがすべての操作に於いてnothrowならば nothrow そうでなければ Strong  

    void movekey(const_iterator where, Diff qty)  
whereのKeyをqtyだけずらします。  
移動した結果として既存のKeyとの順序が入れ替わったり同じ値になったりしてはいけません。  
また移動した結果としてKeyが表現できる値を超えないように注意してください。  
Complexity: Constant  
Exception safety: Diffがすべての操作に於いてnothrowならば Strong そうでなければ Unsafe  

    iterator lower_bound(Key key)  
    const_iterator lower_bound(Key key) const  
    iterator upper_bound(Key key)  
    const_iterator upper_bound(Key key) const  
Complexity: O(logN)  
Exception safety: Strong  

    iterator rlower_bound(Key key)  
    const_iterator rlower_bound(Key key) const  
key以下の一番近い要素へのiteratorを返します。  
Complexity: O(logN)  
Exception safety: Strong  

    iterator rupper_bound(Key key)  
    const_iterator rupper_bound(Key key) const  
key未満の一番近い要素へのiteratorを返します。  
Complexity: O(logN)  
Exception safety: Strong  

    std::pair<iterator, Key> lower_bound2(Key key)  
    std::pair<const_iterator, Key> lower_bound2(Key key) const  
key以上の一番近い要素へのiteratorとそのKeyを返します。  
Complexity: O(logN)  
Exception safety: Strong  

    std::pair<iterator, Key> rlower_bound2(Key key)  
    std::pair<const_iterator, Key> rlower_bound2(Key key) const  
key以下の一番近い要素へのiteratorとそのKeyを返します。  
Complexity: O(logN)  
Exception safety: Strong  

####std::map互換の関数
  
    slidable_map(void)  
    explicit slidable_map(const Alloc&)
Complexity: Constant  
Exception safety: depend on Allocator  

    slidable_map(const slidable_map& rhs)  
    slidable_map(const slidable_map& rhs, const Alloc&)
Complexity: N  
Exception safety: Strong  
      
    slidable_map(std::initializer_list<value_type> list)  
    
    template <class InputItr>
    slidable_map(InputItr first, InputItr second)
Complexity: O(distance(first,second)* log (N+distance(first,second)))  
Exception safety: Diffがすべての操作に於いてnothrowならば Strong そうでなければ Unsafe  
      
    ~slidable_map(void)  
Complexity: N  
Exception safety: nothrow  
  
    slidable_map(slidable_map&& rhs)  
    slidable_map(slidable_map&& rhs, const Alloc&)
    slidable_map& operator = (slidable_map&& rhs)  
Complexity: N  
Exception safety: Strong  

    template <class InputItr>
    void insert(InputItr first, InputItr second)
Complexity: O(distance(first,second)* log (N+distance(first,second)))  
Exception safety: Diffがすべての操作に於いてnothrowならば Basic そうでなければ Unsafe  
      
    void insert(std::initializer_list<value_type> list)  
Complexity: O(list.size()* log (N+list.size()))  
Exception safety: Diffがすべての操作に於いてnothrowならば Basic そうでなければ Unsafe  

    Type& operator [] (const Key& key)  
Complexity: O(log N)  
Exception safety: Diffがすべての操作に於いてnothrowならば Strong そうでなければ Unsafe  

    Type& at(const Key& key)  
    const Type& at(const Key& key) const  
Complexity: O(log N)  
Exception safety: Strong  
  
    slidable_map& operator = (const slidable_map& rhs)  
Complexity: N  
Exception safety: Strong  
  
    size_type erase(key_type key)  
    iterator erase(const_iterator where)  
Complexity: O(log N)  
Exception safety: Diffがすべての操作に於いてnothrowならば Strong そうでなければ Unsafe  

    iterator erase(const_iterator first, const_iterator last)  
Complexity: O(distance(first,last) * log N)  
Exception safety: Diffがすべての操作に於いてnothrowならば Basic そうでなければ Unsafe  

    void clear()  
Complexity: N  
Exception safety: nothrow  

    iterator        begin()  
    const_iterator	begin() const  
    const_iterator  cbegin() const  
    iterator        end()  
    const_iterator	end() const  
    const_iterator	cend() const  
    reverse_iterator        rbegin()  
    const_reverse_iterator	rbegin() const  
    const_reverse_iterator  crbegin() const  
    reverse_iterator        rend()  
    const_reverse_iterator	rend() const  
    const_reverse_iterator	crend() const  
Complexity: Constant  
Exception safety: nothrow  

    iterator find(const Key& key)  
    const_iterator find(const Key& key) const  
    std::pair<iterator, iterator> equal_range(const key_type& keyval)  
    std::pair<const_iterator, const_iterator> equal_range(const key_type& keyval) const  
    size_type count(const key_type& keyval) const  
Complexity: O(log N)  
Exception safety: Strong  
  
    void swap(slidable_map& rhs)  
    friend void swap(slidable_map& left, slidable_map& right)  
Complexity: Constant or N (depend on Allocator::operator == )  
Exception safety: Strong  

    bool empty() const  
    size_type size() const  
Complexity: Constant  
Exception safety: nothrow  

    size_type max_size() const  
    Alloc get_allocator() const  
Exception safety: Strong  

    friend bool operator == (const slidable_map& lhs, const slidable_map& rhs)  
    friend bool operator != (const slidable_map& lhs, const slidable_map& rhs)  
    friend bool operator < (const slidable_map& lhs, const slidable_map& rhs)  
    friend bool operator > (const slidable_map& lhs, const slidable_map& rhs)  
    friend bool operator <= (const slidable_map& lhs, const slidable_map& rhs)  
    friend bool operator >= (const slidable_map& lhs, const slidable_map& rhs)  
Complexity: O(N)  
Exception safety: Strong  


  

