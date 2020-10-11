# Simple Instruments

## Introduction

Simple instruments is a very simple approach for adding instrumentation to your code. This C++17 header only library
helps adding instruments in your code. 

The instruments are created from a factory which is created with an external supplied exporter.

## Why?

I looked at a few library's and investigated the code commonly used for instrumenting applications and found that they
were too coupled with backing time series database and push/pull model that was used. I also found that too many 
specialized instruments where defined, up until the point that metrics where already created in instruments.

Some libraries allow you to use a histogram or summary instrument. However, both need to be configured and have to deal 
with limits. However, I believe these are things that should be configured and calculated somewhere else and not in the 
instrumented code. That code should not need to be changed if for example new network technologies make the current 
configuration of histograms useless. 

Libraries that allow to create instruments like histograms or summaries are basically applying metrics to early.  

You can have a pull model where a time series database pulls data from you instruments. It can pull the data as a stream
or as a table.  

1. All instrument value changes are send, basically a stream. 
2. A snapshot of the instrument values is send, basically tabular data.

You can also have a push model, where you send all value changes to a time series database. 
    
This means there are three situations, where only in one situation the push tabular variant needs histograms 
or summaries.

This can be solved somewhere else, for example inside the exporter or maybe better, outside the application.

I see these models:

### Push 

```                              
+--------------------+      Pushed     +-------------+
| Application        | Instrumentation |             |
|                    |       data      | Time Series |
|====================+---------------->+             |
| simple_instruments |                 |  Database   |
| push_exporter      |                 |             |
+--------------------+                 +-------------+
```

### Pull

```
+--------------------+     Pushed      +-------------+     Pulled      +-------------+
| Application        | Instrumentation |             | Instrumentation |             |
|                    |      data       | Time Series |      data       | Time Series |
|====================+---------------->+             +---------------->+             |
| simple_instruments |                 | Pull Buffer |                 |  Database   |
| push_exporter      |                 |             |                 |             |
+--------------------+                 +-------------+                 +-------------+
```


This makes that I think instrumentation can be simplified to just a value recording instrument submitting values to 
a push exporter compatible with a popular time series database. 

The time series pull buffer should accept the same protocol as the popular time series database. The pull buffer
should be configurable to setup  

## Design

This library works with 4 components: 

1. Instruments
2. Instruments factory
3. Exporter

### Instruments factory

The instrument factory creates instruments and owns a shared pointer to the exporter. 

You can access the exporter by calling exporter() on the factory.

The factory can create the following instrument types: 

| Instrument                   | Description         | Example            |
|------------------------------|---------------------|--------------------|
| atomic_bidirectional_counter | Counts up and down. | Active requests    |
| atomic_monotonic_counter     | Counts up.          | Completed requests |
| atomic_value_recorder        | Records any value.  | Received bytes     |

### Instruments

The following examples use the exporter described below. 


```cpp
    std::stringstream ss;
    csi::instrument_factory factory(exporter{&ss});
    auto counter = factory.make_atomic_bidirectional_counter<uint64_t>({"test"});
    counter.add();
```

```cpp
    std::stringstream ss;
    csi::instrument_factory factory(exporter{&ss});
    auto counter = factory.make_atomic_monotonic_counter<uint64_t>({"test"});
    counter.add();
```

```cpp
    std::stringstream ss;
    csi::instrument_factory factory(exporter{&ss});
    auto counter = factory.make_atomic_bidirectional_counter<uint64_t>({"test"});
    counter.add();
```


### Exporter

The exporter will receive all value changes and metadata and handle it accordingly.

The exporter also determines the type of metadata that is provided in each instrument.

To implement a minimal exporter you have to do the following things:  

1. The exporter determines the type of the metadata that is used like so: 

```cpp
struct metadata { // this is the metadata struct that is going to be used
    std::string name;
    bool emit_initial{true};
};

class exporter {
public:
    // The type alias to the metadata type is required in the exporter
    // This type is used in the instrument_factory, but since it is exporter determined it makes sense to let 
    // the exporter provide the type instead of the user that uses the instrument_factory.
    using metadata_type = metadata;   
};
```

