# Media catalog - Final Lab

A C++ console application that implements the media catalog to encrypt and decrypt texts. It supports Russian and English alphabets, works with data in JSON format, logs operations, and provides a convenient text interface. Description

**Variant:** 10  
**Language:** C++  
**Standard:** C++17  

---

## Project structure

```
media_catalog/
├── src/
│   ├── main.cpp
│   ├── json_parser.cpp
│   ├── media.cpp
│   └── search.cpp
├── include/
│ ├── json_parser.h
│   ├── media.h
│   └── search.h
├── tests/
│   ├── test_parser.cpp
│   └── test_search.cpp
├── data/
│   ├── small_catalog.json
│   └── large_catalog.json
├── docs/
│   ├── Implementation_Plan.md
│   └── bench.md
├── scripts/
│   └── generate_data.py
├── CMakeLists.txt # Assembly Configuration
├── .gitignore # Git ignore
└── README.md # This file
```

---

## Requirements

- **Compiler:** GCC 7+ / Clang 5+ / MSVC 2017+
- **CMake:** version 3.10+
- **OS:** Linux, macOS, Windows

---

## Build the project
