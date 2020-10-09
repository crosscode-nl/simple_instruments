#ifndef SIMPLE_TELEMETRY_TELEMETRY_H
#define SIMPLE_TELEMETRY_TELEMETRY_H
#include <memory>
#include <atomic>

namespace simple_telemetry {

    template <typename Tvalue, typename Tmetadata, typename Texporter>
    struct data_block  {
        using value_type = Tvalue;
        using metadata_type = Tmetadata;
        using exporter_type = Texporter;
        using exporter_shared_ptr_type = std::shared_ptr<exporter_type>;
        exporter_shared_ptr_type exporter_;
        metadata_type metadata_;
        value_type value_;
    };

    template <typename Tvalue, typename Tmetadata, typename Texporter, bool emit_initial>
    class atomic_value_recorder {
    public:
        using value_type = Tvalue;
        using metadata_type = Tmetadata;
        using exporter_type = Texporter;
    private:
        data_block<std::atomic<value_type>,metadata_type,exporter_type> data_;
    public:
        template <typename ...Args>
        explicit atomic_value_recorder(Args ...args) : data_{std::forward<Args>(args)...} {
            if (emit_initial) {
                value_type value = data_.value_.load();
                data_.exporter_->on_data_changed(value, data_.metadata_);
            }
        }

        void set(value_type amount, std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            data_.value_.store(amount,mem_order);
            data_.exporter_->on_data_changed(amount, data_.metadata_);
        }

        value_type value(std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            return data_.value_.load(mem_order);
        }
    };

    template <typename Tvalue, typename Tmetadata, typename Texporter, Tvalue step, bool emit_initial>
    class atomic_bidirectional_counter {
    public:
        using value_type = Tvalue;
        using metadata_type = Tmetadata;
        using exporter_type = Texporter;
    private:
        data_block<std::atomic<value_type>,metadata_type,exporter_type> data_;
    public:
        template <typename ...Args>
        explicit atomic_bidirectional_counter(Args ...args) : data_{std::forward<Args>(args)...} {
            if (emit_initial) {
                value_type value = data_.value_.load();
                data_.exporter_->on_data_changed(value, data_.metadata_);
            }
        }

        void add(value_type amount=step, std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            value_type new_value = value_type{data_.value_.fetch_add(amount, mem_order)} + amount;
            data_.exporter_->on_data_changed(new_value, data_.metadata_);
        }

        void sub(value_type amount=step, std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            value_type new_value = value_type{data_.value_.fetch_sub(amount, mem_order)} - amount;
            data_.exporter_->on_data_changed(new_value, data_.metadata_);
        }

        value_type value(std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            return data_.value_.load(mem_order);
        }
    };

    template <typename Tvalue, typename Tmetadata, typename Texporter, Tvalue step, bool emit_initial>
    class atomic_monotonic_counter {
    public:
        using value_type = Tvalue;
        using metadata_type = Tmetadata;
        using exporter_type = Texporter;
    private:
        atomic_bidirectional_counter<value_type,metadata_type,exporter_type,step,emit_initial> counter_;
    public:
        template <typename ...Args>
        explicit atomic_monotonic_counter(Args ...args) : counter_{std::forward<Args>(args)...} {}

        void add(std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            counter_.add(step,mem_order);
        }

        value_type value(std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            return counter_.value(mem_order);
        }
    };

    template <typename Tmetadata, typename Texporter>
    class telemetry {
    public:
        using metadata_type = Tmetadata;
        using exporter_type = Texporter;
        using exporter_shared_ptr_type = std::shared_ptr<exporter_type>;
    private:
        exporter_shared_ptr_type impl_;
    public:
        template <typename ...Args>
        explicit telemetry(Args ...args) : impl_{std::make_shared<exporter_type>(std::forward<Args>(args)...)} {}

        template<typename Tvalue, Tvalue step=1, bool emit_initial=true>
        auto create_atomic_bidirectional_counter(Tmetadata metadata = {}, Tvalue value = 0) {
            return atomic_bidirectional_counter<Tvalue,Tmetadata,Texporter,step,emit_initial>{impl_, std::move(metadata), value};
        }

        template<typename Tvalue, Tvalue step=1, bool emit_initial=true>
        auto create_atomic_monotonic_counter(Tmetadata metadata = {}, Tvalue value = 0) {
            return atomic_monotonic_counter<Tvalue,Tmetadata,Texporter,step,emit_initial>{impl_, std::move(metadata), value};
        }

        template<typename Tvalue, bool emit_initial=false>
        auto create_atomic_value_recorder_counter(Tmetadata metadata = {}, Tvalue value = 0) {
            return atomic_value_recorder<Tvalue,Tmetadata,Texporter,emit_initial>{impl_, std::move(metadata), value};
        }

    };

}

#endif //SIMPLE_TELEMETRY_TELEMETRY_H
