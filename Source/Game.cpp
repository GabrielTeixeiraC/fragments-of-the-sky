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
#include <SDL2/SDL_image.h>
#include "CSV.h"
#include "Random.h"
#include "Game.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"
#include "DebugManager.h"

Game::Game(int windowWidth, int windowHeight)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mLevelData(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mUpdatingActors(false)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
        ,mIsFullscreen(false)
{

}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Get the display bounds
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
    {
        SDL_Log("Failed to get display mode: %s", SDL_GetError());
        return false;
    }

    // Calculate window position to center it
    int windowX = (displayMode.w - mWindowWidth) / 2;
    int windowY = (displayMode.h - mWindowHeight) / 2;

    mWindow = SDL_CreateWindow("Fragments of the Sky", windowX, windowY, mWindowWidth, mWindowHeight, 0);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    Random::Init();

    mTicksCount = SDL_GetTicks();

    // Init all game actors
    InitializeActors();

    return true;
}

void Game::InitializeActors()
{
    mLevelData = LoadLevel("Assets/Levels/Level1-1/level1-1.csv", Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT);
    if (mLevelData == nullptr) {
        SDL_Log("[Game::InitializeActors] Failed to load level file");
        return;
    }
    BuildLevel(mLevelData, Game::LEVEL_WIDTH, Game::LEVEL_HEIGHT);
}

void Game::BuildLevel(int** levelData, int width, int height)
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Block * block;
            Vector2 position;
            position.x = static_cast<float>(j * TILE_SIZE);
            position.y = static_cast<float>(i * TILE_SIZE);
            switch (levelData[i][j]) {
                // TODO: build level from level data
                default:
                    break;
            }
        }
    }
}

int **Game::LoadLevel(const std::string& fileName, int width, int height)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        return nullptr;
    }

    int ** levelData = new int * [height];

    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < height) {
        std::vector<int> values = CSVHelper::Split(line);
        if (values.size() != width) {
            for (int i = 0; i < row; ++i) {
                delete[] levelData[i];
            }
            delete[] levelData;
            return nullptr;
        }

        levelData[row] = new int[width];

        for (int col = 0; col < width; col++) {
            levelData[row][col] = values[col];
        }

        row++;
    }

    if (row != height)
    {
        for (int i = 0; i < row; ++i) {
            delete[] levelData[i];
        }
        delete[] levelData;
        return nullptr;
    }

    return levelData;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_F11)
                {
                    mIsFullscreen = !mIsFullscreen;
                    if (mIsFullscreen)
                    {
                        SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    }
                    else
                    {
                        SDL_SetWindowFullscreen(mWindow, 0);
                        // Restore window position to center
                        SDL_DisplayMode displayMode;
                        if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
                        {
                            int windowX = (displayMode.w - mWindowWidth) / 2;
                            int windowY = (displayMode.h - mWindowHeight) / 2;
                            SDL_SetWindowPosition(mWindow, windowX, windowY);
                        }
                    }
                }
                else if (event.key.keysym.sym == SDLK_F3)
                {
                    DebugManager::GetInstance().ToggleDebugMode();
                }
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();
}

void Game::UpdateCamera()
{
    // TODO: update camera position
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput()
{
    // Set draw color to black
    SDL_SetRenderDrawColor(mRenderer, 107, 140, 255, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    for (auto drawable : mDrawables)
    {
        if (drawable->IsVisible())
        {
            drawable->Draw(mRenderer);
        }
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath) {
    SDL_Surface * surface = IMG_Load(texturePath.c_str());
    if (surface == nullptr) {
        return nullptr;
    }

    SDL_Texture * texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    if (texture == nullptr) {
        return nullptr;
    }

    SDL_FreeSurface(surface);
    return texture;
}

void Game::Shutdown()
{
    // Delete actors
    while (!mActors.empty())
    {
        delete mActors.back();
    }

    // Delete level data
    if (mLevelData != nullptr)
    {
        for (int i = 0; i < LEVEL_HEIGHT; ++i)
        {
            if (mLevelData[i] != nullptr)
                delete[] mLevelData[i];
        }
    }
    delete[] mLevelData;

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
