#include <SDL.h>
#include "../Actors/Actor.h"
#include "../Game.h"
#include "RigidBodyComponent.h"

#include "../Actors/Aeris.h"
#include "ColliderComponents/AABBColliderComponent.h"

const float MAX_SPEED_X = 750.0f;
const float MAX_SPEED_Y = 750.0f;
const float GRAVITY = 2000.0f;

RigidBodyComponent::RigidBodyComponent(class Actor* owner, float mass,
                                       float friction, bool applyGravity,
                                       int updateOrder)
    : Component(owner, updateOrder)
      , mMass(mass)
      , mApplyGravity(applyGravity)
      , mApplyFriction(true)
      , mFrictionCoefficient(friction)
      , mVelocity(Vector2::Zero)
      , mAcceleration(Vector2::Zero)
{
}

void RigidBodyComponent::ApplyForce(const Vector2& force)
{
    mAcceleration += force * (1.f / mMass);
}

void RigidBodyComponent::Update(float deltaTime)
{
    // Apply gravity acceleration
    if (mApplyGravity) {
        ApplyForce(Vector2::UnitY * GRAVITY);
    }

    // Apply friction
    if (mApplyFriction && Math::Abs(mVelocity.x) > 0.05f) {
        ApplyForce(Vector2::UnitX * -mFrictionCoefficient * mVelocity.x);
    }

    // Euler Integration
    mVelocity += mAcceleration * deltaTime;

    if (mOwner->GetActorType() == ActorType::Player) {
        auto* aeris = dynamic_cast<Aeris*>(mOwner);
        if (!aeris->IsDashing()) {
            mVelocity.x = Math::Clamp<
                float>(mVelocity.x, -MAX_SPEED_X, MAX_SPEED_X);
            mVelocity.y = Math::Clamp<
                float>(mVelocity.y, -MAX_SPEED_Y, MAX_SPEED_Y);
        }
    } else {
        mVelocity.x = Math::Clamp<
            float>(mVelocity.x, -MAX_SPEED_X, MAX_SPEED_X);
        mVelocity.y = Math::Clamp<
            float>(mVelocity.y, -MAX_SPEED_Y, MAX_SPEED_Y);
    }

    if (Math::NearZero(mVelocity.x, 1.0f)) {
        mVelocity.x = 0.f;
    }

    auto collider = mOwner->GetComponent<AABBColliderComponent>();

    // Check if we're dashing and need to subdivide movement
    bool isDashing = false;
    if (mOwner->GetActorType() == ActorType::Player) {
        auto* aeris = dynamic_cast<Aeris*>(mOwner);
        isDashing = aeris->IsDashing();
    }

    // If dashing, subdivide movement to prevent wall clipping
    int subdivisions = 1;
    if (isDashing) {
        // Calculate how many subdivisions we need based on movement distance
        float moveDistance = Math::Abs(mVelocity.x * deltaTime);
        const float maxMovePerStep = 16.0f; // Half a tile size to ensure collision detection
        if (moveDistance > maxMovePerStep) {
            subdivisions = static_cast<int>(ceilf(moveDistance / maxMovePerStep));
        }
    }

    float subDeltaTime = deltaTime / subdivisions;

    for (int i = 0; i < subdivisions; i++) {
        if (mVelocity.x != 0.0f) {
            mOwner->SetPosition(Vector2(
                mOwner->GetPosition().x + mVelocity.x * subDeltaTime,
                mOwner->GetPosition().y));

            if (collider) {
                collider->DetectHorizontalCollision(this);
            }
        }

        if (mVelocity.y != 0.0f) {
            mOwner->SetPosition(Vector2(mOwner->GetPosition().x,
                                        mOwner->GetPosition().y + mVelocity.y *
                                        subDeltaTime));

            if (collider) {
                collider->DetectVerticalCollision(this);
            }
        }
    }

    mAcceleration.Set(0.f, 0.f);
}