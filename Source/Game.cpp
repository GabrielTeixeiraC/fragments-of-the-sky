// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "Utils/CSV.h"
#include "Utils/Math.h"
#include "Utils/Random.h"
#include "Game.h"
#include "UI/HUD.h"
#include "Systems/SpatialHashing.h"
#include "Actors/Actor.h"
#include "Actors/Aeris.h"
#include "Actors/Block.h"
#include "Actors/Fragment.h"
#include "Actors/Spawner.h"
#include "Actors/Void.h"
#include "UI/Elements/UIScreen.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/DrawComponents/DrawSpriteComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"
#include "Actors/FlagBlock.h"
#include "Actors/Object.h"

Game::Game(int windowWidth, int windowHeight)
    : mWindow(nullptr)
      , mRenderer(nullptr)
      , mTicksCount(0)
      , mIsRunning(true)
      , mWindowWidth(windowWidth)
      , mWindowHeight(windowHeight)
      , mIsFullscreen(false)
      , mAeris(nullptr)
      , mHUD(nullptr)
      , mBackgroundColor(0, 0, 0)
      , mModColor(255, 255, 255)
      , mCameraPos(Vector2::Zero)
      , mAudio(nullptr)
      , mSceneManagerTimer(0.0f)
      , mSceneManagerState(SceneManagerState::None)
      , mAlpha(0.0f)
      , mGameScene(GameScene::MainMenu)
      , mNextScene(GameScene::MainMenu)
      , mBackgroundTexture(nullptr)
      , mBackgroundSize(Vector2::Zero)
      , mBackgroundPosition(Vector2::Zero)
      , mPersistentDoubleJump(false)
      , mPersistentDash(false)
      , mPersistentWallJump(false)
      , mIsDeathReset(false)
      , mLevelData(nullptr)
      , mLevelObjectsData(nullptr)
      , mIsIntroductionScreenRunning(false)
      , mIsEndGameScreenRunning(false)
      , mIntroductionTimer(0.0f)
      , mEndGameTimer(0.0f)
      , mCheckPointPosition(Vector2::Zero)
      , isThereCheckPoint(false)
      , mIsDemo(false)
{
}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Fragments of the Sky", 0, 0, mWindowWidth,
                               mWindowHeight, 0);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1,
                                   SDL_RENDERER_ACCELERATED |
                                   SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0) {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    // Start random number generator
    Random::Init();

    mAudio = new AudioSystem(8);

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);
    mTicksCount = SDL_GetTicks();

    // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    // SDL_RenderSetLogicalSize(mRenderer, 800, 502);

    // Init all game actors
    SetGameScene(GameScene::MainMenu);

    return true;
}

void Game::SetGameScene(Game::GameScene scene, float transitionTime)
{
    if (mSceneManagerState == SceneManagerState::None) {
        if (scene == GameScene::MainMenu || scene == GameScene::Introduction || scene == GameScene::EndGame || scene == GameScene::Level1 || scene
            == GameScene::Level2 || scene == GameScene::Level3 || scene == GameScene::Level4) {
            mNextScene = scene;
            mSceneManagerState = SceneManagerState::Entering;
            mSceneManagerTimer = transitionTime;
            mAlpha = 0.0f;
        } else {
            SDL_Log("SetGameScene: invalid scene.");
        }
    } else {
        SDL_Log("SetGameScene: a scene transition is already running.");
    }
}

void Game::ResetGameScene(float transitionTime)
{
    SetGameScene(mGameScene, transitionTime);
}

void Game::LoadIntroduction() {
    mBackgroundColor.Set(0.0f, 0.0f, 0.0f);

    auto introduction = new UIScreen(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf",
                                 UIScreen::UIType::Introduction);

    introduction->AddText("Wake up Aeris, wake up...",
                       Vector2(mWindowWidth / 2.0f - (296 / 2.0f),
                               mWindowHeight / 2.0f - (32 / 2.0f)),
                       Vector2(296, 32), 24, 1024,
                       Vector3(1, 1, 1));
}


void Game::LoadEndGame() {
    // SetBackgroundImage("../Assets/Sprites/background_end_game.png",
    //                        Vector2(0, 0), Vector2(1280, 720));

    mBackgroundColor.Set(0.0f, 0.0f, 0.0f);

    auto endGame = new UIScreen(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf",
                                 UIScreen::UIType::EndGame);

    endGame->AddText("CONGRATULATIONS",
                       Vector2(mWindowWidth / 2.0f - (576 / 2.0f),
                               180),
                       Vector2(576, 82), 64, 1024,
                       Vector3(1, 1, 1));

    endGame->AddText("We hope you had a good time.",
                       Vector2(mWindowWidth / 2.0f - (704 / 2.0f),
                               300),
                       Vector2(704, 62), 48, 1024,
                       Vector3(1, 1, 1));

    endGame->AddText("Thanks for playing!",
                       Vector2(mWindowWidth / 2.0f - (446 / 2.0f),
                               360),
                       Vector2(446, 62), 48, 1024,
                       Vector3(1, 1, 1));
}

