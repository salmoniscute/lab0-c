#include <stdint.h>
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
    if (!head || list_empty(head))
        return false;
    bool is_duplicate = false;
    element_t *entry = list_entry(head->next, element_t, list);
    element_t *safe = list_entry(entry->list.next, element_t, list);

    while (&entry->list != head) {
        if (&safe->list != head && !strcmp(safe->value, entry->value)) {
            is_duplicate = true;
            list_del(&entry->list);
            q_release_element(entry);
        } else if (is_duplicate) {
            is_duplicate = false;
            list_del(&entry->list);
            q_release_element(entry);
        }
        entry = safe;
        safe = list_entry(safe->list.next, element_t, list);
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *first = head->next;
    while (first != head && first->next != head) {
        list_move(first, first->next);
        first = first->next;
    }
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

static void rebuild_list_link(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *prev;
    prev = head;
    node = head->next;
    while (node) {
        node->prev = prev;
        prev = node;
        node = node->next;
    }
    prev->next = head;
    head->prev = prev;
}

struct list_head *mergeTwoLists(struct list_head *left, struct list_head *right)
{
    struct list_head *result = NULL, *current = NULL;
    while (left && right) {
        const element_t *left_element = list_entry(left, element_t, list);
        const element_t *right_element = list_entry(right, element_t, list);
        struct list_head *get;
        if (strcmp(left_element->value, right_element->value) > 0) {
            get = right;
            right = right->next;
        } else {
            get = left;
            left = left->next;
        }
        if (!result)
            result = get;
        else {
            current->next = get;
            get->prev = current;
        }
        current = get;
    }
    if (current) {
        if (left) {
            current->next = left;
            left->prev = current;
        } else if (right) {
            current->next = right;
            right->prev = current;
        } else {
            current->next = NULL;
        }
    } else {
        result = left ? left : right;
    }

    return result;
}

struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head;
    for (const struct list_head *fast = slow->next; fast && fast->next;
         fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left_sorted = merge_sort(head);
    struct list_head *right_sorted = merge_sort(mid);


    return mergeTwoLists(left_sorted, right_sorted);
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = merge_sort(head->next);
    rebuild_list_link(head);
    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (head->next == head->prev)
        return 1;

    element_t *curr = list_entry(head->prev, element_t, list);
    const char *min_value = curr->value;

    while (&curr->list != head) {
        element_t *next = list_entry(curr->list.prev, element_t, list);
        if (strcmp(curr->value, min_value) > 0) {
            list_del(&curr->list);
            q_release_element(curr);
        } else {
            min_value = curr->value;
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
