#ifndef SIMPLE_TELEMETRY_TELEMETRY_H
#define SIMPLE_TELEMETRY_TELEMETRY_H
#include <memory>
#include <atomic>

namespace simple_telemetry {

    template <typename Tvalue, typename Tmetadata, typename Texporter>
    struct data_block  {
        std::shared_ptr<Texporter> exporter_;
        Tmetadata metadata_;
        Tvalue value_;
    };

    template <typename Tvalue, typename Tmetadata, typename Texporter>
    class atomic_counter {
        data_block<std::atomic<Tvalue>,Tmetadata,Texporter> data_;
    public:
        template <typename ...Args>
        explicit atomic_counter(Args ...args) : data_{std::forward<Args>(args)...} {}

        void incr(Tvalue amount=1, std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            auto new_value = data_.value_.fetch_add(amount, mem_order) + amount;
            data_.exporter_->on_data_changed(new_value, data_.metadata_);
        }

        void decr(Tvalue amount=1, std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            auto new_value = data_.value_.fetch_sub(amount, mem_order) - amount;
            data_.exporter_->on_data_changed(new_value, data_.metadata_);
        }

        Tvalue value(std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            return data_.value_.load(mem_order);
        }

        const Tmetadata &metadata() {
            return data_.metadata_;
        }
    };

    template <typename Tmetadata, typename Texporter>
    class telemetry {
    private:
        std::shared_ptr<Texporter> impl_;

        template<typename Tvalue>
        atomic_counter<Tvalue,Tmetadata,Texporter> create_atomic_counter(Tmetadata metadata = {}, Tvalue value = 0) {
            return atomic_counter<Tvalue,Tmetadata,Texporter>{impl_, std::move(metadata), value};
        }

    public:
        template <typename ...Args>
        explicit telemetry(Args ...args) : impl_{std::make_shared<Texporter>(std::forward<Args>(args)...)} {}

        auto create_uint64_atomic_counter(Tmetadata metadata = {}, uint64_t value = 0) {
            return create_atomic_counter(std::move(metadata),value);
        }

        auto create_int64_atomic_counter(Tmetadata metadata = {}, int64_t value = 0) {
            return create_atomic_counter(std::move(metadata),value);
        }

        auto create_uint32_atomic_counter(Tmetadata metadata = {}, uint32_t value = 0) {
            return create_atomic_counter(std::move(metadata),value);
        }

        auto create_int32_atomic_counter(Tmetadata metadata = {}, int32_t value = 0) {
            return create_atomic_counter(std::move(metadata),value);
        }

    };

}

#endif //SIMPLE_TELEMETRY_TELEMETRY_H
