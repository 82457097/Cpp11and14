# Cpp11and14
C++11/14新特性

### nullptr
### constexpr
### auto
- 自动类型推导关键字
- eg：
 - 未使用auto   for(vector<int>::const_iterator itr = vec.cbegin(); itr != vec.cend(); ++itr)
 - 使用auto    for(auto itr = vec.cbegin(); itr != vec.cend(); ++itr);
 - auto i = 5;             // i 被推导为 int
 - auto arr = new auto(10) // arr 被推导为 int *

```cpp
#include <iostream>

int main() {
 auto i = 5;

 int arr[10] = {0};
 auto auto_arr = arr;
 auto auto_arr2[10] = arr;

 return 0;
}
```

## 面对对象增强
- 委托构造
C++ 11 引入了委托构造的概念，这使得构造函数可以在同一个类中一个构造函数调用另一个构造函数，从而达到简化代码的目的：

- 继承构造
在传统 C++ 中，构造函数如果需要继承是需要将参数一一传递的，这将导致效率低下。C++ 11 利用关键字 using 引入了继承构造函数的概念：

## 模板增强
### 类型别名模板
- 在了解类型别名模板之前，需要理解『模板』和『类型』之间的不同。仔细体会这句话：模板是用来产生类型的。在传统 C++中，typedef 可以为类型定义一个新的名称，但是却没有办法为模板定义一个新的名称。因为，模板不是类型。
- 例如：
```cpp
template< typename T, typename U, int value>
class SuckType {
public:
    T a;
    U b;
    SuckType():a(value),b(value){}
};
template< typename U>
typedef SuckType<std::vector<int>, U, 1> NewType; // 不合法
```
- C++ 11 使用 using 引入了下面这种形式的写法，并且同时支持对传统 typedef 相同的功效：
```cpp
typedef int (*process)(void *);  // 定义了一个返回类型为 int，参数为 void* 的函数指针类型，名字叫做 process
using process = int(*)(void *); // 同上, 更加直观

template <typename T>
using NewType = SuckType<int, T, 1>;    // 合法
```
### 变长模板参数
- 模板一直是 C++ 所独有的黑魔法（一起念：Dark Magic）之一。在 C++11 之前，无论是类模板还是函数模板，都只能按其指定的样子，接受一组固定数量的模板参数；而 C++11 加入了新的表示方法，允许任意个数、任意类别的模板参数，同时也不需要在定义时将参数的个数固定。
```cpp
template<typename... Ts> class Magic;
```
模板类 Magic 的对象，能够接受不受限制个数的 typename 作为模板的形式参数，例如下面的定义：
```cpp
class Magic<int, 
            std::vector<int>, 
            std::map<std::string, 
                     std::vector<int>>> darkMagic;
```
既然是任意形式，所以个数为 0 的模板参数也是可以的：
```cpp
class Magic<> nothing;
```
如果不希望产生的模板参数个数为 0，可以手动的定义至少一个模板参数
```cpp
template<typename Require, typename... Args> class Magic;
```
变长参数模板也能被直接调整到到模板函数上。传统 C 中的 printf 函数，虽然也能达成不定个数的形参的调用，但其并非类别安全。而 C++11 除了能定义类别安全的变长参数函数外，还可以使类似 printf 的函数能自然地处理非自带类别的对象。除了在模板参数中能使用 ... 表示不定长模板参数外，函数参数也使用同样的表示法代表不定长参数，这也就为我们简单编写变长参数函数提供了便捷的手段，例如：
```cpp
template<typename... Args> void printf(const std::string &str, Args... args);
```
我们可以使用 sizeof... 来计算参数的个数，：
```cpp
template<typename... Args>
void magic(Args... args) {
    std::cout << sizeof...(args) << std::endl;
}

magic();        // 输出0
magic(1);       // 输出1
magic(1, "");   // 输出2
```

其次，对参数进行解包，到目前为止还没有一种简单的方法能够处理参数包，但有两种经典的处理手法：

1. 递归模板函数

