#define VERIFY_HEIGHT_TRACKER
#ifdef SUBMISSION
#define NDEBUG
#undef VERIFY_HEIGHT_TRACKER
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

constexpr auto maxHeight = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto log2MaxHeight = log2(maxHeight);

class HeightTracker
{
    public:
        HeightTracker()
        {
            int powerOf2 = 2;
            for (int i = 0; i <= log2MaxHeight; i++)
            {
                m_heightsModPowerOf2.push_back(vector<int>(powerOf2, 0));
                m_makesDigitOneBegin.push_back(powerOf2 >> 1);
                m_makesDigitOneEnd.push_back(powerOf2 - 1);
                m_numHeightsThatMakeDigitOne.push_back(0);

                powerOf2 <<= 1;
            }
        }
        void insertHeight(int newHeight)
        {
            int powerOf2 = 2;
            m_grundyNumber = 0;
            for (int i = 0; i <= log2MaxHeight; i++)
            {
                const int heightModPowerOf2 = newHeight % powerOf2;
                m_heightsModPowerOf2[i][heightModPowerOf2]++;
                cout << "i: " << i << " powerOf2: " << powerOf2 << " heightModPowerOf2: " << heightModPowerOf2 << " m_makesDigitOneBegin: " << m_makesDigitOneBegin[i] << " m_makesDigitOneEnd: " << m_makesDigitOneEnd[i] << endl;
                if (m_makesDigitOneBegin[i] <= m_makesDigitOneEnd[i])
                {
                    if (heightModPowerOf2 >= m_makesDigitOneBegin[i] && heightModPowerOf2 <= m_makesDigitOneEnd[i])
                    {
                        m_numHeightsThatMakeDigitOne[i]++;
                    }
                }
                else
                {
                    const int makeDigitZeroBegin = heightModPowerOf2 >= m_makesDigitOneEnd[i] + 1;
                    const int makeDigitZeroEnd = m_makesDigitOneBegin[i] - 1;
                    assert(makeDigitZeroBegin <= makeDigitZeroEnd);
                    if (!(heightModPowerOf2 >= makeDigitZeroBegin && heightModPowerOf2 <= makeDigitZeroEnd))
                    {
                        m_numHeightsThatMakeDigitOne[i]++;
                    }
                }

                cout << "i: " << i << " m_numHeightsThatMakeDigitOne[i]: " << m_numHeightsThatMakeDigitOne[i] << endl;

                m_grundyNumber ^= ((powerOf2 >> 1) * (m_numHeightsThatMakeDigitOne[i] % 2));

                powerOf2 <<= 1;
            }
#ifdef VERIFY_HEIGHT_TRACKER
            m_dbgHeights.push_back(newHeight);
#endif
        };
        void adjustAllHeights(int heightDiff)
        {
            if (heightDiff == 0)
                return;
            assert(heightDiff == 1 || heightDiff == -1);
            m_grundyNumber = 0;
            cout << "adjustAllHeights: " << heightDiff << endl;
            if (heightDiff == 1)
            {
                int powerOf2 = 2;
                for (int i = 0; i <= log2MaxHeight; i++)
                {
                    m_numHeightsThatMakeDigitOne[i] -= m_heightsModPowerOf2[i][m_makesDigitOneEnd[i]];
                    m_makesDigitOneEnd[i] = (powerOf2 + m_makesDigitOneEnd[i] - 1) % powerOf2;

                    m_makesDigitOneBegin[i] = (powerOf2 + m_makesDigitOneBegin[i] - 1) % powerOf2;
                    m_numHeightsThatMakeDigitOne[i] += m_heightsModPowerOf2[i][m_makesDigitOneBegin[i]];

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                int powerOf2 = 2;
                for (int i = 0; i <= log2MaxHeight; i++)
                {
                    m_numHeightsThatMakeDigitOne[i] -= m_heightsModPowerOf2[i][m_makesDigitOneBegin[i]];
                    m_makesDigitOneBegin[i] = (m_makesDigitOneBegin[i] + 1) % powerOf2;

                    m_makesDigitOneEnd[i] = (m_makesDigitOneEnd[i] + 1) % powerOf2;
                    m_numHeightsThatMakeDigitOne[i] += m_heightsModPowerOf2[i][m_makesDigitOneEnd[i]];

                    powerOf2 <<= 1;
                } 
            }
            int powerOf2 = 2;
            m_grundyNumber = 0;
            for (int i = 0; i <= log2MaxHeight; i++)
            {
                m_grundyNumber ^= ((powerOf2 >> 1) * (m_numHeightsThatMakeDigitOne[i] % 2));
                powerOf2 <<= 1;
            }
#ifdef VERIFY_HEIGHT_TRACKER
            for (auto& height : m_dbgHeights)
            {
                height += heightDiff;
            }
#endif
        }
        int grundyNumber() const
        {
#ifdef VERIFY_HEIGHT_TRACKER
            int dbgGrundyNumber = 0;
            for (const auto height : m_dbgHeights)
            {
                dbgGrundyNumber ^= height;
            }
            cout << "dbgGrundyNumber: " << dbgGrundyNumber << " m_grundyNumber: " << m_grundyNumber << endl;
            assert(dbgGrundyNumber == m_grundyNumber);
#endif
            return m_grundyNumber;
        }
        void clear()
        {
            // TODO - use a versionNumber to make this O(1).
            int powerOf2 = 2;
            for (int i = 0; i <= log2MaxHeight; i++)
            {
                m_heightsModPowerOf2.push_back(vector<int>(powerOf2, 0));
                m_makesDigitOneBegin.push_back(powerOf2 >> 1);
                m_makesDigitOneEnd.push_back(powerOf2 - 1);

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
        }
#ifdef VERIFY_HEIGHT_TRACKER
        vector<int> m_dbgHeights;
#endif
        vector<vector<int>> m_heightsModPowerOf2;
        vector<int> m_makesDigitOneBegin;
        vector<int> m_makesDigitOneEnd;

        vector<int> m_numHeightsThatMakeDigitOne;
        int m_grundyNumber = 0;
};


int main()
{
    HeightTracker heightTracker;
    heightTracker.insertHeight(4);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.insertHeight(6);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.insertHeight(7);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
}
