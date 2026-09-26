# Poko Master Engineering Plan

## Poko Master Engineering Plan

Status: Planning baseline
Audience: Moby Productions engineering team and coding agents
Primary use: Vibe coding / implementation guidance
Scope: Engine, Mute, Game Authority, Main Backend, PokoX, Poko Studio, Moderation, platform fundamentals, infrastructure, security, testing, release engineering.
Excluded from this document: detailed character/rig conversion rules and the exhaustive user-facing feature catalog. Those live in conversion.md and features.md.
Core principle: cheap by default, powerful when needed.

## 0. How To Use This Plan

This document is authoritative for architecture and implementation order until an explicit revision is approved.
Every task must have an owner, status, dependency, acceptance criteria, and test strategy before implementation.
Coding agents must prefer existing interfaces over inventing parallel systems.
Do not introduce a new persistent service when an existing module can satisfy the requirement.
Do not expose a third-party dependency directly through creator-facing APIs.
Do not make client state authoritative when the requirement belongs to the Game Authority or Main Backend.
Do not put gameplay hot paths through Cloudflare Workers unless the traffic pattern is explicitly designed for it.
When an implementation decision is ambiguous, follow the dependency direction and failure rules in this document.

## 1. Product Boundary

Poko consists of PokoX, Poko Studio, Poko Moderation, Poko Engine, Mute, Game Authority, and Main Backend.
PokoX is the player runtime and platform client.
Poko Studio is the creator/developer application.
Poko Moderation is the trust and operations application.
Poko Engine is shared runtime/editor technology.
Mute is the game programming language.
Game Authority is a lightweight request-validation authority, not a continuously simulated game server.
Main Backend is the platform control plane and persistent state owner.

## 2. System Dependency Graph

The dependency direction is:
Platform Applications -> Poko SDK/API -> Poko Engine or Backend SDK -> infrastructure.
Poko Engine -> external foundations through adapters.
Mute -> Poko Engine runtime APIs and compiler services.
Game Authority -> WebSocket protocol, Mute runtime, session state, validation, PKX.
Main Backend -> PostgreSQL/Valkey/D1/R2/external providers.
Poko Studio -> Engine runtime plus project/build services.
PokoX -> Engine runtime plus platform client services.
Moderation -> Main Backend APIs and audit systems.
Circular dependencies are prohibited at module boundaries.

## 3. Non-Goals

Do not build an internal CDN.
Do not build a replacement database.
Do not build a replacement physics engine in v1.
Do not build a replacement graphics API.
Do not build a replacement Git implementation.
Do not build a global multi-region platform before APAC operation is stable.
Do not require server-side continuous physics simulation.
Do not require AAA-level rendering defaults.
Do not create dozens of microservices before operational evidence requires decomposition.
Do not create multiple scripting languages.

## 4. Repository Topology

Recommended repositories:
poko-engine
poko-mute
poko-authority
poko-backend
poko-workers
poko-studio
pokox
poko-moderation
poko-protocol
poko-pkx
poko-sdk
poko-host-agent
poko-tooling
Each repository exposes versioned interfaces where cross-repository coupling exists.
Shared contracts must be generated or versioned from a single source.

## 5. Build System

C/C++ uses CMake with reproducible dependency pinning.
Go uses go.mod/go.sum and reproducible builds.
TypeScript uses locked package manifests.
Android uses Gradle/Kotlin with native engine integration.
Windows uses a supported MSVC/Clang toolchain matrix.
Every repository must expose:
build
test
lint
format
package
clean
diagnostics

## 6. CI/CD Baseline

Every merge must run formatting, static analysis, unit tests, integration tests applicable to touched modules, and package validation.
Release pipelines must produce signed or integrity-protected artifacts.
PNV must be assigned by release tooling rather than manually typed.
Artifact provenance must be retained.
Failed release builds must not become published versions.

## 7. Dependency Management

External dependencies are pinned by revision and recorded in a machine-readable dependency manifest.
Use third-party foundations where they save years of work.
Wrap them behind Poko-owned adapters.
Track upstream revision, local patches, license, security notes, compatibility state, and test coverage.
Prefer upstream contributions over permanent forks.

## 8. Planned Foundation Libraries

Expected foundations include:
Jolt Physics for rigid-body physics and related low-level simulation primitives.
Diligent Engine for graphics API abstraction.
miniaudio or equivalent for audio foundation.
Tracy or equivalent for profiling during development.
meshoptimizer for mesh processing.
A glTF importer such as fastgltf for modern model import.
KTX/Basis ecosystem for GPU-friendly texture pipelines where appropriate.
zstd for general-purpose compression where appropriate.
FreeType and HarfBuzz for text/font processing.
libsodium or equivalent for cryptographic primitives where needed.
Exact versions are pinned when implementation begins and validated through CI.

## 9. Coding Standards

C/C++:
Use explicit ownership.
Prefer RAII.
Avoid hidden global state.
Use strong types for IDs and units.
Keep ABI boundaries narrow.
Do not throw across C ABI boundaries.
Go:
Context propagation is mandatory for network and storage calls.
Errors are explicit.
Avoid package-level mutable state.
TypeScript:
Strict compiler settings.
Runtime validation for external input.
Kotlin:
Lifecycle-safe platform integration.
All code:
No silent failure.
No unchecked user input.
No direct provider-specific assumptions in product modules.

## 10. Logging and Diagnostics

Every executable gets structured logging.
Every request crossing a network boundary carries a request or correlation identifier.
Logs must support severity, subsystem, PNV, build ID, and timestamp.
User-facing applications expose safe diagnostics while internal services retain deeper diagnostics.
Secrets must never be logged.
Binary payload logging must be opt-in and redacted by default.

## 11. PNV Versioning

PNV is the public numeric version identity.
PNV is not a cryptographic hash.
PNV is monotonic within its version registry and never reused.
Build IDs identify an exact generated artifact.
Content hashes remain internal integrity/cache identifiers.
PNV applies to engine, Mute, Studio, PokoX, authority, PKX formats, games, packages, plugins, and other released artifacts as appropriate.
Compatibility checks compare PNV ranges or minimum supported values.
Rollback creates a new release state referencing an older artifact; it does not rewrite release history.

## 12. PNV Tooling

Create a PNV registry service or registry module.
Create PNV allocation CLI.
Create build metadata embedding.
Create artifact manifest generation.
Create compatibility validation.
Create release comparison tooling.
Create Studio display of current PNV.
Create diagnostics collection of application PNV, engine PNV, Mute PNV, game PNV, and package PNV.
Test monotonic allocation, duplicate prevention, concurrent allocation, and rollback behavior.

## 13. PKX Architecture

PKX is a structured package format with a header, manifest, version metadata, content index, dependency information, integrity metadata, and payload sections.
PKX must support random-access or section-level loading where useful.
PKX must support compressed payloads.
PKX must support streaming metadata.
PKX must remain provider-independent.
Client and Authority packages are different build products from one project.
Studio project data is editable source and is not the same thing as a published runtime package.

## 14. Client PKX

Client PKX contains client-required runtime content:
client and shared Mute representation
world and scene data
visual assets
audio
UI content
animation
collision data
streaming metadata
rendering metadata
dependencies
integrity metadata
Client PKX must exclude authority-only logic and data whenever possible.

## 15. Authority PKX

Authority PKX contains:
authority Mute representation
game rules
authority actions
request schemas
replication metadata
object and item definitions
collision proxies
compact world verification information
game constants
integrity metadata
It must not contain unnecessary client presentation assets.
It is loaded by Game Authority, not Main Backend.

## 16. PKX Build Pipeline

Source Project
-> validation
-> dependency resolution
-> Mute compilation
-> client/authority capability analysis
-> asset processing
-> world baking
-> collision/nav/LOD generation
-> package assembly
-> compression
-> integrity/signature
-> PNV assignment
-> artifact upload
-> release registration
Every stage emits machine-readable diagnostics.
A later stage must not silently recover from a failed required earlier stage.

## 17. Core Engine Goals

Poko Engine is a scalable runtime/editor foundation.
Default workloads must remain cheap.
Advanced features are opt-in or adaptive.
The engine must be usable by both PokoX and Poko Studio.
Engine APIs are creator-facing abstractions, not wrappers that leak external library types.

## 18. Engine Core Modules

Implement:
core memory
core handles and IDs
core time
core events/signals
core jobs
core configuration
core logging
core serialization
runtime object system
runtime property system
runtime component system
world/scene
resource management
platform layer
diagnostics
profiler hooks

## 19. Instance Model

Instance is the creator-facing object abstraction.
Instance fields:
stable internal ID
class/type
name
parent
children
properties
attributes
tags
lifecycle state
Instance hierarchy is separate from heavy data storage.
Runtime storage may use compact component/archetype-like layouts.
Do not expose ECS terminology as a requirement to game creators.

## 20. Property Metadata

Each engine class exposes machine-readable metadata:
property name
type
default
flags
serialization behavior
replication behavior
editor behavior
read/write permissions
documentation
validation
This metadata drives:
C++ bindings
Mute types
Studio Properties
autocomplete
serialization
replication
documentation

## 21. Attribute and Tag Systems

Attributes are flexible developer-defined data attached to instances.
Tags classify instances across systems.
Tag queries must avoid repeated hierarchy scans where a cached index can be maintained.
Attribute values must be serializable and size-bounded where they can enter network or persistent paths.

## 22. Component System

Components are internal composition units and optionally creator-visible advanced objects.
Common built-in components include:
Transform
Renderable
Physics
Animation
Audio
Network
Health
Inventory
Interactable
Trigger
Vehicle
Quest
Team
Avoid a component explosion by requiring evidence for new built-ins.

## 23. World and Scene

World contains the runtime hierarchy.
Support:
static geometry
dynamic instances
streamable regions
runtime-only instances
client-only instances
authority-only definitions
World loading must be incremental.
Scene serialization must retain stable object references.

## 24. Object Lifetime

Lifecycle states:
Created
Attached
Active
Dormant
Destroyed
Do not access destroyed instances.
Use generation-aware handles internally to prevent stale references.
Object destruction must unsubscribe engine-owned connections.
Pools may recycle memory only after all references are invalidated.

## 25. Memory Strategy

Define memory domains:
persistent engine
frame temporary
asset
VM
network
physics
rendering
Studio transient
Use pools/arenas where allocation patterns justify them.
Track allocation source and subsystem.
Support memory pressure callbacks.
Implement eviction policies for streaming resources.

## 26. Job System

Create a Poko job graph with:
workers
dependencies
priorities
deadlines
resource conflict metadata
thread-affinity rules
The scheduler must not create a thread per object, NPC, connection, or script.
Integrate Jolt workload scheduling into the shared scheduler where feasible.

## 27. Platform Layer

Moby-owned platform abstraction supports Windows 10+ and Android 11+ initially.
Expose:
window/lifecycle
input
timers
threading primitives
files in approved engine locations
display information
clipboard/text input where needed
controller support
thermal/memory signals where available
Platform-specific code lives under platform modules and does not leak upward.

## 28. Windows Platform

Implement Windows platform layer using stable OS APIs.
Handle:
window creation
focus/minimize
keyboard
mouse
raw input/HID where appropriate
gamepad input path
high-resolution timing
display enumeration
fullscreen/windowed modes
file paths/cache paths
crash handling hooks

## 29. Android Platform

Implement Android platform layer using NDK/GameActivity-oriented integration.
Handle:
lifecycle
surface creation/destruction
touch input
text input
controller input
activity pause/resume
memory pressure signals
thermal signals where available
app storage/cache locations
orientation/resolution changes

## 30. Input System

Poko Input is proprietary.
Inputs normalize to actions.
Actions support:
keyboard
mouse
touch
gamepad
virtual controls
Bindings are data-driven and rebindable.
Studio provides action binding configuration.
Game code should not depend directly on Windows or Android input APIs.

## 31. Rendering Architecture

Poko Renderer owns:
render graph
scene visibility
material model
lighting
shadows
LOD
culling
instancing
streaming
post-processing
terrain
water
VFX
Diligent is below this layer.
Renderer must support quality tiers and device scaling.

## 32. Rendering Pipeline

World
-> visibility
-> culling
-> LOD
-> material grouping
-> render graph
-> shadow passes
-> opaque/transparency
-> post processing
-> UI composition
-> presentation
Keep debug overlays independently toggleable.

## 33. Rendering Quality

Automatic and manual modes.
Manual:
Very Low
Low
Medium
High
Very High
Custom
Dynamic mode adjusts expensive options based on measured workload and device state.
Do not change all quality settings at once unless an emergency thermal or memory condition requires it.

## 34. Graphics Optimization

Implement and benchmark:
frustum culling
occlusion culling
distance culling
mesh LOD
texture streaming
shadow LOD
instancing
batching
material reuse
shader/pipeline caching
dynamic resolution
GPU-driven techniques later
Every optimization must have profiling evidence and regression tests where possible.

## 35. Shader System

Shaders are engine-managed assets.
Compile/cache before gameplay when possible.
Generate variants deliberately to avoid combinatorial explosion.
Persist shader cache where platform allows.
Use fallback shaders for unsupported paths.
Failure to compile an optional shader must not crash the game.

