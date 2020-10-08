#include "simple_telemetry.h"
#include "doctest.h"
#include <sstream>

using namespace std::literals;

class Visitor {
    std::ostream *os_;
    public:
    explicit Visitor(std::ostream *os) : os_(os) {}
    template<typename Type>
    void operator()(const simple_telemetry::instrument<Type> &item) const {
        (*os_) << item.name() << " " << item.value();
        auto serialize_label = [this](const auto& label){
            auto [key,value] = label;
            (*os_) << " " << key << "=" << value;
        };
        std::for_each(std::begin(item.labels()),std::end(item.labels()),serialize_label);
        (*os_) << "\n";
    }
};

TEST_SUITE("telemetry") {

    TEST_CASE("atomic_counter is default initialized to 0") {
        std::stringstream ss;
        Visitor v{&ss};
        simple_telemetry::telemetry<Visitor> t{v};

        auto counter = t.create_atomic_counter("test_name");
        REQUIRE(0==counter.value());
        SUBCASE("atomic counter is incremented to 1") {
            counter.increment();
            REQUIRE(counter.value()==uint64_t{1});
        }
        SUBCASE("name is set to test_name") {
            REQUIRE("test_name"==counter.name());
        }
        SUBCASE("Collect renders correct output") {
            t.collect();
            REQUIRE("test_name 0\n"==ss.str());
        }
        SUBCASE("Add another atomic counter with labels, defaults to 0") {
            auto counter2 = t.create_atomic_counter("test_name2"s, simple_telemetry::labels_type{{"key1","value1"},{"key2","value2"}});
            counter.increment();
            counter.increment();
            counter2.increment();
            REQUIRE(counter.value()==uint64_t{2});
            REQUIRE(counter2.value()==uint64_t{1});
            SUBCASE("Collect renders correct output") {
                t.collect();
                REQUIRE("test_name 2\ntest_name2 1 key1=value1 key2=value2\n"==ss.str());
            }
        }
    }

    TEST_CASE("atomic_counter is initialized to 10") {
        std::stringstream ss;
        Visitor v{&ss};
        simple_telemetry::telemetry<Visitor> t{v};

        auto counter = t.create_atomic_counter("test_name"s,simple_telemetry::labels_type{},10);
        REQUIRE(10==counter.value());
        SUBCASE("atomic counter is incremented to 1") {
            counter.increment();
            REQUIRE(counter.value()==uint64_t{11});
            SUBCASE("Collect renders correct output") {
                t.collect();
                REQUIRE("test_name 11\n"==ss.str());
            }
        }
    }


}