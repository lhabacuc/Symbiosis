#include "Simulation.hpp"
#include "raylib.h"
#include "raygui.h"
#include <algorithm>
#include <cstdio>

Simulation::Simulation(Visualizer &viz, const Mapa &mapa)
    : _viz(viz), _ecosystem(mapa),
      _pausado(false), _acumulador(0.0f), _scrollX(0.0f), _scrollY(0.0f)
{
    aplicarEstilo();
}

void Simulation::aplicarEstilo()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x1E1E1EFF);
    GuiSetStyle(DEFAULT, LINE_COLOR, 0x3C3C3CFF);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x2D4B6BFF);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x3D6690FF);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x1F3550FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x4C7AA8FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x7FB2E0FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0x2D4B6BFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 2);

    GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, 0x2A2A2AFF);
    GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, 0x4C7AA8FF);
    GuiSetStyle(SLIDER, BASE_COLOR_PRESSED, 0x7FB2E0FF);
    GuiSetStyle(SLIDER, BASE_COLOR_FOCUSED, 0x5C9AD0FF);
    GuiSetStyle(SLIDER, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(SLIDER, SLIDER_WIDTH, 16);

    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
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

void Simulation::drawSlider(float x, float y, float w, const char *label, float *value, float minV, float maxV, const char *suffix)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d%s", static_cast<int>(*value), suffix);

    _viz.drawText(static_cast<int>(x), static_cast<int>(y), 0xD8D8D8, label);

    int valorW = MeasureText(buf, 16);
    _viz.drawText(static_cast<int>(x + w) - valorW, static_cast<int>(y), 0x7FB2E0, buf);

    GuiSlider({x, y + 22, w, 20}, nullptr, nullptr, value, minV, maxV);
}

void Simulation::drawUI()
{
    const float contentH = 1340.0f;
    int playWidth = _ecosystem.getPlayWidth();
    EcosystemConfig &cfg = _ecosystem.config();

    Rectangle panelBounds = {static_cast<float>(playWidth), 0.0f, static_cast<float>(SIDEBAR_WIDTH), static_cast<float>(_viz.getHeight())};
    Rectangle content = {0.0f, 0.0f, static_cast<float>(SIDEBAR_WIDTH) - 16.0f, contentH};
    Vector2 scroll = {_scrollX, _scrollY};
    Rectangle view;

    GuiScrollPanel(panelBounds, nullptr, content, &scroll, &view);
    _scrollX = scroll.x;
    _scrollY = scroll.y;

    BeginScissorMode(static_cast<int>(view.x), static_cast<int>(view.y), static_cast<int>(view.width), static_cast<int>(view.height));

    float px = panelBounds.x + scroll.x + 14.0f;
    float panelW = static_cast<float>(SIDEBAR_WIDTH) - 44.0f;
    char buf[48];

    float y = scroll.y + 10.0f;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0xFFFFFF, "SYMBIOSIS");
    y += 28.0f;
    GuiLine({px, y, panelW, 10}, nullptr);
    y += 16.0f;
    if (GuiButton({px, y, panelW, 36}, _pausado ? "#131# Continuar" : "#132# Pausar"))
        _pausado = !_pausado;
    y += 48;

    if (GuiButton({px, y, panelW / 2 - 6, 36}, "#76# Reiniciar"))
    {
        _ecosystem.reiniciar();
        _pausado = false;
    }
    if (GuiButton({px + panelW / 2 + 6, y, panelW / 2 - 6, 36}, "#143# Limpar"))
    {
        _ecosystem.limpar();
        _pausado = true;
    }
    y += 50;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 22;

    drawSlider(px, y, panelW, "Bacterias", &cfg.configBacterias, 0.0f, 3000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Comida", &cfg.configComida, 0.0f, 4000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Veneno", &cfg.configVeneno, 0.0f, 1000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Velocidade", &cfg.velocidadeMs, 1.0f, 400.0f, "ms");
    y += 60;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 8;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x999999, "Caracteristicas das bacterias");
    y += 26;

    drawSlider(px, y, panelW, "Energia inicial", &cfg.energiaInicial, 10.0f, 4000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Vida maxima (idade)", &cfg.vidaMaxima, 20.0f, 9000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Valor da comida", &cfg.valorComida, 1.0f, 400.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Dano do veneno", &cfg.danoVeneno, 1.0f, 400.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Limite reproducao", &cfg.limiteReproducao, 50.0f, 3000.0f, "");
    y += 60;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 8;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x999999, "Gregarismo");
    y += 26;

    GuiCheckBox({px, y, 20, 20}, "Precisam de ficar juntas", &cfg.gregarismo);
    y += 40;

    drawSlider(px, y, panelW, "Raio social", &cfg.raioSocial, 20.0f, 400.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Custo de solidao", &cfg.custoSolidao, 0.0f, 30.0f, "");
    y += 60;

    if (GuiButton({px, y, panelW, 32}, "#68# Centrar vista"))
        _viz.resetCamera(static_cast<float>(playWidth), static_cast<float>(_viz.getHeight()));
    y += 42;

    snprintf(buf, sizeof(buf), "Zoom: %.1fx", static_cast<double>(_viz.getCameraZoom()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x888888, buf);
    y += 20;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x888888, "Roda=zoom  Botao dir.=arrastar");
    y += 26;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 22;

    GuiLabel({px, y, panelW, 20}, _pausado ? "Estado: PAUSADO" : "Estado: A CORRER");
    y += 30;

    snprintf(buf, sizeof(buf), "Vivas: %d", static_cast<int>(_ecosystem.getBacterias().size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x00FF00, buf);
    y += 25;

    snprintf(buf, sizeof(buf), "Comida no mapa: %d", static_cast<int>(_ecosystem.getComida().size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0xFFFF00, buf);
    y += 25;

    snprintf(buf, sizeof(buf), "Veneno no mapa: %d", static_cast<int>(_ecosystem.getVeneno().size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0xAA00FF, buf);
    y += 30;

    EndScissorMode();
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
    drawUI();
    _viz.endFrame();
}
