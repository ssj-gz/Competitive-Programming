#include <iostream>
#include <vector>
#include <regex>
#include <charconv>
#include <cassert>

#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.

using namespace std;

const uint64_t numCards = 119'315'717'514'047ULL;
//const uint64_t numCards = 32868853;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (modulus + m_n + other.m_n);
            assert(m_n >= 0);
            m_n = m_n % modulus;
            return *this;
        }
        ModNum& operator-=(const ModNum& other)
        {
            m_n += modulus;
            assert(m_n >= other.m_n);
            m_n = (m_n - other.m_n) % modulus;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            // Use bitwise multiplication to avoid overflow.
            int64_t result = 0;
            int64_t value = m_n;
            int64_t otherValue = other.m_n;
            for (int i = 0; i < 64; i++)
            {
                if ((otherValue & 1) == 1)
                    result = (result + value) % modulus;
                otherValue >>= 1;
                value = (value << 1) % modulus;
            }
            m_n = result;

            return *this;
        }
        ModNum operator++(int)
        {
            m_n += 1;
            return *this;
        }
        int64_t value() const { return m_n; };

        static const int64_t modulus = numCards;
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}


ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

ModNum quickPower2(ModNum base, int64_t exponent)
{
    // Raise base to the exponent mod modulus using as few multiplications as 
    // we can e.g. base ^ 8 ==  (((base^2)^2)^2).
    ModNum result = 1;
    int64_t power = 0;
    while (exponent > 0)
    {
        if (exponent & 1)
        {
            ModNum subResult = base;
            for (int64_t i = 0; i < power; i++)
            {
                subResult = (subResult * subResult);
            }
            result = (result * subResult);
        }
        exponent >>= 1;
        power++;
    }
    return result;
}

int64_t quickPower(__uint128_t base, __uint128_t exponent, __uint128_t modulus)
{
    // Raise base to the exponent mod modulus using as few multiplications as 
    // we can e.g. base ^ 8 ==  (((base^2)^2)^2).
    __uint128_t result = 1;
    __uint128_t power = 0;
    while (exponent > 0)
    {
        if (exponent & 1)
        {
            __uint128_t subResult = base;
            for (__uint128_t i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % modulus;
            }
            result = (result * subResult) % modulus;
        }
        exponent >>= 1;
        power++;
    }
    return result;
}


