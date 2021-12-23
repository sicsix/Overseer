//
// Created by Tim on 28/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMON_PRIORITYQUEUE_H_
#define OVERSEER_WASM_SRC_COMMON_PRIORITYQUEUE_H_
#include <queue>
namespace Overseer::Common
{
    struct Node
    {
        int Index;
        int CostSoFar;

        Node(int index, int costSoFar): Index(index), CostSoFar(costSoFar)
        {
        }

        bool operator<(const Node& rhs) const
        {
            if (Index < rhs.Index)
                return true;
            if (rhs.Index < Index)
                return false;
            return CostSoFar < rhs.CostSoFar;
        }
        bool operator>(const Node& rhs) const
        {
            return rhs < *this;
        }
        bool operator<=(const Node& rhs) const
        {
            return !(rhs < *this);
        }
        bool operator>=(const Node& rhs) const
        {
            return !(*this < rhs);
        }
    };

    template<typename PriorityT, typename T>
    struct PriorityQueue : private std::priority_queue<std::pair<PriorityT, T>,
                                                       std::vector<std::pair<PriorityT, T>>,
                                                       std::greater<std::pair<PriorityT, T>>>
    {
      public:
        inline bool Empty() const
        {
            return this->empty();
        }

        inline void Push(int expectedCost, T node)
        {
            this->emplace(expectedCost, node);
        }

        T Pop()
        {
            T bestElement = this->top().second;
            this->pop();
            return bestElement;
        }

        inline void Clear()
        {
            this->c.clear();
        }
    };
} // namespace Overseer::Common
#endif // OVERSEER_WASM_SRC_COMMON_PRIORITYQUEUE_H_
