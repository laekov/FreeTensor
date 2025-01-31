#ifndef FREE_TENSOR_LAZY_H
#define FREE_TENSOR_LAZY_H

#include <functional>
#include <mutex>
#include <type_traits>

namespace freetensor {

template <typename T> class Lazy {
    std::optional<T> container_;
    std::function<T()> delayedInit_;
    std::mutex mutex_;

  public:
    const T &operator*() {
        if (!container_) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!container_)
                container_ = delayedInit_();
        }
        return *container_;
    }

    template <typename F>
    Lazy(F delayedInit)
        : container_(std::nullopt), delayedInit_(delayedInit), mutex_() {}

    Lazy(const Lazy &other)
        : container_(other.container_), delayedInit_(other.delayedInit_),
          mutex_() {}

    Lazy &operator=(const Lazy &other) {
        container_ = other.container_;
        delayedInit_ = other.delayedInit_;
        return *this;
    }
};

template <typename F>
Lazy(F delayedInit) -> Lazy<std::decay_t<decltype(std::declval<F>()())>>;

#define LAZY(expr) (Lazy([&]() { return (expr); }))

} // namespace freetensor

#endif // FREE_TENSOR_LAZY_H
