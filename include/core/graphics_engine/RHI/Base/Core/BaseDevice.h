#pragma once

#include <memory>
#include <any>
#include "RHI/Base/Core/BaseWindow.h"

class BaseDevice {
protected:
    std::shared_ptr<BaseWindow> window;
    std::any nativeDevice;
    
public:
    explicit BaseDevice(std::shared_ptr<BaseWindow> win) : window(win) {}
    virtual ~BaseDevice();
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void Present() = 0;
    
    std::shared_ptr<BaseWindow> GetWindow() const { return window; }
    const std::any& GetNativeDevice() const { return nativeDevice; }
};