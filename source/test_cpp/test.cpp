#include "test.h"
#include <iostream>
#include <thread>
#include <windows.h>
#include <future>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <type_traits>
#include <thread>
#include <mutex>
#include <memory>

using namespace std;


class Base {
public:
    template<typename T>
    Base(T &&) {
        std::cout << "Base  template" << std::endl;
    }
    Base() {
        std::cout << "Base()" << std::endl;
    }
    Base(const Base&) {
        std::cout << "Base(const Base&)" << std::endl;
    }
    //     virtual ~Base() {
    //         std::cout << "~Base()" << std::endl;
    //     };
    //     virtual void test() const {
    //         std::cout << "in base" << std::endl;
    //     }
};

class Derived :public Base {
public:

    Derived(int a = 10) : Base() {
        std::cout << "Derived()" << std::endl;
    }

    Derived(const Derived& a):Base(a) {
        std::cout << "Derived(const Derived&)" << std::endl;
    }
    Derived(Derived&&) {
        std::cout << "Derived(Derived&&)" << std::endl;
    }
    virtual ~Derived() {
        std::cout << "~Derived()" << std::endl;
    };

private:
    string m_str;
    int m_a;
    int m_b;
};

template<typename C>
auto cend_vp(const C &con)->decltype(std::end(con))
{
    return std::end(con);
}



class Del {
public:
    template<typename T, typename = typename std::enable_if<std::is_same<T, Del>::value>::type>
    //template<typename T>
    Del(T &&n) {
        cout << "template<typename T>" << endl;
    }
    explicit Del(int a=0) {
        cout << "Del(int a=0)" << endl;
    }
    Del(const Del &) {
        cout << "Del(const Del &)" << endl;
    };

     Del(Del&&d) {
         cout << "Del(Del &&) " << endl;
     };

    void test() {
        cout << "sleep111\n";
        Sleep(3000);
        cout << "sleep222\n";
        static int aa = 0;
        auto lamb = [](int a) {
            aa / aa;
        };
        lamb(1);
    }
private:
    int m_delete{ 11 };

};

// template<typename T>
// void test(T a) {
//     cout << "test(T &&a)" << endl;
// }
// void test(int &&a) {
// }
using FilterContainer = // as before
std::vector<std::function<bool(int)>>;
FilterContainer filters;

void test(Del a) {
    return;
}


int main() {
    
    //Del del = Del();
    //del.test();
    //auto un = unique_ptr<Derived, decltype(del)>(new Derived(), del);
    //auto un = make_unique<Derived>();
    //auto sh = shared_ptr<Derived>(new Derived());
    //auto sh = make_shared<Derived>();
    {
        Del a(10);
        test(std::move(a));
    }
    vector<string> a;
    a.emplace_back("111");

    printf("===========\n");
    return 0;
}