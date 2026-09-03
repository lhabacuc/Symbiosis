#include "Visualizer.hpp"
#include <cstdlib>

Visualizer::Visualizer(int width, int height, const char *title)
    : _width(width), _height(height)
{
    _mlx = mlx_init();
    _win = mlx_new_window(_mlx, _width, _height, const_cast<char *>(title));
    mlx_hook(_win, 17, 0, (int (*)())&Visualizer::closeWindow, this);
}

Visualizer::~Visualizer()
{
    if (_win)
        mlx_destroy_window(_mlx, _win);
    if (_mlx)
        free(_mlx);
}

int Visualizer::closeWindow(void *param)
{
    (void)param;
    exit(0);
    return 0;
}

int Visualizer::getWidth() const
{
    return _width;
}

int Visualizer::getHeight() const
{
    return _height;
}

void Visualizer::clear()
{
    mlx_clear_window(_mlx, _win);
}

void Visualizer::putPixel(int x, int y, int color)
{
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return;
    mlx_pixel_put(_mlx, _win, x, y, color);
}

void Visualizer::loopHook(int (*f)(void *param), void *param)
{
    mlx_loop_hook(_mlx, (int (*)())f, param);
}

void Visualizer::run()
{
    mlx_loop(_mlx);
}
