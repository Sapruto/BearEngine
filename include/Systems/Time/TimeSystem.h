#pragma once

#include "Systems/Core/ISystem.h"

#include <SDL2/SDL.h>

class TimeSystem : public Systems::BaseSystem<TimeSystem> {
private:
    Uint64 lastTick = 0;
    Uint64 frequency = 0;
    float deltaTime = 0.0f;
    float timeScale = 1.0f;
    float unscaledDeltaTime = 0.0f;
    float timeSinceStart = 0.0f;
    Uint32 frameCount = 1;
    bool initialized = false;

public:
    void Initialize();
    
    void Tick();
    
    float DeltaTime();
    float UnscaledDeltaTime();
    float FixedDeltaTime();
    float TimeScale();
    void SetTimeScale(float scale);
    float TimeSinceStart();
    Uint32 FrameCount();
    
    void Wait(Uint32 milliseconds);
    float GetFPS();

    static std::string_view GetSystemNameStatic() { return std::string_view("time_system"); }
};