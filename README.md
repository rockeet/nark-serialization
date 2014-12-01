nark-serialization
==================

An easy, fast, small, portable C++ object serialization framework

## Features
  1. Declarative syntax, Easy to use, Easy to integrate
  1. *Fast* at runtime, *Fast* at compile time
  1. Serialized data is *small*, Compiled code is *small*
  1. Support STL containers & types
  1. Can serialize to BigEndian/LittleEndian in any platform
  1. Support variant int/uint encoding
  1. Optional serialization version control
  1. More and more...

## Quick Start

nark-serialization is powerful, but using it is very simple:

```c++
#include <nark/io/DataIO.hpp>
#include <nark/io/MemStream.hpp>
#include <nark/io/FileStream.hpp>

struct MyData1 {
    int  a, b, c;
    var_int32_t d;  // d.t is int32 value
    var_uint64_t e; // e.t is uint64 value
    std::string f;
    std::map<std::string, int> g;
    std::set<int> h;
 
	// The simplest use case(without version control):
    DATA_IO_LOAD_SAVE(MyData1, &a&b&c&d&e&f&g&h)
};
 
struct MyData2 {
    int  a, b, c;
    var_int32_t d;
    var_uint64_t e;
    std::string f;
    std::map<std::string, int> g;
    std::set<int> h;
 
    DATA_IO_LOAD_SAVE_V(MyData2, // With version control
        1, // 1 is current version
        &a&b&c&d&e&f&g&h)
};
 
struct MyData3 {
    int  a, b, c;
    boost::int32_t d;
    boost::uint64_t e;
    std::string f;
    std::map<std::string, int> g;
    std::set<int> h;
    std::multiset<int> i;
    unsigned version;
 
    DATA_IO_LOAD_SAVE_V(MyData3, // With version control
        2, // current version
        &a &b &c
        &as_var_int(d) // 'd' defined as  int32_t, serialized as var_int32_t
        &as_var_int(e) // 'e' defined as uint64_t, serialized as var_uint64_t
        &f &g &h
        &vmg.since(2, i) // 'i' is added in version=2
        &vmg.get_version(version) // optional, you can keep version number for latter use
        )
};
 
// in system or third-party's header, can not be changed
struct SysData1 {
    int a;
    unsigned b;
    string c;
};
DATA_IO_LOAD_SAVE_E(SysData2, &a &b &c)

struct SysData2 {
    int a;
    unsigned b;
    string c;
};
DATA_IO_LOAD_SAVE_EV(SysData2, &a &b& vmg.since(2, c))

int main(int argc, char* argv[]) {
    PortableDataOutput<AutoGrownMemIO> output; // 'Portable' is BigEndian
    PortableDataInput<MemIO> input; // 'Portable' is BigEndian

    MyData1 d1;
    // set d1 values
    // ...
    MyData2 d2;
    // set d2 values
    // ...
    MyData3 d3;
    // set d3 values
    // ...
	SysData1 sd1;
	SysData2 sd2;
    output << d1 << d2 << d3 << sd1 << sd2; // saving

    input = output.head(); // shallow copy, input is the written part of output
    input >> d1 >> d2 >> d3 >> sd1 >> sd2; // loading

    output & d1 & d2 & d3 & sd1 & sd2; // saving, '&' is an alias of '<<'

    input = output.head(); // shallow copy, input is the written part of output 
    input & d1 & d2 & d3 & sd1 & sd2; // loading, '&' is an alias of '>>'
}
```
Is it a magic? I wrote the first version of nark-serialization at July 2006, three months later, I wrote [nark-rpc](https://github.com/rockeet/nark-rpc) on top of nark-serialization.

## More

To be done
