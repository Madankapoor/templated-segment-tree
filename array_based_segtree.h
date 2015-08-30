#ifndef ARRAY_BASED_SEGTREE_H_
#define ARRAY_BASED_SEGTREE_H_

#include <string>

#include <stdlib.h>

#include "updatable_segtree.h"

#define ArrayBasedSegtreeTmplParamSpec template<typename Item, typename Aggregate, typename Aggregator>
#define ArrayBasedSegtreeTmpl ArrayBasedSegtree<Item, Aggregate, Aggregator>
#define UpdatableSegtreeTmpl UpdatableSegtree<Item, Aggregate, Aggregator>
#define SegtreeTmpl Segtree<Aggregate, Aggregator>

namespace gokul2411s {
    ArrayBasedSegtreeTmplParamSpec
        class ArrayBasedSegtree : public UpdatableSegtreeTmpl {
            public:
                template<typename Iterator> ArrayBasedSegtree(Iterator begin, Iterator end, Aggregator const & aggregator = Aggregator());
                ~ArrayBasedSegtree();
            protected:
                using typename UpdatableSegtreeTmpl::UpdatableNode;
                struct WrappedNode : public UpdatableNode {
                    size_t index;

                    WrappedNode(Aggregate const & val, size_t start, size_t end, size_t indexx) :
                        UpdatableNode(val, start, end), index(indexx) {}
                };

                size_t tree_size_;
                char * pool_;

                /**
                 * Gets the size of the array required to represent the segment tree. This computation
                 * is required since the segment tree is not truly a complete tree, but rather an
                 * almost complete tree.
                 */
                size_t tree_size(size_t num_items) const {
                    size_t psz = 1;
                    while (true) {
                        if (num_items <= psz) {
                            break;
                        }
                        psz *= 2;
                    }

                    return 2 * psz - 1;
                }

                /**
                 * Gets the index of the left child of the node placed at index.
                 */
                size_t get_lindex(size_t index) const {
                    return 2 * index + 1;
                }

                /**
                 * Get the index of the right child of the node placed at index.
                 */
                size_t get_rindex(size_t index) const {
                    return 2 * index + 2;
                }

                /**
                 * Recursively builds the segment tree under the node representing the
                 * closed range [l, r].
                 */
                template<typename Iterator> WrappedNode * build(Iterator begin, Iterator end, size_t l, size_t r, size_t index = 0) {
                    Aggregate val;
                    if (l == r) {
                        val = *(begin + l);
                    } else {
                        size_t mid = l + (r - l) / 2;
                        WrappedNode * l_node = build(begin, end, l, mid, get_lindex(index));
                        WrappedNode * r_node = build(begin, end, mid + 1, r, get_rindex(index));
                        val = this->aggregate(l_node->val, r_node->val);
                    }
                    return new (get_node(index)) WrappedNode(val, l, r, index);
                }

                /**
                 * Gets the node placed at the index.
                 */
                WrappedNode * get_node(size_t index) {
                    return (WrappedNode*)pool_ + index;
                }
                
                using typename SegtreeTmpl::Node;
                Node * get_left_child(Node * n) {
                    if (n->end > n->start) {
                        return get_node(get_lindex(static_cast<WrappedNode*>(n)->index));
                    } else {
                        return NULL;
                    }
                }

                Node * get_right_child(Node * n) {
                    if (n->end > n->start) {
                        return get_node(get_rindex(static_cast<WrappedNode*>(n)->index));
                    } else {
                        return NULL;
                    }
                }
        };

    ArrayBasedSegtreeTmplParamSpec
        template<typename Iterator> ArrayBasedSegtreeTmpl::ArrayBasedSegtree(Iterator begin, Iterator end, Aggregator const & aggregator)
        : UpdatableSegtreeTmpl(aggregator), tree_size_(tree_size(end - begin)) {
            size_t l = 0, r = end - begin - 1;
            pool_ = (char *)calloc(tree_size_, sizeof(WrappedNode));
            this->root_ = build(begin, end, l, r);
        }

    ArrayBasedSegtreeTmplParamSpec
        ArrayBasedSegtreeTmpl::~ArrayBasedSegtree() {
            WrappedNode * s = get_node(0);
            for (WrappedNode* p = s; p < s + tree_size_; p++) {
                if (p) {
                    p->~WrappedNode();
                }
            }

            free(pool_);
        }
}

#endif
