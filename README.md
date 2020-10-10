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
    
This means there are 3 situations, where only in one situation the push tabular variant needs histograms or summaries.

This can be solved somewhere else, for example inside the exporter or maybe better, outside the application.   

This makes that I think instrumentation can be simplified to just a value recording instrument submitting values to 
an exporter. 

## Design

This library works with 4 components: 

1. Instruments
2. Instruments factory
3. Exporter

### Instruments

### Instruments factory

### Exporter

The exporter will receive all value changes and metadata and handle it accordingly.

The exporter also determines the type of metadata that is provided in each instrument.

The exporter must adhere to a view rules: 

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

  