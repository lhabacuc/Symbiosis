#include "Simulation.hpp"
#include "raylib.h"
#include <algorithm>

Simulation::Simulation(Visualizer &viz, const Mapa &mapa)
    : _viz(viz), _ecosystem(mapa), _ui(viz, SIDEBAR_WIDTH),
      _pausado(false), _acumulador(0.0f)
{
    _ui.aplicarEstilo();
}

void Simulation::draw()
{
    int playWidth = _ecosystem.getPlayWidth();

    _viz.updateCamera(static_cast<float>(playWidth), static_cast<float>(_viz.getHeight()));
    _viz.beginScissorCamera(0, 0, playWidth, _viz.getHeight());

    const std::vector<Item> &comida = _ecosystem.getComida();
    for (size_t i = 0; i < comida.size(); i++)
        _viz.drawRect(comida[i].x - 2, comida[i].y - 2, 4, 4, 0xFFFF00);

    const std::vector<Item> &veneno = _ecosystem.getVeneno();
    for (size_t i = 0; i < veneno.size(); i++)
        _viz.drawRect(veneno[i].x - 2, veneno[i].y - 2, 4, 4, 0xAA00FF);

    const std::vector<Bacteria> &bacterias = _ecosystem.getBacterias();
    for (size_t i = 0; i < bacterias.size(); i++)
    {
        const Bacteria &b = bacterias[i];
        int intensidade = (int)(b.getEnergy() * 2.0f);
        if (intensidade > 255)
            intensidade = 255;
        if (intensidade < 50)
            intensidade = 50;
        unsigned int color = (unsigned int)(intensidade) << 8;

        if (_ecosystem.desenhoDetalhado())
        {
            float raio = 2.2f + std::min(3.0f, b.getEnergy() / 60.0f);
            _viz.drawBacteria(static_cast<float>(b.getX()), static_cast<float>(b.getY()), raio, color);
        }
        else
        {
            _viz.drawRect(b.getX() - 1, b.getY() - 1, 3, 3, color);
        }
    }

    _viz.endScissorCamera();

    _viz.drawLine(playWidth, 0, playWidth, _viz.getHeight(), 0x666666);
}

void Simulation::tick()
{
    _ecosystem.sincronizarPopulacao();

    float dt = _ecosystem.config().velocidadeMs / 1000.0f;
    _acumulador += GetFrameTime();

    // nunca deixar mais do que N passos por frame por tras: evita a "espiral da morte"
    // em que um update() lento faz o acumulador crescer, obrigando a mais updates() no
    // frame seguinte, o que o torna ainda mais lento, e por ai fora ate travar a app.
    const int MAX_PASSOS_POR_FRAME = 3;
    int passos = 0;
    while (_acumulador >= dt && passos < MAX_PASSOS_POR_FRAME)
    {
        if (!_pausado)
            _ecosystem.update();
        _acumulador -= dt;
        passos++;
    }
    if (_acumulador > dt)
        _acumulador = dt;

    _viz.beginFrame();
    draw();
    _ui.draw(_ecosystem, _pausado);
    _viz.endFrame();
}
