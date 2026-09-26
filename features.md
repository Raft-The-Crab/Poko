# Poko Feature Catalog

Status: Product/feature baseline
Purpose: Separate user-facing/product feature definition from implementation architecture.
This catalog is intentionally concise enough to be readable while still being implementation-oriented.

## 1. Platform

### PokoX
- account login/session
- Home
- Servers
- Communities
- Profile
- DMs
- groups
- friends
- game discovery
- game search
- game pages
- play/join
- notifications
- chat
- player marketplace
- inventory
- avatar
- settings
- graphics controls
- input settings
- updates
- diagnostics

### Poko Studio
- project creation/open/save
- Explorer
- Properties
- 3D viewport
- object insertion
- Mute editor
- UI editor
- animation editor
- material editor
- VFX editor
- audio tools
- terrain tools
- physics tools
- network tools
- debugger
- profiler
- asset browser
- package manager
- plugin manager
- GitHub integration
- VPS deployment
- test/play modes
- device preview
- publishing

### Poko Moderation
- dashboard
- reports
- users
- games
- assets
- chat cases
- bans
- restrictions
- appeals
- evidence
- audit
- moderator permissions
- case assignment

## 2. Engine

- Instance model
- component system
- property system
- attribute system
- tag system
- world/scene
- memory system
- job system
- signals/events
- serialization
- renderer
- physics
- animation
- audio
- game UI
- input
- networking
- assets
- streaming
- performance tooling
- diagnostics
- security boundaries

## 3. Rendering

- PBR materials
- lighting
- shadows
- dynamic lights
- materials
- textures
- post processing
- particles/VFX
- terrain
- water
- culling
- LOD
- instancing
- batching
- texture streaming
- dynamic resolution
- Dynamic graphics mode
- Manual graphics mode
- Very Low through Very High
- Custom controls where supported

## 4. Physics

- Static
- Kinematic
- Dynamic
- Character controller
- Collision groups
- Raycast
- ShapeCast
- Constraints
- Sleeping
- Physics LOD
- Physics budgets
- Physics debugging
- Advanced physics controls

Physics is optional/cheap by default; developers can create physics-heavy games deliberately.

## 5. Animation

- skeletal animation
- animation clips
- blending
- state machines
- layers
- IK
- retargeting
- events
- animation compression
- animation LOD
- GPU skinning where appropriate

## 6. Game UI

Core objects:

- ScreenGui
- Canvas
- Frame
- ScrollingFrame
- TextLabel
- TextButton
- ImageLabel
- ImageButton
- TextBox
- ViewportFrame

Layout is primarily a property/system, not a requirement to create separate layout instances for common cases.

Integrated UI behavior is available while traditional LocalScript usage remains supported.

## 7. Mute

### Core
- local
- const
- functions
- methods
- conditions
- loops
- tables
- interpolation
- compound assignment

### Types
- nil
- bool
- number
- int
- float
- string
- Vector2
- Vector3
- Vector4
- CFrame
- Color
- UDim
- UDim2
- Rect
- Array<T>
- Map<K,V>
- Set<T>
- Instance
- Signal
- Task
- Enum

### Advanced
- nullable types
- unions
- generics
- interfaces
- type aliases
- classes
- components
- async/tasks
- modules
- packages
- authority actions
- Buffer
- reflection
- debugging
- profiling

### Script Instances
- Script
- LocalScript
- ModuleScript
- PluginScript

Mute is the implementation language; creators interact with familiar script instances in Studio.

## 8. Mute Performance

- incremental compilation
- compilation caching
- bytecode caching
- constant folding
- dead code elimination
- type specialization
- collection specialization
- property lookup caching
- method dispatch caching
- allocation avoidance
- optimized native calls
- GC tuning
- safe fast paths
- generic fallbacks
- static diagnostics
- performance warnings
- runtime profiling
- authority-specific optimized representation

## 9. Networking

- binary BitBuffer
- packets
- reliable ordered
- reliable unordered
- unreliable
- unreliable sequenced
- batching
- delta compression
- quantization
- interest management
- replication
- client prediction
- reconciliation
- interpolation
- network diagnostics

## 10. Authority

- persistent WebSocket
- game/session loading
- authority PKX
- Mute authority runtime
- request validation
- state transitions
- replication
- session state
- rate limiting
- backpressure
- anti-cheat invariants
- suspicious-player investigation mode
- shared immutable game definitions

## 11. Main Backend

- identity
- accounts
- authentication
- sessions
- games
- game versions
- server discovery
- matchmaking
- social
- groups
- communities
- notifications
- inventory
- ownership
- economy
- marketplace
- achievements
- leaderboards
- reports
- moderation
- analytics
- service registry
- developer projects
- publishing

## 12. Infrastructure

- Go Main Backend
- TypeScript Cloudflare Workers
- TypeScript Durable Objects
- C++ Game Authority
- PostgreSQL
- Valkey/Redis
- Cloudflare D1 where useful
- Cloudflare R2
- Cloudinary for media
- APAC-first deployment
- replaceable provider adapters
- health states and fallback routing

