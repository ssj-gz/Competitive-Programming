// Simon St James (ssjgz) - 2017-10-11 15:10
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

const int64_t mymodulus = 1000000007ULL;
//const int64_t mymodulus = 1061;
vector<int64_t> factorialLookup;
vector<int64_t> factorialInverseLookup;

class Solution
{
    public:
        vector<vector<int>> vectorContents;
        void stripEmpties()
        {
            auto vecIter = vectorContents.begin();
            while (vecIter != vectorContents.end())
            {
                if (vecIter->empty())
                {
                    vecIter = vectorContents.erase(vecIter);
                }
                else
                {
                    vecIter++;
                }
            }
        }
        void normalise()
        {
            auto vectorSize = [](const vector<int>& lhs, const vector<int>& rhs)
            {
                return lhs.size() < rhs.size();
            };
            sort(vectorContents.begin(), vectorContents.end(), vectorSize);
            while (true)
            {
                bool changeMade = false;
                for (int i = 1; i < vectorContents.size(); i++)
                {
                    if (vectorContents[i].size() == vectorContents[i - 1].size() &&
                        !vectorContents[i].empty() &&
                        vectorContents[i].front() < vectorContents[i - 1].front())
                    {
                        swap(vectorContents[i], vectorContents[i - 1]);
                        changeMade = true;
                    }
                }
                if (!changeMade)
                    break;
            }

        }
};

ostream& operator<<(ostream& os, const Solution& solution)
{
    os << "Solution: ";
    for (const auto& vec : solution.vectorContents)
    {
        //if (vec.empty())
            //continue;
        os << "[";
        for (const auto element : vec)
        {
            cout << element << " ";
        }
        os << "] ";
    }
    return os;
}

bool operator<(const Solution& lhs, const Solution& rhs)
{
    if (lhs.vectorContents.size() != rhs.vectorContents.size())
        return lhs.vectorContents.size() < rhs.vectorContents.size();
    Solution lhsCopy(lhs);
    Solution rhsCopy(rhs);
    lhsCopy.normalise();
    rhsCopy.normalise();

    //cout << "lhs: " << lhs << endl;
    //cout << "lhsCopy: " << lhsCopy << endl;
    //cout << "rhs: " << rhs << endl;
    //cout << "rhsCopy: " << rhsCopy << endl;


    //sort(lhsCopy.vectorContents.begin(), lhsCopy.vectorContents.end(), vectorSize);
    //sort(rhsCopy.vectorContents.begin(), rhsCopy.vectorContents.end(), vectorSize);

    for (int i = 0; i < lhsCopy.vectorContents.size(); i++)
    {
        if (lhsCopy.vectorContents[i].size() != rhsCopy.vectorContents[i].size())
            return lhsCopy.vectorContents[i].size() < rhsCopy.vectorContents[i].size();
    }

    for (int i = 0; i < lhsCopy.vectorContents.size(); i++)
    {
        assert(lhsCopy.vectorContents[i].size() == rhsCopy.vectorContents[i].size());
        if (lhsCopy.vectorContents[i] != rhsCopy.vectorContents[i])
            return lhsCopy.vectorContents[i] < rhsCopy.vectorContents[i];
    }

    return false;
}


