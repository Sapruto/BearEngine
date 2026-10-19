#pragma once

#include "RHI/Base/Core/BaseContextInstanceRHI.h"

namespace RHI::Vulkan {
    class ContextInstanceVK : public Base::BaseContextInstanceRHI {
    private:
        int glMajor{3};
        int glMinor{3};

    public:
        ContextInstanceVK();
        ~ContextInstanceVK() override = default;

        bool Initialize() override;
        void ChangeGLHints(int newMajor, int newMinor);
        void Shutdown() override;

        int GetMajor() { return glMajor; }
        int GetMinor() { return glMinor; }
    };
}