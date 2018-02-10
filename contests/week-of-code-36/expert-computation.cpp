// Simon St James (ssjgz) - 2018-02-10
#define SUBPROBLEM
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <sys/time.h>
#include <cassert>

using namespace std;

const int64_t M = 1000000007UL;

int derangementsFound = 0;

int64_t calcF(const int i, const vector<int64_t>& h, const vector<int64_t>& c, const vector<int64_t>& l, bool quiet = false)
{
    if (!quiet)
        cout << "calcF : i: " << i << " c[i]: " << c[i] << " h[i]: " << h[i] << endl;
    int64_t maxInRange = numeric_limits<int64_t>::min();
    int maxJ = -1;
    const int jLimit = i - l[i];
    cout << "i: " << i << " jLimit: " << jLimit << " l[i]: " << l[i] << endl;
    assert(jLimit >= 1);
    for (int j = jLimit; j >= 1; j--)
    {
        const int64_t blah = h[j] * c[i] - c[j] * h[i];
        if (maxJ != -1)
        {
            if (c[j] < c[maxJ] && c[i] * (h[maxJ] - h[j]) < h[i] * (c[j] - c[maxJ]))
            //if (c[j] < c[jLimit] && c[i] * (h[jLimit] - h[j]) >= h[i] * (c[j] - c[jLimit]))
            {
                cout << "Floop" << endl;
                assert(h[maxJ] - h[j] >= 0);
                cout << "blah: " << blah << " maxInRange: " << maxInRange << endl;
                assert(blah >= maxInRange);
            }

            if (c[j] < c[maxJ])
            {
                cout << "Bleep: " << (c[i] * (h[maxJ] - h[j]) + h[i] * (c[j] - c[maxJ])) << endl;
                cout << "Bleep2: " << (h[maxJ] * c[i] - c[maxJ] * h[i]) - (h[j] * c[i] - c[j] * h[i]) << endl;
                cout << "Bleep3: " << h[maxJ] * c[i] - c[maxJ] * h[i] - h[j] * c[i] + c[j] * h[i] << endl;
                cout << "Bleep3: " << c[i] * (h[maxJ] - h[j]) - c[maxJ] * h[i]  + c[j] * h[i] << endl;
                cout << "Bleep3: " << c[i] * (h[maxJ] - h[j]) - h[i] * (c[maxJ]  - c[j]) << endl;
                const auto bleep = c[i] * (h[maxJ] - h[j]) - h[i] * (c[maxJ]  - c[j]);
                const auto lhs = c[i] * (h[maxJ] - h[j]);
                const auto rhs = h[i] * (c[maxJ]  - c[j]);
                if (lhs < rhs)
                {
                    cout << "lhs: " << lhs << " rhs: " << rhs << endl;
                    assert(blah > maxInRange);
                }
            }
        }
        if (blah > maxInRange)
        {
            if (maxJ != -1)
            {
                assert(c[j] < c[maxJ]);
                //assert(c[i] * (h[maxJ] - h[j]) >= h[i] * (c[j] - c[maxJ]));
                assert(h[maxJ] - h[j] > 0);
                assert(c[i] * (h[maxJ] - h[j]) >= h[i] * (c[j] - c[maxJ]));
                const auto lhs = c[i] * (h[maxJ] - h[j]);
                const auto rhs = h[i] * (c[maxJ]  - c[j]);
                assert(lhs < rhs);
            }
            maxInRange = blah;
            maxJ = j;
            assert((maxJ == jLimit) ||  (c[i] * (h[jLimit] - h[maxJ]) > h[i] * (c[maxJ] - c[jLimit])));
        }
        else
        {
            assert(j != maxJ);
            assert(maxJ != -1);
            //assert(c[i] * (h[jLimit] - h[maxJ]) > h[i] * (c[maxJ] - c[jLimit]));
            assert(c[i] * (h[j] - h[maxJ]) <=  h[i] * (c[j] - c[maxJ]));
        }
        if (!quiet)
            cout << " j: " << j << " blah: " << blah << " maxInRange: " << maxInRange << " maxJ: " << maxJ << " h[j]: " << h[j] << " c[j]: " << c[j] << endl;
    }

    if (maxJ != jLimit)
    {
        assert(c[maxJ] <= c[jLimit]);
        cout << "woo fleep!" << endl;
        //assert(c[i] * (h[jLimit] - h[maxJ]) > h[i] * (c[maxJ] - c[jLimit]));
        //assert(h[i] * c[maxJ] + c[i] * h[maxJ] == c[i] * h[jLimit] + h[i] * c[jLimit]);
    }
    else
    {
        cout << "wee fleep!" << endl;
        assert(c[i] * (h[jLimit] - h[maxJ]) == h[i] * (c[maxJ] - c[jLimit]));
    }

    int64_t currentC = numeric_limits<int64_t>::max();
    int64_t dbgMaxInRange = numeric_limits<int64_t>::min();
    for (int j = jLimit; j >= 1; j--)
    {
        if (c[j] >= currentC)
            continue;
        currentC = c[j];
        const int64_t blah = h[j] * c[i] - c[j] * h[i];
        //cout << " Checking at j: " << j << endl;
        if (blah > dbgMaxInRange)
        {
            dbgMaxInRange = blah;
            cout << "dbgMaxInRange changed to " << dbgMaxInRange << " at index: " << j << endl;
        }
    }
    cout << "dbgMaxInRange: " << dbgMaxInRange << " maxInRange: " << maxInRange << endl;
    assert(dbgMaxInRange == maxInRange);


#if 0
    for (int j = 1; j <= jLimit; j++)
    {
        if (h[i] * c[j] + c[i] * h[j] == c[i] * h[jLimit] + h[i] * c[jLimit])
        {
            assert(h[j] * c[i] - c[j] * h[i] == maxInRange);
        }
    }
#endif

    return maxInRange;
}

