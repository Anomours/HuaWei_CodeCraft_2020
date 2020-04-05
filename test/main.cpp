#include <iostream>
#include <list>
using namespace std;
int main() {
    list<int> data = {1,2,3,4,5,6,7};
    for(list<int>::iterator iter = data.begin();iter != data.end();){
        iter = data.erase(iter);
        if(iter != data.end())
            cout<<"delete data:"<<*iter<<endl;
    }
    cout<<"..."<<endl;
    return 0;
}