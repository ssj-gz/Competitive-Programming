#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>


using namespace std;

int64_t binToDec(const string& bits)
{
    return stol(bits, nullptr, 2);
}

pair<int64_t, int> parse(const string& bitStream, int pos)
{
    const auto version [[maybe_unused]] = binToDec(bitStream.substr(pos, 3));
    pos += 3;
    const auto type = binToDec(bitStream.substr(pos, 3));
    pos += 3;

    if (type == 4)
    {
        // Literal value.
        string valueBin;
        do
        {
            valueBin += bitStream.substr(pos + 1, 4);
            pos += 5;
        } while (bitStream[pos - 5] != '0');

        return {binToDec(valueBin), pos};
    }
    else
    {
        // Operator.
        const auto lengthTypeId = binToDec(""s + bitStream[pos]);
        pos++;
        vector<int64_t> subPacketValues;
        if (lengthTypeId == 0)
        {
            const auto subPacketsLength = binToDec(bitStream.substr(pos, 15));
            pos += 15;
            const int subPacketsStartPos = pos;
            do
            {
                const auto [value, newPos] = parse(bitStream, pos);
                subPacketValues.push_back(value);
                pos = newPos;
            } while (pos != subPacketsStartPos + subPacketsLength);
        }
        else
        {
            auto numSubPackets = binToDec(bitStream.substr(pos, 11));
            pos += 11;
            while (numSubPackets-- > 0)
            {
                const auto [value, newPos] = parse(bitStream, pos);
                subPacketValues.push_back(value);
                pos = newPos;
            }
        }
        int64_t value = 0;
        switch (type)
        {
            case 0:
                for (const auto subPacketValue : subPacketValues)
                    value += subPacketValue;
                break;
            case 1:
                value = 1;
                for (const auto subPacketValue : subPacketValues)
                    value *= subPacketValue;
                break;
            case 2:
                value = *min_element(subPacketValues.begin(), subPacketValues.end());
                break;
            case 3:
                value = *max_element(subPacketValues.begin(), subPacketValues.end());
                break;
            case 5:
                assert(static_cast<int>(subPacketValues.size()) == 2);
                value = (subPacketValues[0] > subPacketValues[1]) ? 1 : 0;
                break;
            case 6:
                assert(static_cast<int>(subPacketValues.size()) == 2);
                value = (subPacketValues[0] < subPacketValues[1]) ? 1 : 0;
                break;
            case 7:
                assert(static_cast<int>(subPacketValues.size()) == 2);
                value = (subPacketValues[0] == subPacketValues[1]) ? 1 : 0;
                break;
            default:
                assert(false);
        }
        return {value, pos};
    }
    assert(false);
    return {-1, -1};
}


int main()
{
    string hex;
    cin >> hex;

    map<char, string> bitsForHex;

    bitsForHex['0'] = "0000";
    bitsForHex['1'] = "0001";
    bitsForHex['2'] = "0010";
    bitsForHex['3'] = "0011";
    bitsForHex['4'] = "0100";
    bitsForHex['5'] = "0101";
    bitsForHex['6'] = "0110";
    bitsForHex['7'] = "0111";
    bitsForHex['8'] = "1000";
    bitsForHex['9'] = "1001";
    bitsForHex['A'] = "1010";
    bitsForHex['B'] = "1011";
    bitsForHex['C'] = "1100";
    bitsForHex['D'] = "1101";
    bitsForHex['E'] = "1110";
    bitsForHex['F'] = "1111";

    assert(hex.size() % 2 == 0);
    string bitStream;
    for (string::size_type i = 0; i < hex.size(); i += 2)
    {
        bitStream += bitsForHex[hex[i]];
        bitStream += bitsForHex[hex[i+1]];
    }
    const auto [value, newPos] = parse(bitStream, 0);

    cout << value << endl;

}
