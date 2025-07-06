// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <SDL.h>
#include <vector>
#include <unordered_map>
#include "UI/HUD.h"
#include "Systems/AudioSystem.h"
#include "Utils/Math.h"

class Game {
public:
    static const int LEVEL_WIDTH = 280;
    static const int LEVEL_HEIGHT = 120;
    static const int TILE_SIZE = 32;
    static const int TRANSITION_TIME = 1;
    static const int SPAWN_DISTANCE = 1200;

    enum class GameScene {
        MainMenu,
        Level1,
        Level2,
        Level3,
        Level4
    };

    enum class SceneManagerState {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GamePlayState {
        Playing,
        Paused,
        GameOver,
        LevelComplete,
        Leaving
    };

    Game(int windowWidth, int windowHeight);

    bool Initialize();

    void RunLoop();

    void Shutdown();

    void Quit() { mIsRunning = false; }

    // Actor functions
    void UpdateActors(float deltaTime);

    void AddActor(class Actor *actor);

    void RemoveActor(class Actor *actor);

    void ProcessInputActors();

    void HandleKeyPressActors(const int key, const bool isPressed);

    // Level functions
    void LoadMainMenu();
  
    void LoadPauseMenu();
  
    void LoadLevel(const std::string &levelName, const int levelWidth, const int levelHeight);

    std::vector<Actor *> GetNearbyActors(const Vector2 &position, const int range = 1);

    std::vector<class AABBColliderComponent *> GetNearbyColliders(const Vector2 &position, const int range = 2);

    void Reinsert(Actor *actor);

    // Camera functions
    Vector2 &GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2 &position) { mCameraPos = position; };

    // Audio functions
    class AudioSystem *GetAudio() { return mAudio; }

    // UI functions
    void PushUI(class UIScreen *screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen *> &GetUIStack() { return mUIStack; }

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }

    // Loading functions
    class UIFont *LoadFont(const std::string &fileName);

    SDL_Texture *LoadTexture(const std::string &texturePath);
    SDL_Texture *GetCachedTexture(const std::string &texturePath);

    void SetGameScene(GameScene scene, float transitionTime = .0f);

    void ResetGameScene(float transitionTime = .0f);

    void UnloadScene();

    void SetBackgroundImage(const std::string &imagePath, const Vector2 &position = Vector2::Zero,
                            const Vector2 &size = Vector2::Zero);

    void TogglePause();

    // Game-specific
    const class Aeris *GetAeris() { return mAeris; }

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

    // Power-up persistence methods
    void SaveAerisPowerUps();
    void RestoreAerisPowerUps();
    void RemoveCurrentLevelPowerUp();

    // Scene management
    GameScene GetCurrentScene() const { return mGameScene; }

    SDL_Renderer *GetRenderer() { return mRenderer; }

    class HUD *GetHUD() { return mHUD; }

private:
    void ProcessInput();

    void UpdateGame();

    void UpdateCamera();

    void GenerateOutput();

    // Scene Manager
    void UpdateSceneManager(float deltaTime);

    void ChangeScene();

    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;
    float mAlpha;

    // Load the level from a CSV file as a 2D array
    int **ReadLevelData(const std::string &fileName, int width, int height);

    // Cache for level data to avoid re-reading files
    std::unordered_map<std::string, int**> mLevelDataCache;
    void ClearLevelDataCache();

    std::string GetTilePath(int tileId);

    // Cache for tile textures to avoid re-loading
    std::unordered_map<std::string, SDL_Texture*> mTextureCache;

    void BuildLevel(int **levelData, int width, int height);

    // Spatial Hashing for collision detection
    class SpatialHashing *mSpatialHashing;

    // All the UI elements
    std::vector<class UIScreen *> mUIStack;
    std::unordered_map<std::string, class UIFont *> mFonts;

    // SDL stuff
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    AudioSystem *mAudio;

    // Window properties
    int mWindowWidth;
    int mWindowHeight;
    bool mIsFullscreen;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track actors state
    bool mIsRunning;
    GamePlayState mGamePlayState;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;

    // Background and camera
    Vector3 mBackgroundColor;
    Vector3 mModColor;
    Vector2 mCameraPos;

    // Game-specific
    class Aeris *mAeris;
    class HUD *mHUD;
    SoundHandle mMusicHandle;

    // Power-up persistence between levels
    bool mPersistentDoubleJump;
    bool mPersistentDash;
    bool mPersistentWallJump;
    
    // Flag to track if we're resetting due to death (to avoid saving power-ups)
    bool mIsDeathReset;

    SDL_Texture *mBackgroundTexture;
    Vector2 mBackgroundSize;
    Vector2 mBackgroundPosition;
};