int main()
{

    std::regex numberRegex("-?\\d+");

    auto extractNumber = [&numberRegex](const std::string& stringContainingNumber)
    {
            std::smatch numMatch;
            if (std::regex_search(stringContainingNumber, numMatch, numberRegex))
            {
                std::string numberAsString(numMatch[0]);
                int number;
                auto [ptr, ec] = std::from_chars(numberAsString.data(), numberAsString.data()+numberAsString.size(), number);
                assert(ec == std::errc{});
                return number;
            }
            else
            {
                assert(false);
                return -1;
            }

    };

    struct Instruction
    {
        enum Type { DealWithIncrement, DealIntoNewDeck, Cut } type;
        int amount = -1;
    };

    string instructionLine;
    vector<Instruction> instructions;
    while (std::getline(cin, instructionLine))
    {
        if (instructionLine.starts_with("deal with increment "))
        {
            const int increment = extractNumber(instructionLine);
            instructions.push_back({Instruction::DealWithIncrement, increment});
            std::cout << "Deal with increment " << increment << std::endl;
        }
        else if (instructionLine.starts_with("deal into new stack"))
        {
            std::cout << "Deal into new stack" << std::endl;
            instructions.push_back({Instruction::DealIntoNewDeck, -1});
        }
        else if (instructionLine.starts_with("cut"))
        {
            const int cutAmount = extractNumber(instructionLine);
            std::cout << "Cut" << cutAmount << std::endl;
            instructions.push_back({Instruction::Cut, cutAmount});
        }
        else
        {
            assert(false);
        }
    }

    std::reverse(instructions.begin(), instructions.end());
    const int64_t initialDesiredPos = 6577;
    int64_t desiredPos = initialDesiredPos;
    int64_t XYay = -1;
    int64_t YYay = -1;
    const int numShuffles = 256;
    for (int i = 0; i < numShuffles; i++)
    {
        int64_t X = 1;
        int64_t Y = 0;
        ModNum XVerify(1);
        ModNum YVerify(0);
        for (const auto instruction : instructions)
        {
            switch (instruction.type)
            {
                case Instruction::DealWithIncrement:
                    {
                        const __uint128_t inverseAmount = quickPower(instruction.amount, numCards - 2, numCards);
                        const ModNum inverseAmountVerify = quickPower2(instruction.amount, numCards - 2);
                        //std::cout << "amount: " << instruction.amount << " inverseAmount: " << inverseAmount << " multiplied:" << (static_cast<__uint128_t>(inverseAmount) * static_cast<__uint128_t>(instruction.amount)) << " modulus: " << ((inverseAmount * instruction.amount) % numCards) << std::endl;
                        assert(((static_cast<__uint128_t>(inverseAmount) * static_cast<__uint128_t>(instruction.amount)) % numCards) == 1);
                        desiredPos = (static_cast<__uint128_t>(desiredPos) * static_cast<__uint128_t>(inverseAmount)) % numCards;
                        X = (static_cast<__uint128_t>(X) * static_cast<__uint128_t>(inverseAmount)) % numCards;
                        Y = (static_cast<__uint128_t>(Y) * static_cast<__uint128_t>(inverseAmount)) % numCards;
                        XVerify = XVerify * inverseAmountVerify;
                        YVerify = YVerify * inverseAmountVerify;
                    }
                    break;
                case Instruction::Cut:
                    desiredPos = numCards + desiredPos + instruction.amount;
                    assert(desiredPos >= 0);
                    desiredPos = desiredPos % numCards;
                    Y = (Y + numCards +  instruction.amount) % numCards;
                    YVerify += instruction.amount;
                    break;
                case Instruction::DealIntoNewDeck:
                    desiredPos = numCards - 1 - desiredPos;
                    X = (static_cast<__uint128_t>(X) * static_cast<__uint128_t>(numCards - 1)) % numCards;
                    Y = (static_cast<__uint128_t>(Y) * static_cast<__uint128_t>(numCards - 1)) % numCards;
                    Y = (Y + numCards + (numCards - 1)) % numCards;
                    XVerify *= (numCards - 1);
                    YVerify *= (numCards - 1);
                    YVerify += numCards - 1;
                    break;
                default:
                    assert(false);
            }
            assert(desiredPos >= 0 && desiredPos < numCards);
        }
        XYay = X;
        YYay = Y;
        __uint128_t pickle = 0;
        pickle = (static_cast<__uint128_t>(X) * initialDesiredPos) % numCards;
        pickle = (pickle + Y) % numCards;
        std::cout << "noodles: " << static_cast<uint64_t>(pickle) << std::endl; 
        std::cout << "desiredPos after shuffle: # " << (i + 1) << " : " << desiredPos << std::endl;
        std::cout << "X: " << X << std::endl;
        std::cout << "Y: " << Y << std::endl;
        std::cout << "XVerify: " << XVerify << std::endl;
        std::cout << "YVerify: " << YVerify << std::endl;
        assert(XVerify.value() == X);
        assert(YVerify.value() == Y);
    }
    std::cout << "desiredPos: " << desiredPos << std::endl;
    //__uint128_t pickle = 0;
    //pickle = (static_cast<__uint128_t>(X) * initialDesiredPos) % numCards;
    //pickle = (pickle + Y) % numCards;
    //std::cout << "haggis: " << static_cast<uint64_t>(pickle) << std::endl; 
    std::cout << "XYay: " << static_cast<int64_t>(XYay) << " YYay: " << static_cast<int64_t>(YYay) << std::endl;
    __uint128_t X = XYay;
    __uint128_t Y = YYay;
    ModNum XVerify(XYay);
    ModNum YVerify(YYay);
    for (int i = 0; i < numShuffles - 1; i++)
    {
        __uint128_t Xnew = (X * X) % numCards;
        __uint128_t Ynew = (X * Y + Y) % numCards; // Squaring.
        ModNum XnewVerify = XVerify * XVerify;
        ModNum YnewVerify = XVerify * YVerify + YVerify;
        assert(XnewVerify.value() == Xnew);
        assert(YnewVerify.value() == Ynew);
        //__uint128_t Xnew = (X * XYay) % numCards;     // Multiplying by XYay YYay.
        //__uint128_t Ynew = (X * YYay + Y) % numCards;
        X = Xnew;
        Y = Ynew;
        XVerify = XnewVerify;
        YVerify = YnewVerify;
        assert(XVerify.value() == X);
        assert(YVerify.value() == Y);

        __uint128_t pickle = 0;
        pickle = (static_cast<__uint128_t>(X) * initialDesiredPos) % numCards;
        pickle = (pickle + Y) % numCards;
        ModNum pickleVerify = XVerify * initialDesiredPos + YVerify;
        std::cout << "interim haggis # " << i << ": " << static_cast<uint64_t>(pickle) << std::endl;  // Will be equal to desiredPos after the (2 ** i)th shuffle.
        assert(pickleVerify.value() == pickle);
    }
    std::cout << "X: " << static_cast<int64_t>(X) << " Y: " << static_cast<int64_t>(Y) << std::endl;

    __uint128_t pickle = 0;
    pickle = (static_cast<__uint128_t>(X) * initialDesiredPos) % numCards;
    pickle = (pickle + Y) % numCards;
    std::cout << "haggis: " << static_cast<uint64_t>(pickle) << std::endl; 
}
