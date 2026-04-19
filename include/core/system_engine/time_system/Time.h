#pragma once

#include <SDL.h>

class Time {
private:
    static Uint64 lastTick;
    static Uint64 frequency;
    static float deltaTime;
    static float timeScale;
    static float unscaledDeltaTime;
    static float timeSinceStart;
    static Uint32 frameCount;
    static bool initialized;

public:
    static void Initialize();
    
    static void Tick();
    
    static float DeltaTime();
    static float UnscaledDeltaTime();
    static float FixedDeltaTime();
    static float TimeScale();
    static void SetTimeScale(float scale);
    static float TimeSinceStart();
    static Uint32 FrameCount();
    
    static void Wait(Uint32 milliseconds);
    static float GetFPS();
};