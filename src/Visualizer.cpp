#include "Visualizer.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma GCC diagnostic pop

static Color toColor(unsigned int rgb)
{
    Color c;
    c.r = (rgb >> 16) & 0xFF;
    c.g = (rgb >> 8) & 0xFF;
    c.b = rgb & 0xFF;
    c.a = 255;
    return c;
}

Visualizer::Visualizer(int width, int height, const char *title)
    : _width(width), _height(height)
{
    InitWindow(_width, _height, title);
    SetTargetFPS(60);

    _camera.offset = { static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f };
    _camera.target = { static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f };
    _camera.rotation = 0.0f;
    _camera.zoom = 1.0f;
}

Visualizer::~Visualizer()
{
    CloseWindow();
}

int Visualizer::getWidth() const
{
    return _width;
}

int Visualizer::getHeight() const
{
    return _height;
}

bool Visualizer::shouldClose() const
{
    return WindowShouldClose();
}

void Visualizer::beginFrame(unsigned int corFundoRGB)
{
    BeginDrawing();
    ClearBackground(toColor(corFundoRGB));
}

void Visualizer::endFrame()
{
    EndDrawing();
}

void Visualizer::drawRect(int x, int y, int w, int h, unsigned int colorRGB)
{
    DrawRectangle(x, y, w, h, toColor(colorRGB));
}

void Visualizer::drawText(int x, int y, unsigned int colorRGB, const char *text)
{
    DrawText(text, x, y, 18, toColor(colorRGB));
}

void Visualizer::drawLine(int x1, int y1, int x2, int y2, unsigned int colorRGB)
{
    DrawLine(x1, y1, x2, y2, toColor(colorRGB));
}

void Visualizer::drawBacteria(float x, float y, float radius, unsigned int colorRGB)
{
    Color corBase = toColor(colorRGB);
    Color corBorda = ColorBrightness(corBase, -0.45f);
    Color corBrilho = Fade(WHITE, 0.55f);
    Color corNucleo = ColorBrightness(corBase, -0.55f);

    DrawCircleV({x, y}, radius, corBase);
    DrawCircleLinesV({x, y}, radius, corBorda);
    DrawCircleV({x - radius * 0.35f, y - radius * 0.35f}, radius * 0.32f, corBrilho);
    DrawCircleV({x + radius * 0.25f, y + radius * 0.15f}, radius * 0.28f, corNucleo);
}

void Visualizer::drawZonaTemperatura(float x, float y, float raio, unsigned int colorRGB, float alpha)
{
    DrawCircleV({x, y}, raio, Fade(toColor(colorRGB), alpha));
}

void Visualizer::drawRectAlpha(int x, int y, int w, int h, unsigned int colorRGB, float alpha)
{
    DrawRectangle(x, y, w, h, Fade(toColor(colorRGB), alpha));
}

void Visualizer::drawGrafico(float x, float y, float w, float h, const std::vector<float> &valores, unsigned int colorRGB)
{
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h), Fade(BLACK, 0.35f));
    DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h), Fade(WHITE, 0.25f));

    if (valores.size() < 2)
        return;

    float maxValor = valores[0];
    for (size_t i = 1; i < valores.size(); i++)
        if (valores[i] > maxValor)
            maxValor = valores[i];
    if (maxValor < 1.0f)
        maxValor = 1.0f;

    std::vector<Vector2> pontos;
    pontos.reserve(valores.size());
    for (size_t i = 0; i < valores.size(); i++)
    {
        float px = x + (w * static_cast<float>(i)) / static_cast<float>(valores.size() - 1);
        float py = y + h - (valores[i] / maxValor) * h;
        pontos.push_back({px, py});
    }

    DrawLineStrip(pontos.data(), static_cast<int>(pontos.size()), toColor(colorRGB));
}

void Visualizer::setCameraCenter(float x, float y)
{
    _camera.target = {x, y};
    _camera.offset = {x, y};
}

void Visualizer::updateCamera(float regiaoW, float regiaoH)
{
    Vector2 mouse = GetMousePosition();
    bool dentro = mouse.x >= 0.0f && mouse.x <= regiaoW && mouse.y >= 0.0f && mouse.y <= regiaoH;

    if (dentro)
    {
        float roda = GetMouseWheelMove();
        if (roda != 0.0f)
        {
            Vector2 mundoAntes = GetScreenToWorld2D(mouse, _camera);
            _camera.zoom += roda * 0.1f * _camera.zoom;
            if (_camera.zoom < 0.3f)
                _camera.zoom = 0.3f;
            if (_camera.zoom > 8.0f)
                _camera.zoom = 8.0f;
            Vector2 mundoDepois = GetScreenToWorld2D(mouse, _camera);
            _camera.target.x += mundoAntes.x - mundoDepois.x;
            _camera.target.y += mundoAntes.y - mundoDepois.y;
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            Vector2 delta = GetMouseDelta();
            _camera.target.x -= delta.x / _camera.zoom;
            _camera.target.y -= delta.y / _camera.zoom;
        }
    }
}

void Visualizer::resetCamera(float regiaoW, float regiaoH)
{
    _camera.target = {regiaoW / 2.0f, regiaoH / 2.0f};
    _camera.offset = {regiaoW / 2.0f, regiaoH / 2.0f};
    _camera.zoom = 1.0f;
}

float Visualizer::getCameraZoom() const
{
    return _camera.zoom;
}

void Visualizer::beginScissorCamera(int x, int y, int w, int h)
{
    BeginScissorMode(x, y, w, h);
    BeginMode2D(_camera);
}

void Visualizer::endScissorCamera()
{
    EndMode2D();
    EndScissorMode();
}

bool Visualizer::mouseLeftPressed() const
{
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

int Visualizer::getMouseX() const
{
    return GetMouseX();
}

int Visualizer::getMouseY() const
{
    return GetMouseY();
}
