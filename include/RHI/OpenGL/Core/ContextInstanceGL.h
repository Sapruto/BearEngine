#pragma once

#include "RHI/Base/Core/BaseContextInstanceRHI.h"

namespace RHI::OpenGL {
    class ContextInstanceGL : public Base::BaseContextInstanceRHI {
    private:
        int glMajor{4};
        int glMinor{5};

    public:
        ContextInstanceGL();
        ~ContextInstanceGL() override = default;

        bool Initialize() override;
        void ChangeGLHints(int newMajor, int newMinor);
        void Shutdown() override;

        int GetMajor() { return glMajor; }
        int GetMinor() { return glMinor; }
    };
}