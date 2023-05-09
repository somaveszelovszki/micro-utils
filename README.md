micro-utils is a collection of C++ utility libraries.
In order to be well-suited for embedded use cases, the libraries do not have any dependencies
that might be missing from an embedded implementation of the standard.
Also, the libraries strictly use static memory allocation.

## Directories:
|Directory     |Role                                                                   |
|--------------|-----------------------------------------------------------------------|
| container/   | STL-like container classes that perform static memory allocation only |
| control/     | Utility classes used for implementing control algorithms              |
| debug/       | Debug tools for embedded use cases                                    |
| hw/          | Drivers for several hardware components                               |
| math/        | Mathematical helper classes and functions                             |
| panel/       | Connection protocols and common cross-component data structures       |
| port/        | Device independent hardware and operating system abstraction layer    |
| sensor/      | Sensor data filtering and fusion algorithms                           |
| sim/         | Simulator classes for testing purposes                                |
| test/        | Unit testing utilities                                                |
| utils/       | Low-level utility classes and functions                               |

Find out more about my projects at [soma.veszelovszki.hu](https://soma.veszelovszki.hu)
