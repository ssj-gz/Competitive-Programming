constexpr auto maxDist = 200'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto maxBinaryDigits = log2(maxDist);

//#define VERIFY_DIST_TRACKER

class DistTracker
{
    public:
        DistTracker()
        {
            auto powerOf2 = 2;
            m_numBits = -1;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_distsModPowerOf2[binaryDigitNum] = std::vector<int>(powerOf2);
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;
                if (powerOf2 / 2 > maxDist)
                    break; // i.e. even if we scrolled this powerOf2 maxDist units, we'd never enter the "make digit 1" zone.
                powerOf2 <<= 1;
                m_numBits++;
            }
        }
        void insertDist(const int newDist)
        {
            const int numBits = m_numBits;
            doPendingDistAdjustments();
            const auto newDistAdjusted = newDist
                // The m_makesDigitOneBegin/End will have been shifted by a total of m_cumulativeDistAdjustment, so counteract that.
                - m_cumulativeDistAdjustment
                // Add a number guarantees that newDistAdjusted >= 0, but does not affect its value modulo the powers of 2 we care about.
                + (1 << (maxBinaryDigits + 1));
            assert(newDistAdjusted >= 0);
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
            {
                const auto distModPowerOf2 = newDistAdjusted & (powerOf2 - 1); // "& (powerOf2 - 1)" is a faster "% powerOf2".
                m_distsModPowerOf2[binaryDigitNum][distModPowerOf2]++;
                if (m_makesDigitOneBegin[binaryDigitNum] <= m_makesDigitOneEnd[binaryDigitNum])
                {
                    if (distModPowerOf2 >= m_makesDigitOneBegin[binaryDigitNum] && distModPowerOf2 <= m_makesDigitOneEnd[binaryDigitNum])
                        m_grundyNumber ^= (powerOf2 >> 1);
                }
                else
                {
                    const auto makeDigitZeroBegin = m_makesDigitOneEnd[binaryDigitNum] + 1;
                    const auto makeDigitZeroEnd = m_makesDigitOneBegin[binaryDigitNum] - 1;
                    assert(0 <= makeDigitZeroBegin && makeDigitZeroBegin <= makeDigitZeroEnd && makeDigitZeroEnd < powerOf2);
                    if (!(distModPowerOf2 >= makeDigitZeroBegin && distModPowerOf2 <= makeDigitZeroEnd))
                        m_grundyNumber ^= (powerOf2 >> 1);
                }

                powerOf2 <<= 1;
            }
#ifdef VERIFY_DIST_TRACKER
            m_dists.push_back(newDist);
#endif
        };

        void adjustAllDists(int distDiff)
        {
            m_pendingDistAdjustment += distDiff;
#ifdef VERIFY_DIST_TRACKER
            for (auto& dist : m_dists)
            {
                dist += distDiff;
            }
#endif
        }
        void doPendingDistAdjustments()
        {
            auto distDiff = m_pendingDistAdjustment;
            if (distDiff == 0)
                return;
            m_pendingDistAdjustment = 0;
            m_cumulativeDistAdjustment += distDiff;

            const int numBits = m_numBits;
            auto powerOf2 = 2;
            if (distDiff > 0)
            {
                for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    const auto reducedDistDiff = distDiff & (powerOf2 - 1); // Scrolling powerOf2 units is the same as not scrolling at all!
                    auto parityChangeToNumberOfDistsThatMakeDigitsOne = 0;
                    for (auto i = 0; i < reducedDistDiff; i++)
                    {
                        parityChangeToNumberOfDistsThatMakeDigitsOne += m_distsModPowerOf2[binaryDigitNum][m_makesDigitOneEnd[binaryDigitNum]];
                        m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) & (powerOf2 - 1);

                        m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) & (powerOf2 - 1);
                        parityChangeToNumberOfDistsThatMakeDigitsOne += m_distsModPowerOf2[binaryDigitNum][m_makesDigitOneBegin[binaryDigitNum]];
                    }

                    if ((parityChangeToNumberOfDistsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1); // This binary digit in the grundy number has flipped; update grundyNumber.

                    powerOf2 <<= 1;
                }
            }
            else
            {
                distDiff = -distDiff;
                assert(distDiff > 0);
                for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    auto parityChangeToNumberOfDistsThatMakeDigitsOne = 0;
                    const auto reducedDistDiff = distDiff & (powerOf2 - 1); // Scrolling powerOf2 units is the same as not scrolling at all!
                    for (auto i = 0; i < reducedDistDiff; i++)
                    {
                        parityChangeToNumberOfDistsThatMakeDigitsOne += m_distsModPowerOf2[binaryDigitNum][m_makesDigitOneBegin[binaryDigitNum]];
                        m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) & (powerOf2 - 1);

                        m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) & (powerOf2 - 1);
                        parityChangeToNumberOfDistsThatMakeDigitsOne += m_distsModPowerOf2[binaryDigitNum][m_makesDigitOneEnd[binaryDigitNum]];
                    }

                    if ((parityChangeToNumberOfDistsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                }
            }
        }
        int grundyNumber()
        {
            if (m_pendingDistAdjustment != 0)
            {
                doPendingDistAdjustments();
            }
#ifdef VERIFY_DIST_TRACKER
            int dbgGrundyNumber = 0;
            for (const auto dist : m_dists)
                dbgGrundyNumber ^= dist;
            assert(dbgGrundyNumber == m_grundyNumber);
#endif
            return m_grundyNumber;
        }
    private:
        int m_numBits = 0;
        std::vector<int> m_distsModPowerOf2[maxBinaryDigits + 1];
        int m_makesDigitOneBegin[maxBinaryDigits + 1];
        int m_makesDigitOneEnd[maxBinaryDigits + 1];

        int m_cumulativeDistAdjustment = 0;
        int m_grundyNumber = 0;

        int m_pendingDistAdjustment = 0;

#ifdef VERIFY_DIST_TRACKER
        std::vector<int> m_dists;
#endif
};

