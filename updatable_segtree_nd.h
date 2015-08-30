#ifndef UPDATABLE_SEGTREE_ND_H_
#define UPDATABLE_SEGTREE_ND_H_

#include <stdlib.h>

#include "segtree_nd.h"

#define UpdatableSegtreeNdTmplParamSpec template<typename Item, typename Aggregate, typename Aggregator, typename Point, size_t NumDims>
#define UpdatableSegtreeNdTmpl UpdatableSegtreeNd<Item, Aggregate, Aggregator, Point, NumDims>
#define SegtreeNdTmpl SegtreeNd<Aggregate, Aggregator, Point, NumDims>

namespace gokul2411s {
    UpdatableSegtreeNdTmplParamSpec
        class UpdatableSegtreeNd : public SegtreeNdTmpl {
            public:
                UpdatableSegtreeNd(Aggregator const & aggregator) :
                    SegtreeNdTmpl(aggregator) {}

                virtual ~UpdatableSegtreeNd() {}

                /**
                 * Overwrites all elements of the closed range [l, r] with the given value.
                 */
                void overwrite(Point const & l, Point const & r, Item const & val);

                /**
                 * Increments all elements of the the closed range [l, r] with the given value.
                 */
                void increment(Point const & l, Point const & r, Item const & val);
            protected:
                enum UpdateType {
                    OVERWRITE,
                    INCREMENT
                };

                using typename SegtreeNdTmpl::Node;
                /**
                 * Wraps the Node object with two objects, one for overwrite updates and another for
                 * increment updates that are used to propagate updates to children nodes lazily.
                 */
                struct UpdatableNode : public Node {
                    Item overwrite_lazy; 
                    Item increment_lazy;
                    bool has_overwrite_lazy;
                    bool has_increment_lazy;

                    UpdatableNode(Aggregate const & val, Point const & start, Point const & end) :
                        Node(val, start, end),
                        overwrite_lazy(0),
                        increment_lazy(0),
                        has_overwrite_lazy(false),
                        has_increment_lazy(false) {}

                    /**
                     * Sets the overwrite lazy object, invalidating the increment lazy object.
                     */
                    void set_overwrite_lazy(Item const & lazy) {
                        overwrite_lazy = lazy;
                        has_overwrite_lazy = true;

                        increment_lazy = 0;
                        has_increment_lazy = false;
                    }

                    /**
                     * Adds to the increment lazy object.
                     */
                    void add_increment_lazy(Item const & lazy) {
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
                void apply_overwrite_and_lazy(UpdatableNode * n, Item const & val) {
                    apply_overwrite(n, val);
                    if (n->non_trivial()) {
                        n->set_overwrite_lazy(val);
                    }
                }

                /**
                 * Applies increment on the node based on the given value and sets the node's lazy accordingly.
                 */
                void apply_increment_and_lazy(UpdatableNode * n, Item const & val) {
                    apply_increment(n, val);
                    if (n->non_trivial()) {
                        n->add_increment_lazy(val);
                    }
                }

                /**
                 * Gets the update that would be applied on the node.
                 */
                Aggregate get_update_value(UpdatableNode const * n, Item const & val) const {
                    return this->aggregate_times(val, n->size());
                }

                /**
                 * Applies overwrite on the node based on the given value.
                 */
                void apply_overwrite(UpdatableNode * n, Item const & val) {
                    n->val = get_update_value(n, val);
                }

                /**
                 * Applies increment on the node based on the given value.
                 */
                void apply_increment(UpdatableNode * n, Item const & val) {
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

                    if (n->has_overwrite_lazy) {
                        for (size_t k = 0; k < SegtreeNdTmpl::NUM_CHILDREN; k++) {
                            UpdatableNode * child_node = cast(this->get_child_node(n, k));
                            if (child_node == NULL) {
                                break;
                            }
                            apply_overwrite_and_lazy(child_node, n->overwrite_lazy);
                        }
                    }

                    if (n->has_increment_lazy) {
                        for (size_t k = 0; k < SegtreeNdTmpl::NUM_CHILDREN; k++) {
                            UpdatableNode * child_node = cast(this->get_child_node(n, k));
                            if (child_node == NULL) {
                                break;
                            }
                            apply_increment_and_lazy(child_node, n->increment_lazy); 
                        }
                    }

                    n->reset_lazy();
                }

                /**
                 * Recursively updates (overwrites or increments as specified by the update type)
                 * the segment tree using the given value under the node,
                 * for any overlap it may have with the closed range [l, r]. 
                 */
                void update(Point const & l, Point const & r, Item const & val, UpdatableNode * n, UpdateType update_type) {
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
                        for (size_t k = 0; k < SegtreeNdTmpl::NUM_CHILDREN; k++) {
                            UpdatableNode * child_node = cast(this->get_child_node(n, k));
                            if (child_node == NULL) {
                                break;
                            }
                            update(l, r, val, child_node, update_type);
                        }
                        n->val = this->get_children_aggregate(n);
                    }
                }
                
                virtual Aggregate query_impl(Point const & l, Point const & r, Node * n) {
                    if (n->outside_range(l, r)) {
                        return this->aggregator_null();
                    }

                    propagate_lazy(cast(n));
                    
                    Aggregate ret;
                    if (n->within_range(l, r)) {
                        return n->val;
                    } else {
                        return this->get_children_aggregate(n, l, r);
                    }
                }
        };

    UpdatableSegtreeNdTmplParamSpec 
        void UpdatableSegtreeNdTmpl::overwrite(Point const & l, Point const & r, Item const & val) {
            update(l, r, val, cast(this->root_), OVERWRITE);
        }

    UpdatableSegtreeNdTmplParamSpec    
        void UpdatableSegtreeNdTmpl::increment(Point const & l, Point const & r, Item const & val) {
            update(l, r, val, cast(this->root_), INCREMENT);
        }
}

#endif
