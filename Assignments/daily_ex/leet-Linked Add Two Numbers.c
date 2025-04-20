/*
You are given two non-empty linked lists representing two non-negative integers. The digits are stored in reverse order, and each of their nodes contains a single digit. Add the two numbers and return the sum as a linked list.

You may assume the two numbers do not contain any leading zero, except the number 0 itself.
*/



/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode* dummy = (struct ListNode*)malloc(sizeof(struct ListNode));
    dummy->val = 0;
    dummy->next = NULL;
    struct ListNode* current = dummy;
    int carry = 0;

    while (l1 != NULL || l2 != NULL || carry != 0) {
        int x = (l1 != NULL) ? l1->val : 0; 
        int y = (l2 != NULL) ? l2->val : 0; 
        int sum = x + y + carry; 
        carry = sum / 10; 
        int digit = sum % 10; 

        current->next = (struct ListNode*)malloc(sizeof(struct ListNode)); 
        current = current->next;
        current->val = digit; 
        current->next = NULL; 

        if (l1 != NULL) l1 = l1->next; 
        if (l2 != NULL) l2 = l2->next; 
    }

    struct ListNode* result = dummy->next; 
    free(dummy);
    return result;
}
