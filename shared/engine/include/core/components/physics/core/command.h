/**
 * @file command.h
 * @brief Physics command buffer for external changes
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_CORE_COMMAND_H
#define POKO_CORE_COMPONENTS_PHYSICS_CORE_COMMAND_H

#include "handle.h"
#include "../math/vector3.h"
#include "../math/quaternion.h"
#include <cstdint>
#include <vector>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace core {

using math::Vector3;
using math::Quaternion;

/**
 * @brief Command types
 */
enum class CommandType : uint32_t {
    CreateBody,
    DestroyBody,
    CreateCollider,
    DestroyCollider,
    SetTransform,
    SetVelocity,
    ApplyForce,
    ApplyImpulse,
    SetMaterial,
    SetShape,
    SetCollisionFilter,
    SetKinematicTarget,
    Wake,
    Sleep,
    CreateConstraint,
    DestroyConstraint
};

/**
 * @brief Base command
 */
struct Command {
    CommandType type;
    
    explicit Command(CommandType type_) noexcept : type(type_) {}
    virtual ~Command() = default;
};

/**
 * @brief Create body command
 */
struct CreateBodyCommand : public Command {
    Vector3 position;
    Quaternion rotation;
    uint32_t collisionLayer;
    uint32_t collisionMask;
    
    CreateBodyCommand() noexcept : Command(CommandType::CreateBody) {}
};

/**
 * @brief Destroy body command
 */
struct DestroyBodyCommand : public Command {
    BodyHandle body;
    
    explicit DestroyBodyCommand(BodyHandle body_) noexcept
        : Command(CommandType::DestroyBody)
        , body(body_) {}
};

/**
 * @brief Create collider command
 */
struct CreateColliderCommand : public Command {
    BodyHandle body;
    ShapeHandle shape;
    MaterialHandle material;
    Vector3 localPosition;
    Quaternion localRotation;
    
    CreateColliderCommand() noexcept : Command(CommandType::CreateCollider) {}
};

/**
 * @brief Destroy collider command
 */
struct DestroyColliderCommand : public Command {
    ColliderHandle collider;
    
    explicit DestroyColliderCommand(ColliderHandle collider_) noexcept
        : Command(CommandType::DestroyCollider)
        , collider(collider_) {}
};

/**
 * @brief Set transform command
 */
struct SetTransformCommand : public Command {
    BodyHandle body;
    Vector3 position;
    Quaternion rotation;
    
    SetTransformCommand() noexcept : Command(CommandType::SetTransform) {}
};

/**
 * @brief Set velocity command
 */
struct SetVelocityCommand : public Command {
    BodyHandle body;
    Vector3 linearVelocity;
    Vector3 angularVelocity;
    
    SetVelocityCommand() noexcept : Command(CommandType::SetVelocity) {}
};

/**
 * @brief Apply force command
 */
struct ApplyForceCommand : public Command {
    BodyHandle body;
    Vector3 force;
    Vector3 point; // Optional: point of application
    bool usePoint;
    
    ApplyForceCommand() noexcept : Command(CommandType::ApplyForce), usePoint(false) {}
};

/**
 * @brief Apply impulse command
 */
struct ApplyImpulseCommand : public Command {
    BodyHandle body;
    Vector3 impulse;
    Vector3 point; // Optional: point of application
    bool usePoint;
    
    ApplyImpulseCommand() noexcept : Command(CommandType::ApplyImpulse), usePoint(false) {}
};

/**
 * @brief Set material command
 */
struct SetMaterialCommand : public Command {
    ColliderHandle collider;
    MaterialHandle material;
    
    explicit SetMaterialCommand(MaterialHandle material_) noexcept
        : Command(CommandType::SetMaterial)
        , material(material_) {}
};

/**
 * @brief Set shape command
 */
struct SetShapeCommand : public Command {
    ColliderHandle collider;
    ShapeHandle shape;
    
    explicit SetShapeCommand(ShapeHandle shape_) noexcept
        : Command(CommandType::SetShape)
        , shape(shape_) {}
};

/**
 * @brief Set collision filter command
 */
struct SetCollisionFilterCommand : public Command {
    ColliderHandle collider;
    uint32_t layer;
    uint32_t mask;
    
    SetCollisionFilterCommand() noexcept : Command(CommandType::SetCollisionFilter) {}
};

/**
 * @brief Set kinematic target command
 */
struct SetKinematicTargetCommand : public Command {
    BodyHandle body;
    Vector3 targetPosition;
    Quaternion targetRotation;
    
    SetKinematicTargetCommand() noexcept : Command(CommandType::SetKinematicTarget) {}
};

/**
 * @brief Wake command
 */
struct WakeCommand : public Command {
    BodyHandle body;
    
    explicit WakeCommand(BodyHandle body_) noexcept
        : Command(CommandType::Wake)
        , body(body_) {}
};

/**
 * @brief Sleep command
 */
struct SleepCommand : public Command {
    BodyHandle body;
    
    explicit SleepCommand(BodyHandle body_) noexcept
        : Command(CommandType::Sleep)
        , body(body_) {}
};

/**
 * @brief Create constraint command
 */
struct CreateConstraintCommand : public Command {
    BodyHandle bodyA;
    BodyHandle bodyB;
    // Additional constraint-specific data
    // (filled in by derived command types)
    
    CreateConstraintCommand() noexcept : Command(CommandType::CreateConstraint) {}
};

/**
 * @brief Destroy constraint command
 */
struct DestroyConstraintCommand : public Command {
    ConstraintHandle constraint;
    
    explicit DestroyConstraintCommand(ConstraintHandle constraint_) noexcept
        : Command(CommandType::DestroyConstraint)
        , constraint(constraint_) {}
};

/**
 * @brief Command buffer
 * 
 * Stores commands to be processed at safe simulation boundaries.
 */
class CommandBuffer {
public:
    /**
     * @brief Constructor
     */
    CommandBuffer() noexcept = default;
    
    /**
     * @brief Add command
     */
    void addCommand(Command* command) {
        commands.push_back(command);
    }
    
    /**
     * @brief Get commands
     */
    [[nodiscard]] const std::vector<Command*>& getCommands() const noexcept {
        return commands;
    }
    
    /**
     * @brief Clear commands
     */
    void clear() {
        for (Command* cmd : commands) {
            delete cmd;
        }
        commands.clear();
    }
    
    /**
     * @brief Get command count
     */
    [[nodiscard]] size_t getCommandCount() const noexcept {
        return commands.size();
    }
    
private:
    std::vector<Command*> commands;
};

} // namespace core
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CORE_COMMAND_H
