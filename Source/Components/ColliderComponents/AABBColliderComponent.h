#pragma once
#include "../Component.h"
#include "../../Utils/Math.h"
#include "../RigidBodyComponent.h"
#include <vector>
#include <map>
#include <set>

enum class ColliderLayer
{
    Player,
    Enemy,
    Blocks,
    Pole,
    Fragment,
    Void
};

class AABBColliderComponent : public Component
{
public:
    // Collider ignore map
    const std::map<ColliderLayer, const std::set<ColliderLayer>>
    ColliderIgnoreMap = {
        {ColliderLayer::Player, {}},
        {ColliderLayer::Blocks,
         {ColliderLayer::Blocks, ColliderLayer::Fragment}},
        {ColliderLayer::Void, {ColliderLayer::Blocks, ColliderLayer::Fragment}},
        {ColliderLayer::Enemy, {ColliderLayer::Fragment}},
        {ColliderLayer::Fragment, {}},
        {ColliderLayer::Pole, {}}
    };

    AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
                          ColliderLayer layer, bool isStatic = false,
                          int updateOrder = 10);

    ~AABBColliderComponent() override;

    bool Intersect(const AABBColliderComponent& b) const;

    float DetectHorizontalCollision(RigidBodyComponent* rigidBody);

    float DetectVerticalCollision(RigidBodyComponent* rigidBody);

    void SetStatic(bool isStatic) { mIsStatic = isStatic; }

    Vector2 GetMin() const;

    Vector2 GetMax() const;

    Vector2 GetCenter() const;

    ColliderLayer GetLayer() const { return mLayer; }

private:
    float GetMinVerticalOverlap(AABBColliderComponent* b) const;

    float GetMinHorizontalOverlap(AABBColliderComponent* b) const;

    void ResolveHorizontalCollisions(RigidBodyComponent* rigidBody,
                                     const float minOverlap);

    void ResolveVerticalCollisions(RigidBodyComponent* rigidBody,
                                   const float minOverlap);

    Vector2 mOffset;
    int mWidth;
    int mHeight;
    bool mIsStatic;

    ColliderLayer mLayer;
};