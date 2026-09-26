# Poko Character & Content Conversion Plan

Status: Planning baseline
Purpose: Define the full conversion pipeline for source rigs/assets into Poko-native runtime content.
This file is intentionally separate from the main engineering plan so conversion rules can evolve independently.

## 1. Goals

- Create a Poko-native canonical character format.
- Preserve a simple six-part humanoid workflow while allowing more detailed source rigs.
- Normalize scale, coordinates, attachments, materials, animation, and collision.
- Convert once during authoring/build, not repeatedly during gameplay.
- Produce optimized Poko-native assets.
- Give creators clear warnings and deterministic results.

## 2. Canonical Poko Character

```text
Character
├── Root
├── Torso
├── Head
├── LeftArm
├── RightArm
├── LeftLeg
└── RightLeg
```

Reference proportions are approximately:

```text
width  ≈ 4 units
height ≈ 4.5 units
depth  ≈ 2 units
```

The exact canonical measurements are implementation configuration until movement, camera, collision, and animation tests are complete.

## 3. Conversion Profiles

### Maximum Compatibility
Preserve the greatest amount of useful source appearance while normalizing the runtime structure.

### Balanced
Normalize geometry, materials, animation, and collision for general UGC.

### Performance
Apply more aggressive mesh reduction, texture processing, animation compression, and LOD generation.

## 4. Pipeline

```text
Source
  ↓
Import
  ↓
Detect source type
  ↓
Validate
  ↓
Coordinate normalization
  ↓
Scale normalization
  ↓
Skeleton/body mapping
  ↓
Joint reconstruction
  ↓
Material conversion
  ↓
Clothing conversion
  ↓
Accessory conversion
  ↓
Animation retargeting
  ↓
Collision generation
  ↓
LOD generation
  ↓
Optimization
  ↓
Validation
  ↓
Poko Character Asset
```

## 5. Supported Source Profiles

Initial profiles:

- Poko native
- Poko six-part
- generic six-part humanoid
- compatible humanoid skeleton
- custom supported rig

Unsupported source formats must fail clearly rather than producing partially broken output.

## 6. Canonical Mapping

Source concepts map into:

```text
HEAD
TORSO
LEFT_ARM
RIGHT_ARM
LEFT_LEG
RIGHT_LEG
ROOT
```

When a source rig contains more detailed hand/foot/arm/leg bones, conversion may collapse them into the canonical six-part runtime body for the basic Poko character while retaining optional extra bones only when the target runtime explicitly supports them.

## 7. Scale Normalization

Record:

- source bounding box
- source pivot
- source unit assumptions
- desired Poko bounds
- generated scale factor
- post-scale validation

The conversion result must fit the canonical body bounds within configured tolerances.

## 8. Coordinate Normalization

Normalize:

- up axis
- handedness
- forward axis
- pivot orientation
- local/world transforms

Never silently guess when the source format provides explicit coordinate metadata.

## 9. Joint Reconstruction

Build canonical joints:

```text
Root -> Torso
Torso -> Head
Torso -> LeftArm
Torso -> RightArm
Torso -> LeftLeg
Torso -> RightLeg
```

Joint limits and offsets are generated from canonical configuration.

## 10. Collision

Visual meshes are not automatically used as runtime colliders.

Generate simplified collision representations:

- capsule/box-like torso
- simplified head
- capsule/box-like limbs
- root controller geometry

Store generated collision separately from render mesh data.

## 11. Materials

Convert source material information into Poko-native material properties:

- base color
- metallic
- roughness
- normal
- emissive
- opacity
- texture references

Unsupported features produce warnings or an explicit fallback.

## 12. Clothing

Support:

- texture clothing
- mesh clothing

Future extension:
- layered clothing

Converted clothing is stored as Poko-native content and does not depend on source runtime code.

## 13. Accessories

Canonical sockets:

```text
Head
Face
Back
Front
Waist
LeftHand
RightHand
LeftFoot
RightFoot
```

