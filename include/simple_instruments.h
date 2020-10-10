#ifndef CROSSCODE_SIMPLE_INSTRUMENTS_H
#define CROSSCODE_SIMPLE_INSTRUMENTS_H
#include <memory>
#include <atomic>

namespace crosscode::simple_instruments {

    template <typename Tvalue, typename Texporter>
    struct data_block  {
        using value_type = Tvalue;
        using metadata_type = typename Texporter::metadata_type;
        using exporter_type = Texporter;
        using exporter_shared_ptr_type = std::shared_ptr<exporter_type>;
        exporter_shared_ptr_type exporter_;
        metadata_type metadata_;
        value_type value_;
    };

    template <typename Tvalue, typename Texporter>
    class atomic_value_recorder {
    public:
        using value_type = Tvalue;
        using exporter_type = Texporter;
    private:
        data_block<std::atomic<value_type>,exporter_type> data_;
    public:
        template <typename ...Args>
        explicit atomic_value_recorder(Args ...args) : data_{std::forward<Args>(args)...} {
            value_type value = data_.value_.load();
            data_.exporter_->emit_init(value, data_.metadata_);
        }

        void set(value_type amount, std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            data_.value_.store(amount,mem_order);
            data_.exporter_->emit(amount, data_.metadata_);
        }

        value_type value(std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            return data_.value_.load(mem_order);
        }
    };

    template <typename Tvalue, typename Texporter, Tvalue step>
    class atomic_bidirectional_counter {
    public:
        using value_type = Tvalue;
        using exporter_type = Texporter;
    private:
        data_block<std::atomic<value_type>,exporter_type> data_;
    public:
        template <typename ...Args>
        explicit atomic_bidirectional_counter(Args ...args) : data_{std::forward<Args>(args)...} {
            value_type value = data_.value_.load();
            data_.exporter_->emit_init(value, data_.metadata_);
        }

        void add(value_type amount=step, std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            value_type new_value = value_type{data_.value_.fetch_add(amount, mem_order)} + amount;
            data_.exporter_->emit(new_value, data_.metadata_);
        }

        void sub(value_type amount=step, std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            value_type new_value = value_type{data_.value_.fetch_sub(amount, mem_order)} - amount;
            data_.exporter_->emit(new_value, data_.metadata_);
        }

        value_type value(std::memory_order mem_order = std::memory_order::memory_order_seq_cst) {
            return data_.value_.load(mem_order);
        }
    };

    template <typename Tvalue, typename Texporter, Tvalue step>
    class atomic_monotonic_counter {
    public:
        using value_type = Tvalue;
        using exporter_type = Texporter;
    private:
        atomic_bidirectional_counter<value_type,exporter_type,step> counter_;
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

    template <typename Texporter>
    class instrument_factory {
    public:
        using metadata_type = typename Texporter::metadata_type;
        using exporter_type = Texporter;
        using exporter_shared_ptr_type = std::shared_ptr<exporter_type>;
    private:
        exporter_shared_ptr_type impl_;
    public:
        template <typename ...Args>
        explicit instrument_factory(Args ...args) : impl_{std::make_shared<exporter_type>(std::forward<Args>(args)...)} {}

        template<typename Tvalue, Tvalue step=1>
        auto make_atomic_bidirectional_counter(metadata_type metadata = {}, Tvalue value = 0) {
            return atomic_bidirectional_counter<Tvalue,Texporter,step>{impl_, std::move(metadata), value};
        }

        template<typename Tvalue, Tvalue step=1>
        auto make_atomic_monotonic_counter(metadata_type metadata = {}, Tvalue value = 0) {
            return atomic_monotonic_counter<Tvalue,Texporter,step>{impl_, std::move(metadata), value};
        }

        template<typename Tvalue>
        auto make_atomic_value_recorder_counter(metadata_type metadata = {}, Tvalue value = 0) {
            return atomic_value_recorder<Tvalue,Texporter>{impl_, std::move(metadata), value};
        }
    };

    /// deduction guide
    template<typename Texporter>
    instrument_factory(Texporter e) -> instrument_factory<decltype(e)>;

}

#endif //CROSSCODE_SIMPLE_INSTRUMENTS_H