## 13. Accounts

- permanent AccountID
- unique username
- non-unique display name
- avatar
- profile
- settings
- sessions
- security
- permissions
- restrictions
- bans
- recovery
- deletion workflow
- data export where appropriate

## 14. Social

- friends
- friend requests
- DMs
- groups
- communities
- parties
- presence
- invites
- blocking
- muting

## 15. Games

- game creation
- draft/private/unlisted/public/restricted/archived states
- versions
- PNV
- public servers
- private servers
- reserved servers
- invite-only
- friends-only
- matchmaking
- sessions
- rollback

## 16. Discovery

Automatic discovery based on:
- games played
- session duration
- replays
- favorites
- searches
- game genre/features
- observed gameplay characteristics
- similar-player behavior

Surfaces:
- Continue Playing
- Because You Played
- More Like This
- Recommended For You
- Friends Are Playing
- New Games

Discovery is separate from search and separate from hosting priority.

## 17. Search

- games
- users
- communities
- assets
- packages
- plugins
- libraries
- partial matching
- typo tolerance
- filters
- tags
- categories

## 18. Marketplace Separation

### Studio Marketplace
For creators:
- models
- meshes
- textures
- materials
- animations
- audio
- VFX
- UI packages
- Mute libraries
- components
- plugins
- templates
- developer tools
- packages

### PokoX Player Marketplace
For players:
- avatar items
- clothing
- accessories
- animations
- emotes
- cosmetics

Both share ownership, creator identity, transactions, and moderation infrastructure.

## 19. Economy

PoKoins:
- earned from platform tasks
- weekly earning reset
- transfer/marketplace taxation according to product rules
- developer infrastructure/hosting priority
- dynamic sinks/pricing
- transaction ledger
- backend authority

PoKoins are not a client-controlled balance.

## 20. Achievements

- 25 included per game
- expandable capacity
- hidden achievements
- visible achievements
- progress
- points
- game-specific definitions
- authority/server-side awarding
- PokoX achievement display
- Studio management

## 21. Leaderboards and Statistics

Leaderboards:
- global
- friends
- server

Game statistics:
- wins
- kills
- deaths
- playtime
- matches
- custom counters

## 22. Character

Poko has a simple canonical six-part humanoid:
- Root
- Torso
- Head
- LeftArm
- RightArm
- LeftLeg
- RightLeg

Target proportions are approximately 4 units wide, 4.5 units tall, and 2 units deep, with final dimensions validated through gameplay testing.

Detailed conversion is in conversion.md.

## 23. Common Game Systems

Built-in optional systems/components:
- Health
- Damage
- Inventory
- Interactable
- Trigger
- Zone
- Team
- Spawn
- Vehicle
- Dialogue
- Quest
- Shop
- Tool/Equipment
- Camera
- Tags

These are intended to cover common UGC patterns while remaining extensible.

## 24. Moderation

- warnings
- chat restrictions
- marketplace restrictions
- upload restrictions
- developer restrictions
- temporary suspension
- permanent platform ban
- game-level bans
- community restrictions
- appeals
- evidence
- audit history
- role-based moderation permissions

## 25. Content Safety

- upload validation
- package validation
- asset moderation
- chat moderation
- report system
- abuse detection
- plugin permission review
- package security
- ban/restriction enforcement

## 26. Versioning

PNV:
- numeric
- unique
- sortable
- not a hash
- not semantic versioning

Internal content hashes remain available for integrity and caching.

## 27. Developer Workflow

- Studio project
- Mute scripting
- asset import
- package management
- plugin management
- GitHub integration
- developer VPS deployment
- playtesting
- multiplayer testing
- profiling
- publishing
- rollback
- analytics

## 28. Performance

Engine:
- CPU optimization
- GPU optimization
- memory optimization
- network optimization
- streaming
- dynamic quality
- frame budgets
- subsystem budgets
- profiling

Mute:
- compiler optimization
- VM optimization
- native engine calls
- low allocation behavior
- diagnostics
- profiling

Authority:
- binary parsing
- batching
- RAM-resident state
- precomputed definitions
- validation fast paths
- backpressure

## 29. Failure/Fallback

- cache fallback
- Worker fallback
- chat fallback
- provider replacement
- game session replacement
- release rollback
- PKX validation failure
- local Studio recovery
- PokoX reconnect
- service health states

## 30. Application UI

Application UI is distinct from game UI.

Design:
- familiar Roblox/Studio mental model
- modern visual design
- cleaner navigation
- better search
- better docking
- better command discovery
- reduced clutter
- stronger diagnostics

Game UI remains an engine/runtime feature and is not the same system.

## 31. Launch Boundaries

Initial:
- Windows 10+
- Android 11+
- APAC-first infrastructure
- single primary operational region
- architecture prepared for later regional expansion
- no player-facing region selector required

## 32. Explicit Product Position

Poko is not intended to be a full AAA engine by default.

Target range:
- simple UGC
- Indie-scale
- advanced Indie
- ambitious AA-like experiences

The engine should let developers spend more performance budget when they intentionally need it.
