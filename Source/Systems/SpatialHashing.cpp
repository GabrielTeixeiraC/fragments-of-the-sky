#include "SpatialHashing.h"
#include <SDL.h>
#include<algorithm>

SpatialHashing::SpatialHashing(int cellSize, int width, int height)
    : mCellSize(cellSize), mWidth(width), mHeight(height)
{
    int cols = (width + cellSize - 1) / cellSize;
    int rows = (height + cellSize - 1) / cellSize;
    mGrid.resize(rows, std::vector<std::vector<Actor*> >(cols));
}

SpatialHashing::~SpatialHashing()
{
    // Delete all actors
    for (auto& row : mGrid) {
        for (auto& cell : row) {
            while (!cell.empty()) {
                delete cell.back();; // Assuming ownership of actors
            }

            cell.clear();
        }
    }

    mGrid.clear();
    mPositions.clear();
    mCellIndices.clear();
}

void SpatialHashing::Insert(Actor* collider)
{
    // Compute positions for each vertex of the collider
    Vector2 position = collider->GetPosition();

    int col = static_cast<int>(position.x / mCellSize);
    int row = static_cast<int>(position.y / mCellSize);

    // Ensure indices are within bounds
    if (col < 0 || col >= mGrid[0].size() || row < 0 || row >= mGrid.size()) {
        return; // Out of bounds, do not insert
    }

    // Insert collider into the grid cell
    mGrid[row][col].push_back(collider);
    mPositions[collider] = position;
    mCellIndices[collider] = std::make_pair(row, col);
}

void SpatialHashing::Remove(Actor* collider)
{
    auto it = mCellIndices.find(collider);
    if (it != mCellIndices.end()) {
        int row = it->second.first;
        int col = it->second.second;

        // Remove the collider from the grid cell
        auto& cell = mGrid[row][col];
        cell.erase(std::remove(cell.begin(), cell.end(), collider), cell.end());

        // Remove from positions and indices maps
        mPositions.erase(collider);
        mCellIndices.erase(it);
    }
}

void SpatialHashing::Reinsert(Actor* actor)
{
    Remove(actor);
    Insert(actor);
}

std::vector<Actor*> SpatialHashing::Query(const Vector2& position,
                                          const int range) const
{
    std::vector<Actor*> results;

    int col = static_cast<int>(position.x / mCellSize);
    int row = static_cast<int>(position.y / mCellSize);

    // Ensure indices are within bounds
    if (col < 0 || col >= mGrid[0].size() || row < 0 || row >= mGrid.size()) {
        return results; // Out of bounds
    }

    // Check the surrounding cells
    for (int r = row - range; r <= row + range; ++r) {
        for (int c = col - range; c <= col + range; ++c) {
            if (r < 0 || r >= mGrid.size() || c < 0 || c >= mGrid[0].size()) {
                continue; // Skip out of bounds cells
            }

            const auto& cell = mGrid[r][c];
            results.insert(results.end(), cell.begin(), cell.end());
        }
    }

    return results;
}

std::vector<AABBColliderComponent*> SpatialHashing::QueryColliders(
    const Vector2& position, const int range) const
{
    std::vector<AABBColliderComponent*> results;

    std::vector<Actor*> actors = Query(position, range);
    for (Actor* actor : actors) {
        auto collider = actor->GetComponent<AABBColliderComponent>();
        if (collider) {
            results.push_back(collider);
        }
    }

    return results;
}

std::vector<Actor*> SpatialHashing::QueryOnCamera(const Vector2& cameraPosition,
                                                  const float screenWidth,
                                                  const float screenHeight,
                                                  const float extraRadius) const
{
    std::vector<Actor*> results;

    // Get the camera vertices
    Vector2 topLeft = Vector2(cameraPosition.x - extraRadius,
                              cameraPosition.y - extraRadius);
    Vector2 bottomRight = Vector2(cameraPosition.x + screenWidth + extraRadius,
                                  cameraPosition.y + screenHeight +
                                  extraRadius);

    // Calculate the grid cells that the camera covers
    int startCol = static_cast<int>(topLeft.x / mCellSize);
    int startRow = static_cast<int>(topLeft.y / mCellSize);
    int endCol = static_cast<int>(bottomRight.x / mCellSize);
    int endRow = static_cast<int>(bottomRight.y / mCellSize);

    // Ensure indices are within bounds
    startCol = std::max(0, startCol);
    startRow = std::max(0, startRow);
    endCol = std::min(static_cast<int>(mGrid[0].size()) - 1, endCol);
    endRow = std::min(static_cast<int>(mGrid.size()) - 1, endRow);

    // Check the cells within the camera bounds
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            const auto& cell = mGrid[r][c];
            results.insert(results.end(), cell.begin(), cell.end());
        }
    }

    return results;
}