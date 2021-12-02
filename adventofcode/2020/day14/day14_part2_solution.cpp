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
            const uint64_t value = stol(matchInfo[2]);
            string memoryAddressBits(numBits, '0');
            for (int bitIndex = 0; bitIndex < numBits; bitIndex++)
            {
                uint64_t powerOf2 = (static_cast<uint64_t>(1) << (numBits - bitIndex - 1));
                if (memoryAddress & powerOf2)
                    memoryAddressBits[bitIndex] = '1';
            }

            cout << "memoryAddress original: " << memoryAddressBits << endl;
            for (int bitIndex = 0; bitIndex < numBits; bitIndex++)
            {
                switch(mask[bitIndex])
                {
                    case '0':
                        break;
                    case '1':
                    case 'X':
                        memoryAddressBits[bitIndex] = mask[bitIndex];
                        break;
                }
            }
            //cout << "  (masked to " << valueBits << ")" << endl;
#if 0
            uint64_t maskedValue = 0;
            for (int bitIndex = 0; bitIndex < numBits; bitIndex++)
            {
                maskedValue += (static_cast<uint64_t>(1) << (numBits - bitIndex - 1)) * (valueBits[bitIndex] - '0');
            }
            cout << " value: " << maskedValue << endl;
            memoryContents[memoryAddress] = maskedValue;
#endif
            cout << "memoryAddress masked  : " << memoryAddressBits << endl;
            uint64_t memoryAddress1BitsValue = 0;
            vector<uint64_t> floatingBitPowersOf2;
            for (int bitIndex = 0; bitIndex < numBits; bitIndex++)
            {
                uint64_t powerOf2 = (static_cast<uint64_t>(1) << (numBits - bitIndex - 1));
                if (memoryAddressBits[bitIndex] == '1')
                    memoryAddress1BitsValue += (static_cast<uint64_t>(1) << (numBits - bitIndex - 1));
                else if (memoryAddressBits[bitIndex] == 'X')
                    floatingBitPowersOf2.push_back(powerOf2);
            }
            const int numFloatingBits = floatingBitPowersOf2.size();
            vector<bool> useFloatingBit(numFloatingBits);
            while (true)
            {
                uint64_t floatingBitsValue = 0;
                for (int floatingBitIndex = 0; floatingBitIndex < numFloatingBits; floatingBitIndex++)
                {
                    if (useFloatingBit[floatingBitIndex])
                        floatingBitsValue += floatingBitPowersOf2[floatingBitIndex];
                }
                const uint64_t finalMemoryAddress = memoryAddress1BitsValue + floatingBitsValue;
                memoryContents[finalMemoryAddress] = value;
                cout << "finalMemoryAddress: " << finalMemoryAddress << " value: " << value << endl;

                // Next combination of floating bits.
                int floatingBitIndex = 0;
                while (floatingBitIndex < numFloatingBits && useFloatingBit[floatingBitIndex])
                {
                    useFloatingBit[floatingBitIndex] = false;
                    floatingBitIndex++;
                }
                if (floatingBitIndex == numFloatingBits)
                    break;
                useFloatingBit[floatingBitIndex] = true;
            }
            
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