## 36. Material System

Materials expose high-level properties:
base color
metallic
roughness
normal
emissive
opacity
and advanced options.
Material instances must reuse shared immutable definitions where possible.
Material editing is available in Studio.

## 37. Physics Architecture

Poko Physics -> Jolt Adapter -> Jolt.
Poko owns the creator-facing API, character controller, collision policy, mobile policy, networking interaction, debugging, and optimization policy.
Jolt remains an implementation foundation.

## 38. Physics Modes

Static:
cheap world collision.
Kinematic:
developer-controlled moving collision.
Dynamic:
full rigid-body simulation.
Character:
dedicated Poko controller using collision queries and appropriate physics support.
Do not create active dynamic bodies for ordinary static map parts.

## 39. Physics Optimization

Implement:
sleeping
collision filtering
broadphase configuration
physics LOD
distance-based activation
solver quality
budgeting
batched raycasts/shape casts
shared job scheduling
simplified distant representations

## 40. Animation System

Implement:
skeletons
clips
animator
state graphs
blending
layers
IK
retargeting
events
compression
LOD
GPU skinning where appropriate
Animation evaluation frequency should adapt by importance and visibility.

## 41. Audio System

Create Poko Audio abstraction.
Support:
2D
3D spatial
distance attenuation
groups
effects
streaming
music
voice
virtualization
Use shared resource caches.
Do not process inaudible sounds unnecessarily.

## 42. Game UI Engine

Game UI is distinct from application UI.
Game UI is an Instance-based runtime system.
Core objects:
ScreenGui
Canvas
Frame
ScrollingFrame
TextLabel
TextButton
ImageLabel
ImageButton
TextBox
ViewportFrame
Layout is property/system driven for normal workflows.
Integrated behavior is supported alongside ordinary LocalScript usage.

## 43. UI Layout

Support:
absolute layout
scale layout
anchors
constraints
vertical/horizontal layout
grid-like positioning as a property
padding
gap
alignment
safe area
aspect constraints
responsive profiles
Avoid unnecessary dedicated layout instances in the common workflow.

## 44. UI Rendering Optimization

Implement:
dirty layout tracking
partial updates
layout caching
batched rendering
text caching
texture atlases where appropriate
virtualized lists
clipping
Do not rebuild an entire UI tree for one changed property.

## 45. Networking Layer

PokoNet provides:
connection
packet
channel
BitBuffer
serializer
replication
prediction
interpolation
interest management
Network code must support binary high-frequency traffic without JSON on the hot path.

## 46. Network Reliability Classes

Support:
reliable ordered
reliable unordered
unreliable
unreliable sequenced
Use the lowest reliable class that satisfies semantics.
Document channel selection for each engine feature.

## 47. Network Compression

Use:
bit packing
quantization
delta encoding
compact identifiers
batched requests
state compression
optional general compression for payloads that justify it
Do not compress tiny packets when compression overhead exceeds savings.

## 48. Interest Management

Determine relevant entities based on:
distance
visibility
gameplay importance
player relevance
object activity
Send only necessary state.
Interest management is shared between Game Authority replication policy and client runtime.

## 49. Client Prediction

Use prediction for responsive mechanics where authority reconciliation is practical.
Implement:
input sequence numbers
prediction buffers
server acknowledgements
correction/reconciliation
interpolation
Do not apply prediction to operations that are inherently transactional such as account purchases.

## 50. Game State Boundaries

Client owns presentation and local simulation.
Game Authority owns authoritative session state.
Main Backend owns durable platform state.
Never substitute one layer for another.
The engine must provide APIs that make the correct ownership boundary obvious.

## 51. Mute Language Mission

Mute is Poko's game programming language.
It is Lua/Luau-inspired but Poko-native.
Goals:
easy onboarding
fast diagnostics
fast incremental compilation
low runtime overhead
safe sandboxing
strong engine integration
advanced development without forcing advanced syntax on beginners

## 52. Mute Syntax Baseline

Implement:
local
const
functions
methods
if/elseif/else
for
while
repeat
break
continue
return
tables
operators
compound assignment
string interpolation
optional typed declarations
Keep syntax compact and familiar.

## 53. Mute Type System

Core:
nil
bool
number
int
float
string
Engine:
Vector2
Vector3
Vector4
CFrame
Color
UDim
UDim2
Rect
Runtime:
Instance
Signal
Task
Function
Enum
Collections:
Array<T>
Map<K,V>
Set<T>
Advanced:
nullable
unions
generics
interfaces
type aliases
classes
components

## 54. Mute Type Modes

Provide project/script type modes:
Relaxed
Checked
Strict
Type inference is the default.
Strict mode is intended for large games, libraries, and frameworks.
Compiler errors must distinguish type errors from runtime warnings.

## 55. Mute Collections

Provide specialized Array, Map, and Set implementations.
Support indexing where sensible.
Provide common operations without requiring verbose utility libraries.
Specialize common element types in the runtime when compiler information permits.
Avoid allocation-heavy functional helpers in hot loops unless optimized.

## 56. Mute Functions and Closures

Functions are first-class values.
Closures are supported.
Methods use self semantics.
Default parameters are supported.
Named arguments may be added to engine APIs where useful.
Function metadata must preserve source locations for debugging and profiling.

## 57. Mute Signals

Signal API:
Connect
Once
Disconnect
Fire for developer-created signals
Engine events use the same abstraction where practical.
Connection cleanup is automatic with object lifetime.
Signal dispatch must avoid per-event temporary allocations where possible.

## 58. Mute Modules

ModuleScript is the creator-facing reusable code unit.
require-style loading is retained.
Modules may export values, functions, classes, and types.
Compilation caches must avoid recompiling unchanged dependencies.
Dependency cycles must produce clear diagnostics.

## 59. Mute Classes and Interfaces

Classes are advanced, not mandatory.
Support:
fields
methods
constructors
limited inheritance
interfaces
composition/components
Prefer shallow inheritance.
Compiler diagnostics must clearly indicate interface mismatch.

## 60. Mute Async

Support task.wait, task.spawn, task.delay, and cancellation.
Future async/await syntax is supported if semantics remain simple.
Await must suspend logical work without blocking an OS thread.
Task cancellation must be safe and observable where relevant.

## 61. Mute Sandbox

Normal game code cannot:
load arbitrary OS files
launch processes
load native libraries
open arbitrary sockets
access native memory
execute native machine code
Game APIs are capability-controlled.
Plugin scripts receive a separate Studio permission model.

## 62. Mute Contexts

Compiler/runtime contexts:
CLIENT
SERVER
SHARED
STUDIO
PLUGIN
APIs are tagged with required capabilities.
Invalid calls should fail at compile time whenever statically provable.
Runtime capability checks remain for dynamic cases.

## 63. Mute Engine API

Public API groups:
Core
Objects
Players
Characters
Input
UI
World
Physics
Rendering
Animation
Audio
Camera
Network
Authority
Data
Assets
Packages
Tasks
Testing
Debugging
Profiling
Studio
Plugins
The API should be generated from shared metadata where practical.

## 64. Mute UI API

Support direct Instance/property access:
button.Text
button.Visible
button.Position
button.Clicked:Connect(...)
Integrated UI behaviors in Studio generate regular Mute code and remain interoperable with LocalScript.

## 65. Mute Input API

Action-oriented API:
Input.Action(name)
Actions expose pressed, released, changed, and value semantics as appropriate.
Bindings are managed outside gameplay code.

## 66. Mute Physics API

High-level:
PhysicsMode
Mass
Friction
Restitution
Damping
Collision groups
Raycast
ShapeCast
Advanced controls are available without exposing Jolt types.

## 67. Mute Networking API

Beginner:
high-level remote/event/request interfaces.
Advanced:
Buffer
Packet
Channel
Serializer
Replication controls
Authority request interfaces
Make secure server/authority APIs easier to use than ad-hoc client trust.

## 68. Mute Data API

Game data APIs are authority/server mediated.
Client code must not directly mutate durable storage.
Provide:
Get
Set/patch
save semantics
batch operations
error handling
migration hooks

## 69. Mute Compiler Pipeline

Source
-> incremental lexer
-> parser
-> AST
-> name resolution
-> type checking
-> capability analysis
-> authority analysis
-> HIR
-> optimization
-> target compilation
Targets:
client bytecode
authority IR/specialized bytecode
Studio tooling representation where necessary.

## 70. Mute Incremental Compiler

Maintain dependency graphs.
Cache parsed units.
Cache semantic analysis.
Recompile only affected units.
Invalidate caches by source/dependency metadata, not by full-project rebuild.
Provide deterministic compiler outputs for identical inputs.

## 71. Mute Optimizer

Implement:
constant folding
constant propagation
dead-code elimination
control-flow simplification
type specialization
collection specialization
fast property access
fast method dispatch
call-site optimization
source-level location preservation
Keep an always-correct generic path.

## 72. Mute Runtime Optimization

Runtime priorities:
compact values
minimal boxing
allocation avoidance
specialized arrays/maps
cached property access
cached method lookup
native engine calls
efficient signals
incremental/generational GC strategy
safe fallback paths
Profile before introducing advanced runtime machinery.

## 73. Mute Performance Requirement

Performance must be measured using workload benchmarks rather than source-line count alone.
Include:
compile time
incremental rebuild time
startup load time
instruction throughput
property access
collection operations
signal dispatch
native API calls
serialization
authority request processing
allocation rate
GC time
Benchmark representative 1,500-line, 10,000-line, and larger projects.

## 74. Mute Diagnostics

Diagnostics need:
stable error IDs
severity
file
line
column
primary message
related spans
suggestion/fix information where safe
Compiler must recover from syntax errors and avoid cascaded nonsense.

## 75. Mute Profiling

Every compiled function retains enough source metadata to map runtime cost back to source.
Studio profiler must show:
file
function
line
calls
time
allocations where measurable
This metadata may be stripped or compacted for release builds while retaining production-safe diagnostic identifiers.

## 76. Mute Testing

Provide a language-level test harness.
Support:
unit tests
expectations
module tests
authority validation tests
serialization tests
compile-fail tests
golden diagnostics
bytecode determinism checks
runtime regression benchmarks

## 77. Engine/Meta Registry

Create a single authoritative metadata registry for engine classes and APIs.
Registry feeds:
C++ runtime bindings
Mute declarations
Studio Explorer/Properties
autocomplete
documentation
serialization
replication
capability checks
This reduces duplicated definitions and drift.

## 78. Studio Architecture

Poko Studio is a desktop creator application using the real Poko Engine runtime.
Studio mode adds:
selection
gizmos
undo/redo
asset tools
debugging
profiling
editing
publishing
Do not create a fake renderer or fake physics runtime for Studio.

## 79. Studio Application UI

Application UI is separate from game UI.
Visual direction:
familiar to Roblox Studio users
cleaner layout
modern typography/icons
better docking
better search
better command palette
better shortcuts
reduced clutter
consistent property editing
Responsive desktop layout where practical.

## 80. Studio Explorer

Explorer represents the Poko Data Model.
Required:
tree hierarchy
search
filtering
multi-select
drag/drop
rename
reparent
duplicate
delete
insert object
favorites/pinning if useful
Stable selection state.

## 81. Studio Properties

Properties panel is metadata-driven.
Support:
search
editing
enum selection
numeric editing
vectors
colors
asset references
attributes
events
component views
advanced property visibility
Changes integrate with undo/redo.

## 82. Studio Undo/Redo

Use a command/transaction system.
Transactions group logically related edits.
Undo must restore:
properties
hierarchy
assets references
script edits where supported
component changes
Studio-generated objects
Large asset imports may use snapshot references rather than copying all binary data repeatedly.

## 83. Studio Viewport

Implement:
camera controls
move/rotate/scale tools
pivot editing
snap
align
selection
gizmos
visibility toggles
wireframe
collision debug
physics debug
lighting preview
performance overlays

## 84. Studio Insert Object

Searchable insert menu.
Object creation uses engine metadata.
If an object requires default children or default configuration, construction is handled by a factory rather than ad-hoc UI code.

## 85. Studio Mute Editor

Provide:
syntax highlighting
autocomplete
type diagnostics
formatting
go-to-definition
find references
rename
debugging
profiling
documentation
client/server context awareness
Generated event handler scaffolding

## 86. Studio Integrated UI Behavior

Selecting a UI object exposes events and integrated behavior.
Add handler generates normal Mute source.
Property edits update the structured object model and source representation.
Runtime/design-time state must remain distinct.

## 87. Studio Asset Browser

Searchable categories:
meshes
textures
materials
animations
audio
VFX
UI resources
packages
plugins
libraries
games/projects
Asset browser must display moderation/compatibility state when relevant.

## 88. Studio Asset Import

Import pipeline supports validation, optimization, LOD creation, collision generation, texture processing, animation processing, and asset metadata generation.
Source files remain editable source assets.
Runtime packages use optimized derived assets.

## 89. Studio Animation Editor

