#include <iostream>
#include <memory>
using std::cout;
using std::endl;

int main() {
    // ===== unique_ptr =====
    auto up = std::make_unique<int>(42);
    cout << "unique_ptr:" << endl;
    cout << "*up = " << *up << "   (value)" << endl;
    cout << " up.get() = " << up.get() << "   (raw pointer address)" << endl;

    // Transfer ownership
    auto up2 = std::move(up);
    cout << "After move, up == nullptr? " << std::boolalpha << (up == nullptr) << endl;
    cout << "*up2 = " << *up2 << endl;

    // ===== shared_ptr =====
    auto sp1 = std::make_shared<int>(100);
    auto sp2 = sp1; // copy -> increases ref count
    cout << "\nshared_ptr:" << endl;
    cout << "*sp1 = " << *sp1 << ", use_count = " << sp1.use_count() << endl;
    sp2.reset(); // decreases ref count
    cout << "After sp2.reset(), use_count = " << sp1.use_count() << endl;

    // ===== weak_ptr =====
    std::weak_ptr<int> wp = sp1;
    cout << "\nweak_ptr:" << endl;
    cout << " wp.use_count() = " << wp.use_count() << endl;

    if (auto locked = wp.lock()) { // lock to get shared_ptr
        cout << " lock() succeeded, *locked = " << *locked << endl;
    } else {
        cout << " lock() failed, object expired" << endl;
    }

    sp1.reset(); // now object is destroyed
    cout << "After sp1.reset(), wp.expired()? " << wp.expired() << endl;

    return 0;
}
