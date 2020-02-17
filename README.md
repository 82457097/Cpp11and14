# Cpp11and14
C++11/14新特性

- nullptr
- constexpr
- auto
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
