#include "Solver.h"

std::vector<unsigned long long> Solver::firstNFactorials(int n)
{
    std::vector<unsigned long long> result;
    if (n <= 0) {
        return result;
    }

    result.reserve(n);
    unsigned long long fact = 1ULL;
    result.push_back(fact);

    for (int i = 1; i < n; i++) {
        if (fact > UINT64_MAX / i) {
            break;
        }
        fact *= static_cast<unsigned long long>(i);
        result.push_back(fact);
    }

    return result;
}

std::vector<int> Solver::removeDuplicates(const std::vector<int>& input)
{
    std::vector<int> result;
    result.reserve(input.size());
    std::unordered_set<int> seen;

    for (int num : input) {
        if (seen.insert(num).second) {
            result.push_back(num);
        }
    }
    return result;
}

ListNode* Solver::reverseList(ListNode* head)
{
    if (!head || !head->next) {
        return head;
    }

    ListNode* newHead = reverseList(head->next);
    head->next->next = head;
    head->next = nullptr;
    return newHead;
}