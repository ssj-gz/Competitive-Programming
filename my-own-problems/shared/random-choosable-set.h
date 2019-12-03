#include <vector>
#include <map>

#include <type_traits>
/**
 * Insert/ remove/ find pointers to an object in O(log N); choose a random
 * pointer to an object which is currently in the set in O(1).
 */
template <typename TPtr>
class RandomChooseableSet
{
    static_assert(std::is_pointer_v<TPtr>);
    public:
        RandomChooseableSet() = default;
        template<typename FwdIter>
        RandomChooseableSet(FwdIter begin, FwdIter end)
        {
            for (FwdIter iter = begin; iter != end; iter++)
            {
                insert(*iter);
            }
        }

        void insert(TPtr toAdd)
        {
            if (contains(toAdd))
                return;
            const int newIndex = m_vec.size();
            m_indexFor[toAdd] = newIndex;
            m_vec.push_back(toAdd);
        }
        void erase(TPtr toRemove)
        {
            if(!contains(toRemove))
            {
                return;
            }
            const int oldIndex = m_indexFor[toRemove];
            m_indexFor.erase(toRemove);
            if (m_vec.size() == 1)
            {
                m_vec.clear();
            }
            else
            {
                TPtr moveToOldIndex = m_vec.back();
                m_vec.pop_back();
                assert(m_indexFor.find(moveToOldIndex) != m_indexFor.end());
                m_indexFor[moveToOldIndex] = oldIndex;
                m_vec[oldIndex] = moveToOldIndex;
            }
        }
        bool contains(TPtr toFind) const
        {
            return (m_indexFor.find(toFind) != m_indexFor.end());
        }

        int size() const
        {
            return m_vec.size();
        }
        bool empty() const
        {
            return m_vec.empty();
        }
        TPtr chooseRandom() const
        {
            assert(!empty());
            return m_vec[rand() % m_vec.size()];
        }
    private:
        std::map<TPtr, int> m_indexFor;
        std::vector<TPtr> m_vec;
};