Provide timeline, keyframes, skeleton/bone selection, IK tools, animation events, blend preview, export/bake operations.
Use the same runtime animation evaluator used by PokoX where possible.

## 90. Studio Material Editor

Provide visual and property-based material editing.
Materials are stored as Poko-native resources.
Validate unsupported features before publishing.
Preview material under multiple lighting conditions.

## 91. Studio VFX Editor

Provide particle, trail, beam, and effect authoring.
Preview performance estimates.
Keep high-cost settings visible.
Provide mobile preview modes.

## 92. Studio Audio Editor

Provide waveform/property editing and runtime preview.
Support spatialized preview.
Show approximate memory/streaming behavior.

## 93. Studio Physics Tools

Provide:
physics mode
mass/density
friction/restitution
collision groups
constraint editing
raycast visualization
body/sleeping visualization
physics budget
Jolt debug information through Poko abstractions

## 94. Studio Network Tools

Local test tooling displays:
connections
messages
channels
requests
replication
latency simulation
packet loss simulation
payload sizes
request rate
Do not require the developer to inspect raw sockets for normal debugging.

## 95. Studio Profiler

Unified profiler:
CPU
GPU
Mute
physics
rendering
network
memory
animation
audio
UI
streaming
Provide frame timeline and per-function/per-system drilldown.

## 96. Studio Test Modes

Implement:
Play
Run
Play Here
Client
Server
local multiplayer
server restart
client reconnect
latency/loss simulation
Old/new game version test where release tooling supports it.

## 97. Studio Device Preview

Preview:
Windows
Android phone
Android tablet
portrait
landscape
touch
keyboard/mouse
gamepad
different resolutions/aspect ratios
Preview does not guarantee perfect hardware emulation; real-device testing remains required.

## 98. Studio Publishing

Publish pipeline:
validate
compile Mute
build client PKX
build authority PKX
process assets
generate manifests
assign PNV
integrity/sign
upload
register game version
switch release state
All publishing steps must be resumable or safely restartable.

## 99. Studio Packages and Plugins

Provide package and plugin management.
Packages may contain code, assets, instances, components, configuration.
Plugins operate in Studio context under explicit capabilities.
Plugin permissions must be reviewable and revocable.

## 100. GitHub Integration

Optional GitHub integration.
Prefer app-based granular permissions.
Support:
repository selection
branch selection
pull
push
commit metadata
history
compare
restore
Avoid pushing generated secrets or provider credentials.

## 101. Developer VPS Integration

Poko Host Agent provides:
authentication
heartbeat
resource reporting
artifact download
process lifecycle
update
logs
crash detection
Host registry exposes a common abstraction for Moby-hosted and developer-owned authority hosts.

## 102. PokoX Architecture

PokoX combines platform shell and Poko Engine runtime.
Subsystems:
launcher
authentication
game discovery
game page
game join
engine runtime
chat
social
marketplace
settings
updates
cache
security
diagnostics

## 103. PokoX Runtime Boot

Launch
-> update check
-> authentication/session restore
-> game metadata when joining
-> package manifest
-> local cache lookup
-> verification
-> critical asset load
-> Authority connection
-> spawn
-> background streaming
The game should become interactive before non-critical assets finish loading when possible.

## 104. PokoX App UI

Application UI is modernized and distinct from game UI.
Core shell:
Home
Servers
Communities
Profile
Home includes game activity, DMs, and groups as appropriate.
Use consistent navigation, search, notifications, and account controls.

## 105. PokoX Game UI Separation

Game UI is rendered by the Poko Engine inside the game presentation layer.
PokoX shell UI must not leak into game UI APIs except through explicit platform interfaces.
Game UI can be themed by the developer without changing the PokoX shell.

## 106. PokoX Graphics

Expose Dynamic and Manual quality.
Dynamic adjusts based on workload.
Manual offers very low through very high plus custom where supported.
Client graphics settings must be bounded by device support and game requirements.

## 107. PokoX Input

PokoX maps platform input into Poko Input actions.
Android touch controls are available to games through UI/input abstractions.
Windows keyboard/mouse/gamepad are normalized.
Store bindings and user preferences safely.

## 108. PokoX Cache

Cache:
game metadata
PKX
assets
thumbnails
shader/pipeline data
compiled Mute
settings where safe
Cache entries are keyed by version/content identity and evictable.
Never trust stale cache as authoritative state.

## 109. PokoX Security

Client is untrusted.
Protect credentials/tokens.
Validate packages.
Keep sensitive secrets out of shipped assets.
Assume a determined attacker can inspect client code/assets.
Rely on Authority/Main Backend for authoritative decisions.

## 110. PokoX Update System

Use staged update artifacts.
Verify before installation.
Use safe replacement/rollback.
Do not destroy a healthy installation before the new version passes verification.
Expose update failures through diagnostics.

## 111. Account Service

Account record:
AccountID
Username
DisplayName
Avatar reference
settings
security state
permissions/capabilities
status
AccountID never changes.
Username uniqueness is enforced server-side.
DisplayName does not need to be unique.

## 112. Authentication

Implement secure credential/session flow.
Use short-lived access credentials.
Support session revocation.
Plan 2FA/recovery.
Never store raw passwords.
Authentication endpoints are rate-limited and audited where appropriate.

## 113. Session Management

Session fields:
session ID
account ID
device/app information
created time
last active
revoked state
refresh metadata
Users can revoke sessions.
Revocation propagates to active services where required.

## 114. Account States

States:
Active
Restricted
Suspended
Banned
PendingDeletion
Deleted
State transitions are auditable.
Restrictions are capability-scoped rather than always total account locks.

## 115. Privacy

Controls:
DM permissions
friend request permissions
invite permissions
join permissions
presence visibility
profile visibility
blocked users
muted users
Settings are divided into local-device and synchronized account settings.

## 116. Permissions

Use capability-based permissions.
Examples:
game.create
game.publish
asset.upload
asset.sell
plugin.publish
moderation.review
admin.system
Do not rely on a single isAdmin flag.

## 117. Bans and Restrictions

Platform actions:
warning
chat restriction
marketplace restriction
upload restriction
developer restriction
temporary suspension
permanent ban
Game owners may issue game-level bans.
Community owners may issue community-level restrictions.
A game ban does not become a platform ban automatically.

## 118. Ban Records

Store:
BanID
AccountID
scope
reason code
human-readable explanation
issuer
created/expiry
evidence references
related report
appeal status
All changes are audited.

## 119. Appeals

Support:
appeal creation
evidence attachment
review assignment
decision
reason
appeal history
A serious moderation action must remain traceable through its complete lifecycle.

## 120. Reports

Report targets:
user
game
asset
message
community
developer
Report includes:
reporter
target
reason
description
evidence
timestamp
status
assignment

## 121. Audit System

Immutable or append-oriented audit records for:
moderation
ownership transfers
economy
marketplace
publishing
permissions
security events
Administrative users cannot silently erase historical action records.

## 122. Social Graph

Entities:
friends
friend requests
blocks
mutes
presence
parties
groups
communities
Store social relationships by AccountID.
Do not use display names as identifiers.

## 123. Communities

Support:
creation
membership
roles
permissions
announcements
chat
moderation
asset/game links
Custom roles are capability-based.

## 124. Parties

Party:
leader
members
invite state
game target
ready state where applicable
Party membership must be synchronized before group join operations.

## 125. Games Data Model

Game:
GameID
OwnerID
current PNV
visibility
content classification
description
thumbnail references
permissions
status
version history
A game record is separate from a game session.

## 126. Game Lifecycle

Draft
Private
Unlisted
Public
Restricted
Archived
Transitions are validated.
Publishing creates a new release artifact and PNV.

## 127. Game Session Data Model

Session:
SessionID
GameID
Game PNV
Authority host
endpoint reference
capacity
player count
status
created time
health
Private/reserved metadata where applicable

## 128. Session Lifecycle

Requested
Provisioning
Starting
Ready
Running
Draining
Stopped
Failed
State transitions are idempotent.
A session may drain old versions while new sessions use a newer PNV.

## 129. Matchmaking

Platform matchmaking can consider:
capacity
party size
game mode
skill/rating where a game uses it
latency
availability
No mandatory public region selector at launch.
Infrastructure is region-aware internally but APAC-first operationally.

## 130. Private/Reserved Servers

Support:
public
private
reserved
invite-only
friends-only
Game developers can enable supported server modes.
Provisioning is independent from game discovery ranking.

## 131. Discovery

Discovery is automatic.
Inputs include:
games played
session duration
replays
favorites
search behavior
game characteristics
genre/features
similar-player behavior
New games receive exploration opportunities.
Discovery is separate from explicit search.

## 132. Search

Indexes:
games
users
communities
assets
packages
plugins
libraries
Provide:
partial matching
typo tolerance
filters
tags
categories
Search queries are intent-driven.

## 133. Developer Project Model

Project ownership is AccountID-based.
Project can have team members and roles.
Project contains:
worlds
assets
source
client
authority
shared
packages
plugins
configuration
Projects must be independently versionable and recoverable.

## 134. Developer Roles

Roles:
Owner
Admin
Developer
Builder
Scripter
Artist
Tester
Viewer
Permissions are explicit and project-scoped.
Transfers require explicit owner confirmation and are audited.

## 135. Persistence

Persistent platform state uses the Main Backend.
Game-specific durable state is backend mediated.
In-memory session state is authoritative only for the active session.
Do not write every gameplay mutation to PostgreSQL individually.
Use change tracking and batching.

## 136. Data Schema Versioning

Every persisted game-data schema can have a schema version independent of PNV.
Migrations are explicit.
Migrations must be deterministic and testable.
Do not overload PNV as a database schema version.

## 137. Transaction Ledger

Transactions need:
TransactionID
actor/source
recipient/system
amount
tax
type
timestamp
reference
Idempotency key
Ledger records are the investigation trail.
Balances are derived/updated from authoritative transaction logic.

## 138. PoKoin Economy Integration

PoKoins are server-authoritative.
Use cases include tasks/rewards, eligible marketplace operations, and hosting priority.
Existing product rules include weekly earning reset, transfer/marketplace taxation, and dynamic demand-based sinks.
Exact economic tuning remains a product configuration concern, not hardcoded logic.

## 139. Marketplace Architecture

There are two user-facing marketplaces:
Studio Marketplace
PokoX Player Marketplace
They share backend ownership/transaction infrastructure but have different catalogs, UI, permissions, and discovery.
Studio Marketplace:
creator assets, packages, plugins, libraries, tools.
Player Marketplace:
avatar items, clothing, accessories, animations, emotes, cosmetics.

## 140. Marketplace Purchase Flow

Purchase
-> authentication
-> authorization
-> item availability
-> price validation
-> idempotency check
-> transaction
-> tax
-> ownership grant
-> notification
-> receipt
No client-side balance or ownership mutation is accepted as truth.

## 141. Achievement System

Each game includes 25 achievement slots.
Additional capacity can be provisioned through the developer monetization/capacity system.
Achievement data includes:
ID
name
description
icon
hidden
points
progress
status
Awarding occurs only from authority/server-side code.

## 142. Leaderboards

Game-owned leaderboards:
global
friends
server
Values are written by authority-side game logic.
Read paths can be cached.
Leaderboard resets must be explicit and auditable where persistent.

## 143. Game Statistics

Game-specific statistics may include:
wins
kills
deaths
playtime
matches
custom counters
Do not confuse game statistics with platform achievements.

## 144. Notifications

Notification categories:
social
games
developer
marketplace
moderation
security
system
In-app first.
Push notifications may be added.
Security-critical notifications should not be suppressible through ordinary preference controls.

## 145. Localization

Platform supports localized strings.
Games can provide translation tables.
UI objects can reference localization keys.
Locale handling is separate from platform account identity.

## 146. Analytics

Platform analytics:
sessions
joins
retention
search
discovery
errors
Developer analytics:
players
session duration
retention
performance
network
crashes
Access must respect ownership and privacy rules.

## 147. Support

Support ticket model:
TicketID
account
category
description
attachments
status
assignment
history
Categories include account, technical, transaction, moderation, developer, and bug.

## 148. Content Moderation Pipeline

Uploaded content:
uploaded
processing
validation
moderation
approved/rejected
published
later restrictions/removal possible
Moderation state must be visible to authorized creator/admin interfaces.

## 149. Asset Ownership

Asset record references:
AssetID
CreatorID
OwnerID
version/PNV as applicable
license
moderation state
dependencies
Filename is not identity.
Ownership changes are audited.

## 150. Package Licensing

Packages must expose:
creator
license
version
dependencies
required engine/mute PNV
permissions
distribution status
Package consumption does not automatically imply player ownership.

## 151. Backup and Recovery

Protect:
critical PostgreSQL data
project metadata
game versions
ownership records
transactions
moderation history
Important binary artifacts in R2 should have versioned references.
Caches remain reconstructable.

## 152. Main Backend HTTP API

Use HTTP/JSON for ordinary platform APIs where practical.
Use binary only where the performance or bandwidth characteristics justify it.
All endpoints have:
authentication policy
authorization policy
validation
rate limits
timeouts
request IDs
structured errors

