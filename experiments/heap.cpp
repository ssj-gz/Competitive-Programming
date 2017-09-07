#include "memorypool.h"

template <typename ValueType, typename Compare, typename KeyDecrease>
class Heap
{
    public:
        Heap(int maxElements, Compare comparator, KeyDecrease keyDecreaser)
            : m_maxElements{maxElements}, 
              m_memoryPool{sizeof(ValueType), maxElements},
              m_comparator{comparator},
              m_keyDecreaser{keyDecreaser}
        {
            m_handles.reserve(maxElements);


        }
        class Handle
        {
            public:
                Handle()
                {
                }
                const ValueType& value() const
                {
                }
            private:
                bool m_isValid = false;
                ValueType* m_value;
                friend class Heap;
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
        Handle minHandle()
        {
            return Handle();
        }
        void extractMin()
        {
        }
        template <typename DecreaseBy>
        void decreaseKey(const Handle& valueHandle, const DecreaseBy& decreaseBy)
        {
            m_keyDecreaser(*(valueHandle.m_value), decreaseBy);
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
        KeyDecrease m_keyDecreaser;
};

struct A
{
    public:
        A(int value)
            : keyValue{value}
        {
        }

        int keyValue = -1;
};


int main()
{
    auto decreaseKey = [](A& value, int decreaseKeyBy) { value.keyValue -= decreaseKeyBy; };
    auto comparator = [](const A& lhs, const A& rhs) { return lhs.keyValue < rhs.keyValue; };
    Heap<A, decltype(comparator), decltype(decreaseKey)> heapOfA(100, comparator, decreaseKey);
    using Handle = decltype(heapOfA)::Handle;

    A a(3);
    Handle aHandle = heapOfA.add(a);
    heapOfA.decreaseKey(aHandle, 5);
}
