#include <cstdlib>
#include <utility>
#include <cstdio>
#include <type_traits>
#include "detail.h"

using namespace detail;


class any {
    static constexpr size_t SMALL_SIZE = 16;
    enum storage_state {
        STATE_SMALL,
        STATE_BIG,
        STATE_VALUELESS
    };

    storage_state state;
    std::aligned_storage<SMALL_SIZE, SMALL_SIZE>::type storage;
    using deleter_tp = void (*)(void*); // take voidptr produce void
    deleter_tp deleter;

    using cassign_tp = void (*)(void*, void const *);
    cassign_tp copier;
    using massign_tp = void (*)(void*, void*);
    massign_tp mover;

    using allocator_tp = void* (*)(void);
    allocator_tp allocator;

    void clear() {
        if (state == STATE_SMALL) {
            deleter(&storage);
        }

        if (state == STATE_BIG) {
            deleter(*(void**)&storage);
        }

        state = STATE_VALUELESS;
    }

    void swap(any& other) {
        std::swap(state, other.state);
        std::swap(storage, other.storage);
        std::swap(deleter, other.deleter);
        std::swap(copier, other.copier);
        std::swap(mover, other.mover);
        std::swap(allocator, other.allocator);
    }

    void* any_raw() const { // it is so not const but it's private at least
        if (state == STATE_SMALL) {
            return (void*)&storage;
        } else if (state == STATE_BIG) {
            return *(void**)&storage;
        } else {
            return nullptr;
        }
    }

public:
    constexpr any() : state(STATE_VALUELESS),
                      deleter([](void*){}), copier([](void*, void const*){}), mover([](void*, void*){}),
                      allocator([]() -> void* {return nullptr;}), storage({}) {
    };

    any(any const &other) : state(other.state), deleter(other.deleter), copier(other.copier), mover(other.mover), allocator(other.allocator) {
        if (state == STATE_SMALL) {
            copier(&storage, &other.storage);
        } else if (state == STATE_BIG) {
            void* nheap = allocator();
            copier(nheap, *(void**)&other.storage);
            *(void**)&storage = nheap;
        }
    }

    any(any &&other) : state(other.state), deleter(other.deleter), copier(other.copier), mover(other.mover), allocator(other.allocator) {
        if (state == STATE_SMALL) {
            mover(&storage, &other.storage);
        } else if (state == STATE_BIG) {
            void* nheap = allocator();
            mover(nheap, *(void**)&other.storage);
        }
    }

    template <typename VT, typename S = typename std::enable_if<!std::is_same_v<std::decay_t<VT>, any>>::type>
    any(VT&& val) {
        using VT_d = std::decay_t<VT>;
        if (sizeof(VT_d) <= SMALL_SIZE) {
            state = STATE_SMALL;
            new(&storage) VT_d(std::forward<VT>(val)); // inplace
            deleter = local_destruct<VT_d>;
        } else {
            state = STATE_BIG;
            void* inheap = new VT_d(std::forward<VT>(val));
            *(void**)&storage = inheap;
            deleter = heap_destruct<VT_d>;
        }

        copier = copy_content<VT_d>;
        mover = move_content<VT_d>;
        allocator = allocate<VT_d>;
    }

    // emplace :: retain emplacer as well as copier, mover, deleter, allocator ...
    // too boring.

    any& operator=(any const &other) {
        any(other).swap(*this);
        return *this;
    }

    any& operator=(any&& other) {
        any(std::move(other)).swap(*this);
        return *this;
    }

    template <typename VT, typename S = typename std::enable_if<!std::is_same_v<std::decay_t<VT>, any>>::type>
    any& operator=(VT&& other) {
        any(std::forward<VT>(other)).swap(*this);
        return *this;
    };

    ~any() {
        clear();
    }


    template <typename VT>
    friend VT any_cast(const any& op);

    template <typename VT>
    friend VT any_cast(any& op);

    template <typename VT>
    friend VT any_cast(any&& op);

    template <typename VT>
    friend VT const * any_cast(any const *op);

    template <typename VT>
    friend VT * any_cast(any *op);
};

template <typename VT>
VT any_cast(const any& op) {
    void* r = op.any_raw();
    return *(std::add_const_t<std::remove_reference_t<VT>>*)r;
}

template <typename VT>
VT any_cast (any& op) {
    void* r = op.any_raw();
    return *(std::remove_reference_t<VT> *)r;
}

template <typename VT>
VT any_cast (any&& op) {
    void* r = op.any_raw();
    return *(std::remove_reference_t<VT> *)r;
}

template <typename VT>
VT const * any_cast(any const *op) {
    const VT *r = op->any_raw();
    return r;
}

template <typename VT>
VT * any_cast(any *op) {
    VT *r = op->any_raw();
    return r;
}