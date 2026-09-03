#ifndef VISUALIZER_HPP
# define VISUALIZER_HPP

# include "raylib.h"

class Visualizer
{
    private:
        int      _width;
        int      _height;
        Camera2D _camera;

    public:
        Visualizer(int width, int height, const char *title);
        ~Visualizer();
        Visualizer(const Visualizer &other) = delete;
        Visualizer &operator=(const Visualizer &other) = delete;

        int getWidth() const;
        int getHeight() const;

        bool shouldClose() const;
        void beginFrame();
        void endFrame();

        void drawRect(int x, int y, int w, int h, unsigned int colorRGB);
        void drawText(int x, int y, unsigned int colorRGB, const char *text);
        void drawLine(int x1, int y1, int x2, int y2, unsigned int colorRGB);
        void drawBacteria(float x, float y, float radius, unsigned int colorRGB);

        void setCameraCenter(float x, float y);
        void updateCamera(float regiaoW, float regiaoH);
        void resetCamera(float regiaoW, float regiaoH);
        float getCameraZoom() const;
        void beginScissorCamera(int x, int y, int w, int h);
        void endScissorCamera();

        bool mouseLeftPressed() const;
        int getMouseX() const;
        int getMouseY() const;
};

#endif
