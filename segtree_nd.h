#ifndef SEGTREE_ND_H_
#define SEGTREE_ND_H_

#include <string>

#include <stdlib.h>

namespace gokul2411s {
    template<typename Aggregate, typename Aggregator, typename Point, size_t NumDims>
        class SegtreeNd {
            public:
                /**
                 * Constructs a segment tree using the given iterable range and aggregator object.
                 */
                SegtreeNd(Aggregator const & aggregator) :
                    aggregator_(aggregator) {}

                /**
                 * Destructs the segment tree.
                 */
                virtual ~SegtreeNd() {}

                /**
                 * Returns the aggregated result in the closed range [l, r].
                 */
                Aggregate query(Point const & l, Point const & r) {
                    query_impl(l, r, root_);
                }

            protected:
                static size_t NUM_CHILDREN;

                /**
                 * Encapsulates a value and a closed range for which that value applies.
                 */
                struct Node {
                    Aggregate val;
                    Point start;
                    Point end;

                    /**
                     * Constructs a node with the given value and the closed range.
                     */
                    Node(Aggregate const & nval, Point const & nstart, Point const & nend) :
                        val(nval),
                        start(nstart),
                        end(nend) {}

                    /**
                     * Gets if a node is non-trivial i.e. if the closed range that it represents
                     * contains more than one element.
                     */
                    bool non_trivial() const {
                        return end != start;
                    }

                    /**
                     * Checks if the closed range represented by the node
                     * falls completely outside the closed range [l, r].
                     */
                    bool outside_range(Point const & l, Point const & r) const {
                        for (size_t k = 0; k < NumDims; k++) {
                            if (start[k] > r[k] || end[k] < l[k]) {
                                return true;
                            }
                        } 
                        return false;    
                    }

                    /**
                     * Checks if the closed range represented by the node
                     * falls completely inside the closed range [l, r].
                     */
                    bool within_range(Point const & l, Point const & r) const {
                        for (size_t k = 0; k < NumDims; k++) {
                            if(start[k] < l[k] || end[k] > r[k]) {
                                return false;
                            }
                        }
                        return true;
                    }

                    size_t size() const {
                        size_t ret = 1;
                        for (size_t k = 0; k < NumDims; k++) {
                            ret *= (end[k] - start[k] + 1);
                        }
                        return ret;
                    }
                };

                /**
                 * Gets the k'th child of the given node.
                 */
                virtual Node * get_child_node(Node * n, size_t k) = 0;

                Node * root_;                
                Aggregator aggregator_;

                /**
                 * Wraps the null method provided by the aggregator.
                 */
                Aggregate aggregator_null() const {
                    return aggregator_.null();   
                }

                /**
                 * Wraps the two-element aggregation method provided by the aggregator.
                 */
                Aggregate aggregate(Aggregate const & a, Aggregate const & b) const {
                    return aggregator_.aggregate(a, b);
                }

                /**
                 * Wraps the n-times aggregation method provided by the aggregator.
                 */
                Aggregate aggregate_times(Aggregate const & a, size_t times) const {
                    return aggregator_.aggregate_times(a, times);
                }

                /**
                 * Sets the value of the node depending on its children.
                 */
                Aggregate get_children_aggregate(Node * n) {
                    Aggregate ret = aggregator_null();
                    for (size_t k = 0; k < NUM_CHILDREN; k++) {
                        Node * child_node = get_child_node(n, k);
                        if (child_node == NULL) {
                            break;
                        }
                        ret = aggregate(ret, child_node->val);
                    }
                    return ret;
                }

                /**
                 * Sets the value of the node depending on its children, and specified range.
                 */
                Aggregate get_children_aggregate(Node * n, Point const & l, Point const & r) {
                    Aggregate ret = aggregator_null();
                    for (size_t k = 0; k < NUM_CHILDREN; k++) {
                        Node * child_node = get_child_node(n, k);
                        if (child_node == NULL) {
                            break;
                        }
                        ret = aggregate(ret, query_impl(l, r, child_node));
                    }
                    return ret;
                }

                /**
                 * Recursively queries the segment tree under the node for
                 * for its contribution towards the aggregate result of the
                 * closed range [l, r].
                 */
                virtual Aggregate query_impl(Point const & l, Point const & r, Node * n) {
                    if (n->outside_range(l, r)) {
                        return aggregator_null();
                    }

                    if (n->within_range(l, r)) {
                        return n->val;
                    } else {
                        return get_children_aggregate(n, l, r);
                    }
                }
        };

    template<typename Aggregate, typename Aggregator, typename Point, size_t NumDims>
        size_t SegtreeNd<Aggregate, Aggregator, Point, NumDims>::NUM_CHILDREN = 1 << NumDims;
}

#endif