void Game::ChangeScene()
{
    // Save Aeris power-ups before unloading scene (unless we're resetting due to death)
    if (!mIsDeathReset) {
        SaveAerisPowerUps();
    }

    // Reset the death flag for next time
    mIsDeathReset = false;

    // Unload current Scene
    UnloadScene();

    // Reset camera position
    mCameraPos.Set(0.0f, 0.0f);

    // Reset gameplay state
    mGamePlayState = GamePlayState::Playing;

    // Reset scene manager state
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu) {
        SetBackgroundImage("../Assets/UI/background_menu.png",
                           Vector2(0, 0), Vector2(1280, 720));

        // Initialize main menu actors
        LoadMainMenu();
    } else if (mNextScene == GameScene::Introduction) {
        mIsIntroductionScreenRunning = true;
        mIntroductionTimer = INTRODUCTION_SCREEN_TIMER;
        LoadIntroduction();
    } else if (mNextScene == GameScene::EndGame) {
        mIsEndGameScreenRunning = true;
        mEndGameTimer = ENDGAME_SCREEN_TIMER;
        LoadEndGame();
    } else if (mNextScene == GameScene::Level1) {
        mHUD = new HUD(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf", UIScreen::UIType::HUD);

        mMusicHandle = mAudio->PlaySound("MusicMain.ogg", true);

        // Set background color
        mBackgroundColor.Set(109.0f, 132.0f, 200.0f);

        SetBackgroundImage("../Assets/Sprites/background_level1.png",
                           Vector2(0, 0), Vector2(TILE_SIZE * LEVEL_WIDTH, TILE_SIZE * LEVEL_HEIGHT));

        // Initialize actors
        LoadLevel("../Assets/Levels/Level1/level1-swamp_BlockLayer1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
        LoadLevelObjects("../Assets/Levels/Level1/level1-swamp_BlockLayer2.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    } else if (mNextScene == GameScene::Level2) {
        mHUD = new HUD(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf", UIScreen::UIType::HUD);

        mMusicHandle = mAudio->PlaySound("MusicMain.ogg", true);

        mBackgroundColor.Set(0.0f, 47.0f, 187.0f);

        SetBackgroundImage(
            "../Assets/Sprites/background_level2.png", Vector2(0, 0),
            Vector2(TILE_SIZE * LEVEL_WIDTH, TILE_SIZE * LEVEL_HEIGHT)
        );

        // Initialize actors
        LoadLevel("../Assets/Levels/Level2/level2.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    } else if (mNextScene == GameScene::Level3) {
        mHUD = new HUD(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf", UIScreen::UIType::HUD);

        mMusicHandle = mAudio->PlaySound("MusicMain.ogg", true);

        mBackgroundColor.Set(179.0f, 109.0f, 96.0f);

        SetBackgroundImage(
            "../Assets/Sprites/background_level_desert.png", Vector2(0, 0),
            Vector2(TILE_SIZE * LEVEL_WIDTH, TILE_SIZE * LEVEL_HEIGHT)
        );

        // Initialize actors
        LoadLevel("../Assets/Levels/Level3/level3_BlockLayer1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    } else if (mNextScene == GameScene::Level4) {
        mHUD = new HUD(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf", UIScreen::UIType::HUD);
        mMusicHandle = mAudio->PlaySound("MusicMain.ogg", true);

        mBackgroundColor.Set(109.0f, 132.0f, 200.0f);
        mHUD->SetSpinSpeed(mIsDemo ? 0.0f : 1.0f);

        SetBackgroundImage(
            "../Assets/Sprites/background_level1.png", Vector2(0, 0),
            Vector2(TILE_SIZE * LEVEL_WIDTH, TILE_SIZE * LEVEL_HEIGHT)
        );

        // Initialize actors
        LoadLevel("../Assets/Levels/Level4/level4_BlockLayer1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }

    // Set new scene
    mGameScene = mNextScene;

    // Restore power-ups to new Aeris after level loading
    RestoreAerisPowerUps();
}

std::pair<int, int> Game::MapScreenToTile(Vector2 position)
{
    int i = position.x / TILE_SIZE;
    int j = position.y / TILE_SIZE;
    return std::make_pair(i, j);
}

void Game::LoadMainMenu()
{
    mIsDemo = false;
    auto mainMenu = new UIScreen(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf",
                                 UIScreen::UIType::MainMenu);

    const Vector2 titleSize = Vector2(800, 300);
    const Vector2 titlePos = Vector2(mWindowWidth / 2.0f - titleSize.x / 2.0f,
                                     90.0f);
    mainMenu->AddImage("../Assets/UI/titlex4.png", titlePos, titleSize);

    mainMenu->AddButton("", // no text, image-only
                        Vector2(mWindowWidth / 2.0f - 128.0f, 410.0f),
                        Vector2(256.0f, 64.0f), [this]() {
                            SetGameScene(GameScene::Introduction, .5f);
                        }, Vector2::Zero, "../Assets/UI/new_game.png");
    mainMenu->AddButton("", // no text, image-only
                        Vector2(mWindowWidth / 2.0f - 128.0f, 482.0f),
                        Vector2(256.0f, 64.0f), [this]() {
                            mPersistentDoubleJump = true;
                            mPersistentDash = true;
                            mPersistentWallJump = true;
                            mIsDemo = true;
                            RestoreAerisPowerUps();
                            SetGameScene(GameScene::Level4, .5f);
                        }, Vector2::Zero, "../Assets/UI/demo.png");
    mainMenu->AddButton("", // no text, image-only
                        Vector2(mWindowWidth / 2.0f - 128.0f, 554.0f),
                        Vector2(256.0f, 64.0f), [this]() {
                            Quit();
                        }, Vector2::Zero, "../Assets/UI/exit_game.png");
}

void Game::LoadLevelObjects(const std::string& levelObjectsName, const int levelWidth,
                     const int levelHeight)
{
    int** levelObjectsData = nullptr;
    auto cachedLevelObjects = mLevelObjectsDataCache.find(levelObjectsName);
    if (cachedLevelObjects != mLevelObjectsDataCache.end()) {
        levelObjectsData = cachedLevelObjects->second;
        SDL_Log("Using cached level objects data for: %s", levelObjectsName.c_str());
    } else {
        // Load level data from file
        levelObjectsData = ReadLevelData(levelObjectsName, levelWidth, levelHeight);
        if (levelObjectsData) {
            // Cache the level data for future use
            mLevelObjectsDataCache[levelObjectsName] = levelObjectsData;
            SDL_Log("Cached level objects data for: %s", levelObjectsName.c_str());
        }
    }

    if (!levelObjectsData) {
        SDL_Log("Failed to load level objects data");
        return;
    }

    // Instantiate level actors
    mLevelObjectsData = levelObjectsData;
    BuildLevelObjects(levelObjectsData, levelWidth, levelHeight);
}

void Game::LoadLevel(const std::string& levelName, const int levelWidth,
                     const int levelHeight)
{
    // Check if level data is already cached
    int** levelData = nullptr;
    auto cachedLevel = mLevelDataCache.find(levelName);
    if (cachedLevel != mLevelDataCache.end()) {
        levelData = cachedLevel->second;
        SDL_Log("Using cached level data for: %s", levelName.c_str());
    } else {
        // Load level data from file
        levelData = ReadLevelData(levelName, levelWidth, levelHeight);
        if (levelData) {
            // Cache the level data for future use
            mLevelDataCache[levelName] = levelData;
            SDL_Log("Cached level data for: %s", levelName.c_str());
        }
    }

    if (!levelData) {
        SDL_Log("Failed to load level data");
        return;
    }

    // Instantiate level actors
    mLevelData = levelData;
    BuildLevel(levelData, levelWidth, levelHeight);
}

std::string Game::GetTilePath(int tileId) {
    std::string basePath = "../Assets/Sprites/Tilesets/Tiles/Tile_";
    int adjustedTileId;
    
    // Adjust tile ID based on current game scene
    // Swamp: 001-060, Snow: 061-120, Desert: 121-180, Dungeon: 181-240
    if (mNextScene == GameScene::Level1) {
        // Swamp tiles: 001-060
        adjustedTileId = tileId + 1;
    } else if (mNextScene == GameScene::Level2) {
        // Snow tiles: 061-120
        adjustedTileId = tileId + 61;
    } else if (mNextScene == GameScene::Level3) {
        // Desert tiles: 121-180
        adjustedTileId = tileId + 121;
    } else if (mNextScene == GameScene::Level4) {
        adjustedTileId = tileId + 1;
    }
    
    // Format with 3 digits (001, 002, etc.)
    return basePath +
           (adjustedTileId < 10 ? "00" : (adjustedTileId < 100 ? "0" : "")) +
           std::to_string(adjustedTileId) +
           ".png";
}

std::string Game::GetObjectsTilePath(int tileId) {
    std::string basePath;

    // Choose tileset based on current game scene
    if (mNextScene == GameScene::Level3) {
        basePath = "../Assets/Sprites/Dungeon/Objects/Tile_";
    } else if (mNextScene == GameScene::Level2) {
        basePath = "../Assets/Sprites/Snow/Objects/Tile_";
    } else {
        basePath = "../Assets/Sprites/Swamp/Objects/Tile_";
    }

    return basePath +
           (tileId + 1 < 10 ? "0" : "") +
           std::to_string(tileId) +
           ".png";
}

std::unordered_map<int, std::tuple<std::string, int, int>> Game::GetObjectsMap()
{
    std::unordered_map<int, std::tuple<std::string, int, int>> tilePaths;
    if (mNextScene == GameScene::Level4) {
        tilePaths = {};
    } else {
        tilePaths = {
            {58, {GetObjectsTilePath(58), 141, 168}},
            {34, {GetObjectsTilePath(35), 16, 21}},
            {45, {GetObjectsTilePath(45), 109, 41}},
            {43, {GetObjectsTilePath(43), 89, 26}},
            {55, {GetObjectsTilePath(55), 104, 132}},
            {47, {GetObjectsTilePath(47), 81, 29}},
            {44, {GetObjectsTilePath(44), 40, 28}},
            {41, {GetObjectsTilePath(41), 11, 21}},
            {38, {GetObjectsTilePath(39), 13, 21}},
            {51, {GetObjectsTilePath(51), 52, 63}},
            {11, {GetObjectsTilePath(11), 31, 26}},
            {13, {GetObjectsTilePath(13), 63, 27}},
            {6, {GetObjectsTilePath(6), 31, 15}},
            {48, {GetObjectsTilePath(48), 47, 55}},
            {10, {GetObjectsTilePath(10), 63, 28}},
            {36, {GetObjectsTilePath(37), 13, 21}},
            {57, {GetObjectsTilePath(57), 104, 135}},
            {50, {GetObjectsTilePath(50), 47, 57}},
            {56, {GetObjectsTilePath(56), 123, 150}},
            {53, {GetObjectsTilePath(53), 31, 31}},
            {19, {GetObjectsTilePath(20), 9, 11}},
            {35, {GetObjectsTilePath(36), 16, 21}},
            {49, {GetObjectsTilePath(49), 63, 59}},
            {42, {GetObjectsTilePath(42), 11, 21}},
        };
    }
    return tilePaths;
}

void Game::BuildLevelObjects(int **levelObjectsData, int width, int height)
{
    std::unordered_map<int, std::tuple<std::string, int, int>> tilePaths = GetObjectsMap();
    for (int y = 0; y < LEVEL_HEIGHT; ++y) {
        for (int x = 0; x < LEVEL_WIDTH; ++x) {
            int tile = levelObjectsData[y][x];
            if (tile == -1) {
                continue;
            }
            Vector2 position(x * TILE_SIZE, y * TILE_SIZE);
            Object* object = new Object(this, std::get<0>(tilePaths[tile]), std::get<1>(tilePaths[tile]), std::get<2>(tilePaths[tile]));
            object->SetPosition(Vector2(position.x, position.y - std::get<2>(tilePaths[tile]) + TILE_SIZE));
        }
    }
}

void Game::BuildLevel(int** levelData, int width, int height)
{
    // Pre-calculate common tile paths to avoid repeated string operations
    std::unordered_map<int, std::string> tilePaths;

    for (int y = 0; y < LEVEL_HEIGHT; ++y) {
        for (int x = 0; x < LEVEL_WIDTH; ++x) {
            int tile = levelData[y][x];
            if (tile == -1) {
                continue;
            }

            Vector2 position(x * TILE_SIZE, y * TILE_SIZE);

            if (tile == 59) {
                mAeris = new Aeris(this);
                if (IsThereCheckPoint()) {
                    // If a checkpoint was set, spawn Aeris there
                    mAeris->SetPosition(mCheckPointPosition);
                } else {
                    // Otherwise, spawn at the default position
                    mAeris->SetPosition(position);
                }
                SetCameraPos(mAeris->GetPosition());
            }
            else if (tile == 19 || tile == 29 || tile == 39) {
                Fragment* fragment;
                if (tile == 19) {
                    fragment = new Fragment(
                        this, Fragment::FragmentType::DoubleJump);
                } else if (tile == 29) {
                    fragment = new Fragment(this, Fragment::FragmentType::Dash);
                } else {
                    fragment = new Fragment(this, Fragment::FragmentType::WallJump);
                }
                fragment->SetPosition(position);
            } else if (tile == 46) {
                Spawner* spawner = new Spawner(this, SPAWN_DISTANCE, mNextScene);
                spawner->SetPosition(position);
            } else if (tile == 58) {
                FlagBlock* pole = new FlagBlock(this);
                pole->SetPosition(position);
            } else if (tile == 38) {
                Void* voidTile = new Void(this);
                voidTile->SetPosition(position);
            } else {
                // Get or calculate tile path
                if (tilePaths.find(tile) == tilePaths.end()) {
                    tilePaths[tile] = GetTilePath(tile);
                }

                Block* block;
                if (tile % 60 == 6 || tile % 60 == 7 || tile % 60 == 8) {
                    block = new Block(this, tilePaths[tile], false, false, true);
                } else if ((mNextScene == GameScene::Level3 || mNextScene == GameScene::Level4) && (tile % 60 == 10 || tile % 60 == 12)) {
                    block = new Block(this, tilePaths[tile], true, true, false);
                    
                } else {
                    block = new Block(this, tilePaths[tile]);
                }

                block->SetPosition(position);
            }
        }
    }
}

int** Game::ReadLevelData(const std::string& fileName, int width, int height)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
        return nullptr;
    }

    // Create a 2D array of size width and height to store the level data
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i) {
        levelData[i] = new int[width];
    }

    // Read the file line by line
    int row = 0;

    std::string line;
    while (!file.eof()) {
        std::getline(file, line);
        if (!line.empty()) {
            auto tiles = CSVHelper::Split(line);
            if (tiles.size() != width) {
                SDL_Log("Invalid level data");
                return nullptr;
            }

            for (int i = 0; i < width; ++i) {
                levelData[row][i] = tiles[i];
            }
        }

        ++row;
    }

    // Close the file
    file.close();

    return levelData;
}

void Game::RunLoop()
{
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                Quit();
                break;
            }
            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_F11) {
                    mIsFullscreen = !mIsFullscreen;
                    if (mIsFullscreen) {
                        SDL_SetWindowFullscreen(
                            mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    } else {
                        SDL_SetWindowFullscreen(mWindow, 0);
                        // Restore window position to center
                        SDL_DisplayMode displayMode;
                        if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
                            int windowX = (displayMode.w - mWindowWidth) / 2;
                            int windowY = (displayMode.h - mWindowHeight) / 2;
                            SDL_SetWindowPosition(mWindow, windowX, windowY);
                        }
                    }
                }

                // Check if the Return key has been pressed to pause/unpause the game
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    TogglePause();
                }

                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                HandleKeyPressActors(event.key.keysym.sym,
                                     event.key.repeat == 0);

                break;
            }
        }
    }

    ProcessInputActors();
}

