#ifndef TREE_BASED_SEGTREE_H_
#define TREE_BASED_SEGTREE_H_

#include <iostream>
#include <map>
#include <string>

#include "segtree.h"

#define TreeBasedSegtreeTmplParamSpec template<typename T, typename U, typename Aggregator>
#define TreeBasedSegtreeTmpl TreeBasedSegtree<T, U, Aggregator>
#define ParentTmpl Segtree<T, U, Aggregator>
#define PAIR(T) std::pair<T, T>
#define MEMO std::map<PAIR(size_t), WrappedNode*>

namespace gokul2411s {
    TreeBasedSegtreeTmplParamSpec
        class TreeBasedSegtree : public ParentTmpl {
            public:
                template<typename Iterator> TreeBasedSegtree(Iterator begin, Iterator end, Aggregator const & aggregator);
                ~TreeBasedSegtree();
            protected:
                using typename ParentTmpl::Node;
                struct WrappedNode : public Node {
                    WrappedNode * left;
                    WrappedNode * right;

                    WrappedNode(U const & val, size_t start, size_t end, WrappedNode * leftt, WrappedNode * rightt) :
                        Node(val, start, end),
                        left(leftt),
                        right(rightt) {}
                };

                WrappedNode * cast(Node * n) {
                    return static_cast<WrappedNode*>(n);
                }

                template<typename Iterator> WrappedNode * build(Iterator begin, Iterator end, size_t l, size_t r) {
                    PAIR(size_t) p(std::make_pair(l, r));
                    typename MEMO::iterator memo_it = memo_.find(p);
                    if (memo_it != memo_.end()) {
                        return memo_it->second;
                    }

                    U val;
                    WrappedNode * l_node = NULL, * r_node = NULL;
                    if (l == r) {
                        val = *(begin + l);
                    } else {
                        size_t mid = l + (r - l) / 2;
                        l_node = build(begin, end, l, mid);
                        r_node = build(begin, end, mid + 1, r);
                        val = this->aggregate(l_node->val, r_node->val); 
                    }
                    WrappedNode * n = new WrappedNode(val, l, r, l_node, r_node);
                    memo_.insert(std::make_pair(p, n));
                    return n;
                }
                
                void destroy(size_t l, size_t r) {
                    PAIR(size_t) p(std::make_pair(l, r));
                    typename MEMO::iterator memo_it = memo_.find(p);
                    if (memo_it == memo_.end()) {
                        return; // already destroyed
                    }

                    WrappedNode * n = memo_it->second;
                    if (l < r) {
                        size_t mid = l + (r - l) / 2;
                        destroy(l, mid);
                        destroy(mid + 1, r);
                    }
                    delete n;
                    memo_.erase(p);
                }

                Node * get_left_child(Node * n) {
                    return cast(n)->left;
                }

                Node * get_right_child(Node * n) {
                    return cast(n)->right;
                }

            private:
                MEMO memo_;
                size_t extent_;
        };

    TreeBasedSegtreeTmplParamSpec
        template<typename Iterator> TreeBasedSegtreeTmpl::TreeBasedSegtree(Iterator begin, Iterator end, Aggregator const & aggregator) :
            ParentTmpl(aggregator), extent_(end - begin) {
                for (size_t index = 0; index < extent_; index++) {
                    build(begin, end, 0, index);
                }
                this->root_ = memo_.find(std::make_pair(0, extent_ - 1))->second; 
        }

    TreeBasedSegtreeTmplParamSpec
        TreeBasedSegtreeTmpl::~TreeBasedSegtree() {
            for (size_t index = 0; index < extent_; index++) {
               destroy(0, index); 
            }
        }
}

#endif
