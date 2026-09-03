#include "Visualizer.hpp"
#include <cstdlib>
#include <cstring>

Visualizer::Visualizer(int width, int height, const char *title)
    : _width(width), _height(height)
{
    _mlx = mlx_init();
    _win = mlx_new_window(_mlx, _width, _height, const_cast<char *>(title));
    _img = mlx_new_image(_mlx, _width, _height);
    _imgData = mlx_get_data_addr(_img, &_bpp, &_lineLen, &_endian);
    mlx_hook(_win, 17, 0, (int (*)())&Visualizer::closeWindow, this);
}

Visualizer::~Visualizer()
{
    if (_img)
        mlx_destroy_image(_mlx, _img);
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
    memset(_imgData, 0, static_cast<size_t>(_lineLen) * static_cast<size_t>(_height));
}

void Visualizer::putPixel(int x, int y, int color)
{
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return;
    char *pixel = _imgData + (y * _lineLen + x * (_bpp / 8));
    *reinterpret_cast<int *>(pixel) = color;
}

void Visualizer::render()
{
    mlx_put_image_to_window(_mlx, _win, _img, 0, 0);
}

void Visualizer::loopHook(int (*f)(void *param), void *param)
{
    mlx_loop_hook(_mlx, (int (*)())f, param);
}

void Visualizer::run()
{
    mlx_loop(_mlx);
}
