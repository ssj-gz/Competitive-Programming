// Simon St James (ssjgz) - 2019-09-07
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/LAPD
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}
struct Blah
{
    Blah(double x, double y, double z)
        : x(x), y(y), z(z)
    {
    }
    double x, y;
    double z;
};

bool operator<(const Blah& lhs, const Blah& rhs)
{
    if (lhs.z != rhs.z)
        return lhs.z < rhs.z;
    if (lhs.x != rhs.x)
        return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}
ostream& operator<<(ostream& os, const Blah& blah)
{
    os << "z: " << blah.z << " x: " << blah.x << " y: " << blah.y << endl;
    return os;
}

#if 1
int64_t solveBruteForce()
{
    // Looks like minimum of f will be < 0 if (a - 1) * (c - 1) < b * b.
    int64_t result = 0;
    while (true)
    {
        const int A = rand() % 20 + 1;
        const int B = rand() % 20 + 1;
        const int C = rand() % 20 + 1;


        const bool isSaddle = (A - 1) * (C - 1) <= B * B;
        cout << "(A - 1) * (C - 1): " << (A - 1) * (C - 1) << endl;
        cout << "B * B: " << (B * B) << endl;
        cout << "isSaddle: " << isSaddle << endl;

        //cout << "sausage: " << (double)(C - 1) - (double)(B) * B / ((A - 1)) << endl; 

        auto f = [A, B, C](double x, double y)
        {
            return ((double)A - 1) * x * x + 2 * (double)B * x * y + ((double)C - 1) * y * y;
        };
        //cout << "bleep: " << f(-100, -8.333335) << endl;

        //vector<Blah> blahs;

        Blah minBlah = {0, 0, 3000};

        const double range = 10;
        for (double x = -range; x <= range; x += 0.01)
        {
            for (double y = -range; y <= range; y += 0.01)
            {
                //blahs.push_back({x, y, f(x, y)});
                if (Blah(x, y, f(x,y)) < minBlah)
                {
                    minBlah = Blah(x, y, f(x,y));
                }
            }
        }
        cout << "minBlah: " << minBlah << endl;

        const bool isMinNegative = (minBlah.z < -0.01);
        cout << "minBlah negative: " << isMinNegative << "  isSaddle: " << isSaddle << endl;
        assert(isMinNegative == isSaddle);
#if 0
        sort(blahs.begin(), blahs.end(), [](const auto lhs, const auto rhs)
                {
                if (lhs.z != rhs.z)
                return lhs.z < rhs.z;
                if (lhs.x != rhs.x)
                return lhs.x < rhs.x;
                return lhs.y < rhs.y;
                });
#endif

#if 0
        for (const auto blah : blahs)
        {
            cout << "z: " << blah.z << " x: " << blah.x << " y: " << blah.y << endl;
        }
#endif
    }
    
    return result;
}
#endif

#if 0
int64_t solveOptimised()
{
    int64_t result = 0;
    
    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce();
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
