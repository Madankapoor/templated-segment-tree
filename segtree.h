#ifndef SEGTREE_H_
#define SEGTREE_H_

#include <string>

#include <stdlib.h>

namespace gokul2411s {
    template<typename Aggregate, typename Aggregator>
        class Segtree {
            public:
                /**
                 * Constructs a segment tree using the given iterable range and aggregator object.
                 */
                Segtree(Aggregator const & aggregator) :
                    aggregator_(aggregator) {}

                /**
                 * Destructs the segment tree.
                 */
                virtual ~Segtree() {}

                /**
                 * Returns the aggregated result in the closed range [l, r].
                 */
                Aggregate query(size_t l, size_t r) {
                    this->query_impl(l, r, root_);
                }

            protected:
                /**
                 * Encapsulates a value and a closed range for which that value applies.
                 */
                struct Node {
                    Aggregate val;
                    size_t start;
                    size_t end;

                    /**
                     * Constructs a node with the given value and the closed range.
                     */
                    Node(Aggregate const & nval, size_t nstart, size_t nend) :
                        val(nval),
                        start(nstart),
                        end(nend) {}

                    /**
                     * Gets if a node is non-trivial i.e. if the closed range that it represents
                     * contains more than one element.
                     */
                    bool non_trivial() const {
                        return end > start;
                    }

                    /**
                     * Checks if the closed range represented by the node
                     * falls completely outside the closed range [l, r].
                     */
                    bool outside_range(size_t l, size_t r) const {
                        return start > r || end < l;
                    }

                    /**
                     * Checks if the closed range represented by the node
                     * falls completely inside the closed range [l, r].
                     */
                    bool within_range(size_t l, size_t r) const {
                        return start >= l && end <= r;
                    }
                };

                /**
                 * Gets the left child of the given node.
                 */
                virtual Node * get_left_child(Node * n) = 0;

                /**
                 * Gets the right child of the given node.
                 */
                virtual Node * get_right_child(Node * n) = 0;

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
                 * Recursively queries the segment tree under the node for
                 * for its contribution towards the aggregate result of the
                 * closed range [l, r].
                 */
                virtual Aggregate query_impl(size_t l, size_t r, Node * n) {
                    if (n->outside_range(l, r)) {
                        return aggregator_null();
                    }

                    if (n->within_range(l, r)) {
                        return n->val;
                    } else {
                        return aggregate(query_impl(l, r, get_left_child(n)), query_impl(l, r, get_right_child(n)));
                    }
                }
        };
}

#endif