void Game::ProcessInputActors()
{
    if (mGamePlayState == GamePlayState::Playing) {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth,
                                           mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool isAerisOnCamera = false;
        for (auto actor : actorsOnCamera) {
            actor->ProcessInput(state);

            if (actor == mAeris) {
                isAerisOnCamera = true;
            }
        }

        // If Aeris is not on camera, process input for her
        if (!isAerisOnCamera && mAeris) {
            mAeris->ProcessInput(state);
        }
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed)
{
    if (mGamePlayState == GamePlayState::Playing) {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth,
                                           mWindowHeight);

        // Handle key press for actors
        bool isAerisOnCamera = false;
        for (auto actor : actorsOnCamera) {
            actor->HandleKeyPress(key, isPressed);

            if (actor == mAeris) {
                isAerisOnCamera = true;
            }
        }

        // If Aeris is not on camera, handle key press for her
        if (!isAerisOnCamera && mAeris) {
            mAeris->HandleKeyPress(key, isPressed);
        }
    }
}

void Game::LoadPauseMenu()
{
    auto pauseMenu = new UIScreen(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf",
                                  UIScreen::UIType::PauseMenu);

    // Black background
    pauseMenu->AddRect(Vector2(0, 0), Vector2(mWindowWidth, mWindowHeight),
                       Vector3(0, 0, 0), 210);

    // Shadow
    pauseMenu->AddText("PAUSED",
                       Vector2(mWindowWidth / 2.0f - 118 + 4,
                               100 + 4),
                       Vector2(236, 79), 64, 1024,
                       Vector3(0, 0, 0));
    pauseMenu->AddText("PAUSED",
                       Vector2(mWindowWidth / 2.0f - 118,
                               100),
                       Vector2(236, 79), 64, 1024,
                       Vector3(1, 1, 1));

    pauseMenu->AddTextButton("Resume",
                             Vector2(mWindowWidth / 2.0f - 106, 220),
                             Vector2(212.0f, 52.0f), [this]() {
                                 TogglePause();
                             }, Vector2(106, 36), 28, 1024, Color::White);

    pauseMenu->AddTextButton("Restart Level",
                             Vector2(mWindowWidth / 2.0f - 177, 270),
                             Vector2(354.0f, 52.0f), [this]() {
                                 RemoveCurrentLevelPowerUp();
                                 ResetGameScene();
                                 SetIsThereCheckPoint(false);
                             }, Vector2(177, 36), 28, 1024, Color::White);

    pauseMenu->AddTextButton("Return to Main Menu",
                             Vector2(mWindowWidth / 2.0f - 280, 340),
                             Vector2(560.0f, 52.0f), [this]() {
                                 SetBackgroundImage("../Assets/UI/background_menu.png",
                                    Vector2(0, 0), Vector2(1280, 720));
                                 RemoveAllPowerUps();
                                 SetIsThereCheckPoint(false);
                                 if (mAeris) {
                                    mAeris->SetState(ActorState::Destroy);
                                 }
                                 SetGameScene(GameScene::MainMenu);
                             }, Vector2(280, 36), 28, 1024, Color::White);
}



