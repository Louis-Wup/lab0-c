#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

element_t *q_new_element(char *str);

void q_delete_element(struct list_head *node);

int q_strncmp(const struct list_head *a, const struct list_head *b);

void q_swap_two_node(struct list_head *l, struct list_head *r);

struct list_head *q_bubble_sort(struct list_head *head, bool descend);

struct list_head *q_merge_sort(struct list_head *head, bool descend);

struct list_head *q_merge_two_lists(struct list_head *head_a,
                                    struct list_head *head_b,
                                    bool descend);

void q_shuffle(struct list_head *head);

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q_head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!q_head)
        return NULL;
    INIT_LIST_HEAD(q_head);
    return q_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        list_del_init(&entry->list);
        q_release_element(entry);
    }
    free(head);
}

element_t *q_new_element(char *str)
{
    element_t *new_node = (element_t *) malloc(sizeof(element_t));
    if (!new_node)
        return NULL;

    new_node->value = (char *) malloc(strlen(str) + 1);
    if (!new_node->value) {
        free(new_node);
        return NULL;
    }

    strncpy(new_node->value, str, strlen(str) + 1);
    return new_node;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = q_new_element(s);

    if (!new_node)
        return false;

    list_add(&new_node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = q_new_element(s);

    if (!new_node)
        return false;

    list_add_tail(&new_node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *ptr = list_first_entry(head, element_t, list);
    list_del_init(&ptr->list);
    if (sp) {
        strncpy(sp, ptr->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return ptr;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *ptr = list_last_entry(head, element_t, list);
    list_del_init(&ptr->list);
    if (sp) {
        strncpy(sp, ptr->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return ptr;
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

void q_delete_element(struct list_head *node)
{
    element_t *element = list_entry(node, element_t, list);
    list_del_init(&element->list);
    q_release_element(element);
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *slow_mid = head->next;
    for (const struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        slow_mid = slow_mid->next;
    }
    q_delete_element(slow_mid);

    return true;
}

int q_strncmp(const struct list_head *a, const struct list_head *b)
{
    const element_t *e_a = list_entry(a, element_t, list);
    const element_t *e_b = list_entry(b, element_t, list);
    int len_a = strlen(e_a->value);
    int len_b = strlen(e_b->value);
    int n = len_a > len_b ? len_a + 1 : len_b + 1;
    return strncmp(e_a->value, e_b->value, n);
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    if (list_is_singular(head))
        return true;

    /*
     * If the value of string in "from" is the same as "to", set the boolean
     * variable "delete" to true and move the "to" pointer forward until a
     * different string value is encountered. Then, delete all nodes from "from"
     * to "to" (not include).
     */

    bool delete = false;
    struct list_head *from = head->next, *to = head->next->next;

    while (from != head && to != head) {
        while (to != head && !q_strncmp(from, to)) {
            delete = true;
            to = to->next;
        }
        if (delete) {
            delete = false;
            struct list_head *tmp = from;
            while (from != to) {
                from = from->next;
                q_delete_element(tmp);
                tmp = from;
            }
        }
        from = to;
        to = to->next;
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/*
 * Be aware that the function will only work correctly when
 * the parameter l is closer to the head than r.
 */
void q_swap_two_node(struct list_head *l, struct list_head *r)
{
    if (l->next == r) {
        struct list_head *l_left = l->prev, *r_right = r->next;

        l->prev = r;
        l->next = r_right;
        r->prev = l_left;
        r->next = l;

        l_left->next = r;
        r_right->prev = l;

    } else {
        struct list_head *l_left = l->prev, *l_right = l->next;
        struct list_head *r_left = r->prev, *r_right = r->next;

        l->prev = r_left;
        l->next = r_right;
        r->prev = l_left;
        r->next = l_right;

        l_left->next = l_right->prev = r;
        r_left->next = r_right->prev = l;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *entry = NULL, *safe = NULL;
    list_for_each_safe (entry, safe, head) {
        entry->next = entry->prev;
        entry->prev = safe;
    }
    entry->next = entry->prev;
    entry->prev = safe;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k == 1)
        return;

    struct list_head tmp_list, *tmp_head = &tmp_list;
    struct list_head reverse_list, *reverse_head = &reverse_list;
    struct list_head *ptr = head->next;
    INIT_LIST_HEAD(tmp_head);
    INIT_LIST_HEAD(reverse_head);

    while (ptr != head) {
        int i = 0;
        for (i = 0; i < k && ptr != head; i++) {
            ptr = ptr->next;
        }
        if (i == k) {
            list_cut_position(tmp_head, head, ptr->prev);
            q_reverse(tmp_head);
            list_splice_tail_init(tmp_head, reverse_head);
        }
    }
    list_splice(reverse_head, head);
}

struct list_head *q_bubble_sort(struct list_head *head, bool descend)
{
    for (struct list_head *ptr_end = head; ptr_end != head->next;
         ptr_end = ptr_end->prev) {
        for (struct list_head *ptr_a = head->next, *ptr_b = head->next->next;
             ptr_b != ptr_end; ptr_a = ptr_b, ptr_b = ptr_b->next) {
            if ((q_strncmp(ptr_a, ptr_b) > 0) ^ descend) {
                q_swap_two_node(ptr_a, ptr_b);
                struct list_head *tmp = ptr_b;
                ptr_b = ptr_a;
                ptr_a = tmp;
            }
        }
    }
    return head;
}

struct list_head *q_merge_sort(struct list_head *head, bool descend)
{
    if (list_empty(head) || list_is_singular(head))
        return head;

    struct list_head *slow_mid = head->next;
    for (const struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        slow_mid = slow_mid->next;
    }

    struct list_head mid_list, *mid_head = &mid_list;
    INIT_LIST_HEAD(mid_head);
    list_cut_position(mid_head, head, slow_mid->prev);

    struct list_head *left = q_merge_sort(head, descend),
                     *right = q_merge_sort(mid_head, descend);
    return q_merge_two_lists(left, right, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /*
     * If you want to use bubble sort, then call q_bubble_sort(head, descend);
     * If you want to use merge sort, then call q_merge_sort(head, descend);
     *
     * Be careful, the descending order sorting is not stable sorting.
     */

    q_merge_sort(head, descend);
}


/* Delete every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return 1;

    struct list_head *small = head->next, *big = head->next->next;
    int cnt = 0;
    while (small != head && big != head) {
        while (big != head && q_strncmp(small, big) > 0) {
            q_delete_element(big);
            big = small->next;
        }
        small = big;
        big = big->next;
        cnt += 1;
    }
    return cnt;
}

/* Delete every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return 1;

    struct list_head *big = head->prev->prev, *small = head->prev;
    int cnt = 0;
    while (big != head && small != head) {
        while (big != head && q_strncmp(big, small) < 0) {
            q_delete_element(big);
            big = small->prev;
        }
        small = big;
        big = big->prev;
        cnt += 1;
    }
    return cnt;
}

struct list_head *q_merge_two_lists(struct list_head *head_a,
                                    struct list_head *head_b,
                                    bool descend)
{
    struct list_head *ptr = head_a;
    struct list_head *a = head_a->next, *b = head_b->next;

    for (struct list_head **node = NULL; a != head_a && b != head_b;
         *node = (*node)->next) {
        node = ((q_strncmp(a, b) < 0) ^ descend) ? &a : &b;
        ptr->next = *node;
        (*node)->prev = ptr;
        ptr = (*node);
    }
    if (a == head_a) {
        ptr->next = b;
        b->prev = ptr;
        head_b->prev->next = head_a;
        head_a->prev = head_b->prev;

    } else {
        ptr->next = a;
        a->prev = ptr;
    }
    INIT_LIST_HEAD(head_b);
    return head_a;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    int size = list_entry(head, queue_contex_t, chain)->size;
    if (size == 0)
        return 0;
    else if (size == 1)
        return q_size(list_entry(head->next, queue_contex_t, chain)->q);

    struct list_head *ptr = head->next, *ptr_end = head->prev;
    for (; size != 1; size = (size + 1) / 2) {
        for (int cnt = size / 2; cnt != 0; cnt--) {
            struct list_head *left = list_entry(ptr, queue_contex_t, chain)->q;
            struct list_head *right =
                list_entry(ptr_end, queue_contex_t, chain)->q;
            q_merge_two_lists(left, right, descend);
            right = NULL;
            ptr = ptr->next;
            ptr_end = ptr_end->prev;
        }
        ptr = head->next;
    }

    return q_size(list_entry(ptr, queue_contex_t, chain)->q);
}

void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    srand(time(NULL));

    struct list_head *ptr_end = head->prev;

    for (int turn = q_size(head) - 1; turn; turn -= 1) {
        struct list_head *ptr = head->next;
        int r = rand();
        for (r = (int) ((float) r / ((unsigned) RAND_MAX + 1) * turn); r;
             r -= 1) {
            ptr = ptr->next;
        }
        q_swap_two_node(ptr, ptr_end);
        ptr_end = ptr->prev;
    }
}