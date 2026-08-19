#include <iostream>

using namespace std;

int main(){
    int another_range[] {2,4,6,8,-10};
    for (auto i: another_range)
        cout << i << endl;        

    for (auto &i: another_range){
        i++;
    }

    for (auto i: another_range)
        cout << i << endl;        


    auto range = {1,2,3,4,5};
    for (auto i: range)
        cout << i << endl;

    range = {1, 3, 5, 7, 9, 11};
    for (auto i: range)
        cout << i << endl;    

    //Question: what is the inferred type of the variable range by the compiler?
}
