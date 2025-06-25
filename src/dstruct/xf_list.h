/**
 * @file xf_list.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 移除 gnu 特性的双向链表.
 * @version 1.0
 * @date 2024-06-25
 *
 * SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details
 *
 * 简单的双链表实现.
 * 一些内部函数（"__xxx"）在处理整个链表而不是单个条目时非常有用，
 * 因为有时我们已经知道下一个/上一个条目，
 * 直接使用它们而不是使用通用的单条目例程可以生成更好的代码。
 *
 * @attention 修改的部分:
 * 1. 添加 `xf_` 前缀防止冲突. 优先使用类型定义而不是结构体.
 * 2. 添加中文注释.
 * 3. 移除 GNU 特性, 如 `typeof`, 因此需要显式写出类型.
 */

#ifndef __XF_LIST_H__
#define __XF_LIST_H__

/* ==================== [Includes] ========================================== */

#include "../common/xf_common.h"

/**
 * @cond XFAPI_USER
 * @ingroup group_xf_utils_common
 * @defgroup group_xf_utils_common_list xf_list
 * @brief 移除 gnu 特性的双向链表和哈希链表。
 * @endcond
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#if !defined(XF_LIST_POISON1)
#   define XF_LIST_POISON1              0x00100100
#endif
#if !defined(XF_LIST_POISON2)
#   define XF_LIST_POISON2              0x00200200
#endif

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 双向链表结构体.
 */
typedef struct xf_list_head {
    struct xf_list_head *next, *prev;
} xf_list_t;
/* 双链表中，表结构等同于节点结构 */
typedef xf_list_t xf_list_head_t;
typedef xf_list_t xf_list_node_t;

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
//static define
/**
 * @brief 静态定义时初始化链表。
 */
#define XF_LIST_HEAD_INIT(name) { &(name), &(name) }
#define XF_INIT_LIST_HEAD(_name) XF_LIST_HEAD_INIT(_name)

/**
 * @brief 定义一个名叫 `name` 的链表, 且以 `name` 为链表头.
 */
#define XF_LIST_HEAD(name) \
    xf_list_t name = XF_LIST_HEAD_INIT(name)

/**
 * @brief 哈希链表结构体.
 */
typedef struct xf_hlist_head {
    struct xf_hlist_node *first;
} xf_hlist_t;

typedef struct xf_hlist_node {
    struct xf_hlist_node *next, **pprev;
} xf_hlist_node_t;

#define XF_HLIST_HEAD_INIT { .first = NULL }
#define XF_HLIST_HEAD(name) xf_hlist_t name = {  .first = NULL }
#define XF_INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 动态初始化链表.
 *
 * @param list 被初始化的链表节点.
 */
__STATIC_INLINE void xf_list_init(xf_list_t *list)
{
    list->next = list;
    list->prev = list;
}
#define XF_LIST_INIT(_list) xf_list_init(_list)
#define XF_LIST_NODE_INIT(_list) xf_list_init(_list)

/*
 * Insert a new_node entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */

/**
 * @brief 在两个已知的连续节点之间插入一个 new_node 节点.
 *
 * @attention 仅用于内部链表操作, 因为已经知道前一个/后一个节点！
 *
 * @param new_node
 * @param prev
 * @param next
 */
__STATIC_INLINE void __xf_list_add(
    xf_list_t *new_node, xf_list_t *prev, xf_list_t *next)
{
    next->prev      = new_node;
    new_node->next  = next;
    new_node->prev  = prev;
    prev->next      = new_node;
}

/*
 * list_add - add a new_node entry
 * @new_node: new_node entry to be added
 * @head: list head to add it after
 *
 * Insert a new_node entry after the specified head.
 * This is good for implementing stacks.
 */

/**
 * @brief xf_list_add - 在指定节点之后添加一个 new_node.
 *
 * 这对于实现栈(stacks)很有好处.
 *
 * @param new_node 要添加的 new_node.
 * @param head 需要添加 new_node 到其后面的节点,
 * 也就是 new_node 将添加到 head 节点后面.
 */
__STATIC_INLINE void xf_list_add(xf_list_t *new_node, xf_list_t *head)
{
    __xf_list_add(new_node, head, head->next);
}

/*
 * list_add_tail - add a new_node entry
 * @new_node: new_node entry to be added
 * @head: list head to add it before
 *
 * Insert a new_node entry before the specified head.
 * This is useful for implementing queues.
 */

