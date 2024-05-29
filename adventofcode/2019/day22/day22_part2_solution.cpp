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
const uint64_t numShuffles = 101'741'582'076'661ULL;
//const uint64_t numShuffles = 10'123ULL;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{(n + modulus) % modulus}
        {
            assert(m_n >= 0);
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

ModNum operator-(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result -= rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return (lhs.value() == rhs.value());
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
    ModNum baseToPower = base; // As we iterate, goes through base, base^2, (base^2)^2, etc.
    int64_t power = 0;
    while (exponent > 0)
    {
        if (exponent & 1)
        {
            result = (result * baseToPower);
        }
        exponent >>= 1;
        baseToPower *= baseToPower;
    }
    return result;
}

class Matrix2x2
{
    public:
        Matrix2x2(ModNum a11, ModNum a12, ModNum a21, ModNum a22)
            : m_a11{a11}, m_a12{a12}, m_a21{a21}, m_a22{a22}
        {
        }
        Matrix2x2& operator*=(const Matrix2x2& other)
        {
            ModNum newA11 = m_a11 * other.m_a11 + m_a12 * other.m_a21;
            ModNum newA12 = m_a11 * other.m_a12 + m_a12 * other.m_a22;
            ModNum newA21 = m_a21 * other.m_a11 + m_a22 * other.m_a21;
            ModNum newA22 = m_a21 * other.m_a12 + m_a22 * other.m_a22;
            m_a11 = newA11;
            m_a12 = newA12;
            m_a21 = newA21;
            m_a22 = newA22;

            return *this;
        }
        ModNum a11() const
        {
            return m_a11;
        }
        ModNum a12() const
        {
            return m_a12;
        }
        ModNum a21() const
        {
            return m_a21;
        }
        ModNum a22() const
        {
            return m_a22;
        }
    private:
        ModNum m_a11; 
        ModNum m_a12; 
        ModNum m_a21; 
        ModNum m_a22;
};

Matrix2x2 operator*(const Matrix2x2& lhs, const Matrix2x2& rhs)
{
    Matrix2x2 result(lhs);
    result *= rhs;
    return result;
}

Matrix2x2 quickMatrixExponentiation(Matrix2x2 base, int64_t exponent)
{
    // Raise base to the exponent using as few multiplications as 
    // we can e.g. base ^ 8 ==  (((base^2)^2)^2).
    Matrix2x2 result(1, 0,
                     0, 1);
             
    Matrix2x2 baseToPower = base; // As we iterate, goes through base, base^2, (base^2)^2, etc.
    while (exponent > 0)
    {
        if (exponent & 1)
        {
            result *= baseToPower;
        }
        exponent >>= 1;
        baseToPower *= baseToPower;
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
    const int64_t initialDesiredPos = 2020;
    ModNum desiredPos = initialDesiredPos;
    ModNum XYay;
    ModNum YYay;
    //const int numShuffles = 256;
    //for (int i = 0; i < numShuffles; i++)
    {
        ModNum X(1);
        ModNum Y(0);
        for (const auto instruction : instructions)
        {
            switch (instruction.type)
            {
                case Instruction::DealWithIncrement:
                    {
                        const ModNum inverseAmount = quickPower2(instruction.amount, numCards - 2);
                        //std::cout << "amount: " << instruction.amount << " inverseAmount: " << inverseAmount << " multiplied:" << (static_cast<__uint128_t>(inverseAmount) * static_cast<__uint128_t>(instruction.amount)) << " modulus: " << ((inverseAmount * instruction.amount) % numCards) << std::endl;
                        assert(inverseAmount * instruction.amount == 1);
                        desiredPos = desiredPos * inverseAmount;
                        X = X * inverseAmount;
                        Y = Y * inverseAmount;
                    }
                    break;
                case Instruction::Cut:
                    desiredPos += instruction.amount;
                    Y += instruction.amount;
                    break;
                case Instruction::DealIntoNewDeck:
                    desiredPos = numCards - 1 - desiredPos;
                    X = X * (numCards - 1);
                    Y = Y * (numCards - 1) + (numCards - 1);
                    break;
                default:
                    assert(false);
            }
            assert((X * initialDesiredPos + Y) == desiredPos);
        }
        XYay = X;
        YYay = Y;
        ModNum pickle = 0;
        pickle = X * initialDesiredPos + Y;
        std::cout << "noodles: " << pickle << std::endl; 
        //std::cout << "desiredPos after shuffle: # " << (i + 1) << " : " << desiredPos << std::endl;
        std::cout << "desiredPos after shuffle: " << desiredPos << std::endl;
        //assert(pickle == desiredPos);
        std::cout << "X: " << X << std::endl;
        std::cout << "Y: " << Y << std::endl;
        //std::cout << "XVerify: " << XVerify << std::endl;
        //std::cout << "YVerify: " << YVerify << std::endl;
        //assert(XVerify.value() == X);
        //assert(YVerify.value() == Y);
    }
    std::cout << "desiredPos: " << desiredPos << std::endl;
    //__uint128_t pickle = 0;
    //pickle = (static_cast<__uint128_t>(X) * initialDesiredPos) % numCards;
    //pickle = (pickle + Y) % numCards;
    //std::cout << "haggis: " << static_cast<uint64_t>(pickle) << std::endl; 
    std::cout << "XYay: " << XYay << " YYay: " << YYay << std::endl;
    ModNum X(XYay);
    ModNum Y(YYay);
    //for (int i = 0; i < numShuffles - 1; i++)
    {
        ModNum Xnew = X * X;
        ModNum Ynew = X * Y + Y;
        X = Xnew;
        Y = Ynew;

        ModNum pickle = X * initialDesiredPos + Y;
        //std::cout << "interim haggis # " << i << ": " << pickle << std::endl;  // Will be equal to desiredPos after the (2 ** i)th shuffle.
    }
    std::cout << "X: " << X << " Y: " << Y << std::endl;

    //assert(XYay * initialDesiredPos + YYay == desiredPos);
    const Matrix2x2 posSingleShuffleMatrix(XYay, YYay, 
                                           0, 1);
    //const int power = numShuffles;
    //for (int i = 0; i < power; i++)
    //{
    //    posShuffleMatrix *= posSingleShuffleMatrix;
    //}
    const Matrix2x2 initialPosMatrix(initialDesiredPos, 0, 
                               1, 0);
    //posShuffleMatrix *= initialPosMatrix;
    std::cout << endl;
    const Matrix2x2 posShuffleMatrix = quickMatrixExponentiation(posSingleShuffleMatrix, numShuffles);
    const Matrix2x2 finalResultMatrix = posShuffleMatrix * initialPosMatrix;
    cout << "blah a11: " << posShuffleMatrix.a11() << std::endl;
    cout << "blah a12: " << posShuffleMatrix.a12() << std::endl;
    cout << "blah a21: " << posShuffleMatrix.a21() << std::endl;
    cout << "blah a22: " << posShuffleMatrix.a22() << std::endl;
    std::cout << endl;
    //cout << "glap a11: " << gloob.a11() << std::endl;
    //cout << "glap a12: " << gloob.a12() << std::endl;
    //cout << "glap a21: " << gloob.a21() << std::endl;
    //cout << "glap a22: " << gloob.a22() << std::endl;
    //std::cout << endl;

    //Matrix2x2 glarb(1, 0,
    //                0 ,1);
    //Matrix2x2 blobe(3, 4,
    //                11 ,97);
    //glarb*= blobe;

    //cout << "blah a11: " << glarb.a11() << std::endl;
    //cout << "blah a12: " << glarb.a12() << std::endl;
    //cout << "blah a21: " << glarb.a21() << std::endl;
    //cout << "blah a22: " << glarb.a22() << std::endl;
    std::cout << endl;
    ModNum pickle = 0;
    pickle = (X * initialDesiredPos) + Y;
    std::cout << "haggis: " << pickle << std::endl; 
    std::cout << "Final result: " << finalResultMatrix.a11() << std::endl;
    std::cout << "desiredPos: " << desiredPos << std::endl;
    //assert(finalResultMatrix.a11() == desiredPos);
}
