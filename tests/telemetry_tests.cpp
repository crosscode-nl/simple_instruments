#include "simple_telemetry.h"
#include "doctest.h"
#include <sstream>

using namespace std::literals;

struct metadata {
    std::string name;
};

std::string name(const metadata& md) {
    return md.name;
}

class exporter {
    std::ostream *os_;
public:
    explicit exporter(std::ostream *os) : os_(os) {}
    template <typename Tvalue>
    void on_data_changed(const Tvalue &value, const metadata& md) const {
        (*os_) << name(md) << " " << value << "\n";
    }
};

TEST_SUITE("telemetry") {
    TEST_CASE("Can create telemetry") {
        std::stringstream ss;
        simple_telemetry::telemetry<metadata, exporter> telemetry{exporter{&ss}};
        SUBCASE("Can create int16_t value recorder and is initialized with 0") {
            auto counter = telemetry.create_atomic_value_recorder_counter<int16_t>({"test"});
            REQUIRE(0==counter.value());
            SUBCASE("Set value to 5736, value is set to 5736") {
                counter.set(5736);
                REQUIRE(5736==counter.value());
                SUBCASE("Serialized to test 5736\\n") {
                    REQUIRE(ss.str()=="test 5736\n");
                }
            }
        }
        SUBCASE("Can create int16_t monotonic counter and is initialized with 0") {
            auto counter = telemetry.create_atomic_monotonic_counter<int16_t>({"test"});
            static_assert(!std::is_copy_assignable_v<decltype(counter)>,"atomic_monotonic_counter should not be copy assignable");
            static_assert(!std::is_copy_constructible_v<decltype(counter)>,"atomic_monotonic_counter should not be copy constructable");
            static_assert(std::is_same_v<decltype(counter)::value_type,std::int16_t>,"value_type should be the same  type as int64_t");
            static_assert(std::is_same_v<decltype(counter)::metadata_type,metadata>,"metadata_type should be the same type as metadata");
            static_assert(std::is_same_v<decltype(counter)::exporter_type,exporter>,"exporter_type should be the same type as exporter");
            REQUIRE(0==counter.value());
            SUBCASE("Add increments it to 1") {
                counter.add();
                REQUIRE(1==counter.value());
                SUBCASE("Serialized correctly") {
                    REQUIRE(ss.str()=="test 0\ntest 1\n");
                }
            }
        }
        SUBCASE("Can create int16_t monotonic counter with init value 10, step 3 and is initialized with 10") {
            auto counter = telemetry.create_atomic_monotonic_counter<int16_t,3>({"test"},10);
            static_assert(std::is_same_v<decltype(counter)::value_type,std::int16_t>,"value_type should be the same  type as int64_t");
            static_assert(std::is_same_v<decltype(counter)::metadata_type,metadata>,"metadata_type should be the same type as metadata");
            static_assert(std::is_same_v<decltype(counter)::exporter_type,exporter>,"exporter_type should be the same type as exporter");
            REQUIRE(10==counter.value());
            SUBCASE("Add increments it to 13") {
                counter.add();
                REQUIRE(13==counter.value());
            }
        }

        SUBCASE("Can create int16_t bidirectional counter and is initialized with 0") {
            auto counter = telemetry.create_atomic_bidirectional_counter<int16_t>({"test_overflow"});
            static_assert(!std::is_copy_assignable_v<decltype(counter)>,"atomic_bidirectional_counter should not be copy assignable");
            static_assert(!std::is_copy_constructible_v<decltype(counter)>,"atomic_bidirectional_counter should not be copy constructable");
            static_assert(std::is_same_v<decltype(counter)::value_type,std::int16_t>,"value_type should be the same  type as int64_t");
            static_assert(std::is_same_v<decltype(counter)::metadata_type,metadata>,"metadata_type should be the same type as metadata");
            static_assert(std::is_same_v<decltype(counter)::exporter_type,exporter>,"exporter_type should be the same type as exporter");
            SUBCASE("Can decrement with std::numeric_limits<int16_t>::min() and value become std::numeric_limits<int16_t>::min()") {
                counter.add(std::numeric_limits<int16_t>::min());
                REQUIRE(std::numeric_limits<int16_t>::min()==counter.value());
                SUBCASE("Can decrement one more, and value becomes std::numeric_limits<int16_t>::max()") {
                    counter.sub(1);
                    REQUIRE(std::numeric_limits<int16_t>::max() == counter.value());
                    SUBCASE("Serialized to test_overflow 0\\ntest_overflow -32768\\ntest_overflow 32767\\n") {
                        REQUIRE(ss.str()=="test_overflow 0\ntest_overflow -32768\ntest_overflow 32767\n");
                    }
                }
            }
        }
        SUBCASE("Can create uint64 bidirectional counter and is initialized with 0")
        {
            auto counter = telemetry.create_atomic_bidirectional_counter<uint64_t>({"test"});
            static_assert(std::is_same_v<decltype(counter)::value_type,std::uint64_t>,"value_type should be the same  type as uint64_t");
            static_assert(std::is_same_v<decltype(counter)::metadata_type,metadata>,"metadata_type should be the same type as metadata");
            static_assert(std::is_same_v<decltype(counter)::exporter_type,exporter>,"exporter_type should be the same type as exporter");
            REQUIRE(counter.value()==0);
            SUBCASE("Increment default increments counter with 1") {
                counter.add();
                REQUIRE(counter.value()==1);
                SUBCASE("Serialized to test 0\\ntest 1\\n") {
                    REQUIRE(ss.str()=="test 0\ntest 1\n");
                }
            }
            SUBCASE("Increment with 2 increments counter with 2") {
                counter.add(2);
                REQUIRE(counter.value()==2);
                SUBCASE("Serialized to test 0\\ntest 2\\n") {
                    REQUIRE(ss.str()=="test 0\ntest 2\n");
                }
            }
        }
        SUBCASE("Can create uint64 bidirectional counter with default value 10 and is initialized with 10")
        {
            auto counter = telemetry.create_atomic_bidirectional_counter<uint64_t>({"test"},10);
            REQUIRE(counter.value()==10);
            SUBCASE("Increment default increments counter with 1 to 11") {
                counter.add();
                REQUIRE(counter.value()==11);
                SUBCASE("Serialized to test 10\\ntest 11\\n") {
                    REQUIRE(ss.str()=="test 10\ntest 11\n");
                }
            }
            SUBCASE("Decrement default decrements counter with 1 to 9") {
                counter.sub();
                REQUIRE(counter.value()==9);
                SUBCASE("Serialized to test10\\ntest 9\\n") {
                    REQUIRE(ss.str()=="test 10\ntest 9\n");
                }
            }
        }
    }
}

