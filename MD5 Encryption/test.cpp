#include <iostream>
#include <string>
#include "md5.cpp"
using namespace std;

int main()
{
    string data[6] = {
        "", "a", "abc", "message digest",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    };

    for (int i = 0; i < 6; i++) {
        cout << "\n\n\t\t\t****Test case " << i + 1 << "****\n";
        test(data[i]);
    }

    return 0;
}
