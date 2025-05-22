#pragma once

/*
    DebugManager is a singleton class that manages the debug mode of the game.
    It is used to toggle the debug mode on and off. Other classes can then
    conditionally run debug behavior or display debug information using
    this globally accessible instance.
 */
class DebugManager
{
public:
    static DebugManager& GetInstance()
    {
        static DebugManager instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;

    bool IsDebugMode() const { return mIsDebugMode; }
    void ToggleDebugMode() { mIsDebugMode = !mIsDebugMode; }

private:
    DebugManager() : mIsDebugMode(false) {} // Private constructor
    bool mIsDebugMode;
}; 