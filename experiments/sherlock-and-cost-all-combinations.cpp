#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> B(n);
        for (int i = 0; i < n; i++)
        {
            cin >> B[i];
        }

        cout << "Testcase " << (t + 1) << " of " << T << "  B: ";
        for (const auto x : B)
            cout << x << " ";
        cout << endl << "All possible A of length " << n << " satisfying 1 <= A[i] <= B[i] for i = 1, 2, ... " << n << endl;

        // Start off with A = 1, 1, 1, ... , 1.
        vector<int> A(n, 1);

        int largestCost = -1;
        vector<int> aWithLargestCost;

        while (true)
        {
            for (const auto x : A)
            {
                cout << " " << x;
            }
            int cost = 0;
            for (int i = 1; i < n; i++)
            {
                cost += abs(A[i] - A[i - 1]);
            }
            cout << " (cost " << cost << ")" << endl;
            if (cost > largestCost)
            {
                aWithLargestCost = A;
                largestCost = cost;
            }
            // Increment from right to left - kind of like ordinary decimal
            // counter, but where the limit of the ith digit is B[i] instead
            // of 10 :)
            int index = n - 1;
            while (index >=0 && A[index] == B[index])
            {
                A[index] = 1;
                index--;
            }
            if (index == -1)
            {
                // We're back to A = 1, 1, 1 ... , 1 - none left.
                break;
            }
            A[index]++;
        }

        cout << "Array with largest cost: ";
        for (const auto x : aWithLargestCost)
        {
            cout << " " << x;
        }
        cout << " ( cost = " << largestCost << ")" << endl;
    }
}