/**
 * @brief xf_list_add_tail - 在指定节点之前添加一个 new_node.
 *
 * 这对于实现队列(queues)很有用.
 *
 * @param new_node 要添加的 new_node.
 * @param head 需要添加 new_node 到其前面的节点,
 * 也就是 new_node 将添加到 head 节点前面.
 */
__STATIC_INLINE void xf_list_add_tail(xf_list_t *new_node, xf_list_t *head)
{
    __xf_list_add(new_node, head->prev, head);
}

/*
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */

/**
 * @brief 删除链表节点(list entry), 使前一个/后一个节点相互指向.
 *
 * @attention 仅用于内部链表操作, 因为已经知道前一个/后一个节点！
 *
 * @param prev
 * @param next
 */
__STATIC_INLINE void __xf_list_del(xf_list_t *prev, xf_list_t *next)
{
    next->prev = prev;
    prev->next = next;
}

/*
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */

__STATIC_INLINE void __xf_list_del_entry(xf_list_t *entry)
{
    __xf_list_del(entry->prev, entry->next);
}

/**
 * @brief xf_list_del - 从链表中删除节点.
 *
 * @param entry 要从链表中删除的元素.
 *
 * @note 之后, 节点上的 xf_list_empty() 不会返回 true, 此时节点处于未定义状态.
 */
__STATIC_INLINE void xf_list_del(xf_list_t *entry)
{
    __xf_list_del(entry->prev, entry->next);
    entry->next = (xf_list_t *)XF_LIST_POISON1;
    entry->prev = (xf_list_t *)XF_LIST_POISON2;
}

/*
 * list_replace - replace old entry by new_node one
 * @old : the element to be replaced
 * @new_node : the new_node element to insert
 *
 * If @old was empty, it will be overwritten.
 */

/**
 * @brief xf_list_replace - 用 new_node 替换旧节点.
 *
 * 如果 `old` 为空, 则会被覆盖. (If `old` was empty, it will be overwritten.)
 *
 * @param old 被替换的节点.
 * @param new_node 要插入的 new_node 节点.
 */
__STATIC_INLINE void xf_list_replace(xf_list_t *old, xf_list_t *new_node)
{
    new_node->next          = old->next;
    new_node->next->prev    = new_node;
    new_node->prev          = old->prev;
    new_node->prev->next    = new_node;
}

/**
 * @brief xf_list_replace_init - 用 new_node 替换旧节点, 并重新初始化旧节点.
 *
 * @param old 被替换的节点.
 * @param new_node 要插入的 new_node 节点.
 */
__STATIC_INLINE void xf_list_replace_init(xf_list_t *old, xf_list_t *new_node)
{
    xf_list_replace(old, new_node);
    xf_list_init(old);
}

/*
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */

/**
 * @brief xf_list_del_init - 从链表中删除节点, 并重新初始化.
 *
 * @param entry 要从链表中删除的节点.
 */
__STATIC_INLINE void xf_list_del_init(xf_list_t *entry)
{
    __xf_list_del_entry(entry);
    xf_list_init(entry);
}

/*
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */

/**
 * @brief xf_list_move - 从一个链表中删除指定节点, 并添加为另一个链表的头节点。.
 *
 * @param list 要移动的节点.
 * @param head list 节点进入之前的头节点,
 *  也就是 list 进入之后, head 变为 list 后一个节点.
 */
__STATIC_INLINE void xf_list_move(xf_list_t *list, xf_list_t *head)
{
    __xf_list_del_entry(list);
    xf_list_add(list, head);
}

/*
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */

/**
 * @brief xf_list_move_tail - 从一个链表中删除指定节点, 并添加为另一个链表的尾节点.
 *
 * @param list 要移动的节点.
 * @param head list 节点进入之后的头节点.
 */
__STATIC_INLINE void xf_list_move_tail(xf_list_t *list, xf_list_t *head)
{
    __xf_list_del_entry(list);
    xf_list_add_tail(list, head);
}

/*
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */

/**
 * @brief xf_list_is_last - 测试 list 是否是链表 head 中的最后一个节点.
 *
 * @param list 要测试的节点.
 * @param head 链表头.
 * @return int
 * - 1          是最后一个节点
 * - 0          不是最后一个节点
 */
