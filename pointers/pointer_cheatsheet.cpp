#include <iostream>
using std::cout;
using std::endl;

int main() {
    // plain int
    int a = 42;
    // reference (alias for a)
    // if we change r it changes a, and vice versa.
    int& r = a;
    // pointer to a
    // stores the memory adress
    int* p = &a;

    cout << "a  = " << a << "   at address " << &a << endl;
    cout << "r  = " << r << "   at address " << &r << endl;
    cout << "p  = " << p << "   (pointer value = address of a)" << endl;
    cout << "*p = " << *p << "   (dereference p, gives value of a)" << endl;

    // change through reference
    r = 100;
    cout << "\nAfter r = 100;" << endl;
    cout << "a  = " << a << "   (changed through reference)" << endl;
    cout << "*p = " << *p << "   (same change seen through pointer)" << endl;

    // change through pointer
    *p = 7;
    cout << "\nAfter *p = 7;" << endl;
    cout << "a  = " << a << "   (changed through pointer)" << endl;
    cout << "r  = " << r << "   (reference also sees the change)" << endl;

    return 0;
}
