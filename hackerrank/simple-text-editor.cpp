// Simon St James (ssjgz) - 2019-04-09
#include <iostream>
#include <stack>

#include <cassert>

using namespace std;

struct UndoableOp
{
    enum OpType { Append, Delete };
    OpType opType;
    string opData;
};

int main()
{
    // Trivially easy.  The only potential snag is the memory usage needed for Undo-ability:
    // if we stored the complete current text for each Undoable
    // operation, we'd use O(n^2) memory under certain circumstances.
    // However, simply storing the information needed to perform the Undo -
    // in the case of Delete, the string deleted from the end; in the case of 
    // Append, the length of the string appended (though I actually store the
    // whole appended string itself rather than the length, for simplicity) -
    // keeps it to O(n).  
    int numOperations;
    cin >> numOperations;

    std::stack<UndoableOp> undoStack;

    string currentText;

    for (int i = 0; i < numOperations; i++)
    {
        int opType;
        cin >> opType;

        switch (opType)
        {
            case 1:
                {
                    string toAppend;
                    cin >> toAppend;

                    currentText += toAppend;

                    UndoableOp appendOp = { UndoableOp::Append, toAppend };
                    undoStack.push(appendOp);
                    break;
                }
            case 2:
                {
                    int numToRemove;
                    cin >> numToRemove;
                    assert(numToRemove <= currentText.length());
                    const string removedText = currentText.substr(currentText.size() - numToRemove);

                    currentText.erase(currentText.begin() + currentText.size() - numToRemove, currentText.end());

                    UndoableOp deleteOp = { UndoableOp::Delete, removedText };
                    undoStack.push(deleteOp);
                    break;
                }
            case 3:
                int charIndexToPrint;
                cin >> charIndexToPrint;
                // Make 0-relative.
                charIndexToPrint--;
                assert(charIndexToPrint >= 0 && charIndexToPrint < currentText.size());
                cout << currentText[charIndexToPrint] << endl;
                break;
            case 4:
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
