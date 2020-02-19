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
