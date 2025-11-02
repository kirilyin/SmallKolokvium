#pragma once

#include <vector>
#include <unordered_set>
struct ListNode
{
    int key;
    ListNode* next;
    ListNode(int x) : key(x), next(nullptr) {}
};

class Solver {
public:
    static std::vector<unsigned long long> firstNFactorials(int n);

    static std::vector<int> removeDuplicates(const std::vector<int>& input);

    static ListNode* reverseList(ListNode* head);
};