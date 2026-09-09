#include "Simulation.hpp"
#include "raylib.h"
#include <algorithm>

// mapeia uma temperatura para uma cor: azulado no frio, avermelhado no quente,
// cinzento neutro a volta dos ~20 graus (usado tanto no fundo como nas zonas)
static unsigned int corTemperatura(float temperatura)
{
    float t = (temperatura - 20.0f) / 20.0f;
    if (t < -1.0f)
        t = -1.0f;
    if (t > 1.0f)
        t = 1.0f;

    unsigned char r, g, b;
    if (t < 0.0f)
    {
        float k = -t;
        r = static_cast<unsigned char>(80 + (1.0f - k) * 40);
        g = static_cast<unsigned char>(80 + (1.0f - k) * 40);
        b = static_cast<unsigned char>(80 + k * 175);
    }
    else
    {
        float k = t;
        r = static_cast<unsigned char>(80 + k * 175);
        g = static_cast<unsigned char>(80 + (1.0f - k) * 40);
        b = static_cast<unsigned char>(80 + (1.0f - k) * 40);
    }
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
}

static unsigned int escurecer(unsigned int colorRGB, float fator)
{
    unsigned char r = static_cast<unsigned char>(((colorRGB >> 16) & 0xFF) * fator);
    unsigned char g = static_cast<unsigned char>(((colorRGB >> 8) & 0xFF) * fator);
    unsigned char b = static_cast<unsigned char>((colorRGB & 0xFF) * fator);
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
}

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

    const std::vector<ZonaTemperatura> &zonas = _ecosystem.getZonasTemperatura();
    for (size_t i = 0; i < zonas.size(); i++)
    {
        const ZonaTemperatura &zona = zonas[i];
        _viz.drawZonaTemperatura(static_cast<float>(zona.x), static_cast<float>(zona.y),
            static_cast<float>(zona.raio), corTemperatura(zona.valor), 0.35f);
    }

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

    unsigned int corFundo = escurecer(corTemperatura(_ecosystem.config().temperaturaAmbiente), 0.3f);
    _viz.beginFrame(corFundo);
    draw();
    _ui.draw(_ecosystem, _pausado);
    _viz.endFrame();
}
