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
