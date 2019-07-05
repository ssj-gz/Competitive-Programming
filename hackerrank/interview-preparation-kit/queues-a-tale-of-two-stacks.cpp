// Simon St James (ssjgz) - 2019-05-04
#include <iostream>
#include <stack>

using namespace std;

int main()
{
    // We *could* just implement this using a std::list or std::deque
    // (the Hackerrank test-runner won't know the difference!) but
    // being a Good Boy and doing it the way the question wants us to
    // (by using two stacks) is almost as easy, so let's do that :)
    //
    // It's very simple: have two stacks, one called oldestOnTop, and the 
    // other newestOnTop.  Both are initially empty.  Any "push" queries
    // (those of type "1") always push onto *newestOnTop*.  The other
    // queries ("pop" - type "2"; and "top" - type "3") always operate on
    // oldestOnTop.  If oldestOnTop is empty when we receive one of these
    // two queries, then we repeatedly pop from newestOnTop to oldestOnTop 
    // until newestOnTop is empty (flushNewestOnTopToOldestOnTop) -
    // this has the effect of reversing the order of the pushed elements and
    // placing them onto oldestOnTop, so that now popping from oldestOnTop gets -
    // you guessed it - the oldest elements, as we want from a queue!
    //
    // Very simple.  The efficiency is still O(q), amortised, as each pushed element (up to
    // O(q) of these) is push'd, pop'd or top'd at most twice - once for each stack.
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
