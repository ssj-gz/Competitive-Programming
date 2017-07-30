#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace std;

namespace
{
    constexpr long calcPower(long base, long power)
    {
        // It seems that Hackerrank's compiler doesn't support the C++14
        // relaxations of constexpr function restrictions :(
        return (power == 1 ? base : base * calcPower(base, power - 1));
    }
}

// "BigNum" class is copied from my solution to "Fibonacci Modified".
class BigNum
{
    public:
        BigNum(const string& decimalDigits)
        {
            stringstream decimalParser;
            string::size_type decimalDigitsForBigDigitBegin = decimalDigits.size();
            const long numDecimalDigitsInBigDigit = power;
            string::size_type digitsToRead = numDecimalDigitsInBigDigit;
            while (decimalDigitsForBigDigitBegin != 0) 
            {
                if (decimalDigitsForBigDigitBegin >= numDecimalDigitsInBigDigit)
                {
                    decimalDigitsForBigDigitBegin -= numDecimalDigitsInBigDigit;
                }
                else
                {
                    digitsToRead = decimalDigitsForBigDigitBegin;
                    decimalDigitsForBigDigitBegin = 0;
                }
                decimalParser.clear();
                decimalParser.str(decimalDigits.substr(decimalDigitsForBigDigitBegin, digitsToRead));
                BigDigit bigDigit;
                decimalParser >> bigDigit;
                m_digits.push_back(bigDigit);
            }
        }
        string toString() const
        {
            stringstream decimalWriter;
            decimalWriter << setfill('0');
            for (auto bigDigitIter = m_digits.rbegin(); bigDigitIter != m_digits.rend(); bigDigitIter++)
            {
                decimalWriter << *bigDigitIter;
                // After the most significant digit, need to left-pad with '0'.
                decimalWriter << setw(power);
            }
            return decimalWriter.str();
        }
        BigNum& operator+=(const BigNum& other) &
        {
            add(&other == this ? BigNum(other) : other);
            return *this;
        }
        BigNum& operator*=(const BigNum& originalOther) &
        {
            const BigNum& other = (&originalOther == this ? BigNum(originalOther) : originalOther);
            const BigNum originalValue(*this);
            int shiftBy = 0;
            BigNum result("0");
            for (auto otherDigit : other.m_digits)
            {
                BigNum multipliedByOtherDigit(originalValue);
                multipliedByOtherDigit.multiplyBy(otherDigit);
                result.add(multipliedByOtherDigit, shiftBy);
                shiftBy++;
            }
            *this = result;
            return *this;
        }
    private:
        // Pick a power of 10 as a base; this aids with converting
        // to and from decimal strings.
        static const long power = 9;
        static constexpr long digitBase = calcPower(10, power);
        using BigDigit = uint64_t;
        static_assert(numeric_limits<BigDigit>::max() / digitBase >= digitBase, "digitBase too large or BigDigit too small!");
       
        // Digits are stored "backwards" i.e. least significant at the front. 
        vector<BigDigit> m_digits;
        using DigitIndex = decltype(m_digits)::size_type;

    public:
        void multiplyBy(BigDigit singleDigit)
        {
            assert(singleDigit >= 0 && singleDigit < digitBase);
            BigDigit carry = 0;
            for (auto& digit : m_digits)
            {
                digit *= singleDigit;
                digit += carry;
                carry = 0;
                if (digit >= digitBase)
                {
                    carry = digit / digitBase;
                    digit %= digitBase;
                }
            }
            if (carry > 0)
            {
                m_digits.push_back(carry);
            }
        }

        void add(const BigNum& other, int shiftOtherBy = 0)
        {
            const auto otherNumDigits = other.m_digits.size() + shiftOtherBy;
            if (otherNumDigits > m_digits.size())
                m_digits.reserve(otherNumDigits + 1);
            bool carry = false;
            for (DigitIndex digitIndex = 0; digitIndex < otherNumDigits || carry; digitIndex++)
            {
                BigDigit toAdd = (carry ? 1 : 0);
                carry = false;
                if (digitIndex < otherNumDigits)
                {
                    if (digitIndex >= shiftOtherBy)
                    {
                        const BigDigit otherDigit = other.m_digits[digitIndex - shiftOtherBy];
                        assert(otherDigit < digitBase);
                        toAdd += otherDigit;
                    }
                }

                if (digitIndex >= m_digits.size())
                {
                    m_digits.push_back(0);
                }
                assert(digitIndex < m_digits.size());

                m_digits[digitIndex] += toAdd;
                if (m_digits[digitIndex] >= digitBase)
                {
                    carry = true;
                    m_digits[digitIndex] -= digitBase;
                }
                assert(m_digits[digitIndex] >= 0 && m_digits[digitIndex] < digitBase);
            }
        }

};

BigNum operator+(const BigNum& lhs, const BigNum& rhs)
{
    BigNum result(lhs);
    result += rhs;
    return result;
}

BigNum operator*(const BigNum& lhs, const BigNum& rhs)
{
    BigNum result(lhs);
    result *= rhs;
    return result;
}

// "TrieThing" was written to try and solve "Gridland Provinces",
// but it turned out not to be useful for that - useful for this,
// though :)
class TrieThing
{
    public:
        TrieThing()
        {
            m_nodes.push_back(Node());
        }
        class NodeRef
        {
            public:
                NodeRef(const NodeRef& other)
                    : m_nodeIndex(other.m_nodeIndex)
                {

                }
            private:
                long m_nodeIndex;

