// adaptors.cpp - stack, queue and priority_queue restrict an underlying container.
#include <iostream>
#include <queue>
#include <stack>
#include <vector>

int main()
{
    std::stack<int> st;                 // LIFO, on std::deque by default
    std::queue<int> q;                  // FIFO, on std::deque by default
    std::priority_queue<int> pq;        // largest first, on std::vector by default

    for (int x : {3, 1, 4, 1, 5}) {
        st.push(x);
        q.push(x);
        pq.push(x);
    }

    std::cout << "stack:         ";
    for (; !st.empty(); st.pop()) std::cout << st.top() << ' ';
    std::cout << "\nqueue:         ";
    for (; !q.empty(); q.pop()) std::cout << q.front() << ' ';
    std::cout << "\npriority_queue:";
    for (; !pq.empty(); pq.pop()) std::cout << ' ' << pq.top();

    // A min-heap: same adaptor, std::greater as the comparison.
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;
    for (int x : {3, 1, 4, 1, 5}) min_heap.push(x);
    std::cout << "\nmin-heap:      ";
    for (; !min_heap.empty(); min_heap.pop()) std::cout << min_heap.top() << ' ';
    std::cout << '\n';
}