bool isValidSolution(const Solution& solution, const vector<int>& array)
{
    //cout << "validating: " << solution << endl;
    int posInArray = 0;
    Solution solutionCopy(solution);
    while (!solutionCopy.vectorContents.empty())
    {
        vector<int> layer;
        auto vecIter = solutionCopy.vectorContents.begin();
        while (vecIter != solutionCopy.vectorContents.end())
        {
            auto& vec = *vecIter;
            if (!vec.empty())
            {
                layer.push_back(vec.front());
                vec.erase(vec.begin());
            }
            if (vec.empty())
            {
                vecIter = solutionCopy.vectorContents.erase(vecIter);
            }
            else
            {
                vecIter++;
            }
        }
        sort(layer.begin(), layer.end());
        assert(!layer.empty());
        //cout << " sorted layer: " << endl << " ";
        //for (const auto element : layer)
        //{
            //cout << element << " ";
        //}
        //cout << endl;
        for (const auto element : layer)
        {
            assert(posInArray < array.size());
            if (array[posInArray] != element)
            {
                //cout << " not valid" << endl;
                return false;
            }
            posInArray++;
        }
    }
    //cout << " valid" << endl;
    return true;
}
int64_t bruteForce(const vector<int>& array)
{
    set<Solution> solutions;
    vector<int> arrayCopy(array);
    //sort(arrayCopy.begin(), arrayCopy.end());
    while (true)
    {
        for (int numVectors = 1; numVectors <= arrayCopy.size(); numVectors++)
        {
            vector<int> vecIndexForarrayCopyElement(arrayCopy.size());
            while (true)
            {
                Solution solution;
                solution.vectorContents.resize(numVectors);
                //cout << "Candidate solution: " << endl;
                for (int i = 0; i < vecIndexForarrayCopyElement.size(); i++)
                {
                    //cout << "arrayCopy element " << arrayCopy[i] << " placed in vector: " << vecIndexForarrayCopyElement[i] << endl;
                    solution.vectorContents[vecIndexForarrayCopyElement[i]].push_back(arrayCopy[i]);
                }
                solution.stripEmpties();
                if (isValidSolution(solution, array))
                {
                    solutions.insert(solution);
                    //cout << "adding solution " << solution << endl;
                }
                int arrayCopyIndex = 0;
                while (arrayCopyIndex != arrayCopy.size() && vecIndexForarrayCopyElement[arrayCopyIndex] == numVectors - 1)
                {
                    vecIndexForarrayCopyElement[arrayCopyIndex] = 0;
                    arrayCopyIndex++;
                }
                if (arrayCopyIndex == arrayCopy.size())
                    break;
                vecIndexForarrayCopyElement[arrayCopyIndex]++;

            }
        }
        //if (!next_permutation(arrayCopy.begin(), arrayCopy.end()))
            break;
    }
    for (const auto& solution : solutions)
    {
        cout << "Distinct solution: " << solution << endl;
    }
    return solutions.size();
}

int64_t factorial(int64_t n)
{
    assert(n >= 0 && n < factorialLookup.size());
    return factorialLookup[n];
}

int64_t nCr(int64_t n, int64_t r)
{
    //cout << "nCr n: " << n << " r: " << r << endl;
    //if ()
        //swap(n, r);
    assert(n >= 0 && r >= 0);
    assert(n >= r);
    int64_t result = factorial(n);
    //cout << "factorial(n)" << factorial(n) << endl;
    //cout << "factorialInverseLookup(r)" << factorialInverseLookup[r] << endl;
    //cout << "factorialInverseLookup(n - r)" << factorialInverseLookup[n - r] << endl;
    assert(r < factorialInverseLookup.size());
    assert(n - r < factorialInverseLookup.size());
    result = (result * factorialInverseLookup[n - r]) % mymodulus;
    result = (result * factorialInverseLookup[r] ) % mymodulus;
    //cout << " nCr " << n << "," << r << " = " << result << endl;
    return result;
}

int64_t quickPower(int64_t n, int64_t m)
{
    // Raise n to the m mod modulus using as few multiplications as 
    // we can e.g. n ^ 8 ==  (((n^2)^2)^2).
    int64_t result = 1;
    int64_t power = 0;
    while (m > 0)
    {
        if (m & 1)
        {
            int64_t subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % mymodulus;
            }
            result = (result * subResult) % mymodulus;
        }
        m >>= 1;
        power++;
    }
    return result;
}

vector<vector<vector<int64_t>>> lookup;

