#pragma once

class BaseContextInstanceRHI {
protected:
    bool initialized{false};
    bool vsync{true};

public:
    BaseContextInstanceRHI() = default;
    virtual ~BaseContextInstanceRHI() = default;

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    bool IsInitialized() const { return initialized; }
    bool IsVSync() const { return vsync; }
};