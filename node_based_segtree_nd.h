#ifndef NODE_BASED_SEGTREE_ND_H_
#define NODE_BASED_SEGTREE_ND_H_

#include <string>
#include <vector>

#include <stdlib.h>

#include "updatable_segtree_nd.h"

#define NodeBasedSegtreeNdTmplParamSpec template<typename Item, typename Aggregate, typename Aggregator, typename Point, size_t NumDims>
#define NodeBasedSegtreeNdTmpl NodeBasedSegtreeNd<Item, Aggregate, Aggregator, Point, NumDims>
#define UpdatableSegtreeNdTmpl UpdatableSegtreeNd<Item, Aggregate, Aggregator, Point, NumDims>
#define SegtreeNdTmpl SegtreeNd<Aggregate, Aggregator, Point, NumDims>

namespace gokul2411s {
    NodeBasedSegtreeNdTmplParamSpec
        class NodeBasedSegtreeNd : public UpdatableSegtreeNdTmpl {
            public:
                template<typename Matrix> NodeBasedSegtreeNd(Matrix const & matrix, Point & l, Point & r, Aggregator const & aggregator = Aggregator());
                ~NodeBasedSegtreeNd();
            protected:
                using typename UpdatableSegtreeNdTmpl::UpdatableNode;
                struct WrappedNode : public UpdatableNode {
                    std::vector<WrappedNode*> children;

                    WrappedNode(Aggregate const & val, Point const & start, Point const & end, std::vector<WrappedNode*> const & childrenn) :
                        UpdatableNode(val, start, end), children(childrenn) {}
                };

                /**
                 * Recursively builds the segment tree under the node representing the
                 * closed range [l, r].
                 */
                template<typename Matrix> WrappedNode * build(Matrix const & matrix, Point & l, Point & r) {
                    Aggregate val;
                    std::vector<WrappedNode*> children;
                    if (l == r) {
                        val = matrix.get(l);
                    } else {
                        build_children(matrix, l, r, 0, children);
                        val = this->aggregator_null();
                        for (typename std::vector<WrappedNode*>::iterator it = children.begin(); it != children.end(); it++) {
                            val = this->aggregate(val, (*it)->val);
                        }
                    }
                    return new WrappedNode(val, l, r, children);
                }

                template <typename Matrix> void build_children(Matrix const & matrix, Point & l, Point & r, size_t index_in_dims, std::vector<WrappedNode*> & children_ref) {
                    size_t r_old = r[index_in_dims];
                    size_t l_old = l[index_in_dims];
                    size_t mid = l_old + (r_old - l_old) / 2;

                    // l to mid
                    r.set(index_in_dims, mid);
                    if (index_in_dims == NumDims - 1) {
                        WrappedNode * child_node = build(matrix, l, r);
                        children_ref.push_back(child_node);
                    } else {
                        build_children(matrix, l, r, index_in_dims + 1, children_ref);
                    }
                    r.set(index_in_dims, r_old);

                    // mid+1 to r (if this dimension permits)
                    if (r_old > l_old) {
                        l.set(index_in_dims, mid + 1);
                        if (index_in_dims == NumDims - 1) {
                            WrappedNode * child_node = build(matrix, l, r);
                            children_ref.push_back(child_node);
                        } else {
                            build_children(matrix, l, r, index_in_dims + 1, children_ref);
                        }
                        l.set(index_in_dims, l_old);
                    }
                }

                using typename SegtreeNdTmpl::Node;
                Node * get_child_node(Node * n, size_t k) {
                    WrappedNode * wn = cast(n);
                    if (wn->children.size() <= k) {
                        return NULL;
                    } else {
                        return wn->children[k];
                    }
                }
                
                WrappedNode * cast(Node * n) {
                    return static_cast<WrappedNode*>(n);
                }
        };

    NodeBasedSegtreeNdTmplParamSpec
        template<typename Matrix> NodeBasedSegtreeNdTmpl::NodeBasedSegtreeNd(Matrix const & matrix, Point & l, Point & r, Aggregator const & aggregator)
        : UpdatableSegtreeNdTmpl(aggregator) {
            this->root_ = build(matrix, l, r);
        }

    NodeBasedSegtreeNdTmplParamSpec
        NodeBasedSegtreeNdTmpl::~NodeBasedSegtreeNd() {
            // TODO
        }
}

#endif