void Game::TogglePause()
{
    if (mGameScene != GameScene::MainMenu) {
        if (mGamePlayState == GamePlayState::Playing) {
            mGamePlayState = GamePlayState::Paused;
            mAudio->PauseSound(mMusicHandle);
            mAudio->PlaySound("tap.wav");
            LoadPauseMenu();
        } else if (mGamePlayState == GamePlayState::Paused) {
            mGamePlayState = GamePlayState::Playing;
            mUIStack.pop_back();
            mAudio->PlaySound("tap.wav");
            mAudio->ResumeSound(mMusicHandle);
        }
    }
}

void Game::LoadFragmentCollectedScreen(Fragment::FragmentType type)
{
    mGamePlayState = GamePlayState::Paused;
    mAudio->PauseSound(mMusicHandle);

    auto fragmentCollected = new UIScreen(this, "../Assets/Fonts/SpaceGrotesk-Medium.ttf",
                                          UIScreen::UIType::FragmentCollected);

    // Fundo preto semi-transparente
    fragmentCollected->AddRect(Vector2(0, 0), Vector2(mWindowWidth, mWindowHeight),
                               Vector3(0, 0, 0), 210);

    // Texto do fragmento
    std::string fragmentText;
    if (type == Fragment::FragmentType::DoubleJump) {
        fragmentText = "Aeris found a fragment of the sky!\nNow you can double jump\nDouble press the jump button to use it!";
    } else if (type == Fragment::FragmentType::Dash) {
        fragmentText = "Aeris found a fragment of the sky!\nNow you can dash\nPress the shift key to use it!";
    } else if (type == Fragment::FragmentType::WallJump) {
        fragmentText = "Aeris found a fragment of the sky!\nNow you can wall jump\nJump on a special wall to check it out!";
    }

    // Tamanho da caixa de texto (largura de 700, altura estimada de 150)
    float textWidth = 700.0f;
    float textHeight = 150.0f;

    // Posição centralizada
    Vector2 textPos = Vector2(
        (mWindowWidth - textWidth) / 2.0f,
        (mWindowHeight - textHeight) / 2.0f
    );

    fragmentCollected->AddText(fragmentText,
                               textPos,
                               Vector2(textWidth, textHeight), // box size
                               32,         // font size
                               1024,       // wrap width
                               Vector3(1, 1, 1)); // white
}


