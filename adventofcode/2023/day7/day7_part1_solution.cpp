#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

const string cardsIncreasingStrength = "23456789TJQKA";

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
        std::map<char, int> cardFrequency;
        for (const auto& card : hand.cards)
            cardFrequency[card]++;
        string cardFrequenciesSorted;
        for (const auto& [card, frequency] : cardFrequency)
        {
            if (frequency != 0)
                cardFrequenciesSorted.push_back(static_cast<char>('0' + frequency));
        }
        sort(cardFrequenciesSorted.begin(), cardFrequenciesSorted.end());
        std::cout << " cardFrequenciesSorted: " << cardFrequenciesSorted << std::endl;
        if (cardFrequenciesSorted == "5")
            hand.type = Hand::FiveOfAKind;
        else if (cardFrequenciesSorted == "14")
            hand.type = Hand::FourOfAKind;
        else if (cardFrequenciesSorted == "23")
            hand.type = Hand::FullHouse;
        else if (cardFrequenciesSorted == "113")
            hand.type = Hand::ThreeOfAKind;
        else if (cardFrequenciesSorted == "122")
            hand.type = Hand::TwoPair;
        else if (cardFrequenciesSorted == "1112")
            hand.type = Hand::OnePair;
        else if (cardFrequenciesSorted == "11111")
            hand.type = Hand::HighCard;
        else
            assert(false);
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
