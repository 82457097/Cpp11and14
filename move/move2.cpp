#include<iostream>

class A {
public:
	int *pointer;
	A():pointer(new int(1)) {
		std::cout << "构造" << std::endl;
	}

	A(A& a):pointer(new int(*a.pointer)) {
		std::cout << "拷贝" << std::endl;
	}

	A(A&& a):pointer(a.pointer) {
		a.pointer = nullptr;
		std::cout << "移动" << std::endl;
	}

	~A() {
		std::cout << "析构" << std::endl;
		delete pointer;
	}
};

A returnRvalue(bool flag) {
	A a,b;
	if(flag) {
		return a;
	} else {
		return b;
	}
}

int main() {
	A obj = returnRvalue(false);
	std::cout << "obj:" << std::endl;
	std::cout << obj.pointer << std::endl;
	std::cout << *obj.pointer << std::endl;

	return 0;
}
