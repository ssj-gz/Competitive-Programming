#include <iostream>

using namespace std;

int main()
{
    int64_t frequency = 0;
    int64_t frequencyChange;
    while (cin >> frequencyChange)
    {
        std::cout << frequencyChange << std::endl;
        frequency += frequencyChange;
    }
    std::cout << "frequency: " << frequency << std::endl;
    return 0;
}