递归是非常容易想到的一种手段，也是最经典的处理方法。这种方法不断递归的向函数传递模板参数，进而达到递归遍历所有模板参数的目的：
```cpp
#include <iostream>
template<typename T>
void printf(T value) {
    std::cout << value << std::endl;
}
template<typename T, typename... Args>
void printf(T value, Args... args) {
    std::cout << value << std::endl;
    printf(args...);
}
int main() {
    printf(1, 2, "123", 1.1);
    return 0;
}
```
2. 初始化列表展开
```cpp
// 编译这个代码需要开启 -std=c++14
// 因为版本原因，实验环境中的 g++ 尚不支持此特性，此处可以使用 clang++ 替代 g++
template<typename T, typename... Args>
auto print(T value, Args... args) {
    std::cout << value << std::endl;
    return std::initializer_list<T>{([&] {
        std::cout << args << std::endl;
    }(), value)...};
}
int main() {
    print(1, 2.1, "123");
    return 0;
}
```
通过初始化列表，(lambda 表达式, value)... 将会被展开。由于逗号表达式的出现，首先会执行前面的 lambda 表达式，完成参数的输出。唯一不美观的地方在于如果不使用 return 编译器会给出未使用的变量作为警告。


## lambda表达式
```cpp
[捕获列表](参数列表) mutable(可选) 异常属性 -> 返回类型 {
    // 函数体
}
```
1. 值捕获

与参数传值类似，值捕获的前期是变量可以拷贝，不同之处则在于，被捕获的变量在 lambda 表达式被创建时拷贝，而非调用时才拷贝：
```cpp
void learn_lambda_func_1() {
    int value_1 = 1;
    auto copy_value_1 = [value_1] {
        return value_1;
    };
    value_1 = 100;
    auto stored_value_1 = copy_value_1();
    // 这时, stored_value_1 == 1, 而 value_1 == 100.
    // 因为 copy_value_1 在创建时就保存了一份 value_1 的拷贝
    cout << "value_1 = " << value_1 << endl;
    cout << "stored_value_1 = " << stored_value_1 << endl;
}
```

2. 引用捕获

与引用传参类似，引用捕获保存的是引用，值会发生变化。
```cpp
void learn_lambda_func_2() {
    int value_2 = 1;
    auto copy_value_2 = [&value_2] {
        return value_2;
    };
    value_2 = 100;
    auto stored_value_2 = copy_value_2();
    // 这时, stored_value_2 == 100, value_1 == 100.
    // 因为 copy_value_2 保存的是引用
    cout << "value_2 = " << value_2 << endl;
    cout << "stored_value_2 = " << stored_value_2 << endl;
}
```
3. 隐式捕获

手动书写捕获列表有时候是非常复杂的，这种机械性的工作可以交给编译器来处理，这时候可以在捕获列表中写一个 & 或 = 向编译器声明采用 引用捕获或者值捕获.

总结一下，捕获提供了 Lambda 表达式对外部值进行使用的功能，捕获列表的最常用的四种形式可以是：

[] 空捕获列表
[name1, name2, ...] 捕获一系列变量
[&] 引用捕获, 让编译器自行推导捕获列表
[=] 值捕获, 让编译器执行推导应用列表


- std::function

Lambda 表达式的本质是一个函数对象，当 Lambda 表达式的捕获列表为空时，Lambda 表达式还能够作为一个函数指针进行传递，例如：
```cpp
#include <iostream>

using foo = void(int);  // 定义函数指针, using 的使用见上一节中的别名语法
void functional(foo f) {
    f(1);
}

int main() {
    auto f = [](int value) {
        std::cout << value << std::endl;
    };
    functional(f);  // 函数指针调用
    f(1);           // lambda 表达式调用
    return 0;
}
```
## 右值引用
右值引用是 C++11 引入的与 Lambda 表达式齐名的重要特性之一。它的引入解决了 C++ 中大量的历史遗留问题，消除了诸如 std::vector、std::string 之类的额外开销，也才使得函数对象容器 std::function 成为了可能。

（1）左值、右值的纯右值、将亡值、右值

要弄明白右值引用到底是怎么一回事，必须要对左值和右值做一个明确的理解。

左值(lvalue, left value)，顾名思义就是赋值符号左边的值。准确来说，左值是表达式（不一定是赋值表达式）后依然存在的持久对象。

右值(rvalue, right value)，右边的值，是指表达式结束后就不再存在的临时对象。

而 C++11 中为了引入强大的右值引用，将右值的概念进行了进一步的划分，分为：纯右值、将亡值。

纯右值(prvalue, pure rvalue)，纯粹的右值，要么是纯粹的字面量，例如 10, true；要么是求值结果相当于字面量或匿名临时对象，例如 1+2。非引用返回的临时变量、运算表达式产生的临时变量、原始字面量、Lambda 表达式都属于纯右值。

