# Poko Documentation

This directory contains all documentation for the Poko project.

## Generated Documentation

API documentation is generated using Doxygen and can be found in `docs/generated/`.

### Building Documentation

To generate the documentation:

```bash
# From the project root
doxygen Doxyfile
```

The HTML documentation will be generated in `docs/generated/html/`.

### Documentation Standards

All C++ code should use Doxygen-style comments:

```cpp
/**
 * @brief Brief description of the function/class
 * 
 * Detailed description of what this does.
 * 
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 */
```

## Project Documentation

- [Engineering Plan](../plan.md) - Master engineering plan and architecture
- [Feature Catalog](../features.md) - User-facing features
- [Conversion Plan](../conversion.md) - Character and content conversion pipeline
- [Changelog](../Chanelog.md) - Project changelog

## External Documentation

- [Mute Language](https://github.com/Raft-The-Crab/Mute) - Separate repository with own documentation