// Simon St James (ssjgz) - 2019-04-09
#include <iostream>
#include <stack>
#include <vector>

#include <sys/time.h>

#include <cassert>

using namespace std;

struct UndoableOp
{
    enum OpType { Append, Delete };
    OpType opType;
    string opData;
};

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        // Try to generate a testcase that will trigger OOM if someone keeps
        // all actual currentText's in the Undo stack, rather than just the operations
        // needed to perform the Undos.
        //
        // Basic aim: keep currentText long!
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        auto randomStringOfLength = [](int n)
        {
            string s;
            for (int i = 1; i <= n; i++)
            {
                s.push_back('a' + rand() % 26);
            }
            return s;
        };

        struct Op
        {
            int opType;
            string opArgString;
            int opArgInt;
        };

        vector<Op> ops;

        stack<UndoableOp> undoStack;
        string currentText;

        int sumOfCharsAdded = 0;
        int sumOfCharsRemoved = 0;

        while (currentText.length() <= 500'000)
        {
            const auto longString = randomStringOfLength(50'000 + (rand() % 50'000));

            currentText += longString;
            undoStack.push({UndoableOp::Append, longString});
            ops.push_back({1, longString, -1});
            sumOfCharsAdded += longString.length();
        }

        // TODO - update firstChangedCharIndexSincePrint with each op, and use it to
        // with opType == 3 to get a bit more variety in what we are asked to print out.
        int firstChangedCharIndexSincePrint = -1;

        auto updateFirstChangedIndex = [&firstChangedCharIndexSincePrint](int indexOfChange)
        {
            if (firstChangedCharIndexSincePrint == -1)
                firstChangedCharIndexSincePrint = indexOfChange;
            else
                firstChangedCharIndexSincePrint = min(firstChangedCharIndexSincePrint, indexOfChange);
        };

        while (ops.size() <= 1'000'000 && sumOfCharsAdded < 1'000'000 && sumOfCharsRemoved < 2 * 1'000'000)
        {
            vector<int> allowableOpTypes;
            assert(!undoStack.empty());
            if (undoStack.top().opType == UndoableOp::Delete || currentText.size() - undoStack.top().opData.size() >= 400'000)
            {
                allowableOpTypes.push_back(4);
            }
            if (currentText.size() > 0)
            {
                allowableOpTypes.push_back(2);
            }
            allowableOpTypes.push_back(1);
            if (currentText.size() > 0)
                allowableOpTypes.push_back(3);

            const int opType = allowableOpTypes[rand() % allowableOpTypes.size()];

            if (opType == 1)
            {
                const int maxCharsToAdd = min(100, 1'000'000 - sumOfCharsAdded);
                const int numCharsToAdd = (rand() % maxCharsToAdd ) + 1;
                const string stringToAdd = randomStringOfLength(numCharsToAdd);

                updateFirstChangedIndex(currentText.size());

                currentText += stringToAdd;
                undoStack.push({UndoableOp::Append, stringToAdd});
                ops.push_back({1, stringToAdd, -1});
                sumOfCharsAdded += stringToAdd.length();

            }
            else if (opType == 2)
            {
                const int maxCharsToRemove = min<int>(100, currentText.size());
                const int numToRemove = (rand() % maxCharsToRemove) + 1;
                const string removedText = currentText.substr(currentText.size() - numToRemove);

                updateFirstChangedIndex(currentText.size() - numToRemove);

                currentText.erase(currentText.begin() + currentText.size() - numToRemove, currentText.end());
                undoStack.push({UndoableOp::Delete, removedText});
                ops.push_back({2, "", numToRemove});
                sumOfCharsRemoved += numToRemove;
            }
            else if (opType == 3)
            {
                if (firstChangedCharIndexSincePrint == -1)
                {
                    firstChangedCharIndexSincePrint = 0;
                }

                int indexToPrint = -1;
                if (firstChangedCharIndexSincePrint >= currentText.size() - 1)
                {
                    indexToPrint = (rand() % static_cast<int>(currentText.length()) + 1);
                }
                else
                {
                    indexToPrint = firstChangedCharIndexSincePrint + 1 + rand() % (currentText.length() - 1 - firstChangedCharIndexSincePrint);

                }

                assert(indexToPrint >= 1 && indexToPrint <= currentText.size());
                ops.push_back({3, "", indexToPrint});

                firstChangedCharIndexSincePrint = -1;
            }
            else if (opType == 4)
            {
                assert(!undoStack.empty());
                const auto& opToUndo = undoStack.top();
                switch (opToUndo.opType)
                {
                    case UndoableOp::Append:
                        {
                            const int numToRemove = opToUndo.opData.size();

                            updateFirstChangedIndex(currentText.size() - numToRemove);

                            assert(numToRemove <= currentText.size());
                            currentText.erase(currentText.begin() + currentText.size() - numToRemove, currentText.end());
                            break;
                        }
                    case UndoableOp::Delete:
                        updateFirstChangedIndex(currentText.size());
                        
                        currentText += opToUndo.opData;
                        break;
                    default:
                        assert(false);
                        break;
                }

                undoStack.pop();
                ops.push_back({4, "", -1});
            }
        }

        cout << ops.size() << endl;
        for (const auto& op : ops)
        {
            cout << op.opType << " ";
            if (op.opType == 1)
            {
                cout << op.opArgString;
            }
            else if (op.opType == 2 || op.opType == 3) 
            {
                cout << op.opArgInt;
            }


            cout << endl;
        }

        return 0;
    }
    // Trivially easy.  The only potential snag is the memory usage needed for Undo-ability:
    // if we stored the complete current text for each Undoable
    // operation, we'd use O(n^2) memory under certain circumstances.
    // However, simply storing the information needed to perform the Undo -
    // in the case of Delete, the string deleted from the end; in the case of 
    // Append, the length of the string appended (though I actually store the
    // whole appended string itself rather than the length, for simplicity) -
    // keeps it to O(n).  

    auto readInt = []() { int x; cin >> x; assert(cin); return x; };

    const int numOperations = readInt();

    std::stack<UndoableOp> undoStack;

    string currentText;

    for (int i = 0; i < numOperations; i++)
    {
        const int opType = readInt();

        switch (opType)
        {
            case 1:
                {
                    string toAppend;
                    cin >> toAppend;

                    currentText += toAppend;

                    const UndoableOp appendOp = { UndoableOp::Append, toAppend };
                    undoStack.push(appendOp);
                    break;
                }
            case 2:
                {
                    const int numToRemove = readInt();
                    assert(numToRemove <= currentText.length());
                    const string removedText = currentText.substr(currentText.size() - numToRemove);

                    currentText.erase(currentText.begin() + currentText.size() - numToRemove, currentText.end());

                    const UndoableOp deleteOp = { UndoableOp::Delete, removedText };
                    undoStack.push(deleteOp);
                    break;
                }
            case 3:
                {
                    const int charIndexToPrint = readInt() - 1; // The "- 1" makes charIndexToPrint 0-relative.
                    assert(charIndexToPrint >= 0 && charIndexToPrint < currentText.size());
                    cout << currentText[charIndexToPrint] << endl;
                    break;
                }
            case 4:
                {
                    assert(!undoStack.empty());
                    const auto& opToUndo = undoStack.top();
                    switch (opToUndo.opType)
                    {
                        case UndoableOp::Append:
                            {
                                const int numToRemove = opToUndo.opData.size();
                                assert(numToRemove <= currentText.size());
                                currentText.erase(currentText.begin() + currentText.size() - numToRemove, currentText.end());
                                break;
                            }
                        case UndoableOp::Delete:
                            currentText += opToUndo.opData;
                            break;
                        default:
                            assert(false);
                            break;
                    }

                    undoStack.pop();
                    break;
                }
        }
    }

}
