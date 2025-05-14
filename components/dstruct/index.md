# dstruct

数据结构组件 (data structure component).

理论上数据结构组件(dstruct)应当可以放到 common 文件夹中，但由于安全数据结构的实现可能依赖于具体的平台，为避免 common 组件依赖 osal 组件，所以 dstruct 单独分离文件夹。如果有必要，dstruct 可能会放回 common。

## TODO

-   [ ] list
    -   [x] list 双链表
    -   [x] hlist 哈希链表
    -   [ ] llist 单链表
    -   [ ] allist 无锁单链表
-   [ ] queue / fifo
-   [ ] stack / filo
-   [ ] ringbuffer
-   [ ] hash
    -   [ ] hashmap
-   [ ] tree
    -   [ ] tree
    -   [ ] binarytree
    -   [ ] btree
    -   [ ] bst
    -   [ ] avl
    -   [ ] rbt
-   [ ] bitmap
