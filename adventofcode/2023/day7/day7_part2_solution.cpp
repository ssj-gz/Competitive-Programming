#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

const string cardsIncreasingStrength = "J23456789TQKA";

vector<string> generateAllJokerReplacements(const std::string& initialCards)
{
    vector<string> jokerReplacements;
    const int numJokers = std::count(initialCards.begin(), initialCards.end(), 'J');
    vector<char> replacedJokers(numJokers, 'J');
    while (true)
    {
        int replaceWithJokerIndex = 0;
        string newCards;
        for (int cardIndex = 0; cardIndex < initialCards.size(); cardIndex++)
        {
            if (initialCards[cardIndex] != 'J')
                newCards.push_back(initialCards[cardIndex]);
            else
            {
                newCards.push_back(replacedJokers[replaceWithJokerIndex]);
                replaceWithJokerIndex++;
            }
        }
        jokerReplacements.push_back(newCards);

        // Next.
        int jokerIndex = 0;
        while (jokerIndex < numJokers && replacedJokers[jokerIndex] == cardsIncreasingStrength.back())
        {
            replacedJokers[jokerIndex] = 'J';
            jokerIndex++;
        }
        if (jokerIndex == numJokers)
            break;
        const int oldStrength = cardsIncreasingStrength.find(replacedJokers[jokerIndex]);
        assert(oldStrength != string::npos);
        replacedJokers[jokerIndex] = cardsIncreasingStrength[oldStrength + 1];
    }

    return jokerReplacements;
}

int main()
{
    struct Hand
    {
        string cards;
        int bid = -1;
        int rank = -1;
        enum Type {Unknown, HighCard, OnePair, TwoPair, ThreeOfAKind, FullHouse, FourOfAKind, FiveOfAKind};
        Type type = Unknown;

        bool operator<(const Hand& other) const
        {
            if (type != other.type)
            {
                return static_cast<int>(type) < static_cast<int>(other.type);
            }
            for (int cardNum = 0; cardNum < 5; cardNum++)
            {
                const int strength = cardsIncreasingStrength.find(cards[cardNum]);
                const int otherStrength = cardsIncreasingStrength.find(other.cards[cardNum]);
                assert(strength != string::npos && otherStrength != string::npos);
                if (strength != otherStrength)
                    return strength < otherStrength;
            }
            assert(false);
            return false;
        }

    };
    vector<Hand> hands;
    while (true)
    {
        Hand hand;
        cin >> hand.cards;
        cin >> hand.bid;
        if (cin)
        {
            hands.push_back(hand);
        }
        else
            break;
    }
    for (auto& hand : hands)
    {
        std::cout << "hand cards: " << hand.cards << std::endl;
        Hand::Type bestType = Hand::Unknown;

        for (const auto& jokerSubstitutedCards : generateAllJokerReplacements(hand.cards))
        {
            std::map<char, int> cardFrequency;
            for (const auto& card : jokerSubstitutedCards)
                cardFrequency[card]++;
            string cardFrequenciesSorted;
            for (const auto& [card, frequency] : cardFrequency)
            {
                if (frequency != 0)
                    cardFrequenciesSorted.push_back(static_cast<char>('0' + frequency));
            }
            sort(cardFrequenciesSorted.begin(), cardFrequenciesSorted.end());
            std::cout << " cardFrequenciesSorted: " << cardFrequenciesSorted << std::endl;
            Hand::Type handType = Hand::Unknown;
            if (cardFrequenciesSorted == "5")
                handType = Hand::FiveOfAKind;
            else if (cardFrequenciesSorted == "14")
                handType = Hand::FourOfAKind;
            else if (cardFrequenciesSorted == "23")
                handType = Hand::FullHouse;
            else if (cardFrequenciesSorted == "113")
                handType = Hand::ThreeOfAKind;
            else if (cardFrequenciesSorted == "122")
                handType = Hand::TwoPair;
            else if (cardFrequenciesSorted == "1112")
                handType = Hand::OnePair;
            else if (cardFrequenciesSorted == "11111")
                handType = Hand::HighCard;
            else
                assert(false);
            if (handType > bestType)
            {
                bestType = handType;
                hand.type = handType;
            }
        }
    }

    sort(hands.begin(), hands.end());
    int rank = 1;
    int64_t result = 0;
    for (auto& hand : hands)
    {
        hand.rank = rank;
        rank++;
        std::cout << "Hand cards: " << hand.cards << " bid: " << hand.bid << " rank: " << hand.rank << std::endl;
        result += hand.bid * hand.rank;
    }
    std::cout << result << std::endl;
}
