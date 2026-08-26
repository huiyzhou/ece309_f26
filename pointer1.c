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
    int a;
    int b;
    void * x = static_cast<void *> (&a);
    void * y = static_cast<void *> (&b);
    std::cout << "Address difference between a and b:" << (long long)x - (long long)y << std::endl;
    std::cout << static_cast<void*> (&a) << std::endl;
    std::cout << static_cast<void*> (&b) << std::endl;

    int *** addr0;
    int ** addr1;
    int * p;
    p = (int *) malloc(100);
    addr1 = &p;
    addr0 = & addr1;
    std::cout << "p value:" << p << " address of p: " << addr1 << std::endl;
    std::cout << addr0;    

    free(p);
    return 0;
}
