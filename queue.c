#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_head = malloc(sizeof(struct list_head));
    if (!new_head)
        return NULL;
    INIT_LIST_HEAD(new_head);
    return new_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *pos, *safe;
    list_for_each_safe (pos, safe, head) {
        element_t *element = list_entry(pos, element_t, list);
        q_release_element(element);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    size_t len = strlen(s) + 1;
    new_element->value = malloc(len * sizeof(char));
    if (!new_element->value) {
        q_release_element(new_element);
        return false;
    }
    strncpy(new_element->value, s, len - 1);
    new_element->value[len - 1] = '\0';
    list_add(&new_element->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;
    size_t len = strlen(s) + 1;
    new_element->value = malloc(len * sizeof(char));
    if (!new_element->value) {
        q_release_element(new_element);
        return false;
    }
    strncpy(new_element->value, s, len - 1);
    new_element->value[len - 1] = '\0';
    list_add_tail(&new_element->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_first_entry(head, element_t, list);
    list_del(&element->list);

    if (sp && element->value && bufsize > 0) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_last_entry(head, element_t, list);
    list_del(&element->list);

    if (sp && element->value && bufsize > 0) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next->next;
    struct list_head *slow = head->next;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    list_del(slow);
    element_t *element = list_entry(slow, element_t, list);
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *curr = head;
    struct list_head *tmp;

    do {
        tmp = curr->next;
        curr->next = curr->prev;
        curr->prev = tmp;
        curr = tmp;
    } while (curr != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || head->next == head->prev)
        return;
    int len = q_size(head);
    LIST_HEAD(result);
    for (int i = 0; i < len; i += k) {
        LIST_HEAD(tmp);
        int j = 0;
        struct list_head *node, *safe;
        list_for_each_safe (node, safe, head) {
            j += 1;
            if (j > k)
                break;
            list_del(node);
            list_add(node, &tmp);
        }
        list_splice_tail(&tmp, &result);
    }
    list_splice(&result, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (head->next == head->prev)
        return 1;

    element_t *curr = list_entry(head->next, element_t, list);
    const char *max_value = curr->value;

    while (&curr->list != head) {
        element_t *next = list_entry(curr->list.next, element_t, list);
        if (strcmp(curr->value, max_value) < 0) {
            list_del(&curr->list);
            q_release_element(curr);
        } else {
            max_value = curr->value;
        }
        curr = next;
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (head->next == head->prev)
        return 1;

    element_t *curr = list_entry(head->prev, element_t, list);
    const char *max_value = curr->value;

    while (&curr->list != head) {
        element_t *prev = list_entry(curr->list.prev, element_t, list);
        if (strcmp(curr->value, max_value) < 0) {
            list_del(&curr->list);
            q_release_element(curr);
        } else {
            max_value = curr->value;
        }
        curr = prev;
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