emit_init exists to receive an initial value and also detect a creation of an instrument. This can be used to directly
send the initial value to a backend or to validate that an instrument is unique. An exporter should assert that is true
because it is a programming error if it is not. 

```cpp
struct metadata {
    std::string name;
    bool emit_initial{true};
};

class exporter {
public:
    using metadata_type = metadata;
private:
    std::ostream *os_;
public:
    explicit exporter(std::ostream *os) : os_(os) {}

    template <typename Tvalue>
    void emit_init(const Tvalue &value, const metadata_type& md) const {
        if (md.emit_initial) {
            (*os_) << md.name << " " << value << "\n";
        }
    }

    template <typename Tvalue>
    void emit(const Tvalue &value, const metadata_type& md) const {
        (*os_) << md.name << " " << value << "\n";
    }
};
```

## Installation

There are multiple ways to add this library to your project. There are too many tools for C++ to describe them all. 

I will describe two methods, both for CMake users. 

The first method will download the source code and build against it, the second will use a system installed version of 
the library.

The first method works best in CI/CD pipelines without to much hassle. This is therefore the preferred method.

### CMake FetchContent (Preferred)

This method emulates dependency management of more modern languages and build systems like Rust+Cargo and Go as best as 
it can.

Consider the following example project: 

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(example main.cpp)
target_compile_features(example PUBLIC cxx_std_17)
```

This will build a C++17 project with a main.cpp file.

Now to add this library to the project you can use the 
[FetchContent CMake module](https://cmake.org/cmake/help/v3.18/module/FetchContent.html). 

The minimal requirement is to add: 

```cmake
include(FetchContent)
FetchContent_Declare(
        simple_instruments
        GIT_REPOSITORY https://github.com/crosscode-nl/simple_instruments
)
FetchContent_MakeAvailable(simple_instruments)
```

And to link against the library: 

```cmake
target_link_libraries(example simple_instruments)
```

This will result in the following CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(example main.cpp)
target_compile_features(example PUBLIC cxx_std_17)

include(FetchContent)
FetchContent_Declare(
        simple_instruments
        GIT_REPOSITORY https://github.com/crosscode-nl/simple_instruments
)
FetchContent_MakeAvailable(simple_instruments)
target_link_libraries(example simple_instruments)
```

Now see the examples to learn how to use this library.

*TIP: Use `GIT_TAG` in `FetchContent_Declare` to pin a certain version to get reproducible builds.*

### CMake Find_Package

This method will use CMake's find_package.

The first step is to install the library on the system.

#### Installing onto the system

This method requires InfluxDBLPTool to be build and installed on the system.

```bash
git clone https://github.com/crosscode-nl/simple_instruments
mkdir simple_instruments-build
cd simple_instruments-build
cmake ../simple_instruments -DCMAKE_BUILD_TYPE=Release
sudo make install 
```

Or if you want to install it somewhere in you home directory, for example: 

```bash
git clone https://github.com/crosscode-nl/simple_instruments
mkdir simple_instruments-build
cd simple_instruments-build
cmake ../simple_instruments -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/developer/libraries
make install 
```

#### Using the installed library

Consider the following example project: 

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(example main.cpp)
target_compile_features(example PUBLIC cxx_std_17)
```

To use an installed library you just have to add: 

```cmake
find_package(SimpleInstruments 1.0.0 REQUIRED)
target_link_libraries(example SimpleInstruments::simple_instruments)
```

This will result in the following CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(example main.cpp)
target_compile_features(example PUBLIC cxx_std_17)

find_package(SimpleInstruments 1.0.0 REQUIRED)
target_link_libraries(example SimpleInstruments::simple_instruments)
```

When you installed the library in a different location then you have to add 
`-DCMAKE_PREFIX_PATH=/home/developer/libraries` to your 
[CMake command](https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html). 

## License

MIT License

Copyright (c) 2020 CrossCode / P. Vollebregt

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the 
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the 
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  