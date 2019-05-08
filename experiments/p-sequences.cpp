#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;


const int64_t modulus = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % ::modulus;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % ::modulus;
            return *this;
        }
        int64_t value() const { return m_n; };
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

ModNum solutionOptimised(int N, int P)
{
    ModNum result = 0;

    vector<int> factorsOfP;
    for (int i = 1; i <= sqrt(P); i++)
    {
        if ((P % i) == 0)
        {
            factorsOfP.push_back(i);
        }
    }
    for (int i = sqrt(P); i >= 1; i--)
    {
        if ((P % i) == 0)
        {
            factorsOfP.push_back(P / i);
        }
    }
    factorsOfP.erase(std::unique(factorsOfP.begin(), factorsOfP.end()), factorsOfP.end());
    for (const auto x : factorsOfP)
    {
        cout << " factor of P: " << x << endl;
    }
    vector<vector<ModNum>> firstNEndingOnFactorIndex(N, vector<ModNum>(factorsOfP.size() + 1, 0));
    for (int i = 0; i < factorsOfP.size(); i++)
    {
        firstNEndingOnFactorIndex[0][i] = 1;
    }
    for (int i = 1; i < N; i++)
    {
        int lastFactorIndex = 0;
        int newFactorIndex = factorsOfP.size() - 1;
        firstNEndingOnFactorIndex[i][newFactorIndex] = firstNEndingOnFactorIndex[i - 1][lastFactorIndex];

        while (newFactorIndex >= 1)
        {
            lastFactorIndex++;
            newFactorIndex--;
            cout << "lastFactorIndex: " << lastFactorIndex << " newFactorIndex: " << newFactorIndex << " factorsOfP[lastFactorIndex] * factorsOfP[newFactorIndex] : " << factorsOfP[lastFactorIndex] * factorsOfP[newFactorIndex] << endl;
            assert(factorsOfP[lastFactorIndex] * factorsOfP[newFactorIndex] <= P);
            assert(lastFactorIndex == factorsOfP.size() - 1 || factorsOfP[lastFactorIndex + 1] * factorsOfP[newFactorIndex] > P);
        }
    }


    return result;
}

ModNum solutionBruteForce(int N, int P)
{
    ModNum result = 0;
    vector<vector<ModNum>> firstNEndingOnP(N, vector<ModNum>(P + 1, 0));
    for (int r = 0; r <= P; r++)
    {
        firstNEndingOnP[0][r] = 1;
    }
    for (int i = 1; i < N; i++)
    {
        for (int q = 1; q <= P; q++)
        {
            for (int r = 1; r * q <= P; r++)
            {
                firstNEndingOnP[i][q] += firstNEndingOnP[i - 1][r];
            }
            cout << "firstNEndingOnP[" << i << "][" << q << "] = " << firstNEndingOnP[i][q] << endl;
        }
    }

    for (int r = 0; r <= P; r++)
    {
        result += firstNEndingOnP[N - 1][r];
    }
    return result;
}


int main(int argc, char* argv[])
{
    int N;
    cin >> N;

    int P;
    cin >> P;

    const auto bruteForceResult = solutionBruteForce(N, P);
    cout << "bruteForceResult: " << bruteForceResult << endl;

    const auto optimisedResult = solutionOptimised(N, P);
    cout << "optimisedResult: " << optimisedResult << endl;

}