void Game::ToggleFragmentCollected(Fragment::FragmentType type)
{
    mGamePlayState = GamePlayState::Paused;
    mAudio->PauseSound(mMusicHandle);
    LoadFragmentCollectedScreen(type);
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    if (mIsIntroductionScreenRunning) {
        mIntroductionTimer -= deltaTime;
        if (mIntroductionTimer <= 0.0f) {
            mIsIntroductionScreenRunning = false;
            mIntroductionTimer = 0.0f;
            SetGameScene(GameScene::Level1, 2.0f);
        }
    }

    if (mIsEndGameScreenRunning) {
        mEndGameTimer -= deltaTime;
        if (mEndGameTimer <= 0.0f) {
            mIsEndGameScreenRunning = false;
            mEndGameTimer = 0.0f;
            SetBackgroundImage("../Assets/UI/background_menu.png",
                                    Vector2(0, 0), Vector2(1280, 720));
            RemoveAllPowerUps();
            SetIsThereCheckPoint(false);
            if (mAeris) {
                mAeris->SetState(ActorState::Destroy);
            }
            SetGameScene(GameScene::MainMenu);
        }
    }

    if (mGamePlayState != GamePlayState::Paused && mGamePlayState !=
        GamePlayState::GameOver) {
        // Reinsert all actors and pending actors
        UpdateActors(deltaTime);
    }

    // Reinsert audio system
    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    // ---------------------
    // Game Specific Updates
    // ---------------------
    UpdateCamera();

    UpdateSceneManager(deltaTime);
}

