#include <iostream>
#include <vector>
#include <regex>
#include <charconv>
#include <cassert>

#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.

using namespace std;

const uint64_t numCards = 119'315'717'514'047ULL;
const uint64_t numShuffles = 101'741'582'076'661ULL;

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

template <typename Type>
Type quickPower(Type initialValue, Type base, int64_t exponent)
{
    // Raise base to the exponent mod modulus using as few multiplications as 
    // we can e.g. base ^ 8 ==  (((base^2)^2)^2).
    Type result = initialValue;
    Type baseToPower = base; // As we iterate, goes through base, base^2, (base^2)^2, etc.
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

    // Read and store the instructions.
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

    // Basic algorithm: run the instructions *in reverse*, and for each instruction,
    // see how the desiredPos (initially 2020) changes (hopefully clear from the code
    // how it changes at each instruction).
    //
    // This will allow us to easily compute the card that would eventually be at position 2020
    // *after a single shuffle*.
    //
    // This is not all that helpful as the number of shuffles is huge.  However, we can also observe
    // that desiredPos after the (reversed) shuffle can be expressed as:
    //
    //      desiredPos = X * 2020 + Y 
    //
    // (all modulo numCards, of course) where X and Y can be updated in-tandem with desiredPos; again, 
    // it's hopefully clear from the code how to update X and Y for each instruction.
    //
    // How does this help? Well, let P = initialDesiredPos = 2020 and observe that:
    //
    //   ┌ X Y ┐┌ P 0 ┐ = ┌ X*P+Y 0 ┐
    //   └ 0 1 ┘└ 1 0 ┘   └ 1     0 ┘
    //
    // (The matrix on the left is called posSingleShuffleMatrix in the code, and the
    // matrix it is multiplied by is called initialPosMatrix.)
    // 
    // We see that the resulting matrix looks very similar to initialPosMatrix, except
    // that the entry that was P is now X*P+Y i.e. the entry that was initialDesiredPos
    // is now desiredPos i.e. the result of the single shuffle.
    //
    // If we multiplied posSingleShuffleMatrix by this new matrix, the top-left entry
    // of the result would be desiredPos after *two* shuffles, etc.
    //
    // In general, if we compute posSingleShuffleMatrix^numShuffles and multiply it by
    // initialPosMatrix, the top-left entry in the result is the desiredPos after numShuffles
    // shuffles, which is what we want.
    //
    // We can easily compute posSingleShuffleMatrix^numShuffles using quickPower 
    // and so get the final result.
    const vector<Instruction> reversedInstructions(instructions.rbegin(), instructions.rend());
    const int64_t initialDesiredPos = 2020;
    ModNum desiredPos = initialDesiredPos; // We don't really need desiredPos; it's mainly used to check
                                           // that X & Y are correct i.e. fulfil X * initialDesiredPos + Y
                                           // == desiredPos.
    ModNum X(1);
    ModNum Y(0);
    {
        for (const auto instruction : reversedInstructions)
        {
            switch (instruction.type)
            {
                case Instruction::DealWithIncrement:
                    {
                        const ModNum inverseAmount = quickPower<ModNum>(1, instruction.amount, numCards - 2);
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
                    desiredPos = numCards - 1 - desiredPos; // Equivalent to multiplying by "-1" (i.e. numCards - 1), then adding (numCards - 1).
                    X = X * (numCards - 1);
                    Y = Y * (numCards - 1) + (numCards - 1);
                    break;
                default:
                    assert(false);
            }
            assert((X * initialDesiredPos + Y) == desiredPos); // Use desiredPos to check that the values of X & Y fulfil the required invariant.
        }
    }

    const Matrix2x2 initialPosMatrix(initialDesiredPos, 0, 
                                     1,                 0);
    const Matrix2x2 posSingleShuffleMatrix(X, Y, 
                                           0, 1);
    const Matrix2x2 identityMatrix(1, 0,
                                   0, 1);
    const Matrix2x2 posShuffleMatrix = quickPower(identityMatrix, posSingleShuffleMatrix, numShuffles);
    const Matrix2x2 finalResultMatrix = posShuffleMatrix * initialPosMatrix;
    std::cout << "Final result: " << finalResultMatrix.a11() << std::endl;
}
