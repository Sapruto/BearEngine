#pragma once

namespace RHI::Base {
    class BaseContextInstanceRHI {
    protected:
        bool initialized = false;

    public:
        BaseContextInstanceRHI() = default;
        virtual ~BaseContextInstanceRHI() = default;

        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;

        bool IsInitialized() const { return initialized; }
    };
}