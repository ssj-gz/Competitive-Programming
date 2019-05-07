#include <iostream>
#include <vector>

using namespace std;

string solveBruteForce(const string& originalProteinString, int N, int K)
{
    auto proteinString = originalProteinString;
    for (int second = 0; second < K; second++)
    {
        cout << proteinString << endl;
        string nextProteinString(N, '\0');
        auto transform = [](char a, char b)
        {
            static const char transformTable[][4] =
            {
                {'A', 'B', 'C', 'D' },
                {'B', 'A', 'D', 'C' },
                {'C', 'D', 'A', 'B' },
                {'D', 'C', 'A', 'A' }
            };
            return transformTable[a - 'A'][b - 'A'];

        };
        for (int i = 0; i < N; i++)
        {
            nextProteinString[i] = transform(proteinString[i], proteinString[(i + 1) % N]);
        }
        proteinString = nextProteinString;
    }
    return proteinString;
}

int main(int argc, char* argv[])
{
    int N, K;
    cin >> N >> K;

    string proteinString;
    cin >> proteinString;

    const auto bruteForceSolution = solveBruteForce(proteinString, N, K);
    cout << "bruteForceSolution: " << bruteForceSolution << endl;
}