## 153. API Batch Endpoint

Provide a batch endpoint for independent cheap reads/writes that benefit from coalescing.
The client SDK should batch common startup reads such as profile, settings, notifications, and lightweight metadata.
Each subrequest remains independently authorized and validated.
Do not allow one batch element to bypass permission checks.

## 154. Main Backend Caching

Valkey caches:
session metadata
presence
game server listings
hot game metadata
rate limits
temporary locks
short-lived tokens
Cache invalidation must be event-driven or TTL-based.
Source of truth remains PostgreSQL or the responsible subsystem.

## 155. D1 Usage

Use D1 only for data that benefits from Worker proximity and fits its operational limits.
Examples:
small public metadata
edge configuration
routing hints
feature flags
Do not make D1 an irreplaceable core database.

## 156. R2 Object Layout

Define stable key namespaces:
games/<GameID>/versions/<PNV>/
projects/<ProjectID>/
assets/<AssetID>/<version>/
packages/<PackageID>/<version>/
plugins/<PluginID>/<version>/
authority/<GameID>/<PNV>/
client/<GameID>/<PNV>/
Use manifests rather than directory scans as the primary lookup mechanism.

## 157. Cloudinary Usage

Use Cloudinary for media workloads:
game thumbnails
preview images
short videos
profile media
Use R2 for generic Poko objects.
Do not couple game runtime downloads to Cloudinary-specific APIs unless the asset type requires it.

## 158. Workers Routing

Worker request flow:
receive
basic validation
cache lookup
rate limit
route
simple response or origin delegation
Workers must not become a required proxy for every gameplay WebSocket message.

## 159. Durable Object Chat

Use DO rooms for realtime chat/presence where cost and limits are acceptable.
Connections hibernate where supported.
Room state is ephemeral.
Fallback to Main Backend chat routing is required.
Do not use DO as Game Authority.

## 160. Authority WebSocket Protocol

Use persistent WebSocket connections.
Handshake:
protocol version
Game PNV
Authority session
client identity
capabilities
connection metadata
After handshake, use compact binary frames.
Reject protocol mismatch cleanly.

## 161. Authority Request Registry

Each request type has:
request ID
decode schema
permission/context
validation handler
cost class
response schema
rate limit class
logging policy
The registry should be generated or centrally declared to avoid client/server drift.

## 162. Authority Cost Classes

Classify requests:
Cheap
Normal
Expensive
Investigation
Rate limiting happens before expensive processing.
Unknown or malformed requests are rejected cheaply.

## 163. Authority Session State

Per-session memory contains:
players
mutable authoritative objects
inventory/session state
cooldowns via timestamps
active requests
connection state
version identifiers
Do not duplicate immutable game definitions for every session.

## 164. Authority Shared Definitions

Load immutable definitions once:
item definitions
weapon definitions
schemas
rules
collision proxies
world verification data
replication policy
Static definitions should be shared between sessions.

## 165. Authority Anti-Cheat

Focus on invariant violations:
impossible movement
invalid distance
cooldown violations
invalid item ownership
invalid transaction
invalid request ordering
invalid authority state transition
Do not attempt to prove that the client binary is unmodified.

## 166. Authority Investigation Mode

Suspicious clients may receive temporary deeper validation:
more history
more request correlation
shadow checks
more detailed diagnostics
Do not run expensive checks on every player by default.

## 167. Authority Rate Limiting

Rate limit by:
connection
player
game
request type
host
global service budget
Use token bucket/leaky bucket style mechanisms where appropriate.
Reject early.

## 168. Authority Queue Backpressure

Bound queues.
When capacity is exhausted:
prioritize critical control traffic
drop or degrade low-priority telemetry
apply per-player budgets
protect the process from memory exhaustion

## 169. Authority Persistence Boundary

Do not round-trip PostgreSQL or Valkey for each gameplay request.
Use RAM for active authoritative state.
Persist meaningful durable transitions asynchronously or in controlled batches.
Use backend APIs for transactional platform operations.

## 170. Authority Failover

Authority hosts report health.
Main Backend tracks host/session assignment.
If a host fails:
mark unhealthy
stop new assignments
reconnect or provision replacements
sessions may require controlled recovery depending on persistent state
Do not silently claim a failed session continued without evidence.

## 171. Service Registry

Main Backend service registry tracks:
service
region
endpoint
version
health
capacity
features
maintenance state
Initially APAC-focused.
Architecture remains region-capable.

## 172. Host Scheduling

Scheduler selects hosts based on:
health
capacity
supported PNV
current load
developer priority where applicable
Do not merge discovery ranking with infrastructure provisioning priority.

## 173. PoKoin Hosting Priority

Hosting priority is an infrastructure scheduler feature.
Developers may spend allowed PoKoins to increase provisioning priority where configured.
Apply budgets and diminishing returns to avoid unlimited domination by one account.
Do not promote game discovery ranking through this mechanism.

## 174. Game Update Drain

When a new game PNV is published:
new sessions use the new PNV
old sessions remain pinned
authority drains or terminates old sessions according to policy
Main Backend maintains both version records during the overlap

## 175. Character Runtime Integration

The runtime Character API must expose a canonical structure independent of source/avatar conversion.
Engine features must target the Poko canonical character rather than external rig formats.
Character creation, animation, physics, and networking consume the same canonical interfaces.
Detailed conversion rules are in conversion.md.

## 176. Avatar Runtime Separation

Avatar is persistent customization state.
Character is the active runtime instance.
Avatar -> character build/spawn pipeline.
Do not store runtime physics or animation state in the persistent avatar record.

## 177. Game-Side Common Systems

Provide built-in primitives/components for:
health
damage
inventory
interactable
trigger
team
spawn
vehicle
dialogue
quest
shop
These cover common UGC patterns without preventing custom implementations.

## 178. Common Gameplay Component Rules

Built-in gameplay components must:
serialize safely
replicate according to metadata
expose Mute APIs
work in local Studio tests
respect authority boundaries
provide reasonable default performance
avoid hidden database traffic

## 179. Camera System

Support:
third person
first person
top down
fixed
scripted
custom
Camera is primarily client-side.
Authority may validate gameplay actions affected by camera-independent conditions.

## 180. Save System

Game state:
change tracking
save queue
batch persistence
retry
player-leave flush
periodic autosave
schema migration
Do not make save latency part of the gameplay hot path unless explicitly required by a transactional mechanic.

## 181. Runtime Testing Harness

Build local testing utilities that can instantiate:
engine-only runtime
client runtime
local authority
main backend mock
fake storage
fake network latency
This allows subsystem testing without the full platform.

## 182. Determinism Requirements

Determinism is required only where the subsystem contract requires it.
Authority validation functions should avoid uncontrolled nondeterminism.
Client presentation may be nondeterministic.
Tests must declare whether deterministic behavior is expected.

## 183. Security Boundaries

Security boundaries:
client < authority
authority < main backend for platform transactions
plugin < Studio privileged operations
game script < OS
ordinary API consumer < privileged administration
Every boundary must have authentication, authorization, input validation, and audit requirements as appropriate.

## 184. Secrets Management

Secrets must be injected through environment/provider configuration.
Never commit credentials.
Never embed backend secrets in PKX.
Never ship developer GitHub tokens in PokoX.
Rotate credentials without changing product code.

## 185. Network Security

Use TLS for HTTP and WebSocket transport.
Validate certificates according to platform policy.
Use short-lived auth credentials.
Apply replay protection to sensitive actions.
Use idempotency for transactional requests.
Validate payload lengths before decoding large structures.

## 186. Package Security

Verify PKX integrity and signature metadata where used.
Reject malformed manifests.
Enforce package size and dependency limits.
Prevent package path traversal.
Do not execute arbitrary native content from packages.

## 187. Plugin Security

Plugins are more privileged than game scripts.
Define capability permissions.
Show permissions during installation.
Allow disable/revocation.
Sandbox where practical.
Plugins must not silently gain new capabilities after update without review/approval policy.

## 188. Crash Handling

Every application captures safe crash context.
Include:
application PNV
engine PNV
Mute PNV where applicable
game PNV
device/OS
memory summary
thread/context
safe logs
Do not include secrets or unrestricted user content in crash payloads.

## 189. Diagnostics Transport

Diagnostics are sampled/rate-limited.
During active incidents, raise diagnostic detail through feature configuration.
Never make diagnostic volume capable of taking down the service.

## 190. Resource Budgets

Each runtime gets budgets:
CPU
GPU
memory
network
physics
script
UI
streaming
Budgets are advisory where hard enforcement would damage correctness and enforceable where safety requires it.

## 191. Dynamic Performance Policy

Dynamic performance responds to:
frame time
CPU/GPU workload
memory pressure
thermal state
device capability
Adjust expensive systems gradually.
Keep user-selected manual settings where possible unless safety requires reduction.

## 192. Mobile Optimization

Prioritize:
startup time
memory use
texture compression
shader variant reduction
asset streaming
dynamic resolution
thermal behavior
touch latency
battery-aware policies
low-memory recovery

## 193. Windows Optimization

Prioritize:
high refresh rates
controller/keyboard/mouse support
multiple display configurations
higher graphics scalability
fast SSD asset caches
advanced quality settings

## 194. Backend Optimization

Main Backend:
batch database operations
cache hot data
avoid redundant serialization
use bounded workers
separate background analytics from critical APIs
Game Authority:
zero/minimal allocation hot paths
binary decode
session-local RAM
specialized Mute execution
backpressure

## 195. Vibe Coding Rules

AI coding agents must:
read local module contracts
preserve architecture
add tests with behavior changes
avoid broad rewrites
prefer small compilable increments
update interface documentation when APIs change
never introduce hidden provider coupling
mark TODOs with subsystem/task IDs
never claim a benchmark passed without actually running it

## 196. Task ID Convention

Task IDs use prefixes:
CORE
ENG
REN
PHY
ANI
AUD
UI
NET
MUTE
PKX
AUTH
BACK
WRK
STU
POKOX
MOD
SOC
ACC
ECO
MKT
DISC
OPS
SEC
TEST
REL
Each task gets a numeric ID, e.g. ENG-001.

## 197. Definition of Done

A task is complete only when:
implementation exists
tests exist or rationale documented
error paths are covered
logs/diagnostics are appropriate
performance impact is measured if hot path
API/docs updated
build passes
relevant integration path passes
no unresolved security issue remains for the touched boundary

## 198. Core Milestone A — Foundation

Deliver:
build system
logging
PNV tooling
core IDs/handles
memory primitives
time
events
job scheduler skeleton
basic serialization
CI
Milestone acceptance:
engine library builds on Windows and Android toolchain targets
core tests pass
PNV tool allocates unique values

## 199. Core Milestone B — Instance Runtime

Deliver:
Instance
hierarchy
properties
attributes
tags
components
world
lifetime
serialization
Milestone acceptance:
construct/save/load/modify/destroy object trees
property metadata drives reflection and tests

## 200. Core Milestone C — Platform

Deliver:
Windows platform
Android platform
window/surface lifecycle
input
filesystem abstraction
timing
Milestone acceptance:
native window/surface opens
input actions reach engine
lifecycle pause/resume works

## 201. Core Milestone D — Rendering

Deliver:
Diligent integration
resource creation
shaders
materials
camera
mesh
render graph
lighting baseline
Milestone acceptance:
basic world renders on Windows and supported Android devices

## 202. Core Milestone E — Physics

Deliver:
Jolt integration
static/kinematic/dynamic
queries
constraints
character controller
Milestone acceptance:
basic physics scene works with profiling and collision debug.

## 203. Core Milestone F — UI/Audio/Animation

Deliver:
game UI
layout
animation
audio
input-to-action flow
Milestone acceptance:
character moves, animates, UI responds, spatial audio works.

## 204. Core Milestone G — Networking

Deliver:
BitBuffer
packet/channel model
replication
prediction/interpolation primitives
local Authority
Milestone acceptance:
two local clients connect and exchange authoritative requests/state.

## 205. Core Milestone H — Mute

Deliver:
lexer/parser
types
modules
VM
native engine calls
compiler diagnostics
client/authority targets
Milestone acceptance:
sample games compile and execute on PokoX/local Authority.

## 206. Core Milestone I — PKX

Deliver:
manifest
client build
authority build
integrity
compression
streaming metadata
Milestone acceptance:
publish/build/load pipeline works end-to-end.

## 207. Core Milestone J — Studio

Deliver:
Explorer
Properties
Viewport
Mute editor
Playtest
debugger
profiler baseline
publish
Milestone acceptance:
developer can create a project, insert objects, script them, play, debug, and publish.

## 208. Core Milestone K — PokoX

Deliver:
account shell
game discovery
game page
game join
runtime
settings
chat/social baseline
update system
Milestone acceptance:
player can authenticate, discover a game, join, play, leave, and return.

## 209. Core Milestone L — Platform

Deliver:
accounts
social
games
sessions
ownership
economy
marketplaces
achievements
moderation
notifications
analytics
Milestone acceptance:
end-to-end platform flows work using test accounts and test games.

## 210. Core Milestone M — Hardening

