#include<iostream>
#include<string>

using namespace std;

void reference(string & str) {
	cout << "lvalue!" << endl;
}

void reference(string&& str) {
	cout << "rvalue!" <<endl;
}

int main() {
	string lv1 = "string,";
	string&& rv1 = std::move(lv1);
	cout << "rv1 = " << rv1 << endl;

	reference(rv1);

	const string& lv2 = lv1 + lv1;
	cout << "lv2 = " << lv2 << endl;

	string&& rv2 = lv1 + lv2;
	rv2 += "string";
	cout << "rv2 = " << rv2 << endl;

	reference(rv2);

	return 0;
}
