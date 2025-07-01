#pragma once
#include "Actor.h"
#include "Fragment.h"

class Aeris : public Actor
{
public:
    explicit Aeris(Game* game, float forwardSpeed = 5000.0f,
                   float jumpSpeed = -600.0f, float dashSpeed = 80000.0f,
                   bool canFallThroughPlatform = false,
                   bool fallingThroughAPlatform = false, float fallTime = 0.0f
    );

    void OnProcessInput(const Uint8* keyState) override;

    void OnUpdate(float deltaTime) override;

    void OnHandleKeyPress(int key, bool isPressed) override;

    void OnHorizontalCollision(float minOverlap,
                               AABBColliderComponent* other) override;

    void OnVerticalCollision(float minOverlap,
                             AABBColliderComponent* other) override;

    void SetOnGround() override;

    void CollectFragment(Fragment* fragment);

    void Jump();

    void SetNotWallCrawling() { mIsWallCrawling = false; }

    bool IsDashing() { return mIsDashing; }

    void SetLifeCount(int lifeCount) { mLifeCount = lifeCount; }

    void TakeDamage();

    void Kill() override;

    void Win(AABBColliderComponent* poleCollider);

private:
    static constexpr float TIME_UNTIL_NEXT_DAMAGE = 0.3f;
    static constexpr int MAX_JUMP_COUNT = 2;
    static constexpr float QUEUED_JUMP_TIME = 0.15f;
    static constexpr float DASH_TIME = 0.2f;
    static constexpr float FALLTHROUGH_TIMER = 0.24f;

    void ManageAnimations();

    Vector2 Orientation();

    int mLifeCount;
    bool mReceivedDamageRecently;
    float mCanReceiveDamageTime;

    bool mHasUnlockedDoubleJump;
    bool mHasUnlockedDash;
    bool mHasUnlockedWallJump;

    bool mIsWallCrawling;

    float mForwardSpeed;

    float mJumpSpeed;
    int mJumpCount;
    bool mHasQueuedJump;
    float mQueuedJumpTime;

    bool mIsDashing;
    float mDashTime;
    float mDashSpeed;

    bool mIsRunning;
    bool mIsDying;

    bool mCanFallThroughPlatform;
    bool mIsFallingThroughPlatform;
    float mFallTime;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};