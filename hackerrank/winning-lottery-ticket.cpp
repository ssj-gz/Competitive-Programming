// Simon St James (ssjgz) - 2018-02-23
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // Easy one - for each ticket, reduce it to a binary number 10-digit B, where the ith digit
    // of B is 1 if and only if the ticket contains the decimal number i.  Call this B
    // the "digit digest".
    // Then a pair of tickets is a Winning Pair if and only if the bitwise OR of their
    // digits is a digest representing the ticket 012345789 (i.e. a 10-digit binary number where
    // every bit is 1).  It is then a simple matter to count all winning pairs.
    int numTickets;
    cin >> numTickets;

    vector<int64_t> numTicketsWithDigitDigest(1 << 10);

    for (int i = 0; i < numTickets; i++)
    {
        string ticket;
        cin >> ticket;

        uint32_t ticketDigitDigest = 0;
        for (const auto digitChar : ticket)
        {
            const uint32_t digit = digitChar - '0';
            ticketDigitDigest |= (1 << digit);
            assert(ticketDigitDigest >= 0 && ticketDigitDigest < numTicketsWithDigitDigest.size());

        }
        numTicketsWithDigitDigest[ticketDigitDigest]++;
    }

    const uint32_t fullDigitsDigest = (1 << 10) - 1;
    int64_t numWinningPairs = 0;
    for (int ticket1Digest = 0; ticket1Digest < numTicketsWithDigitDigest.size(); ticket1Digest++)
    {
        for (int ticket2Digest = ticket1Digest + 1; ticket2Digest < numTicketsWithDigitDigest.size(); ticket2Digest++)
        {
            if ((ticket1Digest | ticket2Digest) == fullDigitsDigest)
            {
                numWinningPairs += numTicketsWithDigitDigest[ticket1Digest] * numTicketsWithDigitDigest[ticket2Digest];
            }
        }
    }
    // Pairs of tickets that each have full digit digests.
    numWinningPairs += numTicketsWithDigitDigest[fullDigitsDigest] * (numTicketsWithDigitDigest[fullDigitsDigest] - 1) / 2;

    cout << numWinningPairs << endl; 
}
