#include "Systems/Time/TimeSystem.h"

void TimeSystem::Initialize() {
    if (!initialized) {
        frequency = SDL_GetPerformanceFrequency();
        lastTick = SDL_GetPerformanceCounter();
        initialized = true;
    }
}

float TimeSystem::DeltaTime(){ return deltaTime * timeScale; }
float TimeSystem::UnscaledDeltaTime() { return deltaTime; }
float TimeSystem::FixedDeltaTime() { return 1.0f / 60.0f; }
float TimeSystem::TimeScale() { return timeScale; }
void TimeSystem::SetTimeScale(float scale) { timeScale = scale; }
float TimeSystem::TimeSinceStart() { return timeSinceStart; }
Uint32 TimeSystem::FrameCount() { return frameCount; }

void TimeSystem::Tick() {
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

void TimeSystem::Wait(Uint32 milliseconds) {
    SDL_Delay(milliseconds);
}

float TimeSystem::GetFPS() {
    if (deltaTime > 0.0f) {
        return 1.0f / (unscaledDeltaTime);
    }
    return 0.0f;
}