# External Dependencies

This directory contains third-party library source code used by Poko Engine.

## Dependency Management Policy

- External dependencies are pinned by revision
- Use third-party foundations where they save years of work
- Wrap them behind Poko-owned adapters
- Track upstream revision, local patches, license, security notes, compatibility state, and test coverage
- Prefer upstream contributions over permanent forks
- Do not expose third-party dependency directly through creator-facing APIs

## Installed Dependencies

### Graphics
- **Diligent Engine** - Graphics API abstraction
  - Repository: https://github.com/DiligentGraphics/DiligentEngine
  - Revision: ee29581ef213c564ae15626e24cc38bf8a35a44a
  - Purpose: Cross-platform graphics API abstraction (DirectX 11/12, Vulkan, Metal, OpenGL)
  - Status: ⚠️ Installed but temporarily disabled (submodule initialization issues)
  - Adapter: `engine/src/renderer/` (abstraction layer ready, awaiting proper CMake configuration)
  - License: Apache 2.0
  - Local patches: None
  - Notes: ThirdParty subdirectories (SPIRV-Tools, glslang, SPIRV-Cross, volk) need proper initialization

- **Diligent Core** - Core utilities for Diligent Engine
  - Repository: https://github.com/DiligentGraphics/DiligentCore
  - Revision: 18bfa7b7563a0ef5b5fe074d37c2e8304100e965
  - Purpose: Core utilities shared across Diligent Engine components
  - Status: ⚠️ Installed but temporarily disabled (submodule initialization issues)
  - Adapter: Integrated with Diligent Engine
  - License: Apache 2.0
  - Local patches: None
  - Notes: ThirdParty subdirectories need proper initialization

### Physics
- **Jolt Physics** - Rigid-body physics simulation
  - Repository: https://github.com/jrouwe/JoltPhysics
  - Revision: a63aa3b8e24cf95f3fab2613f9a3015b164ef62c (v5.2.0)
  - Purpose: Physics simulation, collision detection, constraints
  - Status: ✅ Installed
  - Adapter: `engine/src/physics/` (to be implemented)
  - License: MIT
  - Local patches: None

### Audio
- **miniaudio** - Audio foundation
  - Repository: https://github.com/mackron/miniaudio
  - Revision: 9634bedb5b5a2ca38c1ee7108a9358a4e233f14d
  - Purpose: Cross-platform audio playback and recording
  - Status: ✅ Installed
  - Adapter: `engine/src/audio/` (to be implemented)
  - License: Public Domain (unlicense)
  - Local patches: None

### Profiling
- **Tracy Profiler** - Profiling during development
  - Repository: https://github.com/wolfpld/tracy
  - Revision: 0913edf7be17f2261ae85692f0ef0760ff2423e1
  - Purpose: CPU, GPU, and memory profiling
  - Status: ✅ Installed
  - Adapter: `engine/src/profiling/` (to be implemented)
  - License: BSD 3-Clause
  - Local patches: None

### Mesh Processing
- **meshoptimizer** - Mesh processing and optimization
  - Repository: https://github.com/zeux/meshoptimizer
  - Revision: 9e1f07b159d3cb777f1c67ed31fc11fd117986f4
  - Purpose: Mesh simplification, vertex cache optimization, etc.
  - Status: ✅ Installed
  - Adapter: `engine/src/assets/mesh/` (to be implemented)
  - License: MIT
  - Local patches: None

### Model Import
- **fastgltf** - glTF importer
  - Repository: https://github.com/spnda/fastgltf
  - Revision: f89e438230b6624d5e886fac0d1829b7c7299b2e
  - Purpose: Modern glTF 2.0 model import
  - Status: ✅ Installed
  - Adapter: `engine/src/assets/import/` (to be implemented)
  - License: MIT
  - Local patches: None

### Compression
- **zstd** - Zstandard compression
  - Repository: https://github.com/facebook/zstd
  - Revision: 01b7154f1172432f8abe9b3bb9909e14a1176b7d
  - Purpose: Fast compression algorithm for assets and networking
  - Status: ✅ Installed
  - Adapter: `engine/src/core/compression/` (to be implemented)
  - License: BSD 3-Clause
  - Local patches: None

### Cryptography
- **libsodium** - Cryptography library
  - Repository: https://github.com/jedisct1/libsodium
  - Revision: c960b3b0c6e430ff2dfdb776bf1c990e62f3f18b
  - Purpose: Encryption, hashing, and secure memory operations
  - Status: ✅ Installed
  - Adapter: `engine/src/crypto/` (to be implemented)
  - License: ISC
  - Local patches: None

### Text Rendering
- **HarfBuzz** - Text shaping engine
  - Repository: https://github.com/harfbuzz/harfbuzz
  - Revision: 409c467b8259ad5fcc3fdcc477a1796fec256853
  - Purpose: Unicode text shaping and font rendering
  - Status: ✅ Installed
  - Adapter: `engine/src/ui/text/` (to be implemented)
  - License: MIT
  - Local patches: None

- **FreeType** - Font rasterization library
  - Repository: https://github.com/freetype/freetype
  - Revision: d333439633039de426f943f28a2926c7f97b5ae5
  - Purpose: Font rendering and glyph outline processing
  - Status: ✅ Installed
  - Adapter: `engine/src/ui/text/` (to be implemented)
  - License: FreeType License (FTL)
  - Local patches: None

### Texture Compression
- **KTX Software** - KTX texture container
  - Repository: https://github.com/KhronosGroup/KTX-Software
  - Revision: dd3b8b0a788c9e61ca835bec3c11961d5ffe2d6f
  - Purpose: KTX file format loader/writer
  - Status: ✅ Installed
  - Adapter: `engine/src/assets/texture/` (to be implemented)
  - License: Apache 2.0
  - Local patches: None

- **Basis Universal** - Texture compression
  - Repository: https://github.com/BinomialLLC/basis_universal
  - Revision: 99f52d63aa6799cbdaecfe977111dc5ec3b31d47
  - Purpose: Basis Universal texture compression/transcoding
  - Status: ✅ Installed
  - Adapter: `engine/src/assets/texture/` (to be implemented)
  - License: Apache 2.0
  - Local patches: None

## Directory Structure

```
external/
├── DEPENDENCIES.md      # This file
├── jolt/                # Jolt Physics source (v5.2.0)
├── diligent/            # Diligent Engine source
├── diligentcore/        # Diligent Core source
├── tracy/               # Tracy Profiler source
├── miniaudio/           # miniaudio source
├── fastgltf/            # fastgltf source
├── zstd/                # zstd source
├── libsodium/           # libsodium source
├── harfbuzz/            # HarfBuzz source
├── meshoptimizer/       # meshoptimizer source
├── ktx/                 # KTX Software source
├── basis/               # Basis Universal source
└── freetype/            # FreeType source
```

## Integration Steps

When adding a new dependency:

1. Clone repository to appropriate subdirectory with pinned revision
2. Remove .git directory to prevent accidental updates
3. Record upstream revision and commit hash in this file
4. Document license and any local patches
5. Create Poko adapter in relevant engine module
6. Add CMake integration in engine/CMakeLists.txt
7. Write adapter tests
8. Update this file with integration status

## Compatibility Notes

Before updating any third-party library:
- Run adapter tests
- Run engine tests
- Run performance benchmarks
- Run platform tests
- Record upstream revision
- Update compatibility state in this file

## Last Updated

2026-09-26 - Initial installation of all 13 external dependencies