void solveSubproblem(int n, const vector<int64_t>& h, const vector<int64_t>& c, const vector<int64_t>& l)
{
    for (int i = 1; i <= n; i++)
    //const int i = 10000;
    {
        const auto f = calcF(i, h, c, l);
        cout << " solveSubproblem i: " << i << " i - l[i]: " << (i - l[i]) << " f: " << f << endl;
    }
}


vector<int> makeRandomChoices(int n, int maxValue)
{
    vector<int> toChooseFrom;
    for (int i = 1; i <= maxValue; i++)
    {
        toChooseFrom.push_back(i);
    }

    vector<int> choices;
    int numRemainingChoices = maxValue;
    for (int i = 1; i <=n; i++)
    {
        int choiceIndex = rand() % numRemainingChoices;
        //cout << " blee: " << (maxValue - numRemainingChoices + choiceIndex) << " toChooseFrom.size(): " << toChooseFrom.size() << endl;
        const int choice = toChooseFrom[maxValue - numRemainingChoices + choiceIndex];
        swap(toChooseFrom[maxValue - numRemainingChoices], toChooseFrom[maxValue - numRemainingChoices + choiceIndex]);
        
        choices.push_back(choice);
        

        numRemainingChoices--;
    }

    return choices;
}

void blibble(const int i, const vector<int64_t>& h, const vector<int64_t>& c, const vector<int64_t>& l, bool quiet = false)
{
    int64_t maxInRange = numeric_limits<int64_t>::min();
    int maxJ = -1;
    static int oldMaxJ = -1;
    for (int j = 1; j <= i; j++)
    {
        const int64_t blah = h[j] * c[i] - c[j] * h[i];
        if (blah > maxInRange)
        {
            maxInRange = blah;
            maxJ = j;
        }

        if (!quiet)
            cout << "blibble j: " << j << " blah: " << blah << " maxInRange: " << maxInRange << " maxJ: " << maxJ << endl;
    }

    cout << "i: " << i << " maxJ: " << maxJ << endl;
    if (maxJ < oldMaxJ)
    {
        cout << "******** oldMaxJ: " << oldMaxJ << " maxJ: " << maxJ << " i: " << i << endl;
        derangementsFound++;
    }
    oldMaxJ = maxJ;
}

