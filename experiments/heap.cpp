#include "memorypool.h"

template <typename ValueType, typename Compare>
class Heap
{
    public:
        Heap(int maxElements, Compare comparator)
            : m_maxElements{maxElements}, 
              m_memoryPool{sizeof(ValueType), maxElements},
              m_comparator{comparator}
        {
            m_handles.reserve(maxElements);


        }
        class Handle
        {
            public:
                Handle()
                {
                }
            private:
                bool m_isValid = false;
        };
        Handle add(const ValueType& value)
        {
            assert(m_elements.size() < m_maxElements);
            return Handle{};
        }
        int size()
        {
            return m_elements.size();
        }
        const ValueType& min()
        {
            return ValueType();
        }
        void extractMin()
        {
        }
    private:
        int m_maxElements = -1;
        MemoryPool m_memoryPool;
        struct Element
        {
            ValueType* value;
            Handle* handle;
        };
        vector<Handle> m_handles;
        vector<Element> m_elements;
        Compare m_comparator;

};


int main()
{
}
