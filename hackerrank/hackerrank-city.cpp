// Simon St James (ssjgz) - 2017-10-12 11:42
#include <iostream>
#include <vector>

using namespace std;

const int64_t modulus = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n)
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

int main()
{
    int N;
    cin >> N;

    vector<int64_t> A(N);
    for (int i = 0; i < N; i++)
    {
        cin >> A[i];
    }

    ModNum numNodes = 1;
    ModNum sumOfAllPaths = 0;
    ModNum sumOfPathsToCorner = 0;
    ModNum sumCornerToCorner = 0;


    for (int i = 0; i < N; i++)
    {
        //cout << "i: " << i << " A[i]: " << A[i] << endl;
        //cout << "numNodes: " << numNodes << endl;
        //cout << "sumOfAllPaths: " << sumOfAllPaths << endl;
        //cout << "sumOfPathsToCorner: " << sumOfPathsToCorner << endl;
        //cout << "sumCornerToCorner: " << sumCornerToCorner << endl;
        const ModNum newNumNodes = 4 * numNodes + 2;
        const ModNum newSumOfAllPaths = 4 * sumOfAllPaths + 
            4 * (3 * numNodes * A[i] + 2 * sumOfPathsToCorner) + 
            2 * 6 * numNodes * sumOfPathsToCorner + 
            (3 + 2 + 3 + 3 + 2 + 3) * (A[i] * numNodes * numNodes) +
            A[i];
        const ModNum newSumOfPathsToCorner = 4 * sumOfPathsToCorner + 3 * A[i] + 2 * sumCornerToCorner + numNodes * ((3 + 3 + 2) * A[i] + 3 * sumCornerToCorner);
        const ModNum newSumCornerToCorner = 2 * sumCornerToCorner + 3 * A[i];

        //cout << "newNumNodes: " << newNumNodes << endl;
        //cout << "newSumOfAllPaths: " << newSumOfAllPaths << endl;
        //cout << "newSumOfPathsToCorner: " << newSumOfPathsToCorner << endl;
        //cout << "newSumCornerToCorner: " << newSumCornerToCorner << endl;


        numNodes = newNumNodes;
        sumOfAllPaths = newSumOfAllPaths;
        sumOfPathsToCorner = newSumOfPathsToCorner;
        sumCornerToCorner = newSumCornerToCorner;
    }

    cout << sumOfAllPaths << endl; 

}