bool generateTestcaseAux(int n)
{
    vector<int64_t> h(n + 1);
    vector<int64_t> c(n + 1);
    vector<int64_t> l(n + 1);

    const int maxThing = 1000000;
    //const int maxThing = 2000;
#ifdef SUBPROBLEM

    {
        cout << n << endl;
        auto choicesForH = makeRandomChoices(n + 1, maxThing);
        sort(choicesForH.begin(), choicesForH.end());

        //int currentH = rand() % 10;
        for (int i = 1; i <= n; i++)
        {
            cout << choicesForH[i] << " ";
            //cout << i << " ";
        }
        cout << endl;

        int currentC = rand() % 10;
        for (int i = 1; i <= n; i++)
        {
            cout << rand() % maxThing + 1 << " ";
            //currentC += rand() % 500;
            //if (i <= 1000)
            //{
                //cout << currentC << " ";
                //cout << (i + 1) << " ";
            //}
            //else
            //{
                //cout << rand() % maxThing + 1 + 500'000 << " ";
                //cout << 500'000 + i << " ";
            //}

        }
        cout << endl;

        for (int i = 1; i <= n; i++)
        {
            cout << rand() % i << " ";
        }
        cout << endl;

    }

    return true;
#endif
    //cout << "Trying to generate a testcase of length " << n << endl;
    vector<int64_t> x(n + 1);
    vector<int64_t> y(n + 1);
    vector<int64_t> z(n + 1);

    const int maxRandomNumber = 100;
    y[1] = rand() % maxRandomNumber + 1;
    z[1] = 0;

    c[1] = y[1];
    l[1] = z[1];

    vector<int64_t> F(n + 1);
    F[1] = calcF(1, h, c, l, true);
    vector<int64_t> G(n + 1);
    G[1] = F[1];


    auto choicesForH = makeRandomChoices(n + 1, maxThing);
    sort(choicesForH.begin(), choicesForH.end());
    x[1] = choicesForH[1];
    h[1] = x[1];

    vector<int> randomNumbers(maxRandomNumber + 1);
    for (int i = 0; i < maxRandomNumber; i++)
    {
        randomNumbers[i] = i;
    }

    for (int i = 2; i <= n; i++)
    {
#if 0
        bool valid = false;
        random_shuffle(randomNumbers.begin(), randomNumbers.end());
        for (int j = 0; j < maxRandomNumber; j++)
        {
            //x[i] = randomNumbers[j];
            const auto nextH = h[i - 1] + randomNumbers[j];
            x[i] = nextH ^ G[i - 1];
            h[i] = x[i] ^ G[i - 1];
            if (1 <= h[i] && h[i] <= 10'000'000 && h[i] > h[i - 1])
            {
                valid = true;
                break;
            }
        }
        if (!valid)
            return false;
        valid = false;
        random_shuffle(randomNumbers.begin(), randomNumbers.end());
        for (int j = 0; j <maxRandomNumber; j++)
        {
            y[i] = randomNumbers[j];
            //const auto nextC = rand() % (1000) + 1;
            //y[i] = nextC ^ G[i - 1];
            c[i] = y[i] ^ G[i - 1];
            if (1 <= c[i] && c[i] <= 10'000'000)
            {
                valid = true;
                break;
            }
        }
        if (!valid)
            return false;
        valid = false;
        random_shuffle(randomNumbers.begin(), randomNumbers.end());
        for (int j = 0; j < maxRandomNumber; j++)
        {
            //z[i] = randomNumbers[j];
            const int nextL = randomNumbers[j] % i;
            z[i] = nextL ^ G[i - 1];
            l[i] = z[i] ^ G[i - 1];
            if (0 <= l[i] && l[i] <= i - 1)
            {
                valid = true;
                break;
            }
        }
        if (!valid)
            return false;
#endif
        const int nextH = choicesForH[i];
        //cout << "lastH: " << h[i - 1] << " nextH: " << nextH << endl;
        x[i] = G[i - 1] ^ nextH;
        const int nextC = rand() % maxThing + 1;
        y[i] = G[i - 1] ^ nextC;
        const int nextL = rand() % i;
        z[i] = G[i - 1] ^ nextL;
        


        h[i] = x[i] ^ G[i - 1];
        c[i] = y[i] ^ G[i - 1];
        l[i] = z[i] ^ G[i - 1];

        F[i] = calcF(i, h, c, l, true);
        //if (F[i] >= 0)
            //return false;
        if (F[i] < 0)
        {
            const auto blee = (F[i] / -M) + 1;
            F[i] += blee * M;
        }

        G[i] = (G[i - 1] + F[i]) % M;

        //cout << "i: " << i << " h: " << h[i] << " c: " << c[i] << " l: " << l[i] <<  " F: " << F[i] << " G: " << G[i] << endl;
        assert(l[i] <= i - 1);
        assert(F[i] >= 0);
        assert(1 <= c[i] && c[i] <= 10'000'000);
        assert(1 <= h[i] && h[i] <= 10'000'000);
        assert(h[i] > h[i - 1]);
    }

#if 0
    int numNonZeroZs = 0;
    for (const auto a : z)
    {
        if (a != 0)
            numNonZeroZs++;
    }
    if (numNonZeroZs < 2)
        return false;
#endif

    cout << n << endl;
#ifndef SUBPROBLEM
    for (int i = 1; i <= n; i++)
        cout << x[i] << " ";
    cout << endl;
    for (int i = 1; i <= n; i++)
        cout << y[i] << " ";
    cout << endl;
    for (int i = 1; i <= n; i++)
        cout << z[i] << " ";
    cout << endl;
#else
    for (int i = 1; i <= n; i++)
        cout << h[i] << " ";
    cout << endl;
    for (int i = 1; i <= n; i++)
        cout << c[i] << " ";
    cout << endl;
    for (int i = 1; i <= n; i++)
        cout << l[i] << " ";
    cout << endl;

#endif

    return true;
}