__STATIC_INLINE int xf_list_is_last(const xf_list_t *list, const xf_list_t *head)
{
    return list->next == head;
}

/*
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */

/**
 * @brief xf_list_empty - 测试链表是否为空.
 *
 * @param head 要测试的链表头节点.
 * @return int
 * - 1          空表
 * - 0          非空表
 */
__STATIC_INLINE int xf_list_empty(const xf_list_t *head)
{
    return head->next == head;
}

/*
 * xf_list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using xf_list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is xf_list_del_init(). Eg. it cannot be used
 * if another CPU could re-xf_list_add() it.
 */

/**
 * @brief xf_list_empty_careful - 测试链表是否为空且未被修改.
 *
 * 测试链表是否为空并检查是否没有其他 CPU 正在修改任一成员（下一个或上一个）.
 *
 * @attention 不同步地使用 xf_list_empty_careful(),
 * 只有在 xf_list_del_init() 是链表节点唯一可能发生的活动时才是安全的.
 * 例如, 如果另一个 CPU 可以重新添加 xf_list_add(), 则不能使用该函数.
 *
 * @param head 要测试的链表头节点.
 * @return int
 * - 1          为空且未被修改
 * - 0          不为空或正在被修改
 */
__STATIC_INLINE int xf_list_empty_careful(const xf_list_t *head)
{
    xf_list_t *next = head->next;
    return (next == head) && (next == head->prev);
}

/*
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */

/**
 * @brief xf_list_rotate_left - 将链表向左旋转.
 *
 * @param head 链表头节点.
 */
__STATIC_INLINE void xf_list_rotate_left(xf_list_t *head)
{
    xf_list_t *first;

    if (!xf_list_empty(head)) {
        first = head->next;
        xf_list_move_tail(first, head);
    }
}

/*
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */

/**
 * @brief xf_list_is_singular - 测试链表是否只有一个节点.
 *
 * @param head 链表头节点.
 * @return int
 * - 1          只有一个节点
 * - 0          不只有一个节点
 */
__STATIC_INLINE int xf_list_is_singular(const xf_list_t *head)
{
    return !xf_list_empty(head) && (head->next == head->prev);
}

__STATIC_INLINE void __xf_list_cut_position(
    xf_list_t *list, xf_list_t *head, xf_list_t *entry)
{
    xf_list_t *new_node_first   = entry->next;
    list->next                  = head->next;
    list->next->prev            = list;
    list->prev                  = entry;
    entry->next                 = list;
    head->next                  = new_node_first;
    new_node_first->prev        = head;
}

/*
 * xf_list_cut_position - cut a list into two
 * @list: a new_node list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *  and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */

/**
 * @brief xf_list_cut_position - 将链表切成两部分.
 *
 * 该辅助函数将 head 的初始部分（包括 entry 在内）从 head 移至 list.
 * 你应该在 entry 中传递一个你知道在 head 中的元素.
 * list 应该是一个空链表, 或者是一个你不希望丢失数据的链表.
 *
 * @param list 一个 new_node 链表, 用于添加所有已删除的节点.
 * @param head 包含节点的链表
 * @param entry head 中的一个节点, 可以是 head 本身, 如果这样, 则不会删除该链表.
 */
__STATIC_INLINE void xf_list_cut_position(
    xf_list_t *list, xf_list_t *head, xf_list_t *entry)
{
    if (xf_list_empty(head)) {
        return;
    }
    if (xf_list_is_singular(head)
            && (head->next != entry && head != entry)) {
        return;
    }
    if (entry == head) {
        xf_list_init(list);
    } else {
        __xf_list_cut_position(list, head, entry);
    }
}

