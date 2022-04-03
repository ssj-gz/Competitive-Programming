// Simon St James (ssjgz) - 2022-03-31
// 
// Solution to: https://www.codechef.com/MARCH221D/problems/GENIUS
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct Answer
{
    bool isPossible = false;
    int A = -1;
    int B = -1;
    int C = -1;
};

Answer getAnswer(const int totalQuestions, const int targetScore)
{
    if (targetScore % 3 == 0)
    {
        const auto A = targetScore / 3;
        const auto B = 0;
        const auto C = totalQuestions - A;
        if (C < 0)
        {
            return {false, -1, -1, -1};
        }
        else
        {
            return {true, A, B, C};
        }
    }
    else
    {
        const auto nextMultipleOf3 = targetScore + (3 - (targetScore % 3));
        assert(nextMultipleOf3 % 3 == 0);
        const auto A = nextMultipleOf3 / 3;
        const auto B = nextMultipleOf3 - targetScore;
        const auto C = totalQuestions - (A + B);
        if (C < 0)
        {
            return {false, -1, -1, -1};
        }
        else
        {
            return {true, A, B, C};
        }
    }

}

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (auto t = 0; t < T; t++)
    {
        const auto totalQuestions = read<int>();
        const auto targetScore = read<int>();
        const auto answer = getAnswer(totalQuestions, targetScore);
        if (answer.isPossible)
        {
            assert(answer.A + answer.B + answer.C == totalQuestions);
            assert(answer.A * 3 + answer.B * -1 + answer.C * 0 == targetScore);
            cout << "YES" << endl;
            cout << answer.A << " " << answer.B << " " << answer.C << endl;
        }
        else
        {
            cout << "NO" << endl;
        }
    }

    assert(cin);
}
