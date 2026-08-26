int main()
{
    int a;
    int b;
    std::cout << "Address difference between a and b:" << (&a - &b) << std::endl;
    std::cout << &a << std::endl;
    std::cout << &b << std::endl;

    int *** addr0;
    int ** addr1;
    int * p; 
    p = (int *) malloc(100);
    addr1 = &p;
    addr0 = & addr1;
    std::cout << "p value:" << p << " address of p: " << addr1 << std::endl;
    std::cout << addr0;    

    //free(p);
}


//Corrected version:
#include <iostream>
int main()
{
    int a {0};
    int * b {0};
    void * x = static_cast<void *> (&a);
    void * y = static_cast<void *> (&b);
    int addr_a = reinterpret_cast<long long>(x);
    int addr_b = reinterpret_cast<long long>(y);
    std::cout << "Address difference between a and b:" << addr_a - addr_b << std::endl;
    std::cout << x << std::endl;
    std::cout << y << std::endl;

    int *** addr0;
    int ** addr1;
    int * p;
    p = new int[100];
    addr1 = &p;
    addr0 = & addr1;
    std::cout << "p value:" << p << " address of p: " << addr1 << std::endl;
    std::cout << addr0;    

    delete [] p;
    return 0;
}