int64_t solve2(vector<int>& array, const int posInArray, bool isFirst, int remaining, int layerSize, string indent = string())
{
    //cout << "indent.size(): " << indent.size() << endl;
    assert(remaining >= 0 && remaining < lookup[0].size());
    assert(layerSize >= 1 && layerSize < lookup[0][0].size());
    //cout << indent << "remaining: " << remaining << " posInArray: " << posInArray << " layerSize: " << layerSize << " isFirst: " << isFirst << endl;
    if (remaining == 0)
    {
        //cout << indent << "returning 1 as remaining == 0" << endl;
        return 1;
    }
    if (remaining < layerSize)
    {
        //cout << indent << "returning 0 as remaining < layerSize" << endl;
        return 0; 
    }
    if (lookup[isFirst][remaining][layerSize] != -1)
        return lookup[isFirst][remaining][layerSize];

    int64_t result = 0;
    bool layerIsInOrder = true;
    for (int i = posInArray + 1; i < posInArray + layerSize; i++)
    {
        assert(i < array.size());
        assert(i - 1 >= 0);
        if (array[i] < array[i - 1])
        {
            //cout << indent << "returning 0 as the next layerSize are not in order" << endl;
            layerIsInOrder = false;
            break;
        }
    }
    if (layerIsInOrder)
    {
        int64_t numPermutationsForThisLayer = 1;
        if (!isFirst)
        {
            numPermutationsForThisLayer = factorial(layerSize);
        }
        if (remaining != layerSize)
        {
            for (int nextLayerSize = layerSize; nextLayerSize >= 1; nextLayerSize--)
            {
                //int64_t contributionFromFirstLayer = (isFirst ? 1 : nCr(layerSize, layerSize - nextLayerSize));
                auto contributionFromFirstLayer = nCr(layerSize, layerSize - nextLayerSize);
                const auto blah = (numPermutationsForThisLayer * contributionFromFirstLayer) % mymodulus;
                const auto subresult = solve2(array, posInArray + layerSize, false, remaining - layerSize, nextLayerSize, indent + " ");
                const auto blah2 = (blah * subresult) % mymodulus;
                //cout << indent << "trying next layer size: " << nextLayerSize << " numPermutationsForThisLayer: " << numPermutationsForThisLayer << " contributionFromFirstLayer: " << contributionFromFirstLayer << endl;
                result = (result + blah2) % mymodulus;
                //result = (result + (((numPermutationsForThisLayer * contributionFromFirstLayer) % mymodulus) *  % mymodulus)) % mymodulus;
            }
        }
        else
        {
            //cout << indent << "returning " << numPermutationsForThisLayer << " as remaining == layerSize " << numPermutationsForThisLayer << endl;
            result = numPermutationsForThisLayer;
        }
    }
    else
    {
        //cout << "layer not in order" << endl;
    }
    //cout << indent << "returning " << result << endl;
    assert(result >= 0 && result < mymodulus);
    assert(lookup[isFirst][remaining][layerSize] == -1);
    lookup[isFirst][remaining][layerSize] = result;
    return result;

}

int main()
{
    int M;
    cin >> M;

    vector<int> array(M);

    for (int i = 0; i < M; i++)
    {
        cin >> array[i];
    }

    lookup.resize(2, vector<vector<int64_t>>(M + 1, vector<int64_t>(M + 1, -1)));

    factorialLookup.resize(M + 1);
    factorialInverseLookup.resize(M + 1);
    factorialLookup[0] = 1;
    factorialInverseLookup[0] = 1;
    int64_t factorial = 1;
    for (int64_t i = 1; i <= M; i++)
    {
        factorial = (factorial * i) % mymodulus;
        //cout << "factorial: " << factorial << endl;
        factorialLookup[i] = factorial;
        const auto factorialInverse = quickPower(factorial, mymodulus - 2);
        assert((factorial * factorialInverse) % mymodulus == 1);
        assert(factorialInverse >= 0 && factorialInverse < mymodulus);
        factorialInverseLookup[i] = factorialInverse;
    }

    int64_t result = 0;
    for (int i = 1; i <= M; i++)
    {
        result = (result + solve2(array, 0, true, M, i)) % mymodulus;
    }
    //const auto bruteForceResult  = bruteForce(array);
    //cout << "brute force: " << bruteForceResult << endl;
    //cout << "result: " << result << endl;
    //cout << "mymodulus: " << mymodulus << endl;
    cout << result << endl;
}