void Game::UpdateSceneManager(float deltaTime)
{
    if (mSceneManagerState == SceneManagerState::Entering) {
        mSceneManagerTimer -= deltaTime;
        mAlpha = Math::Clamp(
            1.0f - (mSceneManagerTimer / (TRANSITION_TIME / 2.0f)),
            0.0f, 1.0f);
        if (mSceneManagerTimer <= 0.0f) {
            mSceneManagerTimer = 0.3f;
            mAlpha = 1.0f;
            mSceneManagerState = SceneManagerState::Active;
        }
    }

    if (mSceneManagerState == SceneManagerState::Exiting) {
        mSceneManagerTimer -= deltaTime;
        mAlpha = Math::Clamp(mSceneManagerTimer / (TRANSITION_TIME / 2.0f),
                             0.0f,
                             1.0f);
        if (mSceneManagerTimer <= 0.0f) {
            mAlpha = 0.0f;
            mSceneManagerState = SceneManagerState::None;
        }
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0.0f) {
            ChangeScene();
            mSceneManagerState = SceneManagerState::Exiting;
            mSceneManagerTimer = TRANSITION_TIME / 2.0f;
        }
    }
}

void Game::UpdateCamera()
{
    if (!mAeris) return;
    float horizontalCameraPos = mAeris->GetPosition().x - (mWindowWidth / 2.0f);
    float verticalCameraPos = mAeris->GetPosition().y - (mWindowHeight / 2.0f);
    float maxHorizontalCameraPos = (LEVEL_WIDTH * TILE_SIZE) - mWindowWidth;
    float maxVerticalCameraPos = (LEVEL_WIDTH * TILE_SIZE) - mWindowHeight;
    horizontalCameraPos = Math::Clamp(horizontalCameraPos, 0.0f,
                                      maxHorizontalCameraPos);
    verticalCameraPos = Math::Clamp(verticalCameraPos, 0.0f,
                                    maxVerticalCameraPos);
    mCameraPos.x = horizontalCameraPos;
    mCameraPos.y = verticalCameraPos;
}

