#include <iostream>
#include <regex>
#include <map>
#include <cassert>

using namespace std;

constexpr int numBits = 36;

int main()
{
    string statement;
    string mask;
    map<int64_t, uint64_t> memoryContents;
    while (getline(cin, statement))
    {
        static regex maskRegex(R"(mask\s*=\s*([01X]{36}))");
        static regex memsetRegex(R"(mem\[(\d+)\]\s*=\s*(\d+))");
        
        std::smatch matchInfo;
        if (regex_match(statement, matchInfo, maskRegex))
        {
            cout << "Mask:        " << matchInfo[1] << endl;
            mask = matchInfo[1];
            cout << "# X's: " << count(mask.begin(), mask.end(), 'X') << endl;
        }
        else if (regex_match(statement, matchInfo, memsetRegex))
        {
            const auto memoryAddress = stol(matchInfo[1]);
            uint64_t value = stol(matchInfo[2]);
            string valueBits(numBits, '0');
            for (int bitIndex = 0; bitIndex < numBits; bitIndex++)
            {
                uint64_t powerOf2 = (static_cast<uint64_t>(1) << (numBits - bitIndex - 1));
                if (value & powerOf2)
                    valueBits[bitIndex] = '1';
            }

            cout << "memset: " << memoryAddress << " to " << valueBits << endl;
            for (int bitIndex = 0; bitIndex < numBits; bitIndex++)
            {
                if (mask[bitIndex] != 'X')
                    valueBits[bitIndex] = mask[bitIndex];
            }
            cout << "  (masked to " << valueBits << ")" << endl;
            uint64_t maskedValue = 0;
            for (int bitIndex = 0; bitIndex < numBits; bitIndex++)
            {
                maskedValue += (static_cast<uint64_t>(1) << (numBits - bitIndex - 1)) * (valueBits[bitIndex] - '0');
            }
            cout << " value: " << maskedValue << endl;
            memoryContents[memoryAddress] = maskedValue;
        }
        else
        {
            assert(false);
        }
    }
    uint64_t totalContentSum = 0;
    for (const auto& [addressUnused, value] : memoryContents)
    {
        totalContentSum += value;
    }
    cout << totalContentSum << endl;
}

