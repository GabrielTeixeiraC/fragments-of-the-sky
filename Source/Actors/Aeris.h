#pragma once
#include "Actor.h"

class Aeris : public Actor
{
public:
    explicit Aeris(Game* game, float forwardSpeed = 5000.0f,
                   float jumpSpeed = -600.0f, float dashSpeed = 9000.0f);

    void OnProcessInput(const Uint8* keyState) override;

    void OnUpdate(float deltaTime) override;

    void OnHandleKeyPress(int key, bool isPressed) override;

    void OnHorizontalCollision(float minOverlap,
                               AABBColliderComponent* other) override;

    void OnVerticalCollision(float minOverlap,
                             AABBColliderComponent* other) override;

    void SetOnGround() override;

    void Jump();

    void Kill() override;

    void Win(AABBColliderComponent* poleCollider);

private:
    // Time in seconds to slide down the pole
    static const int POLE_SLIDE_TIME = 1;
    static constexpr int MAX_JUMP_COUNT = 2;
    static constexpr float QUEUED_JUMP_TIME = 0.16f;

    void ManageAnimations();

    bool mHasUnlockedDoubleJump;
    bool mHasUnlockedDash;
    bool mHasUnlockedWallJump;

    bool mIsWallCrawling;

    float mForwardSpeed;

    float mJumpSpeed;
    int mJumpCount;
    bool mHasQueuedJump;
    float mQueuedJumpTime;

    float mDashSpeed;

    float mPoleSlideTimer;
    bool mIsRunning;
    bool mIsOnPole;
    bool mIsDying;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};