#ifndef NEPBILL_WINDOW_H
#define NEPBILL_WINDOW_H

#include <string>
#include "Maths.h"

struct GLFWwindow;

namespace NepBill
{
    class Window
    {
    public:
        Window() {}
        ~Window() {}

        int Init(const std::string &Name, const IVec2 &Size);
        void Destroy();

        bool ShouldClose();
        void Update();

        void *GetRawWindow() { return (void *)_Window; }

    private:
        GLFWwindow *_Window;
        IVec2 _Size;
        std::string _Name;
    };
} // namespace NepBill

#endif




