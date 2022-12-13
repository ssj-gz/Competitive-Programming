#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

struct DistressSignal
{
    enum Type { List, Number } type;

    int number = -1;
    vector<DistressSignal> list;
};

ostream& operator<<(ostream& os, const DistressSignal& toPrint)
{
    if (toPrint.type == DistressSignal::Number)
    {
        os << " " << toPrint.number << " ";
    }
    else
    {
        os << "[ ";
        for (int subElementIndex = 0; subElementIndex < static_cast<int>(toPrint.list.size()); subElementIndex++)
        {
            os << toPrint.list[subElementIndex];
            if (subElementIndex != static_cast<int>(toPrint.list.size()) - 1)
            {
                os << ", ";
            }

        }
        os << " ]";
    }
    return os;
};

bool operator<(const DistressSignal& lhs, const DistressSignal& rhs)
{
    std::cout << "Comparing " << lhs << " vs " << rhs << std::endl;
    if (lhs.type == DistressSignal::Number && rhs.type == DistressSignal::Number)
    {
        return lhs.number < rhs.number;
    }
    if (lhs.type == DistressSignal::List && rhs.type == DistressSignal::List)
    {
        auto lhsSubElementIter = lhs.list.begin();
        auto rhsSubElementIter = rhs.list.begin();

        while (lhsSubElementIter != lhs.list.end() && rhsSubElementIter != rhs.list.end())
        {
            if (*lhsSubElementIter < *rhsSubElementIter)
                return true;
            if (*rhsSubElementIter < *lhsSubElementIter)
                return false;

            lhsSubElementIter++;
            rhsSubElementIter++;
        }
        if (lhsSubElementIter == lhs.list.end() && rhsSubElementIter != rhs.list.end())
            return true;
        return false;
    }
    auto asList = [](const DistressSignal& d)
    {
        if (d.type == DistressSignal::List)
            return d;
        else
        {
            DistressSignal asList;
            asList.type = DistressSignal::List;
            asList.list.push_back(d);
            return asList;
        }
    };

    auto newLhs = asList(lhs);
    auto newRhs = asList(rhs);
    return newLhs < newRhs;
}

DistressSignal parse(const string& distressSignalLineOrig)
{
    string distressSignal(distressSignalLineOrig);
    distressSignal.erase(std::remove(distressSignal.begin(), distressSignal.end(), ' '), distressSignal.end());
    std::cout << "parsing: " << distressSignal << std::endl;

    assert(!distressSignal.empty());

    DistressSignal returnVal;
#if 0
    if (distressSignal.empty())
    {
        returnVal.type = DistressSignal::List;
        return returnVal;
    }
    else
#endif
    if (distressSignal[0] == '[')
    {
        returnVal.type = DistressSignal::List;
        assert(distressSignal.back() == ']');
        int subElementBeginPos = 1;
        int bracketLevel = 1;
        for (int pos = 1; pos < static_cast<int>(distressSignal.size()); pos++)
        {
            if (distressSignal[pos] == '[')
            {
                bracketLevel++;
            }
            else if (distressSignal[pos] == ']')
            {
                bracketLevel--;
            }
            else if (distressSignal[pos] == ',' && bracketLevel == 1)
            {
                assert(subElementBeginPos != -1);
                string subElement = distressSignal.substr(subElementBeginPos, pos - subElementBeginPos);
                std::cout << " before comma: " << subElement <<  std::endl;
                returnVal.list.push_back(parse(subElement));
                subElementBeginPos = pos + 1;
            }
        }
        string subElement = distressSignal.substr(subElementBeginPos, distressSignal.size() - 1 - subElementBeginPos);
        std::cout << " remainder: " << subElement <<  std::endl;
        if (!subElement.empty())
            returnVal.list.push_back(parse(subElement));
    }
    else
    {
        returnVal.type = DistressSignal::Number;
        returnVal.number = 0;
        for (const auto digit : distressSignal)
        {
            assert(digit >= '0' && digit <= '9');
            returnVal.number = returnVal.number * 10 + (digit - '0');
        }

    }
    return returnVal;

}

int main()
{
    vector<DistressSignal> distressSignals;
    const auto decoderSignal1 = parse("[[2]]");
    const auto decoderSignal2 = parse("[[6]]");
    distressSignals.push_back(decoderSignal1);
    distressSignals.push_back(decoderSignal2);
    while (true)
    {
        string distressSignalLine;
        getline(cin, distressSignalLine);
        if (!cin)
            break;
        if (distressSignalLine.empty())
            continue;

        distressSignals.push_back(parse(distressSignalLine));

    }
    sort(distressSignals.begin(), distressSignals.end());

    std::cout << "Distress signals: " << std::endl;

    int pos = 1;
    int decoderSignal1Pos = -1;
    int decoderSignal2Pos = -1;
    for (const auto& distressSignal : distressSignals)
    {
        if (!(distressSignal < decoderSignal1) && !(decoderSignal1 < distressSignal))
        {
            assert(decoderSignal1Pos == -1);
            decoderSignal1Pos = pos;
        }
        if (!(distressSignal < decoderSignal2) && !(decoderSignal2 < distressSignal))
        {
            assert(decoderSignal2Pos == -1);
            decoderSignal2Pos = pos;
        }
        pos++;
    }
    assert(decoderSignal1Pos != -1);
    assert(decoderSignal2Pos != -1);

    std::cout << "decoderSignal1Pos: " <<  decoderSignal1Pos << " decoderSignal2Pos: " << decoderSignal2Pos << std::endl;
    std::cout << decoderSignal1Pos * decoderSignal2Pos << std::endl;

}


