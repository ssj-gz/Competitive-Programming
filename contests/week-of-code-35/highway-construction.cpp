// Simon St James (ssjgz) - 2017-11-17
// This is just a "correctness" submission - it's too slow to pass all testcase!
#include <iostream>

using namespace std;

int64_t quickPower(int64_t n, uint64_t k, int64_t m)
{
    int64_t result = 1;
    int64_t power = 0;
    while (k > 0)
    {
        if (k & 1)
        {
            int64_t subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % m;
            }
            result = (result * subResult) % m;
        }
        k >>= 1;
        power++;
    }
    return result;
}


constexpr auto modulus = 1'000'000'009ULL;

class Rational
{
    public:
        Rational(int64_t numerator, int64_t denominator = 1)
            : m_numerator{numerator}, m_denominator{denominator}
        {
        };
        int64_t numerator() const
        {
            return m_numerator;
        }
        int64_t denominator() const
        {
            return m_denominator;
        }
        Rational& operator+=(const Rational& other)
        {
            if (other.m_denominator == m_denominator)
            {
                m_numerator += other.m_numerator;
            }
            else
            {
                const int64_t newNumerator = m_numerator * other.m_denominator + other.m_numerator * m_denominator;
                const int64_t newDenominator = m_denominator * other.m_denominator;
                m_numerator = newNumerator;
                m_denominator = newDenominator;
            }
            return *this;
        }
        Rational& operator*=(const Rational& other)
        {
            m_numerator *= other.m_numerator;
            m_denominator *= other.m_denominator;
            return *this;
        }
        Rational& operator/=(const Rational& other)
        {
            m_numerator *= other.m_denominator;
            m_denominator *= other.m_numerator;
            return *this;
        }
    private:
        int64_t m_numerator;
        int64_t m_denominator;

};

Rational operator+(const Rational& lhs, const Rational& rhs)
{
    Rational result = lhs;
    result += rhs;
    return result;
}

Rational operator*(const Rational& lhs, const Rational& rhs)
{
    Rational result = lhs;
    result *= rhs;
    return result;
}

Rational operator/(const Rational& lhs, const Rational& rhs)
{
    Rational result = lhs;
    result /= rhs;
    return result;
}

ostream& operator<<(ostream& os, const Rational& rational)
{
    os << rational.numerator() << "/" << rational.denominator();
    return os;
}

int main()
{
    Rational r1{3, 4};
    Rational r2{5, 4};
    Rational r3{7, 6};

    cout << r1 << " + " << r2 << " = " << (r1 + r2) << endl;
    cout << r1 << " * " << r2 << " = " << (r1 * r2) << endl;
    cout << r1 << " / " << r2 << " = " << (r1 / r2) << endl;
    cout << r1 << " / " << r3 << " = " << (r1 / r3) << endl;
    cout << r2 << " + " << r3 << " = " << (r2 + r3) << endl;

    int q;
    cin >> q;

    for (int t = 0; t < q; t++)
    {
        int n, k;
        cin >> n >> k;

        //cout << "n: " << n << " k: " << k << endl;

        int64_t total = 0;

        for (int i = 2; i <= n - 1; i++)
        {
            //cout << "i : " << i << " i ^^ k: " << quickPower(i, k, ::modulus) << endl;
            //total = (total + (n - 1 - i) * quickPower(i, k, ::modulus)) % ::modulus;
            total = (total + quickPower(i, k, ::modulus)) % ::modulus;
            //cout << "i: " << i << " total: " << total << endl;
            cout << "i: " << i << " total: " << total << endl;
            
        }
        cout << total << endl;
    }

    const auto blah = quickPower(5, 8, ::modulus);
    const auto blah2 = quickPower(::modulus - 5, 8, ::modulus);
    cout << "blah: " << blah << " blah2: " << blah2 << " sum: " << (blah + blah2) << " sum mod: " << (blah + blah2) % ::modulus << endl;

}
