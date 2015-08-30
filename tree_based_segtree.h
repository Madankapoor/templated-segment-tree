#ifndef TREE_BASED_SEGTREE_H_
#define TREE_BASED_SEGTREE_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "segtree.h"

#define TreeBasedSegtreeTmplParamSpec template<typename Item, typename Aggregate, typename Aggregator>
#define TreeBasedSegtreeTmpl TreeBasedSegtree<Item, Aggregate, Aggregator>
#define SegtreeTmpl Segtree<Aggregate, Aggregator>
#define PAIR(a) std::pair<a, a>
#define MEMO std::map<PAIR(size_t), WrappedNode*>
#define ITEMCPY std::vector<Item>

namespace gokul2411s {
    TreeBasedSegtreeTmplParamSpec
        class TreeBasedSegtree : public SegtreeTmpl {
            public:
                template<typename Iterator> TreeBasedSegtree(Iterator begin, Iterator end, Aggregator const & aggregator);
                ~TreeBasedSegtree();
                void push(Item const & val);
                void pop();
            protected:
                using typename SegtreeTmpl::Node;
                struct WrappedNode : public Node {
                    WrappedNode * left;
                    WrappedNode * right;
                    size_t reference_count;

                    WrappedNode(Aggregate const & val, size_t start, size_t end, WrappedNode * leftt, WrappedNode * rightt) :
                        Node(val, start, end),
                        left(leftt),
                        right(rightt),
                        reference_count(0) {}
                };

                WrappedNode * cast(Node * n) {
                    return static_cast<WrappedNode*>(n);
                }

                WrappedNode * build(size_t l, size_t r) {
                    PAIR(size_t) p(std::make_pair(l, r));
                    typename MEMO::iterator memo_it = memo_.find(p);
                    if (memo_it != memo_.end()) {
                        return memo_it->second;
                    }

                    Aggregate val;
                    WrappedNode * l_node = NULL, * r_node = NULL;
                    if (l == r) {
                        val = itemcpy_[l];
                    } else {
                        size_t mid = l + (r - l) / 2;
                        l_node = build(l, mid);
                        l_node->reference_count++;
                        
                        r_node = build(mid + 1, r);
                        r_node->reference_count++;
                        
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
                        WrappedNode * l_node = n->left;
                        l_node->reference_count--;
                        if (l_node->reference_count == 0) {
                            destroy(l, mid);
                        }
                        
                        WrappedNode * r_node = n->right;
                        r_node->reference_count--;
                        if (r_node->reference_count == 0) {
                            destroy(mid + 1, r);
                        }
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
                ITEMCPY itemcpy_;
                MEMO memo_;
                size_t size_;
        };

    TreeBasedSegtreeTmplParamSpec
        template<typename Iterator> TreeBasedSegtreeTmpl::TreeBasedSegtree(Iterator begin, Iterator end, Aggregator const & aggregator) :
            SegtreeTmpl(aggregator), size_(end - begin), itemcpy_(begin, end) {
                for (size_t index = 0; index < size_; index++) {
                    build(0, index);
                }
                this->root_ = memo_.find(std::make_pair(0, size_ - 1))->second; 
        }
    
    TreeBasedSegtreeTmplParamSpec
        void TreeBasedSegtreeTmpl::push(Item const & val) {
            size_++;
            build(0, size_ - 1);
            this->root_ = memo_.find(std::make_pair(0, size_ - 1))->second; 
            
            itemcpy_.push_back(val);
        }
    
    TreeBasedSegtreeTmplParamSpec
        void TreeBasedSegtreeTmpl::pop() {
            itemcpy_.pop_back();
            
            this->root_ = memo_.find(std::make_pair(0, size_ - 2))->second; 
            destroy(0, size_ - 1);
            size_--;
        }

    TreeBasedSegtreeTmplParamSpec
        TreeBasedSegtreeTmpl::~TreeBasedSegtree() {
            for (size_t index = 0; index < size_; index++) {
               destroy(0, index); 
            }
        }
}

#endif