void Game::UpdateActors(float deltaTime)
{
    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
        mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight);

    bool isAerisOnCamera = false;
    for (auto actor : actorsOnCamera) {
        actor->Update(deltaTime);
        if (actor == mAeris) {
            isAerisOnCamera = true;
        }
    }

    // If Aeris is not on camera, reset camera position
    if (!isAerisOnCamera && mAeris) {
        mAeris->Update(deltaTime);
    }

    for (auto actor : actorsOnCamera) {
        if (actor->GetState() == ActorState::Destroy) {
            delete actor;
            if (actor == mAeris) {
                mAeris = nullptr;
            }
        }
    }
}

void Game::AddActor(Actor* actor)
{
    mSpatialHashing->Insert(actor);
}

void Game::RemoveActor(Actor* actor)
{
    mSpatialHashing->Remove(actor);
}

void Game::Reinsert(Actor* actor)
{
    mSpatialHashing->Reinsert(actor);
}

std::vector<Actor*> Game::GetNearbyActors(const Vector2& position,
                                          const int range)
{
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent*> Game::GetNearbyColliders(
    const Vector2& position, const int range)
{
    return mSpatialHashing->QueryColliders(position, range);
}

void Game::GenerateOutput()
{
    // Clear frame with background color
    SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.x, mBackgroundColor.y,
                           mBackgroundColor.z, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    // Draw background texture considering camera position
    if (mBackgroundTexture) {
        SDL_Rect dstRect = {
            static_cast<int>(mBackgroundPosition.x - mCameraPos.x),
            static_cast<int>(mBackgroundPosition.y - mCameraPos.y),
            static_cast<int>(mBackgroundSize.x),
            static_cast<int>(mBackgroundSize.y)};

        SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &dstRect);
    }

    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
        mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight);

    // Get list of drawables in draw order
    std::vector<DrawComponent*> drawables;

    for (auto actor : actorsOnCamera) {
        auto drawable = actor->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible()) {
            drawables.emplace_back(drawable);
        }
    }

    // Sort drawables by draw order
    std::sort(drawables.begin(), drawables.end(),
              [](const DrawComponent* a, const DrawComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });

    // Draw all drawables
    for (auto drawable : drawables) {
        drawable->Draw(mRenderer, mModColor);
    }

    // Draw all UI screens
    for (auto ui : mUIStack) {
        ui->Draw(mRenderer);
    }

    if (mSceneManagerState != SceneManagerState::None && mAlpha > 0.0f) {
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0,
                               static_cast<Uint8>(255.0f * mAlpha));
        SDL_Rect fullscreenRect = {0, 0, mWindowWidth, mWindowHeight};
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(mRenderer, &fullscreenRect);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::SetBackgroundImage(const std::string& texturePath,
                              const Vector2& position, const Vector2& size)
{
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Load background texture
    mBackgroundTexture = LoadTexture(texturePath);
    if (!mBackgroundTexture) {
        SDL_Log("Failed to load background texture: %s", texturePath.c_str());
    }

    // Set background position
    mBackgroundPosition.Set(position.x, position.y);

    // Set background size
    mBackgroundSize.Set(size.x, size.y);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}

SDL_Texture* Game::GetCachedTexture(const std::string& texturePath)
{
    // Check if texture is already cached
    auto cachedTexture = mTextureCache.find(texturePath);
    if (cachedTexture != mTextureCache.end()) {
        return cachedTexture->second;
    }

    // Load texture from file
    SDL_Texture* texture = LoadTexture(texturePath);
    if (texture) {
        // Cache the texture for future use
        mTextureCache[texturePath] = texture;
    }

    return texture;
}

