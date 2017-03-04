//
// Created by cat on 04.03.17.
//


#ifndef ANY_DETAIL_H
#define ANY_DETAIL_H


namespace detail {
    template <typename D>
    static void copy_content(void* dst, void const* src) {
        new(dst) D(*(D const*)src);
    }

    template <typename D>
    static void heap_destruct(void* raw) {
        delete ((D*)raw);
    }

    template <typename D>
    static void local_destruct(void* raw) {
        ((D*)raw)->~D();
    }

    template <typename D>
    static void move_content(void* dst, void* src) {
        new(dst) D(std::move(*(D*)src));
    }

    template <typename D>
    static void* allocate() {
        return new typename std::aligned_storage<sizeof(D), alignof(D)>::type;
    }
};


#endif //ANY_DETAIL_H