__STATIC_INLINE void __xf_list_splice(
    const xf_list_t *list, xf_list_t *prev, xf_list_t *next)
{
    xf_list_t *first = list->next;
    xf_list_t *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

/*
 * xf_list_splice - join two lists, this is designed for stacks
 * @list: the new_node list to add.
 * @head: the place to add it in the first list.
 */

/**
 * @brief xf_list_splice - 连接两个链表, 这是为栈设计的.
 *
 * @param list 要添加的 new_node 链表.
 * @param head 将其添加到第一个链表中的位置.
 */
__STATIC_INLINE void xf_list_splice(
    const xf_list_t *list, xf_list_t *head)
{
    if (!xf_list_empty(list)) {
        __xf_list_splice(list, head, head->next);
    }
}

/*
 * xf_list_splice_tail - join two lists, each list being a queue
 * @list: the new_node list to add.
 * @head: the place to add it in the first list.
 */

/**
 * @brief xf_list_splice_tail - 连接两个链表, 每个链表都是一个队列
 *
 * @param list 要添加的 new_node 链表.
 * @param head 将其添加到第一个链表中的位置.
 */
__STATIC_INLINE void xf_list_splice_tail(xf_list_t *list, xf_list_t *head)
{
    if (!xf_list_empty(list)) {
        __xf_list_splice(list, head->prev, head);
    }
}

/*
 * xf_list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new_node list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */

/**
 * @brief xf_list_splice_init - 连接两个链表并重新初始化空链表.
 *
 * list 处的链表被重新初始化.
 *
 * @param list 要添加的 new_node 链表.
 * @param head 将其添加到第一个链表中的位置.
 */
__STATIC_INLINE void xf_list_splice_init(xf_list_t *list, xf_list_t *head)
{
    if (!xf_list_empty(list)) {
        __xf_list_splice(list, head, head->next);
        xf_list_init(list);
    }
}

/*
 * xf_list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new_node list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */

/**
 * @brief xf_list_splice_tail_init - 连接两个链表并重新初始化空链表.
 *
 * 每个链表都是一个队列.
 * list 处的链表被重新初始化.
 *
 * @param list 要添加的 new_node 链表.
 * @param head 将其添加到第一个链表中的位置.
 */
__STATIC_INLINE void xf_list_splice_tail_init(xf_list_t *list, xf_list_t *head)
{
    if (!xf_list_empty(list)) {
        __xf_list_splice(list, head->prev, head);
        xf_list_init(list);
    }
}

__STATIC_INLINE void xf_init_hlist_node(xf_hlist_node_t *h)
{
    h->next = NULL;
    h->pprev = NULL;
}
#define XF_INIT_HLIST_NODE(_h) xf_init_hlist_node(_h)

/*
 * xf_hlist_unhashed - Has node been removed from list and reinitialized?
 * @h: Node to be checked
 *
 * Not that not all removal functions will leave a node in unhashed
 * state.  For example, xf_hlist_nulls_del_init_rcu() does leave the
 * node in unhashed state, but xf_hlist_nulls_del() does not.
 */
__STATIC_INLINE int xf_hlist_unhashed(const xf_hlist_node_t *h)
{
    return !h->pprev;
}

/*
 * xf_hlist_unhashed_lockless - Version of xf_hlist_unhashed for lockless use
 * @h: Node to be checked
 *
 * This variant of xf_hlist_unhashed() must be used in lockless contexts
 * to avoid potential load-tearing.  The XF_READ_ONCE() is paired with the
 * various XF_WRITE_ONCE() in hlist helpers that are defined below.
 */
__STATIC_INLINE int xf_hlist_unhashed_lockless(const xf_hlist_node_t *h)
{
    return !XF_READ_ONCE(h->pprev);
}

/*
 * xf_hlist_empty - Is the specified xf_hlist_head structure an empty hlist?
 * @h: Structure to check.
 */
__STATIC_INLINE int xf_hlist_empty(const xf_hlist_t *h)
{
    return !XF_READ_ONCE(h->first);
}

__STATIC_INLINE void __xf_hlist_del(xf_hlist_node_t *n)
{
    xf_hlist_node_t *next = n->next;
    xf_hlist_node_t **pprev = n->pprev;

    XF_WRITE_ONCE(*pprev, next);
    if (next) {
        XF_WRITE_ONCE(next->pprev, pprev);
    }
}

/*
 * xf_hlist_del - Delete the specified xf_hlist_node from its list
 * @n: Node to delete.
 *
 * Note that this function leaves the node in hashed state.  Use
 * xf_hlist_del_init() or similar instead to unhash @n.
 */
__STATIC_INLINE void xf_hlist_del(xf_hlist_node_t *n)
{
    __xf_hlist_del(n);
    n->next = (xf_hlist_node_t *)XF_LIST_POISON1;
    n->pprev = (xf_hlist_node_t **)XF_LIST_POISON2;
}

/*
 * xf_hlist_del_init - Delete the specified xf_hlist_node from its list and initialize
 * @n: Node to delete.
 *
 * Note that this function leaves the node in unhashed state.
 */
__STATIC_INLINE void xf_hlist_del_init(xf_hlist_node_t *n)
{
    if (!xf_hlist_unhashed(n)) {
        __xf_hlist_del(n);
        XF_INIT_HLIST_NODE(n);
    }
}

/*
 * xf_hlist_add_head - add a new entry at the beginning of the hlist
 * @n: new entry to be added
 * @h: hlist head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
__STATIC_INLINE void xf_hlist_add_head(xf_hlist_node_t *n, xf_hlist_t *h)
{
    xf_hlist_node_t *first = h->first;
    XF_WRITE_ONCE(n->next, first);
    if (first) {
        XF_WRITE_ONCE(first->pprev, &n->next);
    }
    XF_WRITE_ONCE(h->first, n);
    XF_WRITE_ONCE(n->pprev, &h->first);
}

/*
 * xf_hlist_add_before - add a new entry before the one specified
 * @n: new entry to be added
 * @next: hlist node to add it before, which must be non-NULL
 */
__STATIC_INLINE void xf_hlist_add_before(xf_hlist_node_t *n, xf_hlist_node_t *next)
{
    XF_WRITE_ONCE(n->pprev, next->pprev);
    XF_WRITE_ONCE(n->next, next);
    XF_WRITE_ONCE(next->pprev, &n->next);
    XF_WRITE_ONCE(*(n->pprev), n);
}

/*
 * xf_hlist_add_behind - add a new entry after the one specified
 * @n: new entry to be added
 * @prev: hlist node to add it after, which must be non-NULL
 */
__STATIC_INLINE void xf_hlist_add_behind(xf_hlist_node_t *n, xf_hlist_node_t *prev)
{
    XF_WRITE_ONCE(n->next, prev->next);
    XF_WRITE_ONCE(prev->next, n);
    XF_WRITE_ONCE(n->pprev, &prev->next);

    if (n->next) {
        XF_WRITE_ONCE(n->next->pprev, &n->next);
    }
}

/*
 * xf_hlist_add_fake - create a fake hlist consisting of a single headless node
 * @n: Node to make a fake list out of
 *
 * This makes @n appear to be its own predecessor on a headless hlist.
 * The point of this is to allow things like xf_hlist_del() to work correctly
 * in cases where there is no list.
 */
__STATIC_INLINE void xf_hlist_add_fake(xf_hlist_node_t *n)
{
    n->pprev = &n->next;
}

/*
 * xf_hlist_fake: Is this node a fake hlist?
 * @h: Node to check for being a self-referential fake hlist.
 */
__STATIC_INLINE bool xf_hlist_fake(xf_hlist_node_t *h)
{
    return h->pprev == &h->next;
}

/*
 * xf_hlist_is_singular_node - is node the only element of the specified hlist?
 * @n: Node to check for singularity.
 * @h: Header for potentially singular list.
 *
 * Check whether the node is the only node of the head without
 * accessing head, thus avoiding unnecessary cache misses.
 */
__STATIC_INLINE bool
hlist_is_singular_node(xf_hlist_node_t *n, xf_hlist_t *h)
{
    return !n->next && n->pprev == &h->first;
}

/*
 * xf_hlist_move_list - Move an hlist
 * @old: xf_hlist_head for old list.
 * @new: xf_hlist_head for new list.
 *
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
__STATIC_INLINE void xf_hlist_move_list(xf_hlist_t *old, xf_hlist_t *new)
{
    new->first = old->first;
    if (new->first) {
        new->first->pprev = &new->first;
    }
    old->first = NULL;
}

/*
 * xf_hlist_splice_init() - move all entries from one list to another
 * @from: xf_hlist_head from which entries will be moved
 * @last: last entry on the @from list
 * @to:   xf_hlist_head to which entries will be moved
 *
 * @to can be empty, @from must contain at least @last.
 */
__STATIC_INLINE void xf_hlist_splice_init(
    xf_hlist_t *from, xf_hlist_node_t *last, xf_hlist_t *to)
{
    if (to->first) {
        to->first->pprev = &last->next;
    }
    last->next = to->first;
    to->first = from->first;
    from->first->pprev = &to->first;
    from->first = NULL;
}

/* ==================== [Macros] ============================================ */

/*
 * list_entry - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */

/**
 * @brief xf_list_entry - 获取节点的结构体.
 *
 * 从包含链表节点的结构体中获得结构体指针.
 *
 * @param ptr 指向成员的指针.
 * @param type 包含 xf_list_t 的结构体类型.
 * @param member 结构中成员 list_struct 的名称.
 * @return 结构体的地址.
 */
#define xf_list_entry(ptr, type, member) \
    xf_container_of(ptr, type, member)

/*
 * list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */

/**
 * @brief xf_list_first_entry - 获取链表中的第一个元素.
 *
 * 请注意，该链表不应为空.
 *
 * @param ptr 链表头.
 * @param type 包含 xf_list_t 的结构体类型.
 * @param member 结构中成员 list_struct 的名称.
 * @return 结构体的地址.
 */
#define xf_list_first_entry(ptr, type, member) \
    xf_list_entry((ptr)->next, type, member)

/*
 * list_for_each    -   iterate over a list
 * @pos:    the &struct list_head to use as a loop cursor.
 * @head:   the head for your list.
 */

/**
 * @brief xf_list_for_each - 迭代链表.
 *
 * @param pos 迭代游标 &xf_list_t.
 * @param head 链表头.
 */
#define xf_list_for_each(pos, head) \
    for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

/*
 * __list_for_each  -   iterate over a list
 * @pos:    the &struct list_head to use as a loop cursor.
 * @head:   the head for your list.
 *
 * This variant doesn't differ from list_for_each() any more.
 * We don't do prefetching in either case.
 */

/**
 * @brief __xf_list_for_each - 迭代链表.
 *
 * 此变体与 xf_list_for_each() 不再有区别。
 *
 * @param pos 迭代游标 &xf_list_t.
 * @param head 链表头.
 */
#define __xf_list_for_each(pos, head) \
    for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

/*
 * list_for_each_prev   -   iterate over a list backwards
 * @pos:    the &struct list_head to use as a loop cursor.
 * @head:   the head for your list.
 */

/**
 * @brief xf_list_for_each_prev - 反向迭代链表.
 *
 * @param pos 迭代游标 &xf_list_t.
 * @param head 链表头.
 */
#define xf_list_for_each_prev(pos, head) \
    for ((pos) = (head)->prev; (pos) != (head); (pos) = (pos)->prev)

/*
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop cursor.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */

/**
 * @brief xf_list_for_each_safe - 迭代链表的安全版本, 防止删除链表节点时可能出现的问题.
 *
 * @param pos 迭代游标 &xf_list_t.
 * @param n 另一个 &xf_list_t 用于临时存储.
 * @param head 链表头.
 */
#define xf_list_for_each_safe(pos, n, head) \
    for ((pos) = (head)->next, (n) = (pos)->next; (pos) != (head); \
         (pos) = (n), (n) = (pos)->next)

/*
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop cursor.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */

/**
 * @brief xf_list_for_each_prev_safe - 方向迭代链表的安全版本, 防止删除链表节点时可能出现的问题.
 *
 * @param pos 迭代游标 &xf_list_t.
 * @param n 另一个 &xf_list_t 用于临时存储.
 * @param head 链表头.
 */
#define xf_list_for_each_prev_safe(pos, n, head) \
    for ((pos) = (head)->prev, (n) = (pos)->prev; \
         (pos) != (head); \
         (pos) = (n), (n) = (pos)->prev)

/**
 * @attention 注意, 由于移除了 GUN 的 `typeof`,
 * 以下带有 `_entry` 后缀的宏都必须显式写出包含链表的含有链表节点的结构体的类型。，
 * 详情见 @ref xf_container_of.
 *
 * pos 和 n 的数据类型必须相同!
 */

/*
 * list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */

/**
 * @brief list_for_each_entry - 迭代给定类型的链表。
 *
 * 正序迭代含有链表节点的结构体的链表，并取得结构体对象指针。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。。
 */
#define xf_list_for_each_entry(pos, head, type, member) \
    for ((pos) = xf_list_entry((head)->next, type, member); \
         &(pos)->member != (head); \
         (pos) = xf_list_entry((pos)->member.next, type, member))

/*
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */

/**
 * @brief list_for_each_entry_reverse - 倒序迭代给定类型的链表。
 *
 * 倒序迭代含有链表节点的结构体的链表，并取得结构体对象指针。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_for_each_entry_reverse(pos, head, type, member) \
    for ((pos) = xf_list_entry((head)->prev, type, member); \
         &(pos)->member != (head); \
         (pos) = xf_list_entry((pos)->member.prev, type, member))

/*
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:    the type * to use as a start point
 * @head:   the head of the list
 * @member: the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */

/**
 * @brief list_prepare_entry - 准备一个 pos 条目以在 list_for_each_entry_continue() 中使用。
 *
 * 在 xf_list_for_each_entry_continue() 调用之前，确保传入的 pos 有效。
 * 如果 pos 不存在，则从头强制拓展一个 pos 参数。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_prepare_entry(pos, head, type, member) \
    ((pos) ? : xf_list_entry(head, type, member))

/*
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */

/**
 * @brief list_for_each_entry_continue - 继续迭代给定类型的链表。
 *
 * 继续迭代给定类型的链表，在当前位置之后继续（迭代不包括当前节点）。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_for_each_entry_continue(pos, head, type, member) \
    for ((pos) = xf_list_entry((pos)->member.next, type, member); \
         &(pos)->member != (head); \
         (pos) = xf_list_entry((pos)->member.next, type, member))

/*
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */

/**
 * @brief list_for_each_entry_continue_reverse - 从给定点倒序迭代。
 *
 * 迭代之前迭代过的链表（迭代不包括当前值且顺序相反）。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_for_each_entry_continue_reverse(pos, head, type, member) \
    for ((pos) = xf_list_entry((pos)->member.prev, type, member); \
         &(pos)->member != (head); \
         (pos) = xf_list_entry((pos)->member.prev, type, member))

/*
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */

/**
 * @brief list_for_each_entry_from - 从当前点开始迭代给定类型的链表。
 *
 * 继续迭代给定类型的链表，从当前位置开始迭代（迭代包括当前值）。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_for_each_entry_from(pos, head, type, member) \
    for (; &(pos)->member != (head); \
         (pos) = xf_list_entry((pos)->member.next, type, member))

/*
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */

/**
 * @brief list_for_each_entry_safe - 安全地迭代给定类型的链表，可删除链表节点。
 *
 * 迭代链表的安全版本，额外提供结构体指针 n 用于缓存下一个节点。
 *
 * @note 在迭代的过程中需要删除链表节点时，使用该宏以确保链表后续能正确迭代。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param n 用作迭代缓存的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_for_each_entry_safe(pos, n, head, type, member) \
    for ((pos) = xf_list_entry((head)->next, type, member), \
         (n) = xf_list_entry((pos)->member.next, type, member); \
         &(pos)->member != (head); \
         (pos) = (n), (n) = xf_list_entry(n->member.next, type, member))

/*
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */

/**
 * @brief list_for_each_entry_safe_continue - 安全地继续迭代链表，可删除链表节点。
 *
 * 从给定点继续迭代链表的安全版本（迭代不包括当前节点），额外提供结构体指针 n 用于缓存下一个节点。
 *
 * @note 在迭代的过程中需要删除链表节点时，使用该宏以确保链表后续能正确迭代。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param n 用作迭代缓存的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_for_each_entry_safe_continue(pos, n, head, type, member) \
    for ((pos) = xf_list_entry((pos)->member.next, type, member), \
         (n) = xf_list_entry((pos)->member.next, type, member); \
         &(pos)->member != (head); \
         (pos) = (n), (n) = xf_list_entry((n)->member.next, type, member))

/*
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */

/**
 * @brief list_for_each_entry_safe_from - 安全地从当前点迭代链表，可删除链表节点。
 *
 * 从给定点迭代链表的安全版本（迭代包括当前节点），额外提供结构体指针 n 用于缓存下一个节点。
 *
 * @note 在迭代的过程中需要删除链表节点时，使用该宏以确保链表后续能正确迭代。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param n 用作迭代缓存的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_for_each_entry_safe_from(pos, n, head, type, member) \
    for ((n) = xf_list_entry((pos)->member.next, type, member); \
         &(pos)->member != (head); \
         (pos) = (n), (n) = xf_list_entry((n)->member.next, type, member))

/*
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */

/**
 * @brief list_for_each_entry_safe_reverse - 安全地倒序迭代链表，可删除链表节点。
 *
 * 倒序迭代链表的安全版本，额外提供结构体指针 n 用于缓存下一个节点。
 *
 * @note 在迭代的过程中需要删除链表节点时，使用该宏以确保链表后续能正确迭代。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param n 用作迭代缓存的结构体指针（类型为参数 type 的指针）。
 * @param head 链表的头节点。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_for_each_entry_safe_reverse(pos, n, head, type, member) \
    for ((pos) = xf_list_entry((head)->prev, type, member), \
         (n) = xf_list_entry((pos)->member.prev, type, member); \
         &(pos)->member != (head); \
         (pos) = (n), (n) = xf_list_entry((n)->member.prev, type, member))

/*
 * list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @pos:    the loop cursor used in the list_for_each_entry_safe loop
 * @n:      temporary storage used in list_for_each_entry_safe
 * @member: the name of the list_struct within the struct.
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */

/**
 * @brief list_safe_reset_next - 重置过时的 list_for_each_entry_safe 循环。
 *
 * 刷新用于缓存的节点 n(因为该节点可能过时)。
 *
 * 如果链表可能被并发修改（例如，锁在循环体中被删除），
 * 那么 xf_list_safe_reset_next 通常使用起来不安全。
 * 一个例外是，如果游标元素 (pos) 固定在链表中，
 * 并且在重新获取锁定之后和完成循环体的当前迭代之前调用 xf_list_safe_reset_next。
 *
 * @param pos 用作迭代游标的结构体指针（类型为参数 type 的指针）。
 * @param n 用作迭代缓存的结构体指针（类型为参数 type 的指针）。
 * @param type 含有链表节点的结构体的类型。
 * @param member 链表节点在结构体中的成员名。
 */
#define xf_list_safe_reset_next(pos, n, type, member) \
    (n) = xf_list_entry((pos)->member.next, type, member)

/*
 * list_count_nodes - count nodes in the list
 * @head:   the head for your list.
 */
__STATIC_INLINE size_t xf_list_count_nodes(xf_list_t *head)
{
    xf_list_t *pos;
    size_t count = 0;
    xf_list_for_each(pos, head) {
        count++;
    }
    return count;
}

#define xf_hlist_entry(ptr, type, member) xf_container_of(ptr, type, member)

#define xf_hlist_for_each(pos, head) \
    for (pos = (head)->first; pos ; pos = pos->next)

#define xf_hlist_for_each_safe(pos, n, head) \
    for (pos = (head)->first; pos && ((n = pos->next, 1)); pos = n)

#define xf_hlist_entry_safe(ptr, type, member) \
    ((ptr) ? xf_hlist_entry(ptr, type, member) : NULL)

/*
 * xf_hlist_for_each_entry - iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the xf_hlist_node within the struct.
 */
#define xf_hlist_for_each_entry(pos, head, type, member) \
    for (pos = xf_hlist_entry_safe((head)->first, type, member); \
         pos; \
         pos = xf_hlist_entry_safe((pos)->member.next, type, member))

/*
 * xf_hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @pos:    the type * to use as a loop cursor.
 * @member: the name of the xf_hlist_node within the struct.
 */
#define xf_hlist_for_each_entry_continue(pos, type, member) \
    for (pos = xf_hlist_entry_safe((pos)->member.next, type, member); \
         pos; \
         pos = xf_hlist_entry_safe((pos)->member.next, type, member))

/*
 * xf_hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @pos:    the type * to use as a loop cursor.
 * @member: the name of the xf_hlist_node within the struct.
 */
#define xf_hlist_for_each_entry_from(pos, type, member) \
    for (; pos; pos = xf_hlist_entry_safe((pos)->member.next, type, member))

/*
 * xf_hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      a &xf_hlist_node_t to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the xf_hlist_node within the struct.
 */
#define xf_hlist_for_each_entry_safe(pos, n, head, type, member) \
    for (pos = xf_hlist_entry_safe((head)->first, type, member); \
         pos && ((n) = xf_hlist_entry((pos)->member.next, type, member), 1); \
         pos = xf_hlist_entry_safe(n, type, member))

/*
 * xf_hlist_count_nodes - count nodes in the hlist
 * @head:   the head for your hlist.
 */
__STATIC_INLINE size_t xf_hlist_count_nodes(xf_hlist_t *head)
{
    xf_hlist_node_t *pos;
    size_t count = 0;
    xf_hlist_for_each(pos, head) {
        count++;
    }
    return count;
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

/**
 * End of group_xf_utils_common_list
 * @}
 */

#endif /* __XF_LIST_H__ */
