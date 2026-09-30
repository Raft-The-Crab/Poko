/**
 * @file test_renderable.cpp
 * @brief Renderable component unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/renderable/renderable.h"
#include "core/components/component.h"
#include <cassert>
#include <iostream>

using namespace poko::core::components;

void test_bounding_box() {
    std::cout << "Testing BoundingBox..." << std::endl;
    
    // Default constructor
    renderable::BoundingBox box1;
    assert(box1.minX == 0.0f);
    assert(box1.minY == 0.0f);
    assert(box1.minZ == 0.0f);
    assert(box1.maxX == 0.0f);
    assert(box1.maxY == 0.0f);
    assert(box1.maxZ == 0.0f);
    
    // Component constructor
    renderable::BoundingBox box2(-1.0f, -2.0f, -3.0f, 1.0f, 2.0f, 3.0f);
    assert(box2.minX == -1.0f);
    assert(box2.minY == -2.0f);
    assert(box2.minZ == -3.0f);
    assert(box2.maxX == 1.0f);
    assert(box2.maxY == 2.0f);
    assert(box2.maxZ == 3.0f);
    
    // Center calculation
    assert(box2.centerX() == 0.0f);
    assert(box2.centerY() == 0.0f);
    assert(box2.centerZ() == 0.0f);
    
    // Extent calculation
    assert(box2.extentX() == 2.0f);
    assert(box2.extentY() == 4.0f);
    assert(box2.extentZ() == 6.0f);
    
    std::cout << "✓ BoundingBox tests passed" << std::endl;
}

void test_renderable_basics() {
    std::cout << "Testing Renderable basics..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Check component type
    assert(renderable.getTypeID() == renderable::RENDERABLE_COMPONENT_ID);
    assert(std::string(renderable.getTypeName()) == renderable::RENDERABLE_COMPONENT_NAME);
    
    // Default values
    assert(renderable.isVisible());
    assert(renderable.getMaterialId() == renderable::INVALID_RESOURCE_ID);
    assert(renderable.getMeshId() == renderable::INVALID_RESOURCE_ID);
    assert(renderable.getRenderLayer() == renderable::RenderLayer::Default);
    assert(renderable.getRenderQueue() == renderable::RenderQueue::Opaque);
    assert(renderable.castsShadows());
    assert(renderable.receivesShadows());
    assert(renderable.getUserData() == nullptr);
    
    std::cout << "✓ Renderable basics tests passed" << std::endl;
}

void test_renderable_visibility() {
    std::cout << "Testing Renderable visibility..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Default visible
    assert(renderable.isVisible());
    
    // Set invisible
    renderable.setVisible(false);
    assert(!renderable.isVisible());
    
    // Set visible
    renderable.setVisible(true);
    assert(renderable.isVisible());
    
    std::cout << "✓ Renderable visibility tests passed" << std::endl;
}

void test_renderable_resources() {
    std::cout << "Testing Renderable resources..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Set material ID
    renderable.setMaterialId(12345);
    assert(renderable.getMaterialId() == 12345);
    
    // Set mesh ID
    renderable.setMeshId(67890);
    assert(renderable.getMeshId() == 67890);
    
    // Set to invalid
    renderable.setMaterialId(renderable::INVALID_RESOURCE_ID);
    assert(renderable.getMaterialId() == renderable::INVALID_RESOURCE_ID);
    
    renderable.setMeshId(renderable::INVALID_RESOURCE_ID);
    assert(renderable.getMeshId() == renderable::INVALID_RESOURCE_ID);
    
    std::cout << "✓ Renderable resources tests passed" << std::endl;
}

void test_renderable_layer() {
    std::cout << "Testing Renderable layer..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Default layer
    assert(renderable.getRenderLayer() == renderable::RenderLayer::Default);
    
    // Set UI layer
    renderable.setRenderLayer(renderable::RenderLayer::UI);
    assert(renderable.getRenderLayer() == renderable::RenderLayer::UI);
    
    // Set background layer
    renderable.setRenderLayer(renderable::RenderLayer::Background);
    assert(renderable.getRenderLayer() == renderable::RenderLayer::Background);
    
    std::cout << "✓ Renderable layer tests passed" << std::endl;
}

void test_renderable_queue() {
    std::cout << "Testing Renderable queue..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Default queue
    assert(renderable.getRenderQueue() == renderable::RenderQueue::Opaque);
    
    // Set transparent queue
    renderable.setRenderQueue(renderable::RenderQueue::Transparent);
    assert(renderable.getRenderQueue() == renderable::RenderQueue::Transparent);
    
    // Set overlay queue
    renderable.setRenderQueue(renderable::RenderQueue::Overlay);
    assert(renderable.getRenderQueue() == renderable::RenderQueue::Overlay);
    
    std::cout << "✓ Renderable queue tests passed" << std::endl;
}

void test_renderable_bounds() {
    std::cout << "Testing Renderable bounds..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Set bounds
    renderable::BoundingBox bounds(-5.0f, -5.0f, -5.0f, 5.0f, 5.0f, 5.0f);
    renderable.setBounds(bounds);
    
    // Get bounds
    const renderable::BoundingBox& retrieved = renderable.getBounds();
    assert(retrieved.minX == -5.0f);
    assert(retrieved.maxX == 5.0f);
    assert(retrieved.centerX() == 0.0f);
    
    std::cout << "✓ Renderable bounds tests passed" << std::endl;
}

void test_renderable_shadows() {
    std::cout << "Testing Renderable shadows..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Default shadows
    assert(renderable.castsShadows());
    assert(renderable.receivesShadows());
    
    // Disable shadow casting
    renderable.setCastShadows(false);
    assert(!renderable.castsShadows());
    
    // Disable shadow receiving
    renderable.setReceiveShadows(false);
    assert(!renderable.receivesShadows());
    
    // Enable again
    renderable.setCastShadows(true);
    renderable.setReceiveShadows(true);
    assert(renderable.castsShadows());
    assert(renderable.receivesShadows());
    
    std::cout << "✓ Renderable shadows tests passed" << std::endl;
}

void test_renderable_lifecycle() {
    std::cout << "Testing Renderable lifecycle..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Initial state
    assert(renderable.getState() == ComponentState::None);
    
    // On create
    renderable.onCreate();
    assert(renderable.getState() == ComponentState::Created);
    
    // On activate
    renderable.onActivate();
    assert(renderable.getState() == ComponentState::Active);
    assert(renderable.isActive());
    
    // On deactivate
    renderable.onDeactivate();
    assert(renderable.getState() == ComponentState::Deactivating);
    assert(!renderable.isActive());
    
    // On destroy
    renderable.onDestroy();
    assert(renderable.getState() == ComponentState::Destroyed);
    
    std::cout << "✓ Renderable lifecycle tests passed" << std::endl;
}

void test_renderable_user_data() {
    std::cout << "Testing Renderable user data..." << std::endl;
    
    renderable::Renderable renderable;
    
    // Default null
    assert(renderable.getUserData() == nullptr);
    
    // Set user data
    int data = 42;
    renderable.setUserData(&data);
    assert(renderable.getUserData() == &data);
    
    // Clear user data
    renderable.setUserData(nullptr);
    assert(renderable.getUserData() == nullptr);
    
    std::cout << "✓ Renderable user data tests passed" << std::endl;
}

void test_renderable_registration() {
    std::cout << "Testing Renderable registration..." << std::endl;
    
    // Register with global registry
    registerRenderableComponent();
    
    // Get global registry
    auto& registry = getGlobalComponentRegistry();
    
    // Create renderable component
    auto component = registry.createComponent(renderable::RENDERABLE_COMPONENT_ID);
    assert(component != nullptr);
    
    // Check type
    assert(component->getTypeID() == renderable::RENDERABLE_COMPONENT_ID);
    assert(std::string(component->getTypeName()) == renderable::RENDERABLE_COMPONENT_NAME);
    
    std::cout << "✓ Renderable registration tests passed" << std::endl;
}

int main() {
    std::cout << "=== Renderable Component Unit Tests ===" << std::endl;
    
    test_bounding_box();
    test_renderable_basics();
    test_renderable_visibility();
    test_renderable_resources();
    test_renderable_layer();
    test_renderable_queue();
    test_renderable_bounds();
    test_renderable_shadows();
    test_renderable_lifecycle();
    test_renderable_user_data();
    test_renderable_registration();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    return 0;
}
