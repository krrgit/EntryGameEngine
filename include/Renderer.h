#pragma once

namespace Entry
{
    enum class RendererAPI
    {
        None = 0,
        Citro3D = 1
    };

    class Renderer
    {
    public:
        inline static RendererAPI GetAPI()
        {
            return s_RendererAPI;
        }

    private:
        static RendererAPI s_RendererAPI;
    };
}