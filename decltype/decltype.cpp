#include<iostream>

using namespace std;

template<typename R = int, typename T, typename U>
R add1(T x, U y) {
	return x + y;
}

template<typename T, typename U>
auto add2(T x, U y) -> decltype(x + y) {
	return x + y;
}

template<typename T, typename U>
auto add3(T x, U y) {
	return x + y;
}

int main() {
	int x, y;
	cout << "please input two num:";
	cin >> x >> y;
	int result1 = 0;

	result1 = add1(x, y);
	auto result2 = add2(x, y);
	auto result3 = add3(x, y);

	cout << "result1 = " << result1 << endl;
	cout << "result2 = " << result2 << endl; 
	cout << "result3 = " << result3 << endl;

	return 0;
}
