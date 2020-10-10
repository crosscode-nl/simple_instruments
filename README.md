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
4. Metadata

### Instruments
### Instruments factory
### Exporter
The exporter will receive all value changes and metadata and handle it accordingly.
### Metadata
The exporter determines what metadata type is used. 
  