Deliver:
load tests
failure tests
security review
provider failure simulation
backup/restore tests
rollback tests
mobile soak tests
Milestone acceptance:
no critical unresolved defects in defined launch gates.

## 211. Engine Task Backlog — Core

ENG-001 Core ID types
ENG-002 Handle generation
ENG-003 Generation counters
ENG-004 Property metadata schema
ENG-005 Instance base
ENG-006 Parent/child operations
ENG-007 Object lifetime manager
ENG-008 Attributes
ENG-009 Tags
ENG-010 Components
ENG-011 Signals
ENG-012 Job scheduler
ENG-013 Memory telemetry
ENG-014 Serializer
ENG-015 Runtime configuration
ENG-016 Diagnostics registry

## 212. Engine Task Backlog — Platform

ENG-017 Windows window lifecycle
ENG-018 Windows input
ENG-019 Windows controller
ENG-020 Android surface lifecycle
ENG-021 Android touch
ENG-022 Android text input
ENG-023 Android controller
ENG-024 Device profile
ENG-025 Thermal/memory signals
ENG-026 platform file paths
ENG-027 monotonic clock
ENG-028 crash hooks

## 213. Engine Task Backlog — Rendering

REN-001 graphics device wrapper
REN-002 swapchain
REN-003 resource lifetime
REN-004 shader compiler integration
REN-005 pipeline cache
REN-006 material registry
REN-007 render graph
REN-008 camera
REN-009 mesh
REN-010 texture
REN-011 PBR baseline
REN-012 lights
REN-013 shadows
REN-014 culling
REN-015 LOD
REN-016 instancing
REN-017 dynamic resolution
REN-018 post processing
REN-019 debug rendering
REN-020 render profiler

## 214. Engine Task Backlog — Physics

PHY-001 Jolt build integration
PHY-002 adapter types
PHY-003 body handle mapping
PHY-004 static bodies
PHY-005 kinematic bodies
PHY-006 dynamic bodies
PHY-007 colliders
PHY-008 collision filters
PHY-009 raycast
PHY-010 shapecast
PHY-011 constraints
PHY-012 sleeping
PHY-013 physics budget
PHY-014 character controller
PHY-015 physics debug
PHY-016 physics profiling

## 215. Engine Task Backlog — UI/Audio/Animation

UI-001 root canvas
UI-002 frame
UI-003 text
UI-004 buttons
UI-005 images
UI-006 layout properties
UI-007 dirty layout
UI-008 event routing
UI-009 text shaping
UI-010 accessibility scale
ANI-001 skeleton
ANI-002 clip
ANI-003 animator
ANI-004 blend graph
ANI-005 IK
ANI-006 retargeting
AUD-001 device output
AUD-002 sound
AUD-003 3D emitter
AUD-004 streaming
AUD-005 groups

## 216. Network Task Backlog

NET-001 BitBuffer
NET-002 packet header
NET-003 reliable channel
NET-004 unreliable channel
NET-005 sequencing
NET-006 serialization registry
NET-007 batching
NET-008 delta compression
NET-009 quantization
NET-010 replication graph
NET-011 interest manager
NET-012 prediction
NET-013 interpolation
NET-014 network profiler

## 217. Mute Task Backlog — Front End

MUTE-001 lexer
MUTE-002 tokens
MUTE-003 parser
MUTE-004 AST
MUTE-005 source spans
MUTE-006 name resolver
MUTE-007 type checker
MUTE-008 diagnostics
MUTE-009 formatter
MUTE-010 module resolver
MUTE-011 capability analysis
MUTE-012 authority analysis

## 218. Mute Task Backlog — Runtime

MUTE-013 HIR
MUTE-014 constant folding
MUTE-015 dead code elimination
MUTE-016 specialization
MUTE-017 bytecode format
MUTE-018 bytecode loader
MUTE-019 VM
MUTE-020 closures
MUTE-021 signals
MUTE-022 task scheduler bridge
MUTE-023 native calls
MUTE-024 GC
MUTE-025 runtime errors
MUTE-026 profiler hooks
MUTE-027 authority IR target

## 219. PKX Task Backlog

PKX-001 header
PKX-002 manifest
PKX-003 dependency index
PKX-004 section table
PKX-005 compression
PKX-006 integrity metadata
PKX-007 client package builder
PKX-008 authority package builder
PKX-009 cache validation
PKX-010 streaming manifest
PKX-011 compatibility checker
PKX-012 package test corpus

## 220. Authority Task Backlog

AUTH-001 WebSocket server
AUTH-002 handshake
AUTH-003 connection manager
AUTH-004 session registry
AUTH-005 request registry
AUTH-006 binary decode
AUTH-007 rate limiting
AUTH-008 backpressure
AUTH-009 validation pipeline
AUTH-010 Mute runtime integration
AUTH-011 authority state store
AUTH-012 replication output
AUTH-013 suspicion escalation
AUTH-014 diagnostics
AUTH-015 load-test harness

## 221. Backend Task Backlog

BACK-001 API server
BACK-002 identity
BACK-003 account
BACK-004 session
BACK-005 permissions
BACK-006 games
BACK-007 versions
BACK-008 sessions
BACK-009 social
BACK-010 groups
BACK-011 communities
BACK-012 notifications
BACK-013 inventory
BACK-014 ownership
BACK-015 economy
BACK-016 marketplace
BACK-017 achievements
BACK-018 reports
BACK-019 moderation
BACK-020 analytics
BACK-021 service registry
BACK-022 batch API

## 222. Backend Storage Backlog

BACK-023 PostgreSQL migrations
BACK-024 Valkey key conventions
BACK-025 D1 schema
BACK-026 R2 namespace
BACK-027 Cloudinary media metadata
BACK-028 transaction ledger
BACK-029 idempotency table/store
BACK-030 audit log store
BACK-031 backup workflow
BACK-032 restore verification

## 223. Worker/DO Backlog

WRK-001 routing
WRK-002 cache
WRK-003 rate limit
WRK-004 signed URL
WRK-005 origin failover
WRK-006 service health
WRK-007 batch endpoint
WRK-008 chat room DO
WRK-009 presence
WRK-010 chat fallback

## 224. Studio Task Backlog

STU-001 application shell
STU-002 docking
STU-003 Explorer
STU-004 Properties
STU-005 command palette
STU-006 viewport
STU-007 gizmos
STU-008 undo/redo
STU-009 Mute editor
STU-010 debugger
STU-011 profiler
STU-012 UI editor
STU-013 animation editor
STU-014 material editor
STU-015 VFX editor
STU-016 audio editor
STU-017 physics tools
STU-018 network tools
STU-019 package manager
STU-020 publishing

## 225. PokoX Task Backlog

POKOX-001 launcher
POKOX-002 updater
POKOX-003 auth
POKOX-004 Home
POKOX-005 game page
POKOX-006 discovery feed
POKOX-007 search
POKOX-008 game join
POKOX-009 runtime shell
POKOX-010 chat
POKOX-011 social
POKOX-012 marketplace
POKOX-013 settings
POKOX-014 cache
POKOX-015 diagnostics

## 226. Moderation Task Backlog

MOD-001 dashboard
MOD-002 reports
MOD-003 user view
MOD-004 game view
MOD-005 asset view
MOD-006 ban/restriction actions
MOD-007 appeals
MOD-008 audit view
MOD-009 permissions
MOD-010 case assignment
MOD-011 evidence

## 227. Platform Operations

Define operational runbooks for:
service outage
provider quota exhaustion
Authority host failure
database failure
cache failure
package corruption
bad game release
bad engine release
security incident
marketplace incident
moderation incident
Each runbook must include detection, containment, recovery, verification, and post-incident record.

## 228. Load Testing

Load-test Main Backend separately from Game Authority.
Authority tests:
connections
messages/sec
request batches
session count
memory/session
CPU/request
Main Backend tests:
HTTP RPS
DB queries
cache hit rate
batch endpoint
transaction throughput
Do not use synthetic numbers as claims; record actual benchmark results.

## 229. Failure Testing

Inject:
network loss
latency
packet loss
provider timeout
provider quota exhaustion
database restart
cache restart
Authority crash
client reconnect
invalid PKX
invalid Mute
bad migration
partial upload
Each failure mode must have expected behavior and an automated test where practical.

## 230. Compatibility Testing

Test:
engine version compatibility
Mute version compatibility
PKX version compatibility
package compatibility
plugin compatibility
game rollback
old sessions during new release
minimum supported Android devices
minimum supported Windows configuration

## 231. Performance Gates

No optimization is accepted only because it sounds faster.
Performance gates must specify:
workload
hardware
build
baseline
new result
variance
pass threshold
Automated benchmark regressions should fail CI for critical hot paths.

## 232. Security Gates

Before launch:
threat model
auth review
permission review
package verification review
plugin review
network protocol fuzzing
input validation fuzzing
economy transaction testing
moderation authorization testing
secret scanning
dependency security review

## 233. Release Channels

Support:
development
canary
beta
stable
Each channel has PNV and artifact metadata.
Production rollback must identify an already validated artifact.

## 234. Release Checklist

Before release:
all required tests green
PNV allocated
release notes
migration plan
rollback plan
artifact integrity
dependencies recorded
crash symbols stored where applicable
monitoring ready
support notes ready
moderation tools ready

## 235. Project Naming

Use consistent names:
Poko Engine
PokoX
Poko Studio
Poko Moderation
Mute
PokoNet
PKX
PNV
Game Authority
Main Backend
Avoid introducing alternate names in code unless required for external compatibility.

## 236. API Stability

Public creator APIs require compatibility policy.
Internal APIs can change faster.
Mark:
experimental
stable
deprecated
removed
Studio should flag deprecated APIs during development.

## 237. Deprecation Process

Deprecation:
announce
document replacement
keep compatibility period
emit diagnostic
remove only at planned boundary
Maintain migration tooling for common changes.

## 238. Documentation

Every stable engine/Mute/backend API needs:
purpose
signature
examples
context
permissions
performance notes
failure behavior
version introduced
version deprecated if relevant

## 239. Generated Documentation

Where possible generate:
Mute API declarations
Studio Properties docs
autocomplete metadata
reference documentation
serialization metadata
replication metadata
from the same source registry.

## 240. AI-Agent Interface Contracts

Expose machine-readable:
task manifest
module dependency graph
build commands
test commands
API contracts
error code registry
schema definitions
An AI agent should be able to discover how to modify a subsystem without relying on prose alone.

## 241. AI-Agent Change Rules

Agents must:
touch the smallest module set
run nearest tests first
run impacted integration tests next
avoid changing stable interfaces without explicit task scope
record migration notes for schema changes
avoid deleting failing tests instead of fixing behavior

## 242. Schema Migration Workflow

Schema change:
draft migration
update model
update read/write paths
test old -> new
test rollback where supported
test mixed-version deployment where applicable
deploy migration
verify
remove compatibility code later

## 243. Data Consistency

Transactions are required for multi-record invariants.
Use idempotency for retried external requests.
Cache invalidation follows successful source-of-truth writes.
Do not update derived cache before a durable write succeeds unless the cache can safely be invalidated on failure.

## 244. Observability

Metrics:
request count
error count
latency
CPU
memory
GC
cache hits
DB latency
queue depth
WebSocket connections
messages
bytes
game sessions
active scripts
Frame time
Metrics must be tagged by stable low-cardinality identifiers.

## 245. Alerting

Alerts should cover:
service down
error spike
latency spike
queue saturation
memory pressure
database health
Authority connection saturation
invalid package surge
moderation backlog
marketplace anomaly
Avoid high-cardinality alert dimensions.

## 246. Quota Management

Track provider quotas and internal budgets.
Maintain health states:
GREEN
YELLOW
ORANGE
RED
EMERGENCY
Reserve capacity for critical operations.

## 247. Provider Replacement

Every external provider integration has:
interface
adapter
configuration
health check
fallback behavior
migration notes
Do not embed provider SDK calls throughout business logic.

## 248. Database Access Layer

Use repositories or query modules with clear transaction boundaries.
Prevent arbitrary SQL from request handlers.
Centralize migrations.
Test query plans on hot paths.
Use bulk operations where appropriate.

## 249. Cache Key Conventions

Key format includes:
namespace
object identity
version where needed
Use bounded TTLs.
Never let untrusted user input directly become a giant unbounded key.

## 250. R2 Upload Security

Uploads use signed/time-limited URLs.
Backend controls:
object key
content type
size
expiration
ownership
After upload, validate and register the object.
Do not trust client-supplied ownership metadata.

## 251. Asset Pipeline Security

Validate:
file type
size
container structure
resource counts
embedded paths
compression bombs
malformed model structures
shader/material constraints
Audio/image metadata
Reject unsafe or unsupported content before derived processing.

## 252. Game Build Validation

Before publication:
dependency closure
Mute compile
API compatibility
asset references
broken links
memory/size budget warnings
authority/client separation
package integrity
content classification metadata

## 253. Studio Publish Dry Run

Provide a dry-run mode:
compile
validate
build manifests
estimate package sizes
show warnings
do not upload/publish
This should catch most release failures before the developer commits a public version.