将亡值(xvalue, expiring value)，是 C++11 为了引入右值引用而提出的概念（因此在传统 C++中，纯右值和右值是统一个概念），也就是即将被销毁、却能够被移动的值。

将亡值可能稍有些难以理解，我们来看这样的代码：
```cpp
std::vector<int> foo() {
    std::vector<int> temp = {1, 2, 3, 4};
    return temp;
}

std::vector<int> v = foo();
```
在这样的代码中，函数 foo 的返回值 temp 在内部创建然后被赋值给 v，然而 v 获得这个对象时，会将整个 temp 拷贝一份，然后把 temp 销毁，如果这个 temp 非常大，这将造成大量额外的开销（这也就是传统 C++ 一直被诟病的问题）。在最后一行中，v 是左值、foo() 返回的值就是右值（也是纯右值）。

但是，v 可以被别的变量捕获到，而 foo() 产生的那个返回值作为一个临时值，一旦被 v 复制后，将立即被销毁，无法获取、也不能修改。

将亡值就定义了这样一种行为：临时的值能够被识别、同时又能够被移动。

（2）右值引用和左值引用

需要拿到一个将亡值，就需要用到右值引用的申明：T &&，其中 T 是类型。右值引用的声明让这个临时值的生命周期得以延长、只要变量还活着，那么将亡值将继续存活。

C++11 提供了 std::move 这个方法将左值参数无条件的转换为右值，有了它我们就能够方便的获得一个右值临时对象，例如：
```cpp
#include <iostream>
#include <string>

void reference(std::string& str) {
    std::cout << "左值" << std::endl;
}
void reference(std::string&& str) {
    std::cout << "右值" << std::endl;
}

int main()
{
    std::string  lv1 = "string,";       // lv1 是一个左值
    // std::string&& r1 = s1;           // 非法, s1 在全局上下文中没有声明
    std::string&& rv1 = std::move(lv1); // 合法, std::move 可以将左值转移为右值
    std::cout << "rv1 = " << rv1 << std::endl;      // string,

    const std::string& lv2 = lv1 + lv1; // 合法, 常量左值引用能够延长临时变量的生命周期
    // lv2 += "Test";                   // 非法, 引用的右值无法被修改
    std::cout << "lv2 = "<<lv2 << std::endl;      // string,string

    std::string&& rv2 = lv1 + lv2;      // 合法, 右值引用延长临时对象的生命周期
    rv2 += "string";                    // 合法, 非常量引用能够修改临时变量
    std::cout << "rv2 = " << rv2 << std::endl;      // string,string,string,

    reference(rv2);                     // 输出左值
}
```
注意：rv2 虽然引用了一个右值，但由于它是一个引用，所以 rv2 依然是一个左值。

（3）移动语义

传统 C++ 通过拷贝构造函数和赋值操作符为类对象设计了拷贝/复制的概念，但为了实现对资源的移动操作，调用者必须使用先复制、再析构的方式，否则就需要自己实现移动对象的接口。试想，搬家的时候是把家里的东西直接搬到新家去，而不是将所有东西复制一份（重买）再放到新家、再把原来的东西全部销毁，这是非常反人类的一件事情。

传统的 C++ 没有区分『移动』和『拷贝』的概念，造成了大量的数据移动，浪费时间和空间。右值引用的出现恰好就解决了这两个概念的混淆问题，例如：
```cpp
#include <iostream>
class A {
public:
    int *pointer;
    A() :pointer(new int(1)) { 
        std::cout << "构造" << pointer << std::endl; 
    }
    // 无意义的对象拷贝
    A(A& a) :pointer(new int(*a.pointer)) { 
        std::cout << "拷贝" << pointer << std::endl; 
    }    

    A(A&& a) :pointer(a.pointer) { 
        a.pointer = nullptr; 
        std::cout << "移动" << pointer << std::endl; 
    }

    ~A() { 
        std::cout << "析构" << pointer << std::endl; 
        delete pointer; 
    }
};
// 防止编译器优化
A return_rvalue(bool test) {
    A a,b;
    if(test) return a;
    else return b;
}
int main() {
    A obj = return_rvalue(false);
    std::cout << "obj:" << std::endl;
    std::cout << obj.pointer << std::endl;
    std::cout << *obj.pointer << std::endl;

    return 0;
}
```
在上面的代码中：