void Game::ClearLevelDataCache()
{
    // Clean up cached level data
    for (auto& pair : mLevelDataCache) {
        int** levelData = pair.second;
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            delete[] levelData[i];
        }
        delete[] levelData;
    }
    mLevelDataCache.clear();
}

void Game::ClearLevelObjectsDataCache()
{
    // Clean up cached level objects data
    for (auto& pair : mLevelObjectsDataCache) {
        int** levelObjectsData = pair.second;
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            delete[] levelObjectsData[i];
        }
        delete[] levelObjectsData;
    }
    mLevelObjectsDataCache.clear();
}

UIFont* Game::LoadFont(const std::string& fileName)
{
    auto fontIsLoaded = mFonts.find(fileName);
    if (fontIsLoaded != mFonts.end()) {
        return fontIsLoaded->second;
    }

    UIFont* newFont = new UIFont(mRenderer);
    if (newFont->Load(fileName)) {
        mFonts[fileName] = newFont;
        return newFont;
    } else {
        newFont->Unload();
        delete newFont;
        return nullptr;
    }
}

void Game::UnloadScene()
{
    // Delete actors
    delete mSpatialHashing;

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete background texture
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::Shutdown()
{
    UnloadScene();

    // Clean up level data cache
    ClearLevelDataCache();
    ClearLevelObjectsDataCache();

    // Clean up texture cache
    for (auto& pair : mTextureCache) {
        SDL_DestroyTexture(pair.second);
    }
    mTextureCache.clear();

    for (auto font : mFonts) {
        font.second->Unload();
        delete font.second;
    }
    mFonts.clear();

    delete mAudio;
    mAudio = nullptr;

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::SaveAerisPowerUps()
{
    if (mAeris) {
        // Save current power-up states from Aeris
        mPersistentDoubleJump = mAeris->HasUnlockedDoubleJump();
        mPersistentDash = mAeris->HasUnlockedDash();
        mPersistentWallJump = mAeris->HasUnlockedWallJump();
    }
}

void Game::RemoveAllPowerUps()
{
    mPersistentDoubleJump = false;
    mPersistentDash = false;
    mPersistentWallJump = false;

    if (mAeris) {
        mAeris->SetUnlockedDoubleJump(false);
        mAeris->SetUnlockedDash(false);
        mAeris->SetUnlockedWallJump(false);
    }
}

void Game::RestoreAerisPowerUps()
{
    if (mAeris) {
        // Restore saved power-up states to new Aeris
        if (mPersistentDoubleJump) {
            mAeris->SetUnlockedDoubleJump(true);
        }
        if (mPersistentDash) {
            mAeris->SetUnlockedDash(true);
        }
        if (mPersistentWallJump) {
            mAeris->SetUnlockedWallJump(true);
        }
    }
    
    // Also restore HUD fragment states to match the power-ups
    if (mHUD) {
        mHUD->RestoreFragmentStates(mPersistentDoubleJump, mPersistentDash, mPersistentWallJump);
    }
}

void Game::RemoveCurrentLevelPowerUp()
{
    // Set flag to indicate this is a death reset (to avoid saving power-ups in ChangeScene)
    mIsDeathReset = true;
    
    // Remove the power-up for the current level if it was collected
    switch (mGameScene) {
        case GameScene::Level1:
            // Level 1 has Double Jump power-up
            if (mPersistentDoubleJump) {
                mPersistentDoubleJump = false;
                if (mAeris) {
                    mAeris->SetUnlockedDoubleJump(false);
                }
                if (mHUD) {
                    mHUD->RemoveFragmentFromDisplay(Fragment::FragmentType::DoubleJump);
                }
                SDL_Log("Removed Double Jump power-up on death in Level 1");
            }
            break;
        case GameScene::Level2:
            // Level 2 has Dash power-up
            if (mPersistentDash) {
                mPersistentDash = false;
                if (mAeris) {
                    mAeris->SetUnlockedDash(false);
                }
                if (mHUD) {
                    mHUD->RemoveFragmentFromDisplay(Fragment::FragmentType::Dash);
                }
                SDL_Log("Removed Dash power-up on death in Level 2");
            }
            break;
        case GameScene::Level3:
            // Level 3 has Wall Jump power-up
            if (mPersistentWallJump) {
                mPersistentWallJump = false;
                if (mAeris) {
                    mAeris->SetUnlockedWallJump(false);
                }
                if (mHUD) {
                    mHUD->RemoveFragmentFromDisplay(Fragment::FragmentType::WallJump);
                }
                SDL_Log("Removed Wall Jump power-up on death in Level 3");
            }
            break;
        default:
            // No power-up removal for MainMenu or other scenes
            break;
    }
}
