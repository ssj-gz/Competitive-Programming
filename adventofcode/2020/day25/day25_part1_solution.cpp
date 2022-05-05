#include <iostream>
#include <cassert>

using namespace std;

constexpr int64_t Mod = 20201227;

int64_t calcLoopSize(int64_t publicKey)
{
    int64_t subjectNumber = 1;
    int loopSize = 0;
    while (subjectNumber != publicKey)
    {
        //cout << "subjectNumber: " << subjectNumber << " loopSize: " << loopSize << " publicKey: " << publicKey << endl;
        subjectNumber = (subjectNumber * 7) % Mod;
        loopSize++;
    }
    return loopSize;
}

int64_t applyLoop(const int64_t subjectNumber, int64_t loopSize)
{
    int64_t result = 1;
    for (int i = 0; i < loopSize; i++)
    {
        result = (result * subjectNumber) % Mod;
    }
    return result;
}

int main()
{
    int64_t doorPublicKey;
    cin >> doorPublicKey;

    int64_t cardPublicKey;
    cin >> cardPublicKey;

    assert(cin);

    const int64_t doorLoopSize = calcLoopSize(doorPublicKey);
    const int64_t cardLoopSize = calcLoopSize(cardPublicKey);

    cout << "doorLoopSize: " << doorLoopSize << endl;
    cout << "cardLoopSize: " << cardLoopSize << endl;

    cout << "encryption key: " << applyLoop(doorPublicKey, cardLoopSize) << endl;
    cout << "encryption key: " << applyLoop(cardPublicKey, doorLoopSize) << endl;
}