首先会在 return_rvalue 内部构造两个 A 对象，于是获得两个构造函数的输出；
函数返回后，产生一个将亡值，被 A 的移动构造（A(A&&)）引用，从而延长生命周期，并将这个右值中的指针拿到，保存到了 obj 中，而将亡值的指针被设置为 nullptr，防止了这块内存区域被销毁。
从而避免了无意义的拷贝构造，加强了性能。再来看看涉及标准库的例子：
```cpp
#include <iostream> // std::cout
#include <utility>  // std::move
#include <vector>   // std::vector
#include <string>   // std::string

int main() {

    std::string str = "Hello world.";
    std::vector<std::string> v;

    // 将使用 push_back(const T&), 即产生拷贝行为
    v.push_back(str);
    // 将输出 "str: Hello world."
    std::cout << "str: " << str << std::endl;

    // 将使用 push_back(const T&&), 不会出现拷贝行为
    // 而整个字符串会被移动到 vector 中，所以有时候 std::move 会用来减少拷贝出现的开销
    // 这步操作后, str 中的值会变为空
    v.push_back(std::move(str));
    // 将输出 "str: "
    std::cout << "str: " << str << std::endl;

    return 0;
}
```
（4）完美转发

前面我们提到了，一个声明的右值引用其实是一个左值。这就为我们进行参数转发（传递）造成了问题：
```cpp
void reference(int& v) {
    std::cout << "左值" << std::endl;
}
void reference(int&& v) {
    std::cout << "右值" << std::endl;
}
template <typename T>
void pass(T&& v) {
    std::cout << "普通传参:";
    reference(v);   // 始终调用 reference(int& )
}
int main() {
    std::cout << "传递右值:" << std::endl;
    pass(1);        // 1是右值, 但输出左值

    std::cout << "传递左值:" << std::endl;    
    int v = 1;
    pass(v);        // v是左引用, 输出左值

    return 0;
}
```
对于 pass(1) 来说，虽然传递的是右值，但由于 v 是一个引用，所以同时也是左值。因此 reference(v) 会调用 reference(int&)，输出『左值』。而对于pass(v)而言，v是一个左值，为什么会成功传递给 pass(T&&) 呢？

这是基于引用坍缩规则的：在传统 C++ 中，我们不能够对一个引用类型继续进行引用，但 C++ 由于右值引用的出现而放宽了这一做法，从而产生了引用坍缩规则，允许我们对引用进行引用，既能左引用，又能右引用。但是却遵循如下规则：

函数形参类型	实参参数类型	推导后函数形参类型
T&	左引用	T&
T&	右引用	T&
T&&	左引用	T&
T&&	右引用	T&&
因此，模板函数中使用 T&& 不一定能进行右值引用，当传入左值时，此函数的引用将被推导为左值。更准确的讲，无论模板参数是什么类型的引用，当且仅当实参类型为右引用时，模板参数才能被推导为右引用类型。这才使得 v 作为左值的成功传递。

完美转发就是基于上述规律产生的。所谓完美转发，就是为了让我们在传递参数的时候，保持原来的参数类型（左引用保持左引用，右引用保持右引用）。为了解决这个问题，我们应该使用 std::forward 来进行参数的转发（传递）：
```cpp
#include <iostream>
#include <utility>
void reference(int& v) {
    std::cout << "左值引用" << std::endl;
}
void reference(int&& v) {
    std::cout << "右值引用" << std::endl;
}
template <typename T>
void pass(T&& v) {
    std::cout << "普通传参:";
    reference(v);
    std::cout << "std::move 传参:";
    reference(std::move(v));
    std::cout << "std::forward 传参:";
    reference(std::forward<T>(v));

}
int main() {
    std::cout << "传递右值:" << std::endl;
    pass(1);

    std::cout << "传递左值:" << std::endl;
    int v = 1;
    pass(v);

    return 0;
}

输出结果为：

传递右值:
普通传参:左值引用
std::move 传参:右值引用
std::forward 传参:右值引用
传递左值:
普通传参:左值引用
std::move 传参:右值引用
std::forward 传参:左值引用
```
无论传递参数为左值还是右值，普通传参都会将参数作为左值进行转发，所以 std::move 总会接受到一个左值，从而转发调用了reference(int&&) 输出右值引用。

唯独 std::forward 即没有造成任何多余的拷贝，同时完美转发(传递)了函数的实参给了内部调用的其他函数。

这里 std::forward 和 std::move 一样，没有做任何事情，std::move 单纯的将左值转化为右值，std::forward 也只是单纯的将参数做了一个类型的转换，从是实现来看，std::forward<T>(v) 和 static_cast<T&&>(v) 是完全一样的。
