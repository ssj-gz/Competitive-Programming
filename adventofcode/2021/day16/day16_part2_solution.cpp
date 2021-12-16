#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>


using namespace std;

int64_t binToDec(const string& bits)
{
    //cout << "binToDec bits: " << bits << endl;
    return stol(bits, nullptr, 2);
}

pair<int64_t, int> parse(const string& bitStream, int pos, int end, string indent, int64_t& versionSum)
{
    cout << indent << "parsing: " << bitStream << " from " << pos << endl;
    cout << indent << "         " << string(pos, ' ') << "^" << endl;
    const int startPos = pos;
    const auto version = binToDec(bitStream.substr(pos, 3));
    versionSum += version;
    cout << indent << "version: " << version << endl;
    pos += 3;
    const auto type = binToDec(bitStream.substr(pos, 3));
    pos += 3;

    if (type == 4)
    {
        // Literal value.
        string valueBin;
        cout << "literal value; blocks begin at pos " << pos << endl;
        do
        {
            valueBin += bitStream.substr(pos + 1, 4);
            pos += 5;
            cout << indent << "pos now:" << pos << " startPos: " << startPos << endl;
        } while (bitStream[pos - 5] != '0');
        cout << indent << "parsed literal value: " << binToDec(valueBin) << endl;
        cout << indent << "         " << string(pos, ' ') << "^ original end here" << endl;
        //while ((pos - startPos) % 4 != 0)
            //pos++;
        //cout << indent << "         " << string(pos, ' ') << "^ padded end here" << endl;

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
            cout << indent << "Operator; subPacketsLength: " << subPacketsLength << endl;
            const int subPacketsStartPos = pos;
            do
            {
                const auto [value, newPos] = parse(bitStream, pos,  -1, indent + " ", versionSum);
                subPacketValues.push_back(value);
                pos = newPos;
            } while (pos != subPacketsStartPos + subPacketsLength);
        }
        else
        {
            auto numSubPackets = binToDec(bitStream.substr(pos, 11));
            cout << indent << "Operator; numSubPackets: " << numSubPackets << endl;
            pos += 11;
            while (numSubPackets-- > 0)
            {
                const auto [value, newPos] = parse(bitStream, pos, end, indent + " ", versionSum);
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
    cout << "bitStream: " << bitStream << endl;
    int64_t versionSum = 0;
    const auto [value, newPos] = parse(bitStream, 0, -1, "", versionSum);

    cout << "bitStream size: " << bitStream.size() << " endPos: " << newPos << endl;


    cout << value << endl;

}
