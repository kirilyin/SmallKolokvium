#include <gtest/gtest.h>
#include "Solver.h"
#include <vector>

TEST(FactorialsTest, HandlesZeroAndPositiveInput) {
    EXPECT_TRUE(Solver::firstNFactorials(0).empty());

    auto res1 = Solver::firstNFactorials(1);
    ASSERT_EQ(res1.size(), 1);
    EXPECT_EQ(res1[0], 1ULL);

    auto res5 = Solver::firstNFactorials(5);
    ASSERT_EQ(res5.size(), 5);
    EXPECT_EQ(res5[0], 1ULL);
    EXPECT_EQ(res5[1], 1ULL);
    EXPECT_EQ(res5[2], 2ULL);
    EXPECT_EQ(res5[3], 6ULL);
    EXPECT_EQ(res5[4], 24ULL);
}

TEST(FactorialsTest, HandlesOverflowGracefully) {
    auto res20 = Solver::firstNFactorials(21);
    ASSERT_EQ(res20.size(), 21);
    EXPECT_EQ(res20.back(), 2432902008176640000ULL);

    auto res21 = Solver::firstNFactorials(22);
    EXPECT_EQ(res21.size(), 21);
    EXPECT_EQ(res21.back(), 2432902008176640000ULL);
}

TEST(FactorialsTest, HandlesNegativeInput) {
    auto res = Solver::firstNFactorials(-5);
    EXPECT_TRUE(res.empty());
}

TEST(RemoveDuplicatesTest, RemovesDuplicatesPreservesOrder) {
    std::vector<int> input = { 1, 2, 3, 2, 1, 4, 3, 5 };
    auto result = Solver::removeDuplicates(input);
    std::vector<int> expected = { 1, 2, 3, 4, 5 };

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}

TEST(RemoveDuplicatesTest, HandlesEmptyVector) {
    std::vector<int> input;
    auto result = Solver::removeDuplicates(input);
    EXPECT_TRUE(result.empty());
}

TEST(RemoveDuplicatesTest, HandlesAllDuplicates) {
    std::vector<int> input = { 7, 7, 7, 7 };
    auto result = Solver::removeDuplicates(input);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 7);
}

TEST(RemoveDuplicatesTest, HandlesNoDuplicates) {
    std::vector<int> input = { 1, 2, 3, 4, 5 };
    auto result = Solver::removeDuplicates(input);
    ASSERT_EQ(result.size(), input.size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i], input[i]);
    }
}

TEST(ReverseListTest, ReversesSimpleList) {
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);

    ListNode* reversed = Solver::reverseList(head);

    ASSERT_NE(reversed, nullptr);
    EXPECT_EQ(reversed->key, 3);
    EXPECT_EQ(reversed->next->key, 2);
    EXPECT_EQ(reversed->next->next->key, 1);
    EXPECT_EQ(reversed->next->next->next, nullptr);

    delete reversed->next->next;
    delete reversed->next;
    delete reversed;
}

TEST(ReverseListTest, HandlesSingleNode) {
    ListNode* head = new ListNode(42);
    ListNode* reversed = Solver::reverseList(head);
    EXPECT_EQ(reversed, head);
    EXPECT_EQ(reversed->key, 42);
    EXPECT_EQ(reversed->next, nullptr);
    delete reversed;
}

TEST(ReverseListTest, HandlesEmptyList) {
    ListNode* head = nullptr;
    ListNode* reversed = Solver::reverseList(head);
    EXPECT_EQ(reversed, nullptr);
}

TEST(ReverseListTest, HandlesTwoNodes) {
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);

    ListNode* reversed = Solver::reverseList(head);
    EXPECT_EQ(reversed->key, 2);
    EXPECT_EQ(reversed->next->key, 1);
    EXPECT_EQ(reversed->next->next, nullptr);

    delete reversed->next;
    delete reversed;
}