## 254. Client Startup Budget

Track:
process start to UI
UI to game metadata
metadata to critical assets
critical assets to playable
Log each milestone.
Optimize startup after measuring representative projects.

## 255. Game Join Budget

Join flow tracks:
matchmaking
session allocation
network handshake
PKX verification
critical asset load
initial spawn
Use progress diagnostics and timeouts.
Retry only safe/idempotent operations.

## 256. Streaming Strategy

World streaming uses:
manifest
region/asset dependency graph
priority
distance
visibility
gameplay importance
cache
eviction
Do not load all optional content before play when unnecessary.

## 257. Asset Residency

Track:
disk cached
memory resident
GPU resident
streaming requested
streaming complete
evictable
Do not retain unbounded GPU resources.

## 258. LOD Strategy

LOD applies to:
mesh
texture
animation
shadow
physics
VFX
The Poko Engine may independently choose LOD for each subsystem.
Creator overrides exist for advanced use.

## 259. Job Scheduling Policy

Prefer coarse useful jobs over thousands of tiny tasks.
Use system dependencies.
Measure worker contention.
Avoid synchronization on frame-critical hot paths when data partitioning can remove it.

## 260. Frame Budgeting

Target budgets vary by frame rate.
For 60Hz target, total budget is approximately 16.67ms.
For 120Hz target, approximately 8.33ms.
Actual engine policy should measure subsystem time and adapt rather than assume a fixed split.

## 261. Script Budgeting

Track per-script and per-task CPU usage.
Provide warnings when a script repeatedly dominates the frame.
Game code should be allowed to be complex, but runaway loops and pathological execution require runtime protection.

## 262. Physics Budgeting

Track active bodies, constraints, queries, and physics time.
Provide warnings before severe degradation.
Allow developers to choose quality where appropriate.

## 263. Network Budgeting

Track:
bytes/sec
packets/sec
requests/sec
replication rate
per-player cost
per-game cost
Use batching and interest management before increasing infrastructure.

## 264. Memory Budgeting

Track subsystem allocations.
Provide:
soft budget
warning
pressure
eviction
hard safety limits only where required

## 265. Mobile Thermal Policy

When thermal pressure increases:
reduce dynamic quality
reduce expensive post processing
reduce animation/physics work where safe
avoid abrupt changes
restore gradually
Record policy decisions for diagnostics without sending excessive telemetry.

## 266. Game Authority Memory Policy

Prefer RAM for immutable definitions and hot session state when CPU savings justify it.
Avoid duplicated immutable definitions.
Use compact structs.
Reuse request buffers.
Track per-session memory.
Set global and per-session memory ceilings.

## 267. Game Authority CPU Policy

Avoid:
per-request database access
per-frame world simulation
large temporary allocations
reflection-heavy hot paths
generic serialization for every common request
Prefer:
precomputed lookup tables
binary schemas
specialized validation
batched processing
timestamp comparisons

## 268. Main Backend CPU Policy

Keep handlers small.
Move expensive work to background workers or precomputed caches when safe.
Use database batching.
Avoid repeated JSON encode/decode inside internal layers.
Keep analytics and notifications from starving critical transactional endpoints.

## 269. Security Rate Limits

Define default limits for:
login
password reset
friend requests
messages
uploads
publishing
marketplace transactions
achievement awards
game requests
authority requests
Limits are configurable per environment and backed by quotas.

## 270. Abuse Detection

Signals can include:
frequency
burst patterns
invalid request ratio
repeated failures
transaction anomalies
malformed packages
suspicious account linkage
Do not automatically treat one weak signal as proof of abuse.

## 271. Content Ownership Transfer

Game/project/asset ownership transfers require:
source owner confirmation
target acceptance
permission validation
audit record
cache invalidation
notification
Rollback handling only through formal admin/support procedures.

## 272. Account Deletion

Deletion flow:
request
confirmation
pending period
recovery
deletion/anonymization
Retain only what is necessary under applicable policy and legal requirements.

## 273. Data Export

Support export of user-owned platform data where appropriate.
Export is generated asynchronously.
Downloads are temporary and authenticated.
Do not include secrets or other users' private information.

## 274. Social Safety

Block and mute operations must take effect on:
messages
invites
friend requests
presence visibility
party interactions
where applicable
Caches must invalidate promptly.

## 275. Chat Architecture

Platform chat and game chat are logically distinct.
Platform chat:
DMs
groups
communities
Game chat:
session-scoped
moderated
reconnectable
Both share safety infrastructure but not necessarily the same room lifecycle.

## 276. Notification Reliability

Important notifications have durable records.
Non-critical notifications may be ephemeral.
Client fetches unread state from Main Backend.
Push delivery failure must not imply the notification was lost.

## 277. Game Developer Analytics Privacy

Developers only access analytics for games/projects they are authorized to view.
Do not expose unrelated user-sensitive platform data.
Aggregate where individual-level data is unnecessary.

## 278. Discovery Data Pipeline

Game profile generation combines:
developer metadata
observed gameplay characteristics
behavioral signals
moderation/content state
Discovery candidate generation is separate from final ranking.
Keep the initial system simple enough to debug.

## 279. Discovery Cold Start

New players:
broad popular/new content
safe categories
early behavioral learning
New games:
exploration allocation
quality/safety filtering
Do not require pre-existing popularity to become visible.

## 280. Discovery Separation

Discovery ranking and hosting priority are separate systems.
PoKoin spending for server provisioning must never directly become a user-facing recommendation guarantee.

## 281. Marketplace Separation

Studio Marketplace and PokoX Marketplace are distinct products backed by shared transaction/ownership infrastructure.
Do not expose developer-only packages as player purchases by accident.
Do not expose player cosmetics as Studio dependencies unless explicitly designed.

## 282. Achievement Capacity

Default:
25 achievements per game.
Additional capacity is an explicit developer resource.
Capacity changes must be atomic.
Existing achievement records survive capacity changes.
Deletion of an achievement must have defined handling for player history.

## 283. Achievement API Security

Only authority/server code may award.
Validate achievement ID exists and belongs to the game.
Validate eligibility/progress.
Use idempotency so duplicate awards do not duplicate state.

## 284. Leaderboard Integrity

Write operations are authority-side.
Use monotonic or explicit reset rules.
Prevent client score spoofing.
Cache reads where appropriate.
Audit abnormal write patterns where useful.

## 285. Avatar and Character Boundary

Persistent avatar owns configuration.
Character runtime owns transforms, physics, animator state, health, and other dynamic data.
Do not persist runtime physics state as avatar customization.

## 286. Character API Stability

All games target the canonical Character API.
Source rig differences must be hidden after conversion.
Animation and physics systems consume canonical skeleton/controller interfaces.
Detailed conversion rules are maintained separately.

## 287. Platform Feature Flags

Feature flags can gate:
new discovery behavior
new marketplace UI
new engine features
new Studio tools
new backend paths
Flags must have:
owner
purpose
rollout state
expiry/review date
fallback

## 288. Emergency Disable

Critical systems must support safe disable/degraded mode.
Examples:
marketplace purchases
new publishing
new package installs
chat
specific VFX feature
specific provider
Emergency disable must be audited and reversible.

## 289. Maintenance Mode

Support platform-wide and subsystem maintenance states.
PokoX should receive explicit maintenance responses rather than hanging.
Studio should preserve unsaved work locally where possible before service failure.

## 290. Local Autosave in Studio

Studio should autosave project metadata and recent editing state to a local recovery area.
Autosave must not replace explicit publishing/versioning.
Recovery prompts must distinguish:
unsaved local work
published game version
source control state

## 291. Studio Crash Recovery

After a crash:
detect recoverable session
offer restore
preserve original project
log recovery source
Do not silently overwrite the project with a recovery snapshot.

## 292. PokoX Crash Recovery

After client crash:
restore safe local settings
attempt session reconnect only if the game session still exists
never assume gameplay transaction success based on local UI state

## 293. Authority Crash Recovery

Authority crash detection:
host heartbeat failure
session state transitions
persistent data checkpoint where available
reconnect policy
new session provisioning
Do not duplicate reward/transaction state after recovery.

## 294. Publish Rollback

Rollback:
choose prior validated artifact
create new active release record using PNV
route new sessions
drain current incompatible sessions
retain history

## 295. Engine Update Compatibility

Engine updates must define:
API changes
serialization changes
PKX changes
Mute changes
migration requirements
backward-compatibility window
Games that cannot run under a new engine should be prevented from launching rather than partially loaded.

## 296. Mute Update Compatibility

Mute language/runtime updates must define:
syntax changes
type changes
bytecode changes
standard library changes
breaking API changes
Compiler emits compatibility diagnostics.

## 297. Third-Party Library Update Compatibility

Before updating Jolt/Diligent/etc.:
run adapter tests
run engine tests
run performance benchmarks
run platform tests
record upstream revision
record local patches
Only then promote the dependency revision.

## 298. Library Fork Policy

Fork only when:
upstream behavior blocks a required feature/fix
upstream response time is incompatible with project needs
security or correctness requires immediate patch
Maintain a small patch queue.
Rebase periodically.
Document divergence.

## 299. Third-Party License Compliance

Track license and notices for each dependency.
Ship required notices.
Do not remove upstream attribution.
CI should verify license metadata is present for release builds.

## 300. Test Pyramid

Unit tests:
fast logic
Integration tests:
subsystem contracts
End-to-end:
real application flows
Load tests:
capacity
Soak tests:
stability
Security/fuzz:
untrusted input
Benchmark:
performance regressions
Use the smallest test layer that proves the requirement.

## 301. Golden Test Artifacts

Maintain known-good:
Mute compiler outputs
diagnostics
PKX packages
network packets
serialization data
API responses
Keep fixtures minimal and versioned.
Update golden files only with intentional semantic changes.

## 302. Protocol Fuzzing

Fuzz:
BitBuffer
packet decoders
PKX parser
asset metadata parser
Mute parser
Authority request decoder
Use timeouts and memory limits.
A malformed packet must not crash the process.

## 303. Memory Safety Testing

Use sanitizers and dynamic tools in development CI where practical.
Run:
ASan
UBSan
thread checks
fuzzing
allocation tracking
Pay extra attention to native engine boundaries and custom allocators.

## 304. Android Test Matrix

Track representative:
low-memory device
mid-range device
high-end device
different GPU families
different Android versions within supported range
Real-device testing is required before launch.

## 305. Windows Test Matrix

Track:
supported Windows versions
integrated GPU
mid-range GPU
high-end GPU
keyboard/mouse
controller
high refresh display
windowed/fullscreen

## 306. Performance Optimization Backlog

Later, after profiling:
GPU-driven rendering
more advanced culling
additional scheduler optimization
SIMD hot paths
faster Mute hot-code optimization/JIT investigation
advanced physics LOD
asset residency optimization
network packet coalescing
database query plan tuning

## 307. Optimization Rule

No subsystem may optimize by silently changing gameplay semantics.
All fast paths require a correct fallback.
Performance improvements must be benchmarked against a baseline.

## 308. API Design Rule

Prefer:
simple beginner API
advanced escape hatch
strong validation
good diagnostics
same underlying runtime
Do not require advanced developers to use internal implementation types.

## 309. Architecture Review Triggers

Require architecture review when:
adding a persistent service
changing PNV semantics
changing PKX format
changing Mute execution contexts
changing public engine APIs
changing authority trust boundaries
adding a third platform
adding a new external provider

## 310. Documentation Structure

Maintain:
architecture overview
engine API reference
Mute language spec
PKX format spec
PNV spec
authority protocol spec
backend API spec
Studio extension spec
moderation model
deployment/runbooks
developer onboarding

## 311. Milestone Exit Criteria

A milestone exits only when:
acceptance tests pass
known blockers are documented
operational metrics exist
backward compatibility impact is understood
the next milestone's dependencies are available

## 312. Priority Levels

P0:
security, corruption, data loss, crash, broken release.
P1:
core functionality blockers.
P2:
quality/performance.
P3:
convenience/future optimization.
Do not allow P3 features to block P0/P1 delivery.

## 313. Risk Register

Primary risks:
scope explosion
free-provider instability
2-vCPU Authority capacity
Mute compiler complexity
Studio complexity
mobile optimization
security/abuse workload
marketplace/moderation workload
third-party dependency drift
incomplete tooling
Each risk needs:
trigger
impact
mitigation
owner
fallback

## 314. Highest-Risk Engineering Areas

The highest-risk areas are:
Mute compiler/runtime
Game Authority correctness/security
Poko Studio scope
mobile rendering/performance
PKX build pipeline
identity/security
marketplace/economy correctness
These receive early prototypes and tests.

## 315. Prototype-First Areas

Prototype before committing to production architecture:
Mute bytecode representation
authority Mute execution
Poko Instance/property metadata
Diligent render abstraction
Jolt adapter
PKX section loading
Studio object/code synchronization
Character conversion pipeline

## 316. Prototype Exit Criteria

Prototype must prove:
API ergonomics
performance feasibility
integration viability
failure behavior
testability
Only after this does it become a production contract.

## 317. Definition of Launch-Ready

