//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"

#include <algorithm>

#include "../../Actors/Actor.h"
#include "../../Game.h"

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
                                             ColliderLayer layer, bool isStatic, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mIsStatic(isStatic)
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    mOwner->GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    mOwner->GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    return mOwner->GetPosition() + mOffset;
}

Vector2 AABBColliderComponent::GetMax() const
{
    return Vector2(GetMin().x + mWidth, GetMin().y + mHeight);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
     bool notColliding = (this->GetMax().x <= b.GetMin().x) || (b.GetMax().x <= this->GetMin().x) ||
                        (this->GetMax().y <= b.GetMin().y) || (b.GetMax().y <= this->GetMin().y);

    return !notColliding;
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b) const
{
    const float topDistance = this->GetMax().y - b->GetMin().y;
    const float bottomDistance = b->GetMax().y - this->GetMin().y;

    return (topDistance < bottomDistance) ? topDistance : -bottomDistance;
}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b) const
{
    const float leftDistance = this->GetMax().x - b->GetMin().x;
    const float rightDistance = b->GetMax().x - this->GetMin().x;

    return (leftDistance < rightDistance) ? leftDistance : -rightDistance;
}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody)
{
    if (mIsStatic) return false;

    auto colliders = mOwner->GetGame()->GetColliders();
    float velocityX = mOwner->GetComponent<RigidBodyComponent>()->GetVelocity().x;
    if (velocityX == 0.0f) {
        return 0.0f;
    }

    std::sort(colliders.begin(), colliders.end(), [=](AABBColliderComponent * a, AABBColliderComponent * b) {
        if (velocityX > 0) {
            float distA = std::abs(a->GetMin().x - GetMax().x);
            float distB = std::abs(b->GetMin().x - GetMax().x);
            return distA < distB;
        }

        float distA = std::abs(a->GetMax().x - GetMin().x);
        float distB = std::abs(b->GetMax().x - GetMin().x);
        return distA < distB;
    });

    for (auto collider : colliders) {
        if (!collider->IsEnabled() || collider == this || !this->Intersect(*collider)) {
            continue;
        }

        float minHorizontalOverlap = GetMinHorizontalOverlap(collider);

        if (Math::Abs(minHorizontalOverlap) > 0.0f) {
            ResolveHorizontalCollisions(rigidBody, minHorizontalOverlap);
            mOwner->OnHorizontalCollision(minHorizontalOverlap, collider);
            return minHorizontalOverlap;
        }
    }

    return 0.0f;
}

float AABBColliderComponent::DetectVerticalCollision(RigidBodyComponent *rigidBody) {
    if (mIsStatic) return false;

    auto colliders = mOwner->GetGame()->GetColliders();
    float velocityY = mOwner->GetComponent<RigidBodyComponent>()->GetVelocity().y;
    if (velocityY == 0.0f) {
        return 0.0f;
    }

    std::sort(colliders.begin(), colliders.end(), [=](AABBColliderComponent * a, AABBColliderComponent * b) {
        if (velocityY > 0) {
            float distA = std::abs(a->GetMin().y - GetMax().y);
            float distB = std::abs(b->GetMin().y - GetMax().y);
            return distA < distB;
        }

        float distA = std::abs(a->GetMax().y - GetMin().y);
        float distB = std::abs(b->GetMax().y - GetMin().y);
        return distA < distB;
    });

    for (auto collider : colliders) {
        if (!collider->IsEnabled() || collider == this || !this->Intersect(*collider)) {
            continue;
        }

        float minVerticalOverlap = GetMinVerticalOverlap(collider);

        if (Math::Abs(minVerticalOverlap) > 0.0f) {
            ResolveVerticalCollisions(rigidBody, minVerticalOverlap);
            mOwner->OnVerticalCollision(minVerticalOverlap, collider);
            return minVerticalOverlap;
        }
    }

    return 0.0f;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap)
{
    rigidBody->GetOwner()->SetPosition(Vector2(rigidBody->GetOwner()->GetPosition().x - minXOverlap,
                                               rigidBody->GetOwner()->GetPosition().y));
    rigidBody->SetVelocity(Vector2(0.0f, rigidBody->GetVelocity().y));
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap)
{
    rigidBody->GetOwner()->SetPosition(Vector2(rigidBody->GetOwner()->GetPosition().x,
                                           rigidBody->GetOwner()->GetPosition().y - minYOverlap));
    rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0.0f));

    if (minYOverlap > 0.0f) {
        rigidBody->GetOwner()->SetOnGround();
    }
}