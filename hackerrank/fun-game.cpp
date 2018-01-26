// Simon St James (ssjgz) - 2018-01-26
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <sys/time.h>
#include <cassert>

using namespace std;

int main(int argc, char** argv)
{
    // Fairly easy one, though again, I did a Brute Force implementation just to check :)
    // The winning strategy is simple: be greedy! That is, make the move that *gains* you the most score, and *deprives*
    // your opponent of the most score! If Player 1 chooses i, then he gains A[i] directly, and deprives Player 2 of 
    // B[i], so the net effect on the *difference* in scores in A[i] + B[i].  Similar for Player 2.
    //
    // Thus, each player chooses the i amongst all unused i that maximises A[i] + B[i].  We can very easily implement this 
    // strategy and thus find out who the winner would be.
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> A(n);
        vector<int> B(n);

        for (int i = 0; i < n; i++)
            cin >> A[i];
        for (int i = 0; i < n; i++)
            cin >> B[i];

        struct AAndBSum
        {
            int sum = 0;
            int originalIndex = -1;
        };
        vector<AAndBSum> aAndBSumsInIncreasingOrder;
        for (int i = 0; i < n; i++)
        {
            AAndBSum sum;
            sum.sum = A[i] + B[i];
            sum.originalIndex = i;
            aAndBSumsInIncreasingOrder.push_back(sum);
        }
        sort(aAndBSumsInIncreasingOrder.begin(), aAndBSumsInIncreasingOrder.end(), [](const auto& lhs, const auto& rhs) { return lhs.sum < rhs.sum;});

        // Simulate perfect play using the Greedy strategy.
        bool isPlayer1 = true;
        int player1Score = 0;
        int player2Score = 0;
        while (!aAndBSumsInIncreasingOrder.empty())
        {
            const int originalIndexOfHighestSum = aAndBSumsInIncreasingOrder.back().originalIndex;
            if (isPlayer1)
            {
                player1Score += A[originalIndexOfHighestSum];
            }
            else
            {
                player2Score += B[originalIndexOfHighestSum];
            }

            aAndBSumsInIncreasingOrder.pop_back();
            isPlayer1 = !isPlayer1;
        }

        if (player1Score < player2Score)
            cout << "Second";
        else if (player1Score > player2Score)
            cout << "First";
        else if (player1Score == player2Score)
            cout << "Tie";
        cout << endl;
    }
}

