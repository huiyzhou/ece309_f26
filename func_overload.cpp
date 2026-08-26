#include <iostream>

int foo(int a){
    return (a+1);
}

float foo(float a){
    return (a  + 2.0);
}


int main()
{
    std::cout << foo(10) << std::endl;
    std::cout << foo((float)0.5) << std::endl;
    std::cout << foo((int)0.5) << std::endl;
    //    std::cout << foo(0.5) << std::endl; //try to uncomment this line and see what happens. 
}