                NodeRef(long nodeIndex)
                    : m_nodeIndex(nodeIndex)
                {
                }
                friend class TrieThing;

                long nodeIndex() const
                {
                    return m_nodeIndex;
                }
        };
        NodeRef addString(const string& stringToAdd)
        {
            return addSuffix(NodeRef(0), stringToAdd);
        }
        NodeRef addSuffix(const NodeRef& prefixNodeRef, const string& suffix)
        {
            Node* node = &(m_nodes[prefixNodeRef.nodeIndex()]);
            int i = 0;
            for (const auto letter : suffix)
            {
                const int letterIndex = letter - '0';
                assert(letterIndex >= 0 && letterIndex < numLetters);
                if (node->nodeIdAfterLetterIndex[letterIndex] == 0)
                {
                    const long index = node->index;
                    m_nodes.push_back(Node());
                    Node *newNode = &(m_nodes.back());
                    node = &(m_nodes[index]);
                    node->nodeIdAfterLetterIndex[letterIndex] = (m_nodes.size() - 1);
                    newNode->depth = node->depth + 1;
                    newNode->index = (m_nodes.size() - 1);

                }
                assert(node->nodeIdAfterLetterIndex[letterIndex] != 0);
                node = &(m_nodes[node->nodeIdAfterLetterIndex[letterIndex]]);

                i++;
                if (i == suffix.size())
                {
                    node->isTerminal = true;
                }
            }
            return NodeRef(node->index);
        }
#if 0
        bool containsString(const string& stringToFind)
        {
            Node* node = &(m_nodes.front());
            for (const auto letter : stringToFind)
            {
                const int letterIndex = letter - '0';
                assert(letterIndex >= 0 && letterIndex < numLetters);

                const long nodeIdAfterFollowingLetter = node->nodeIdAfterLetterIndex[letterIndex];
                if (nodeIdAfterFollowingLetter == 0)
                    return false;

                node = &(m_nodes[nodeIdAfterFollowingLetter]);
            }
            return node->isTerminal;
        }
#endif
        enum IncrementalSearchResult { PotentialMatch, Match, NoMoreMatches };
        void beginIncrementalSearch()
        {
            m_incrementalSearchNode = &(m_nodes.front());
        }
        IncrementalSearchResult addNextIncrementalSearchChar(char letter)
        {
            assert(m_incrementalSearchNode);
            const int letterIndex = letter - '0';
            assert(letterIndex >= 0 && letterIndex < numLetters);
            const long nodeIdAfterFollowingLetter = m_incrementalSearchNode->nodeIdAfterLetterIndex[letterIndex];
            if (nodeIdAfterFollowingLetter == 0)
            {
                m_incrementalSearchNode = nullptr;
                return NoMoreMatches;
            }

            m_incrementalSearchNode = &(m_nodes[nodeIdAfterFollowingLetter]);
            if (m_incrementalSearchNode->isTerminal)
                return Match;
            return PotentialMatch;
        }
    private:
        static const int numLetters = 10;
        struct Node
        {
            long depth = 0;
            long index = 0;
            bool isTerminal = false;
            // node Id == 0 is a code for "no node".
            long nodeIdAfterLetterIndex[numLetters] = {};
        };
        vector<Node> m_nodes;
        Node *m_incrementalSearchNode = nullptr;

};


int main() {
    // Well, that was anti-climactic ("Expert"? "150 points"? WTF??)
    // I'd actually spent a huge amount of time thinking about this,
    // and was working on a strategy for finding huge (i.e. ~100000)-digit
    // length powers of 2 in a string, when I noticed that the maximum power
    // was "800" XD
    // With this restriction, it's absolutely trivially easy: all such powers of 
    // 2 can be generated with a BigNum class, and putting them into a Trie-type
    // of thing allows us to easily find them in s.
    // As a premature "optimisation", powers of 2 with less than 6 decimal digits
    // don't go in the Trie - they are searched for manually.
    BigNum powerOf2("1");
    BigNum two("2");
    
    const int maxPowerOf2 = 800;

    vector<string> powersOf2;
    for (int i = 0; i <= maxPowerOf2; i++)
    {
        const string powerOf2AsString = powerOf2.toString();
        powersOf2.push_back(powerOf2AsString);
        powerOf2 *= two;
    }
    
    TrieThing powersOf2WithAtLeast6Digits;
    for (const auto& powerOf2AsString : powersOf2)
    {
        if (powerOf2AsString.size() >= 6)
        {
            powersOf2WithAtLeast6Digits.addString(powerOf2AsString);
        }
    }

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        string s;
        cin >> s;
        long twoCount = 0;

        for (const auto& powerOf2AsString : powersOf2)
        {
            // Don't want the overhead of the TrieThing for
            // finding small strings - just find them "manually".
            if (powerOf2AsString.size() < 6)
            {
                string::size_type searchStartPos = s.find(powerOf2AsString);
                while (searchStartPos != string::npos)
                {
                    twoCount++;
                    searchStartPos = s.find(powerOf2AsString, searchStartPos + 1);
                }
            }
        }

        using SearchResult = TrieThing::IncrementalSearchResult;
        for (int i = 0; i < s.size(); i++)
        {
            powersOf2WithAtLeast6Digits.beginIncrementalSearch();
            for (int j = i; j < s.size(); j++)
            {
                const SearchResult result = powersOf2WithAtLeast6Digits.addNextIncrementalSearchChar(s[j]);
                if (result == SearchResult::NoMoreMatches)
                    break;
                if (result == SearchResult::Match)
                    twoCount++;
            }
        }
        cout << twoCount << endl;
    }
}

