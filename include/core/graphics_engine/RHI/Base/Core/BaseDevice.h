#pragma once

#include <memory>
#include <any>
#include "RHI/Base/Core/BaseWindow.h"

struct ClearColorRHI {
    float r{0.0f}, g{0.0f}, b{0.0f}, a{1.0f};
};

enum class ClearFlagsRHI : unsigned int {
    None = 0,
    Color = 1u << 0,
    Depth = 1u << 1,
    Stencil = 1u << 2,
    ColorDepth = Color | Depth,
    All = Color | Depth | Stencil
};

inline ClearFlagsRHI operator|(ClearFlagsRHI a, ClearFlagsRHI b) {
    return static_cast<ClearFlagsRHI>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}
inline ClearFlagsRHI operator&(ClearFlagsRHI a, ClearFlagsRHI b) {
    return static_cast<ClearFlagsRHI>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
}
inline bool HasFlag(ClearFlagsRHI v, ClearFlagsRHI f) {
    return (static_cast<unsigned>(v) & static_cast<unsigned>(f)) != 0;
}

class BaseDevice {
protected:
    std::shared_ptr<BaseWindow> window;
    std::any nativeDevice;

    ClearColorRHI clearColor{};
    double clearDepthValue{1.0};
    int clearStencilValue{0};
    
public:
    explicit BaseDevice(std::shared_ptr<BaseWindow> win) : window(win) {}
    virtual ~BaseDevice() = default;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    virtual void Present() = 0;

    virtual void Clear(ClearFlagsRHI flags = ClearFlagsRHI::All) = 0;

    void SetClearColor(float r, float g, float b, float a) { clearColor = {r, g, b, a}; }
    void SetClearColor(const ClearColorRHI& c) { clearColor = c; }
    void SetClearDepth(double d) { clearDepthValue = d; }
    void SetClearStencil(int s) { clearStencilValue = s; }

    const ClearColorRHI& GetClearColor() const { return clearColor; }
    double GetClearDepth() const { return clearDepthValue; }
    int GetClearStencil() const { return clearStencilValue; }
    
    std::shared_ptr<BaseWindow> GetWindow() const { return window; }
    const std::any& GetNativeDevice() const { return nativeDevice; }
};