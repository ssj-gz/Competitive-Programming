#include <iostream>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    string initialState;
    cin >> initialState;
    assert(cin);
    constexpr int discLength = 35'651'584;

    string state = initialState;
    while (static_cast<int>(state.size()) < discLength)
    {
        const auto a = state;
        string b = a;
        std::reverse(b.begin(), b.end());
        for (auto& character : b)
        {
            if (character == '0')
                character = '1';
            else if (character == '1')
                character = '0';
            else
                assert(false);
        }

        state = a + "0" + b;
        //std::cout << "next state: " << state << std::endl;
    }
    //std::cout << "Full state     : " << state << std::endl;
    state.erase(state.begin() + discLength, state.end());
    //std::cout << "Truncated state: " << state << std::endl;

    // Checksum.
    string checksum = state;
    do
    {
        assert(checksum.size() % 2 == 0);
        string nextChecksum;
        for (string::size_type pairPos = 0; pairPos < checksum.size(); pairPos += 2)
        {
            if (checksum[pairPos] == checksum[pairPos + 1])
            {
                nextChecksum += "1";
            }
            else
            {
                nextChecksum += "0";
            }
        }
        checksum = nextChecksum;
        //std::cout << "nextChecksum: " << nextChecksum << std::endl;
    } while (checksum.size() % 2 == 0);
    std::cout << "checksum: " << checksum << std::endl;


    return 0;
}
