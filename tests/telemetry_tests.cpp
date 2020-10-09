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
        SUBCASE("Can create uint64 counter and is initialized with 0")
        {
            auto counter = telemetry.create_uint64_atomic_counter({"test"});
            REQUIRE(counter.value()==0);
            SUBCASE("Increment default increments counter with 1") {
                counter.incr();
                REQUIRE(counter.value()==1);
                SUBCASE("Serialized to test 1\\n") {
                    REQUIRE(ss.str()=="test 1\n");
                }
            }
            SUBCASE("Increment with 2 increments counter with 2") {
                counter.incr(2);
                REQUIRE(counter.value()==2);
                SUBCASE("Serialized to test 2\\n") {
                    REQUIRE(ss.str()=="test 2\n");
                }
            }
        }
        SUBCASE("Can create uint64 counter with default value 10 and is initialized with 10")
        {
            auto counter = telemetry.create_uint64_atomic_counter({"test"},10);
            REQUIRE(counter.value()==10);
            SUBCASE("Increment default increments counter with 1 to 11") {
                counter.incr();
                REQUIRE(counter.value()==11);
                SUBCASE("Serialized to test 11\\n") {
                    REQUIRE(ss.str()=="test 11\n");
                }
            }
            SUBCASE("Descrement default decrements counter with 1 to 9") {
                counter.decr();
                REQUIRE(counter.value()==9);
                SUBCASE("Serialized to test 9\\n") {
                    REQUIRE(ss.str()=="test 9\n");
                }
            }
        }
        SUBCASE("Can create int64 counter and is initialized with 0")
        {
            auto counter = telemetry.create_int64_atomic_counter({"test"});
            REQUIRE(counter.value()==0);
            SUBCASE("Increment default increments counter with 1") {
                counter.incr();
                REQUIRE(counter.value()==1);
                SUBCASE("Serialized to test 1\\n") {
                    REQUIRE(ss.str()=="test 1\n");
                }
            }
            SUBCASE("Increment with 2 increments counter with 2") {
                counter.incr(2);
                REQUIRE(counter.value()==2);
                SUBCASE("Serialized to test 2\\n") {
                    REQUIRE(ss.str()=="test 2\n");
                }
            }
        }
        SUBCASE("Can create int64 counter with default value 10 and is initialized with 10")
        {
            auto counter = telemetry.create_int64_atomic_counter({"test"},10);
            REQUIRE(counter.value()==10);
            SUBCASE("Increment default increments counter with 1 to 11") {
                counter.incr();
                REQUIRE(counter.value()==11);
                SUBCASE("Serialized to test 11\\n") {
                    REQUIRE(ss.str()=="test 11\n");
                }
            }
            SUBCASE("Decrement default decrements counter with 1 to 9") {
                counter.decr();
                REQUIRE(counter.value()==9);
                SUBCASE("Serialized to test 9\\n") {
                    REQUIRE(ss.str()=="test 9\n");
                }
            }
        }

        SUBCASE("Can create uint32 counter and is initialized with 0")
        {
            auto counter = telemetry.create_uint32_atomic_counter({"test"});
            REQUIRE(counter.value()==0);
            SUBCASE("Increment default increments counter with 1") {
                counter.incr();
                REQUIRE(counter.value()==1);
                SUBCASE("Serialized to test 1\\n") {
                    REQUIRE(ss.str()=="test 1\n");
                }
            }
            SUBCASE("Increment with 2 increments counter with 2") {
                counter.incr(2);
                REQUIRE(counter.value()==2);
                SUBCASE("Serialized to test 2\\n") {
                    REQUIRE(ss.str()=="test 2\n");
                }
            }
        }
        SUBCASE("Can create uint32 counter with default value 10 and is initialized with 10")
        {
            auto counter = telemetry.create_uint32_atomic_counter({"test"},10);
            REQUIRE(counter.value()==10);
            SUBCASE("Increment default increments counter with 1 to 11") {
                counter.incr();
                REQUIRE(counter.value()==11);
                SUBCASE("Serialized to test 11\\n") {
                    REQUIRE(ss.str()=="test 11\n");
                }
            }
            SUBCASE("Descrement default decrements counter with 1 to 9") {
                counter.decr();
                REQUIRE(counter.value()==9);
                SUBCASE("Serialized to test 9\\n") {
                    REQUIRE(ss.str()=="test 9\n");
                }
            }
        }
        SUBCASE("Can create int32 counter and is initialized with 0")
        {
            auto counter = telemetry.create_int32_atomic_counter({"test"});
            REQUIRE(counter.value()==0);
            SUBCASE("Increment default increments counter with 1") {
                counter.incr();
                REQUIRE(counter.value()==1);
                SUBCASE("Serialized to test 1\\n") {
                    REQUIRE(ss.str()=="test 1\n");
                }
            }
            SUBCASE("Increment with 2 increments counter with 2") {
                counter.incr(2);
                REQUIRE(counter.value()==2);
                SUBCASE("Serialized to test 2\\n") {
                    REQUIRE(ss.str()=="test 2\n");
                }
            }
        }
        SUBCASE("Can create int32 counter with default value 10 and is initialized with 10")
        {
            auto counter = telemetry.create_int32_atomic_counter({"test"},10);
            REQUIRE(counter.value()==10);
            SUBCASE("Increment default increments counter with 1 to 11") {
                counter.incr();
                REQUIRE(counter.value()==11);
                SUBCASE("Serialized to test 11\\n") {
                    REQUIRE(ss.str()=="test 11\n");
                }
            }
            SUBCASE("Decrement default decrements counter with 1 to 9") {
                counter.decr();
                REQUIRE(counter.value()==9);
                SUBCASE("Serialized to test 9\\n") {
                    REQUIRE(ss.str()=="test 9\n");
                }
            }
        }
    }
}

