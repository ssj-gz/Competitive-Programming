// Simon St James (ssjgz) - 2018-02-23
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // Easy one - for each ticket, reduce it to a binary number 10-digit B, where the ith digit
    // of B is 1 if and only if the ticket contains the decimal number i.  Call this B
    // the "digit bitmask".
    // Then a pair of tickets is a Winning Pair if and only if the bitwise OR of their
    // digits is a bitmask representing the ticket 012345789 (i.e. a 10-digit binary number where
    // every bit is 1).  It is then a simple matter to count all winning pairs.
    int numTickets;
    cin >> numTickets;

    const uint32_t fullDigitsBitmask = (1 << 10) - 1;
    vector<int64_t> numTicketsWithDigitBitmask(fullDigitsBitmask + 1);

    for (int i = 0; i < numTickets; i++)
    {
        string ticket;
        cin >> ticket;

        uint32_t ticketDigitBitmask = 0;
        for (const auto digitChar : ticket)
        {
            const uint32_t digit = digitChar - '0';
            ticketDigitBitmask |= (1 << digit);

        }
        assert(ticketDigitBitmask >= 0 && ticketDigitBitmask < numTicketsWithDigitBitmask.size());
        numTicketsWithDigitBitmask[ticketDigitBitmask]++;
    }

    int64_t numWinningPairs = 0;
    for (int ticket1Bitmask = 0; ticket1Bitmask < numTicketsWithDigitBitmask.size(); ticket1Bitmask++)
    {
        for (int ticket2Bitmask = ticket1Bitmask + 1; ticket2Bitmask < numTicketsWithDigitBitmask.size(); ticket2Bitmask++)
        {
            if ((ticket1Bitmask | ticket2Bitmask) == fullDigitsBitmask)
            {
                numWinningPairs += numTicketsWithDigitBitmask[ticket1Bitmask] * numTicketsWithDigitBitmask[ticket2Bitmask];
            }
        }
    }
    // Pairs of tickets that each have full digit Bitmasks.
    numWinningPairs += numTicketsWithDigitBitmask[fullDigitsBitmask] * (numTicketsWithDigitBitmask[fullDigitsBitmask] - 1) / 2;

    cout << numWinningPairs << endl; 
}
