#ifndef UPDATABLE_SEGTREE_H_
#define UPDATABLE_SEGTREE_H_

#include <stdlib.h>

#include "segtree.h"

#define UpdatableSegtreeTmplParamSpec template<typename T, typename U, typename Aggregator>
#define UpdatableSegtreeTmpl UpdatableSegtree<T, U, Aggregator>
#define SegtreeTmpl Segtree<T, U, Aggregator>

namespace gokul2411s {
    UpdatableSegtreeTmplParamSpec
        class UpdatableSegtree : public SegtreeTmpl {
            public:
                UpdatableSegtree(Aggregator const & aggregator = Aggregator()) :
                    SegtreeTmpl(aggregator) {}

                virtual ~UpdatableSegtree() {}

                /**
                 * Overwrites all elements of the closed range [l, r] with the given value.
                 */
                void overwrite(size_t l, size_t r, T const & val);

                /**
                 * Increments all elements of the the closed range [l, r] with the given value.
                 */
                void increment(size_t l, size_t r, T const & val);
            protected:
                enum UpdateType {
                    OVERWRITE,
                    INCREMENT
                };

                using typename SegtreeTmpl::Node;
                /**
                 * Wraps the Node object with two objects, one for overwrite updates and another for
                 * increment updates that are used to propagate updates to children nodes lazily.
                 */
                struct UpdatableNode : public Node {
                    T overwrite_lazy; 
                    T increment_lazy;
                    bool has_overwrite_lazy;
                    bool has_increment_lazy;

                    UpdatableNode(U const & val, size_t start, size_t end) :
                        Node(val, start, end),
                        overwrite_lazy(0),
                        increment_lazy(0),
                        has_overwrite_lazy(false),
                        has_increment_lazy(false) {}

                    /**
                     * Sets the overwrite lazy object, invalidating the increment lazy object.
                     */
                    void set_overwrite_lazy(T const & lazy) {
                        overwrite_lazy = lazy;
                        has_overwrite_lazy = true;

                        increment_lazy = 0;
                        has_increment_lazy = false;
                    }

                    /**
                     * Adds to the increment lazy object.
                     */
                    void add_increment_lazy(T const & lazy) {
                        increment_lazy += lazy;
                        has_increment_lazy = true;
                    }

                    /**
                     * Resets the lazy objects back to zero.
                     */
                    void reset_lazy() {
                        overwrite_lazy = 0;
                        has_overwrite_lazy = false;

                        increment_lazy = 0;
                        has_increment_lazy = false;
                    }
                };

                UpdatableNode * cast(Node * n) {
                    return static_cast<UpdatableNode*>(n);
                }

                /**
                 * Applies overwrite on the node based on the given value and sets the node's lazy accordingly.
                 */
                void apply_overwrite_and_lazy(UpdatableNode * n, T const & val) {
                    apply_overwrite(n, val);
                    if (n->non_trivial()) {
                        n->set_overwrite_lazy(val);
                    }
                }

                /**
                 * Applies increment on the node based on the given value and sets the node's lazy accordingly.
                 */
                void apply_increment_and_lazy(UpdatableNode * n, T const & val) {
                    apply_increment(n, val);
                    if (n->non_trivial()) {
                        n->add_increment_lazy(val);
                    }
                }

                /**
                 * Gets the update that would be applied on the node.
                 */
                U get_update_value(UpdatableNode const * n, T const & val) const {
                    return this->aggregate_times(val, n->end - n->start + 1);
                }

                /**
                 * Applies overwrite on the node based on the given value.
                 */
                void apply_overwrite(UpdatableNode * n, T const & val) {
                    n->val = get_update_value(n, val);
                }

                /**
                 * Applies increment on the node based on the given value.
                 */
                void apply_increment(UpdatableNode * n, T const & val) {
                    n->val += get_update_value(n, val);
                }

                /**
                 * Applies any lazy objects from the given node to its children, if any. This also
                 * propagates the lazy objects to the children.
                 */
                void propagate_lazy(UpdatableNode * n) {
                    if (!n->non_trivial()) {
                        return;
                    }

                    UpdatableNode * ln = cast(this->get_left_child(n));
                    UpdatableNode * rn = cast(this->get_right_child(n));
                    if (n->has_overwrite_lazy) {
                        apply_overwrite_and_lazy(ln, n->overwrite_lazy);
                        apply_overwrite_and_lazy(rn, n->overwrite_lazy);
                    }

                    if (n->has_increment_lazy) {
                        apply_increment_and_lazy(ln, n->increment_lazy); 
                        apply_increment_and_lazy(rn, n->increment_lazy); 
                    }

                    n->reset_lazy();
                }

                /**
                 * Recursively updates (overwrites or increments as specified by the update type)
                 * the segment tree using the given value under the node,
                 * for any overlap it may have with the closed range [l, r]. 
                 */
                void update(size_t l, size_t r, T const & val, UpdatableNode * n, UpdateType update_type) {
                    if (n->outside_range(l, r)) {
                        return; // noop
                    }

                    propagate_lazy(n);

                    if (n->within_range(l, r)) {
                        // prefer if loop over function pointers, since most users will either call one of either increment
                        // or update most of the time, and the processor will be able to guess well. function pointers will
                        // also screw up with the compiler's optimization mechanisms.
                        if (update_type == OVERWRITE) {
                            apply_overwrite_and_lazy(n, val);
                        } else {
                            apply_increment_and_lazy(n, val);
                        }
                    } else {
                        // node is non-trivial
                        UpdatableNode * ln = cast(this->get_left_child(n));
                        UpdatableNode * rn = cast(this->get_right_child(n));
                        update(l, r, val, ln, update_type);
                        update(l, r, val, rn, update_type);
                        n->val = this->aggregate(ln->val, rn->val);
                    }
                }
                
                virtual U query_impl(size_t l, size_t r, Node * n) {
                    if (n->outside_range(l, r)) {
                        return this->aggregator_null();
                    }

                    propagate_lazy(cast(n));

                    if (n->within_range(l, r)) {
                        return n->val;
                    } else {
                        return this->aggregate(query_impl(l, r, this->get_left_child(n)), query_impl(l, r, this->get_right_child(n)));
                    }
                }
        };

    UpdatableSegtreeTmplParamSpec 
        void UpdatableSegtreeTmpl::overwrite(size_t l, size_t r, T const & val) {
            update(l, r, val, cast(this->root_), OVERWRITE);
        }

    UpdatableSegtreeTmplParamSpec    
        void UpdatableSegtreeTmpl::increment(size_t l, size_t r, T const & val) {
            update(l, r, val, cast(this->root_), INCREMENT);
        }
}

#endif
