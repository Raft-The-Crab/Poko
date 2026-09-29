/**
 * @file constructor.cpp
 * @brief Scene constructor implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/world/scene.h"
#include <atomic>

namespace poko {
namespace core {
namespace world {

namespace {
    std::atomic<SceneID> g_nextSceneId{1};
}

Scene::Scene()
    : m_id(g_nextSceneId.fetch_add(1, std::memory_order_relaxed))
    , m_name("UnnamedScene")
    , m_instances()
    , m_active(false)
    , m_mutex()
{
}

Scene::Scene(const std::string& name)
    : m_id(g_nextSceneId.fetch_add(1, std::memory_order_relaxed))
    , m_name(name.empty() ? "UnnamedScene" : name)
    , m_instances()
    , m_active(false)
    , m_mutex()
{
}

Scene::~Scene() {
    clear();
}

void Scene::setName(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_name = name.empty() ? "UnnamedScene" : name;
}

void Scene::setName(std::string&& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_name = name.empty() ? "UnnamedScene" : std::move(name);
}

} // namespace world
} // namespace core
} // namespace poko