For each accessory:

1. detect source attachment
2. map attachment to canonical socket
3. normalize scale
4. normalize orientation
5. validate bounds
6. generate derived LOD where appropriate

Studio must provide manual correction when automatic mapping is ambiguous.

## 14. Animation Retargeting

Pipeline:

```text
source clip
  ↓
source skeleton map
  ↓
canonical bone map
  ↓
retarget
  ↓
validate foot/root motion
  ↓
compress
  ↓
Poko animation
```

Required tests:

- idle
- walk
- run
- jump
- fall
- basic attack

## 15. Animation Validation

Detect:

- unmapped bones
- excessive root motion
- broken rotations
- invalid keyframes
- discontinuities
- unsupported tracks

Warnings should identify exact clips and affected bones.

## 16. Mesh Optimization

Derived meshes may include:

- simplified geometry
- normals/tangents
- meshlets or engine-appropriate structures
- LOD levels
- optimized index/vertex order

Use mesh optimization tooling behind a Poko asset pipeline.

## 17. Texture Processing

Pipeline:

```text
source image
→ validate dimensions
→ color-space normalization
→ mip generation
→ platform compression
→ size checks
→ derived asset
```

Different platform profiles may choose different compression/output settings.

## 18. LOD Generation

Generate per-asset LODs where useful.

LOD metadata should include:

- geometric reduction
- screen size threshold
- texture policy
- shadow policy

Do not generate huge numbers of LODs for tiny objects.

## 19. Character Validation Report

Every conversion produces:

```text
result
errors[]
warnings[]
source metadata
output metadata
estimated memory
estimated geometry
estimated texture cost
```

Example:

```text
✓ body mapped
✓ scale normalized
✓ collision generated
✓ animation retargeted

Warnings:
- 2 accessories exceed recommended geometry budget
- 1 animation has root-motion drift
- 1 texture was downscaled
```

## 20. Determinism

The same source, converter PNV, conversion profile, and configuration should produce the same derived content.

Record conversion configuration in the output metadata.

## 21. Conversion Cache

Cache conversion results by source identity and conversion configuration.

The cache may use content hashes internally.

The hash is not the public product version.

Invalidate when:

- source changes
- converter changes
- engine conversion rules change
- profile changes
- material rules change

## 22. Studio Workflow

```text
Import
→ Detect
→ Preview
→ Configure profile
→ Convert
→ Review warnings
→ Apply manual fixes
→ Save Poko-native asset
```

Do not make creators repeat conversion for every playtest.

## 23. PokoX Runtime Rule

PokoX consumes the converted Poko-native asset.

It does not:

- execute source importers
- run source rig detection
- perform expensive retargeting
- rebuild collision from raw source
- rebuild LODs

unless a runtime streaming format explicitly requires a lightweight derived operation.

## 24. Package Integration

Converted character assets enter:

```text
Studio project
→ client asset pipeline
→ Client PKX
```

Authority receives only the character/gameplay definitions it needs.

## 25. Conversion Failure Policy

Hard errors stop conversion.

Warnings produce an asset only when the result remains valid.

Never silently discard required bones, materials, or animations.

## 26. Security

Conversion inputs are untrusted.

Validate:

- file size
- vertex counts
- bone counts
- animation counts
- texture dimensions
- embedded paths
- archive structure
- recursive dependencies
- malformed metadata

No imported asset may execute arbitrary native code.

## 27. Acceptance Tests

A release-quality converter must pass:

- canonical six-part source
- varied proportions
- reversed axis source
- scaled source
- missing optional accessory
- missing required body part
- malformed mesh
- malformed animation
- large asset
- many accessories
- repeated import
- cache hit
- cache invalidation

## 28. Future Extensions

Potential later conversion systems:

- advanced humanoid rigs
- facial rigs
- layered clothing
- custom skeleton preservation
- non-humanoid creature conversion
- vehicle/prop conversion
- animation libraries
- batch conversion