void generateTestcase()
{
    //const int n = rand() % 7 + 2;
    //const int n = rand() % 100 + 1;
    const int n = 100000;
    while (true)
    {
        if (generateTestcaseAux(n))
            return;
    }
}


int main(int argc, char** argv)
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        generateTestcase();
        return 0;
    }

    int n;
    cin >> n;

    vector<int64_t> h(n + 1);
    vector<int64_t> c(n + 1);
    vector<int64_t> l(n + 1);

    vector<int64_t> x(n + 1);
    vector<int64_t> y(n + 1);
    vector<int64_t> z(n + 1);

#ifndef SUBPROBLEM
    for (int i = 1; i <= n; i++)
        cin >> x[i];
    for (int i= 1; i <= n; i++)
        cin >> y[i];
    for (int i = 1; i <= n; i++)
        cin >> z[i];

    h[1] = x[1];
    c[1] = y[1];
    l[1] = z[1];
#endif

#ifdef SUBPROBLEM
    for (int i = 1; i <= n; i++)
        cin >> h[i];
    for (int i= 1; i <= n; i++)
        cin >> c[i];
    for (int i = 1; i <= n; i++)
        cin >> l[i];

    cout << "h: " << endl;
    for (int i = 1; i <= n; i++)
        cout << h[i] << " ";
    cout << endl;
    cout << "c: " << endl;
    for (int i = 1; i <= n; i++)
        cout << c[i] << " ";
    cout << endl;
    cout << "l: " << endl;
    for (int i = 1; i <= n; i++)
    {
        cout << l[i] << " ";
        assert(l[i] <= i - 1);
    }
    cout << endl;


    solveSubproblem(n, h, c ,l);
    return 0;
#endif


    vector<int64_t> F(n + 1);
    F[1] = calcF(1, h, c, l);
    vector<int64_t> G(n + 1);
    G[1] = F[1];

    //vector<int64_t> maxHCCH(n);
    //maxHCCH[0] = h[0] 

    //cout << "M: " << M << endl;

    {
        int i = 1;
        //cout << "i: " << i << " h: " << h[i] << " c: " << c[i] << " l: " << l[i] <<  " F: " << F[i] << " G: " << G[i] << endl;
    }
    for (int i = 2; i <= n; i++)
    {
        h[i] = x[i] ^ G[i - 1];
        c[i] = y[i] ^ G[i - 1];
        l[i] = z[i] ^ G[i - 1];

        F[i] = calcF(i, h, c, l);
        blibble(i, h, c, l);
        if (F[i] < 0)
        {
            cout << "F was -ve" << endl;
            const auto blee = (F[i] / -M) + 1;
            F[i] += blee * M;
        }

        G[i] = (G[i - 1] + F[i]) % M;

        cout << "i: " << i << " h: " << h[i] << " c: " << c[i] << " l: " << l[i] <<  " F: " << F[i] << " G: " << G[i] << endl;
        assert(l[i] <= i - 1);
        assert(F[i] >= 0);
        assert(1 <= c[i] && c[i] <= 10'000'000);
        assert(1 <= h[i] && h[i] <= 10'000'000);
        assert(h[i] > h[i - 1]);
    }

    cout << G[n] << endl;

    //assert(!assertOnExit);
    assert(derangementsFound < 4);

}