Poko is launch-ready when:
a user can create an account
install/update PokoX
discover and play a game
developers can create/test/publish
Authority can host/validate sessions
platform state persists safely
moderation can operate the system
rollback exists
monitoring/backup exists
critical security tests pass

## 318. Final Architecture Summary

Poko is:
a familiar UGC data model
a scalable Poko Engine
Mute as the game language
PokoX as the player runtime
Poko Studio as the creator environment
Game Authority as lightweight authoritative validation
Main Backend as persistent control plane
PKX as separated runtime packages
PNV as numeric release identity
APAC-first replaceable infrastructure
The implementation should preserve these boundaries from the first production commit.

## CORE-001: Instance creation and destruction

Status: Planned
Subsystem: CORE
Implementation goal:
Implement instance creation and destruction as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful instance creation and destruction behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-002: parent/child mutation

Status: Planned
Subsystem: CORE
Implementation goal:
Implement parent/child mutation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful parent/child mutation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-003: property metadata

Status: Planned
Subsystem: CORE
Implementation goal:
Implement property metadata as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful property metadata behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-004: attribute serialization

Status: Planned
Subsystem: CORE
Implementation goal:
Implement attribute serialization as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful attribute serialization behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-005: tag indexing

Status: Planned
Subsystem: CORE
Implementation goal:
Implement tag indexing as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful tag indexing behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-006: generation-safe handles

Status: Planned
Subsystem: CORE
Implementation goal:
Implement generation-safe handles as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful generation-safe handles behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-007: component attach/detach

Status: Planned
Subsystem: CORE
Implementation goal:
Implement component attach/detach as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful component attach/detach behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-008: signal connection lifecycle

Status: Planned
Subsystem: CORE
Implementation goal:
Implement signal connection lifecycle as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful signal connection lifecycle behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-009: job dependency graph

Status: Planned
Subsystem: CORE
Implementation goal:
Implement job dependency graph as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful job dependency graph behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-010: frame temporary allocator

Status: Planned
Subsystem: CORE
Implementation goal:
Implement frame temporary allocator as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful frame temporary allocator behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-011: asset allocator

Status: Planned
Subsystem: CORE
Implementation goal:
Implement asset allocator as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful asset allocator behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-012: diagnostic event registry

Status: Planned
Subsystem: CORE
Implementation goal:
Implement diagnostic event registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful diagnostic event registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-013: runtime configuration loading

Status: Planned
Subsystem: CORE
Implementation goal:
Implement runtime configuration loading as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful runtime configuration loading behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-014: schema-driven serialization

Status: Planned
Subsystem: CORE
Implementation goal:
Implement schema-driven serialization as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful schema-driven serialization behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-015: world load/save

Status: Planned
Subsystem: CORE
Implementation goal:
Implement world load/save as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful world load/save behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## CORE-016: object reference resolution

Status: Planned
Subsystem: CORE
Implementation goal:
Implement object reference resolution as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful object reference resolution behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-017: graphics device initialization

Status: Planned
Subsystem: REN
Implementation goal:
Implement graphics device initialization as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful graphics device initialization behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-018: swapchain lifecycle

Status: Planned
Subsystem: REN
Implementation goal:
Implement swapchain lifecycle as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful swapchain lifecycle behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-019: shader compilation cache

Status: Planned
Subsystem: REN
Implementation goal:
Implement shader compilation cache as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful shader compilation cache behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-020: pipeline cache

Status: Planned
Subsystem: REN
Implementation goal:
Implement pipeline cache as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful pipeline cache behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-021: material registry

Status: Planned
Subsystem: REN
Implementation goal:
Implement material registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful material registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-022: mesh resource lifetime

Status: Planned
Subsystem: REN
Implementation goal:
Implement mesh resource lifetime as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful mesh resource lifetime behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-023: texture residency

Status: Planned
Subsystem: REN
Implementation goal:
Implement texture residency as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful texture residency behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-024: render graph pass scheduling

Status: Planned
Subsystem: REN
Implementation goal:
Implement render graph pass scheduling as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful render graph pass scheduling behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-025: camera system

Status: Planned
Subsystem: REN
Implementation goal:
Implement camera system as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful camera system behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-026: frustum culling

Status: Planned
Subsystem: REN
Implementation goal:
Implement frustum culling as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful frustum culling behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-027: occlusion culling

Status: Planned
Subsystem: REN
Implementation goal:
Implement occlusion culling as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful occlusion culling behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-028: mesh LOD

Status: Planned
Subsystem: REN
Implementation goal:
Implement mesh lod as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful mesh lod behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-029: shadow LOD

Status: Planned
Subsystem: REN
Implementation goal:
Implement shadow lod as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful shadow lod behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-030: instancing

Status: Planned
Subsystem: REN
Implementation goal:
Implement instancing as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful instancing behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-031: dynamic resolution

Status: Planned
Subsystem: REN
Implementation goal:
Implement dynamic resolution as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful dynamic resolution behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-032: post-processing

Status: Planned
Subsystem: REN
Implementation goal:
Implement post-processing as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful post-processing behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-033: debug render overlays

Status: Planned
Subsystem: REN
Implementation goal:
Implement debug render overlays as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful debug render overlays behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## REN-034: GPU profiling

Status: Planned
Subsystem: REN
Implementation goal:
Implement gpu profiling as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful gpu profiling behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-035: Jolt adapter bootstrap

Status: Planned
Subsystem: PHY
Implementation goal:
Implement jolt adapter bootstrap as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful jolt adapter bootstrap behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-036: static body creation

Status: Planned
Subsystem: PHY
Implementation goal:
Implement static body creation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful static body creation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-037: kinematic body updates

Status: Planned
Subsystem: PHY
Implementation goal:
Implement kinematic body updates as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful kinematic body updates behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-038: dynamic body lifecycle

Status: Planned
Subsystem: PHY
Implementation goal:
Implement dynamic body lifecycle as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful dynamic body lifecycle behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-039: collision groups

Status: Planned
Subsystem: PHY
Implementation goal:
Implement collision groups as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful collision groups behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-040: raycast batching

Status: Planned
Subsystem: PHY
Implementation goal:
Implement raycast batching as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful raycast batching behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-041: shape cast batching

Status: Planned
Subsystem: PHY
Implementation goal:
Implement shape cast batching as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful shape cast batching behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-042: constraint registry

Status: Planned
Subsystem: PHY
Implementation goal:
Implement constraint registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful constraint registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-043: sleep state tracking

Status: Planned
Subsystem: PHY
Implementation goal:
Implement sleep state tracking as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful sleep state tracking behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-044: physics budgets

Status: Planned
Subsystem: PHY
Implementation goal:
Implement physics budgets as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful physics budgets behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-045: character controller

Status: Planned
Subsystem: PHY
Implementation goal:
Implement character controller as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful character controller behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-046: physics debug draw

Status: Planned
Subsystem: PHY
Implementation goal:
Implement physics debug draw as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful physics debug draw behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-047: physics profiling

Status: Planned
Subsystem: PHY
Implementation goal:
Implement physics profiling as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful physics profiling behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PHY-048: distance-based deactivation

Status: Planned
Subsystem: PHY
Implementation goal:
Implement distance-based deactivation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful distance-based deactivation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-049: BitBuffer encoding

Status: Planned
Subsystem: NET
Implementation goal:
Implement bitbuffer encoding as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful bitbuffer encoding behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-050: BitBuffer decoding

Status: Planned
Subsystem: NET
Implementation goal:
Implement bitbuffer decoding as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful bitbuffer decoding behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-051: packet framing

Status: Planned
Subsystem: NET
Implementation goal:
Implement packet framing as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful packet framing behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-052: reliable ordered channel

Status: Planned
Subsystem: NET
Implementation goal:
Implement reliable ordered channel as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful reliable ordered channel behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-053: reliable unordered channel

Status: Planned
Subsystem: NET
Implementation goal:
Implement reliable unordered channel as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful reliable unordered channel behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-054: unreliable channel

Status: Planned
Subsystem: NET
Implementation goal:
Implement unreliable channel as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful unreliable channel behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-055: unreliable sequenced channel

Status: Planned
Subsystem: NET
Implementation goal:
Implement unreliable sequenced channel as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful unreliable sequenced channel behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-056: request batching

Status: Planned
Subsystem: NET
Implementation goal:
Implement request batching as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful request batching behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-057: delta state encoding

Status: Planned
Subsystem: NET
Implementation goal:
Implement delta state encoding as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful delta state encoding behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-058: quantized transform encoding

Status: Planned
Subsystem: NET
Implementation goal:
Implement quantized transform encoding as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful quantized transform encoding behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-059: interest management

Status: Planned
Subsystem: NET
Implementation goal:
Implement interest management as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful interest management behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-060: prediction buffer

Status: Planned
Subsystem: NET
Implementation goal:
Implement prediction buffer as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful prediction buffer behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-061: reconciliation

Status: Planned
Subsystem: NET
Implementation goal:
Implement reconciliation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful reconciliation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## NET-062: network diagnostics

Status: Planned
Subsystem: NET
Implementation goal:
Implement network diagnostics as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful network diagnostics behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-063: lexer tokenization

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement lexer tokenization as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful lexer tokenization behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-064: parser recovery

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement parser recovery as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful parser recovery behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-065: AST source spans

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement ast source spans as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful ast source spans behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-066: name resolution

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement name resolution as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful name resolution behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-067: type inference

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement type inference as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful type inference behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-068: nullable type checking

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement nullable type checking as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful nullable type checking behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-069: union narrowing

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement union narrowing as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful union narrowing behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-070: generic checking

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement generic checking as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful generic checking behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-071: module dependency graph

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement module dependency graph as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful module dependency graph behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-072: capability analysis

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement capability analysis as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful capability analysis behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-073: authority analysis

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement authority analysis as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful authority analysis behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-074: HIR lowering

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement hir lowering as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful hir lowering behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-075: constant folding

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement constant folding as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful constant folding behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-076: dead code elimination

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement dead code elimination as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful dead code elimination behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-077: collection specialization

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement collection specialization as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful collection specialization behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-078: property cache

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement property cache as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful property cache behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-079: method dispatch cache

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement method dispatch cache as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful method dispatch cache behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-080: bytecode emission

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement bytecode emission as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful bytecode emission behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-081: VM call frames

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement vm call frames as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful vm call frames behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-082: closures

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement closures as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful closures behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-083: GC

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement gc as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful gc behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-084: native API binding

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement native api binding as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful native api binding behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-085: task scheduler bridge

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement task scheduler bridge as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful task scheduler bridge behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-086: runtime diagnostics

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement runtime diagnostics as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful runtime diagnostics behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MUTE-087: profiler mapping

Status: Planned
Subsystem: MUTE
Implementation goal:
Implement profiler mapping as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful profiler mapping behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-088: PKX header parser

Status: Planned
Subsystem: PKX
Implementation goal:
Implement pkx header parser as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful pkx header parser behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-089: PKX manifest parser

Status: Planned
Subsystem: PKX
Implementation goal:
Implement pkx manifest parser as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful pkx manifest parser behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-090: dependency resolution

Status: Planned
Subsystem: PKX
Implementation goal:
Implement dependency resolution as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful dependency resolution behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-091: section table loading

Status: Planned
Subsystem: PKX
Implementation goal:
Implement section table loading as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful section table loading behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-092: compression layer

Status: Planned
Subsystem: PKX
Implementation goal:
Implement compression layer as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful compression layer behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-093: integrity verification

Status: Planned
Subsystem: PKX
Implementation goal:
Implement integrity verification as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful integrity verification behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-094: client package builder

Status: Planned
Subsystem: PKX
Implementation goal:
Implement client package builder as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful client package builder behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-095: authority package builder

Status: Planned
Subsystem: PKX
Implementation goal:
Implement authority package builder as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful authority package builder behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-096: streaming manifest

Status: Planned
Subsystem: PKX
Implementation goal:
Implement streaming manifest as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful streaming manifest behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-097: cache validation

Status: Planned
Subsystem: PKX
Implementation goal:
Implement cache validation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful cache validation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-098: compatibility checks

Status: Planned
Subsystem: PKX
Implementation goal:
Implement compatibility checks as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful compatibility checks behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## PKX-099: corrupt-package recovery

