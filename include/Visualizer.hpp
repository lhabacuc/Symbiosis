#ifndef VISUALIZER_HPP
# define VISUALIZER_HPP

extern "C"
{
# include "mlx.h"
}

class Visualizer
{
    private:
        void    *_mlx;
        void    *_win;
        void    *_img;
        char    *_imgData;
        int     _width;
        int     _height;
        int     _bpp;
        int     _lineLen;
        int     _endian;

    public:
        Visualizer(int width, int height, const char *title);
        ~Visualizer();
        Visualizer(const Visualizer &other) = delete;
        Visualizer &operator=(const Visualizer &other) = delete;

        int getWidth() const;
        int getHeight() const;
        void clear();
        void putPixel(int x, int y, int color);
        void drawRect(int x, int y, int w, int h, int color);
        void drawText(int x, int y, int color, const char *text);
        void render();
        void loopHook(int (*f)(void *param), void *param);
        void mouseHook(int (*f)(int button, int x, int y, void *param), void *param);
        void run();
        static int closeWindow(void *param);
};

#endif
