// Simon St James (ssjgz) - 2019-05-04
#include <iostream>
#include <stack>

using namespace std;

int main()
{
    int q;
    cin >> q;

    stack<int> oldestOnTop;
    stack<int> newestOnTop;

    auto flushNewestOnTopToOldestOnTop = [&oldestOnTop, &newestOnTop]()
    {
        while (!newestOnTop.empty())
        {
            const int value = newestOnTop.top();
            oldestOnTop.push(value);
            newestOnTop.pop();
        }
    };

    for (int i = 0; i < q; i++)
    {
        int type;
        cin >> type;

        switch(type)
        {
            case 1:
                {
                    int value;
                    cin >> value;
                    newestOnTop.push(value);
                }
                break;
            case 2:
                if (oldestOnTop.empty())
                    flushNewestOnTopToOldestOnTop();
                oldestOnTop.pop();
                break;
            case 3:
                if (oldestOnTop.empty())
                    flushNewestOnTopToOldestOnTop();
                cout << oldestOnTop.top() << endl;
                break;
        }
    }

}
