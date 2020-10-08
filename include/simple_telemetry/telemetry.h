#ifndef SIMPLE_TELEMETRY_TELEMETRY_H
#define SIMPLE_TELEMETRY_TELEMETRY_H
#include <string>
#include <memory>
#include <atomic>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>

namespace simple_telemetry {

    using labels_type = std::map<std::string,std::string>;
    using label_type = labels_type::value_type;

    template <typename Type>
    struct instrument {
        [[nodiscard]] virtual Type value() const = 0;
        [[nodiscard]] virtual const std::string& name() const = 0;
        [[nodiscard]] virtual const labels_type& labels() const = 0;
        virtual ~instrument() = default;
    };

    template <typename Type>
    class atomic_counter_impl : public instrument<Type> {
        struct Data {
            std::string name_{};
            labels_type labels_{};
            std::atomic<Type> value_{};
        };
        Data data_;
        public:
        template <typename ...Args>
        explicit atomic_counter_impl(Args... args) : data_{std::forward<Args>(args)...}{}

        void increment() {
            data_.value_.fetch_add(1,std::memory_order_relaxed);
        }

        [[nodiscard]] Type value() const override {
            return data_.value_.load(std::memory_order_relaxed);
        }

        [[nodiscard]] const std::string& name() const override {
            return data_.name_;
        }

        [[nodiscard]] const labels_type& labels() const override {
            return data_.labels_;
        }
    };

    template <typename Type>
    class atomic_counter : public instrument<Type> {
        std::shared_ptr<atomic_counter_impl<Type>> impl_;
        public:
        explicit atomic_counter(std::shared_ptr<atomic_counter_impl<Type>> impl) : impl_(impl) {}

        void increment() {
            impl_->increment();
        }

        [[nodiscard]] Type value() const override {
            return impl_->value();
        }

        [[nodiscard]] const std::string& name() const override {
            return impl_->name();
        }

        [[nodiscard]] const labels_type& labels() const override {
            return impl_->labels();
        }
    };

    template <typename TVisitor>
    class telemetry {
    private:
        using uint64_t_atomic_counter_impl = atomic_counter_impl<uint64_t>;
        using uint64_t_atomic_counters = std::vector<std::weak_ptr<uint64_t_atomic_counter_impl>>;
        uint64_t_atomic_counters uint64_t_atomic_counters_;
        TVisitor visitor_;
    public:
        explicit telemetry(TVisitor visitor) : visitor_{std::move(visitor)} {};

        void collect() const {
            auto handler = [this](const auto& weak_item) {
                if (const auto item = weak_item.lock()) {
                    visitor_(*item);
                }
            };
            std::for_each(begin(uint64_t_atomic_counters_),end(uint64_t_atomic_counters_),handler);
        }

        template <typename ...Args>
        atomic_counter<uint64_t> create_atomic_counter(Args&&... args) {
            auto s = std::make_shared<uint64_t_atomic_counter_impl>(std::forward<Args>(args)...);
            uint64_t_atomic_counters_.emplace_back(s);
            return atomic_counter<uint64_t>(s);
        }

     //   atomic_counter<uint64_t> create_atomic_counter(std::string name, uint64_t init_value = 0) {
//            return create_atomic_counter(std::move(name),{},init_value);
//        }

    };

}

#endif //SIMPLE_TELEMETRY_TELEMETRY_H
