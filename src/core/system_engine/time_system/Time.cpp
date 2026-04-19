#include "Time.h"

#include "SDL.h"
#include <cstdint>

Uint64 Time::lastTick = 0;
Uint64 Time::frequency = 0;
float Time::deltaTime = 0.0f;
float Time::timeScale = 1.0f;
float Time::unscaledDeltaTime = 0.0f;
float Time::timeSinceStart = 0.0f;
Uint32 Time::frameCount = 0;
bool Time::initialized = false;

void Time::Initialize() {
    if (!initialized) {
        frequency = SDL_GetPerformanceFrequency();
        lastTick = SDL_GetPerformanceCounter();
        initialized = true;
    }
}

float Time::DeltaTime(){ return deltaTime * timeScale; }
float Time::UnscaledDeltaTime() { return deltaTime; }
float Time::FixedDeltaTime() { return 1.0f / 60.0f; }
float Time::TimeScale() { return timeScale; }
void Time::SetTimeScale(float scale) { timeScale = scale; }
float Time::TimeSinceStart() { return timeSinceStart; }
Uint32 Time::FrameCount() { return frameCount; }

void Time::Tick() {
    if (!initialized) Initialize();
    
    Uint64 currentTick = SDL_GetPerformanceCounter();
    
    float diff = static_cast<float>(currentTick - lastTick);
    if (diff < 0.001f) diff = 0.001f;
    
    unscaledDeltaTime = diff / frequency;
    deltaTime = unscaledDeltaTime;
    
    timeSinceStart += unscaledDeltaTime;
    frameCount++;
    
    lastTick = currentTick;
}

void Time::Wait(Uint32 milliseconds) {
    SDL_Delay(milliseconds);
}

float Time::GetFPS() {
    if (deltaTime > 0.0f) {
        return 1.0f / (unscaledDeltaTime);
    }
    return 0.0f;
}