Status: Planned
Subsystem: PKX
Implementation goal:
Implement corrupt-package recovery as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful corrupt-package recovery behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-100: WebSocket accept loop

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement websocket accept loop as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful websocket accept loop behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-101: handshake validation

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement handshake validation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful handshake validation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-102: connection registry

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement connection registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful connection registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-103: session registry

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement session registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful session registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-104: request schema registry

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement request schema registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful request schema registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-105: binary request decode

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement binary request decode as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful binary request decode behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-106: cheap validation path

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement cheap validation path as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful cheap validation path behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-107: dynamic validation path

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement dynamic validation path as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful dynamic validation path behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-108: investigation path

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement investigation path as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful investigation path behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-109: authority Mute invocation

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement authority mute invocation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful authority mute invocation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-110: state mutation

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement state mutation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful state mutation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-111: replication event generation

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement replication event generation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful replication event generation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-112: rate limiting

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement rate limiting as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful rate limiting behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-113: backpressure

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement backpressure as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful backpressure behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-114: session memory accounting

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement session memory accounting as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful session memory accounting behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## AUTH-115: host health reporting

Status: Planned
Subsystem: AUTH
Implementation goal:
Implement host health reporting as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful host health reporting behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-116: identity service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement identity service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful identity service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-117: account service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement account service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful account service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-118: session service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement session service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful session service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-119: permission service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement permission service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful permission service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-120: game registry

Status: Planned
Subsystem: BACK
Implementation goal:
Implement game registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful game registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-121: game version registry

Status: Planned
Subsystem: BACK
Implementation goal:
Implement game version registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful game version registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-122: session scheduler

Status: Planned
Subsystem: BACK
Implementation goal:
Implement session scheduler as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful session scheduler behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-123: social graph

Status: Planned
Subsystem: BACK
Implementation goal:
Implement social graph as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful social graph behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-124: community service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement community service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful community service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-125: notification service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement notification service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful notification service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-126: inventory service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement inventory service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful inventory service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-127: ownership service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement ownership service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful ownership service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-128: achievement service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement achievement service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful achievement service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-129: leaderboard service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement leaderboard service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful leaderboard service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-130: economy ledger

Status: Planned
Subsystem: BACK
Implementation goal:
Implement economy ledger as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful economy ledger behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-131: marketplace service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement marketplace service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful marketplace service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-132: report service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement report service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful report service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-133: moderation service

Status: Planned
Subsystem: BACK
Implementation goal:
Implement moderation service as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful moderation service behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-134: analytics ingestion

Status: Planned
Subsystem: BACK
Implementation goal:
Implement analytics ingestion as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful analytics ingestion behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## BACK-135: service registry

Status: Planned
Subsystem: BACK
Implementation goal:
Implement service registry as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful service registry behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-136: application shell

Status: Planned
Subsystem: STU
Implementation goal:
Implement application shell as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful application shell behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-137: docking layout

Status: Planned
Subsystem: STU
Implementation goal:
Implement docking layout as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful docking layout behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-138: Explorer

Status: Planned
Subsystem: STU
Implementation goal:
Implement explorer as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful explorer behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-139: Properties

Status: Planned
Subsystem: STU
Implementation goal:
Implement properties as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful properties behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-140: object insertion

Status: Planned
Subsystem: STU
Implementation goal:
Implement object insertion as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful object insertion behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-141: undo/redo transactions

Status: Planned
Subsystem: STU
Implementation goal:
Implement undo/redo transactions as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful undo/redo transactions behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-142: viewport selection

Status: Planned
Subsystem: STU
Implementation goal:
Implement viewport selection as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful viewport selection behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-143: gizmos

Status: Planned
Subsystem: STU
Implementation goal:
Implement gizmos as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful gizmos behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-144: Mute editor integration

Status: Planned
Subsystem: STU
Implementation goal:
Implement mute editor integration as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful mute editor integration behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-145: autocomplete integration

Status: Planned
Subsystem: STU
Implementation goal:
Implement autocomplete integration as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful autocomplete integration behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-146: debugger UI

Status: Planned
Subsystem: STU
Implementation goal:
Implement debugger ui as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful debugger ui behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-147: profiler UI

Status: Planned
Subsystem: STU
Implementation goal:
Implement profiler ui as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful profiler ui behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-148: UI editor

Status: Planned
Subsystem: STU
Implementation goal:
Implement ui editor as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful ui editor behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-149: animation editor

Status: Planned
Subsystem: STU
Implementation goal:
Implement animation editor as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful animation editor behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-150: material editor

Status: Planned
Subsystem: STU
Implementation goal:
Implement material editor as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful material editor behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-151: VFX editor

Status: Planned
Subsystem: STU
Implementation goal:
Implement vfx editor as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful vfx editor behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-152: audio editor

Status: Planned
Subsystem: STU
Implementation goal:
Implement audio editor as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful audio editor behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-153: physics tools

Status: Planned
Subsystem: STU
Implementation goal:
Implement physics tools as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful physics tools behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-154: network tools

Status: Planned
Subsystem: STU
Implementation goal:
Implement network tools as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful network tools behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-155: package manager

Status: Planned
Subsystem: STU
Implementation goal:
Implement package manager as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful package manager behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-156: publishing flow

Status: Planned
Subsystem: STU
Implementation goal:
Implement publishing flow as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful publishing flow behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-157: GitHub connector

Status: Planned
Subsystem: STU
Implementation goal:
Implement github connector as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful github connector behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## STU-158: VPS deployment

Status: Planned
Subsystem: STU
Implementation goal:
Implement vps deployment as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful vps deployment behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-159: launcher

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement launcher as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful launcher behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-160: updater

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement updater as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful updater behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-161: authentication UI

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement authentication ui as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful authentication ui behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-162: Home shell

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement home shell as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful home shell behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-163: game search

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement game search as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful game search behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-164: discovery feed

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement discovery feed as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful discovery feed behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-165: game page

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement game page as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful game page behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-166: game join

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement game join as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful game join behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-167: runtime bootstrap

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement runtime bootstrap as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful runtime bootstrap behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-168: game UI layer

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement game ui layer as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful game ui layer behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-169: chat client

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement chat client as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful chat client behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-170: social client

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement social client as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful social client behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-171: player marketplace

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement player marketplace as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful player marketplace behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-172: settings

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement settings as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful settings behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-173: cache

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement cache as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful cache behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-174: diagnostics

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement diagnostics as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful diagnostics behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## POKOX-175: session reconnect

Status: Planned
Subsystem: POKOX
Implementation goal:
Implement session reconnect as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful session reconnect behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-176: moderation dashboard

Status: Planned
Subsystem: MOD
Implementation goal:
Implement moderation dashboard as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful moderation dashboard behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-177: report queue

Status: Planned
Subsystem: MOD
Implementation goal:
Implement report queue as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful report queue behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-178: user case view

Status: Planned
Subsystem: MOD
Implementation goal:
Implement user case view as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful user case view behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-179: game case view

Status: Planned
Subsystem: MOD
Implementation goal:
Implement game case view as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful game case view behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-180: asset case view

Status: Planned
Subsystem: MOD
Implementation goal:
Implement asset case view as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful asset case view behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-181: ban action flow

Status: Planned
Subsystem: MOD
Implementation goal:
Implement ban action flow as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful ban action flow behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-182: restriction action flow

Status: Planned
Subsystem: MOD
Implementation goal:
Implement restriction action flow as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful restriction action flow behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-183: appeal review

Status: Planned
Subsystem: MOD
Implementation goal:
Implement appeal review as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful appeal review behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-184: evidence viewer

Status: Planned
Subsystem: MOD
Implementation goal:
Implement evidence viewer as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful evidence viewer behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-185: audit viewer

Status: Planned
Subsystem: MOD
Implementation goal:
Implement audit viewer as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful audit viewer behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-186: moderator permissions

Status: Planned
Subsystem: MOD
Implementation goal:
Implement moderator permissions as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful moderator permissions behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## MOD-187: case assignment

Status: Planned
Subsystem: MOD
Implementation goal:
Implement case assignment as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful case assignment behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-188: credential handling

Status: Planned
Subsystem: SEC
Implementation goal:
Implement credential handling as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful credential handling behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-189: session revocation

Status: Planned
Subsystem: SEC
Implementation goal:
Implement session revocation as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful session revocation behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-190: rate limiting

Status: Planned
Subsystem: SEC
Implementation goal:
Implement rate limiting as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful rate limiting behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-191: package verification

Status: Planned
Subsystem: SEC
Implementation goal:
Implement package verification as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful package verification behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-192: plugin permissions

Status: Planned
Subsystem: SEC
Implementation goal:
Implement plugin permissions as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful plugin permissions behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-193: API authorization

Status: Planned
Subsystem: SEC
Implementation goal:
Implement api authorization as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful api authorization behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-194: fuzz harness

Status: Planned
Subsystem: SEC
Implementation goal:
Implement fuzz harness as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful fuzz harness behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-195: secret scanning

Status: Planned
Subsystem: SEC
Implementation goal:
Implement secret scanning as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful secret scanning behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-196: audit logging

Status: Planned
Subsystem: SEC
Implementation goal:
Implement audit logging as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful audit logging behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-197: abuse signal pipeline

Status: Planned
Subsystem: SEC
Implementation goal:
Implement abuse signal pipeline as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful abuse signal pipeline behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## SEC-198: transaction idempotency

Status: Planned
Subsystem: SEC
Implementation goal:
Implement transaction idempotency as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful transaction idempotency behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-199: unit test harness

Status: Planned
Subsystem: TEST
Implementation goal:
Implement unit test harness as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful unit test harness behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-200: integration harness

Status: Planned
Subsystem: TEST
Implementation goal:
Implement integration harness as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful integration harness behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-201: end-to-end harness

Status: Planned
Subsystem: TEST
Implementation goal:
Implement end-to-end harness as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful end-to-end harness behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-202: benchmark harness

Status: Planned
Subsystem: TEST
Implementation goal:
Implement benchmark harness as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful benchmark harness behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-203: load test runner

Status: Planned
Subsystem: TEST
Implementation goal:
Implement load test runner as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful load test runner behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-204: soak test runner

Status: Planned
Subsystem: TEST
Implementation goal:
Implement soak test runner as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful soak test runner behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-205: fuzz runner

Status: Planned
Subsystem: TEST
Implementation goal:
Implement fuzz runner as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful fuzz runner behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-206: golden artifact tests

Status: Planned
Subsystem: TEST
Implementation goal:
Implement golden artifact tests as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful golden artifact tests behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-207: compatibility matrix

Status: Planned
Subsystem: TEST
Implementation goal:
Implement compatibility matrix as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful compatibility matrix behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## TEST-208: backup restore verification

Status: Planned
Subsystem: TEST
Implementation goal:
Implement backup restore verification as a small, isolated production unit with a stable interface.
Design constraints:
- Preserve the dependency direction defined in this plan.
- Avoid adding persistent state unless the task explicitly owns it.
- Keep hot-path allocations bounded where applicable.
- Expose structured errors rather than silent fallback.
Dependencies:
Depends on the nearest stable module contract; do not bypass the module boundary.
Required outputs:
- implementation
- unit/integration tests
- diagnostics where applicable
- documentation/update notes for public behavior
Test requirements:
- Test successful backup restore verification behavior.
- Test invalid input and boundary conditions.
- Test repeated invocation and cleanup behavior.
- Test failure propagation and diagnostic output.
Acceptance criteria:
- Builds in all supported target configurations for the subsystem.
- Tests pass without disabling existing coverage.
- Failure behavior matches the subsystem contract.
- No public API is leaked from third-party dependencies.
Performance note:
Benchmark if the task is on a frame, network, serialization, storage, or authority hot path.
Security note:
Treat all external/user-controlled input as untrusted until validated.
Compatibility note:
Record any PNV, schema, PKX, or public API compatibility impact before merge.
Implementation note:
Prefer the smallest change that satisfies the acceptance criteria; defer unrelated cleanup.
Code-agent instruction:
Before modifying files, read the local interface and tests. After modification, run the nearest targeted tests first.

## 400. Final Implementation Order

The coding order is:
1. Repository and CI foundation.
2. Game Authority transport/session skeleton.
3. Main Backend skeleton and storage.
4. Mute compiler prototype and VM.
5. Poko Engine Core and Instance system.
6. Platform/input.
7. Renderer.
8. Physics.
9. Animation/audio/UI.
10. Networking.
11. PKX.
12. Studio.
13. PokoX.
14. Platform services.
15. Character runtime/conversion integration.
16. Marketplace/economy/achievements.
17. Moderation.
18. Discovery.
19. Performance hardening.
20. Security/reliability hardening.
Do not advance a dependent phase merely because a demo works; its contract and tests must exist.


https://github.com/Raft-The-Crab/Poko.git