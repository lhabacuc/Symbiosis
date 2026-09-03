#include "Simulation.hpp"
#include "raylib.h"
#include "raygui.h"
#include <cstdlib>
#include <algorithm>
#include <cstdio>

Simulation::Simulation(Visualizer &viz, const Mapa &mapa)
    : _viz(viz), _playWidth(mapa.getWidth()), _playHeight(mapa.getHeight()),
      _pausado(false), _velocidadeMs(50.0f),
      _energiaInicial(90.0f), _vidaMaxima(150.0f), _valorComida(10.0f),
      _danoVeneno(6.0f), _limiteReproducao(130.0f), _acumulador(0.0f),
      _scrollX(0.0f), _scrollY(0.0f)
{
    _configBacterias = static_cast<float>(mapa.getBacterias().size());
    _configComida = static_cast<float>(mapa.getComida().size());
    _configVeneno = static_cast<float>(mapa.getVeneno().size());
    _ultimoConfigBacterias = _configBacterias;

    if (mapa.getBacteriaVida() >= 0.0f)
        _energiaInicial = mapa.getBacteriaVida();
    if (mapa.getBacteriaVidaMaxima() >= 0.0f)
        _vidaMaxima = mapa.getBacteriaVidaMaxima();

    const std::vector<std::pair<int, int> > &bacterias = mapa.getBacterias();
    for (size_t i = 0; i < bacterias.size(); i++)
    {
        Bacteria b(bacterias[i].first, bacterias[i].second);
        b.setEnergy(_energiaInicial);
        b.setIdadeMaxima(static_cast<int>(_vidaMaxima));
        _bacterias.push_back(b);
    }

    const std::vector<std::pair<int, int> > &comida = mapa.getComida();
    for (size_t i = 0; i < comida.size(); i++)
    {
        Item c;
        c.x = comida[i].first;
        c.y = comida[i].second;
        _comida.push_back(c);
    }

    const std::vector<std::pair<int, int> > &veneno = mapa.getVeneno();
    for (size_t i = 0; i < veneno.size(); i++)
    {
        Item v;
        v.x = veneno[i].first;
        v.y = veneno[i].second;
        _veneno.push_back(v);
    }

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

void Simulation::spawnComida(int n)
{
    for (int i = 0; i < n; i++)
    {
        Item c;
        c.x = rand() % _playWidth;
        c.y = rand() % _playHeight;
        _comida.push_back(c);
    }
}

void Simulation::spawnVeneno(int n)
{
    for (int i = 0; i < n; i++)
    {
        Item v;
        v.x = rand() % _playWidth;
        v.y = rand() % _playHeight;
        _veneno.push_back(v);
    }
}

void Simulation::spawnBacterias(int n)
{
    for (int i = 0; i < n; i++)
    {
        Bacteria b(rand() % _playWidth, rand() % _playHeight);
        b.setEnergy(_energiaInicial);
        b.setIdadeMaxima(static_cast<int>(_vidaMaxima));
        _bacterias.push_back(b);
    }
}

void Simulation::sincronizarPopulacao()
{
    int alvo = static_cast<int>(_configBacterias);
    int ultimo = static_cast<int>(_ultimoConfigBacterias);

    if (alvo == ultimo)
        return;

    int delta = alvo - ultimo;
    if (delta > 0)
        spawnBacterias(delta);
    else
    {
        int remover = std::min(-delta, static_cast<int>(_bacterias.size()));
        _bacterias.erase(_bacterias.end() - remover, _bacterias.end());
    }

    _ultimoConfigBacterias = _configBacterias;
}

void Simulation::reiniciar()
{
    _bacterias.clear();
    _comida.clear();
    _veneno.clear();
    spawnBacterias(static_cast<int>(_configBacterias));
    spawnComida(static_cast<int>(_configComida));
    spawnVeneno(static_cast<int>(_configVeneno));
    _ultimoConfigBacterias = _configBacterias;
    _pausado = false;
}

void Simulation::limpar()
{
    _bacterias.clear();
    _comida.clear();
    _veneno.clear();
    _ultimoConfigBacterias = 0.0f;
    _pausado = true;
}

void Simulation::update()
{
    for (size_t i = 0; i < _bacterias.size(); i++)
    {
        _bacterias[i].envelhecer();

        float raio = _bacterias[i].getRaioVisaoPixels();
        float raio2 = raio * raio;
        float melhorDist2 = -1.0f;
        int melhorX = 0;
        int melhorY = 0;

        for (size_t j = 0; j < _comida.size(); j++)
        {
            float dx = static_cast<float>(_bacterias[i].getX() - _comida[j].x);
            float dy = static_cast<float>(_bacterias[i].getY() - _comida[j].y);
            float dist2 = dx * dx + dy * dy;
            if (dist2 <= raio2 && (melhorDist2 < 0.0f || dist2 < melhorDist2))
            {
                melhorDist2 = dist2;
                melhorX = _comida[j].x;
                melhorY = _comida[j].y;
            }
        }

        if (melhorDist2 >= 0.0f)
            _bacterias[i].moverPara(melhorX, melhorY);
        else
            _bacterias[i].moverPreferido();
    }

    for (size_t i = 0; i < _bacterias.size(); i++)
    {
        for (size_t j = 0; j < _comida.size();)
        {
            int dx = _bacterias[i].getX() - _comida[j].x;
            int dy = _bacterias[i].getY() - _comida[j].y;
            if (dx > -8 && dx < 8 && dy > -8 && dy < 8)
            {
                _bacterias[i].setEnergy(_bacterias[i].getEnergy() + _valorComida);
                _comida.erase(_comida.begin() + j);
            }
            else
                j++;
        }

        for (size_t j = 0; j < _veneno.size();)
        {
            int dx = _bacterias[i].getX() - _veneno[j].x;
            int dy = _bacterias[i].getY() - _veneno[j].y;
            if (dx > -8 && dx < 8 && dy > -8 && dy < 8)
            {
                _bacterias[i].setEnergy(_bacterias[i].getEnergy() - _danoVeneno);
                _veneno.erase(_veneno.begin() + j);
            }
            else
                j++;
        }
    }

    if (static_cast<int>(_comida.size()) < static_cast<int>(_configComida))
        spawnComida(2);
    if (static_cast<int>(_veneno.size()) < static_cast<int>(_configVeneno) && rand() % 4 == 0)
        spawnVeneno(1);

    for (size_t i = 0; i < _bacterias.size();)
    {
        if (_bacterias[i].estaMorto())
            _bacterias.erase(_bacterias.begin() + i);
        else
            i++;
    }

    size_t nAtual = _bacterias.size();
    for (size_t i = 0; i < nAtual; i++)
    {
        if (_bacterias[i].getEnergy() > _limiteReproducao)
        {
            _bacterias[i].setEnergy(_bacterias[i].getEnergy() / 2.0f);
            Bacteria filha = _bacterias[i].divide();
            filha.setIdadeMaxima(static_cast<int>(_vidaMaxima));
            _bacterias.push_back(filha);
        }
    }
}

void Simulation::draw()
{
    _viz.updateCamera(static_cast<float>(_playWidth), static_cast<float>(_viz.getHeight()));
    _viz.beginScissorCamera(0, 0, _playWidth, _viz.getHeight());

    for (size_t i = 0; i < _comida.size(); i++)
        _viz.drawRect(_comida[i].x - 2, _comida[i].y - 2, 4, 4, 0xFFFF00);

    for (size_t i = 0; i < _veneno.size(); i++)
        _viz.drawRect(_veneno[i].x - 2, _veneno[i].y - 2, 4, 4, 0xAA00FF);

    for (size_t i = 0; i < _bacterias.size(); i++)
    {
        Bacteria &b = _bacterias[i];
        int intensidade = (int)(b.getEnergy() * 2.0f);
        if (intensidade > 255)
            intensidade = 255;
        if (intensidade < 50)
            intensidade = 50;
        unsigned int color = (unsigned int)(intensidade) << 8;

        float raio = 2.2f + std::min(3.0f, b.getEnergy() / 60.0f);
        _viz.drawBacteria(static_cast<float>(b.getX()), static_cast<float>(b.getY()), raio, color);
    }

    _viz.endScissorCamera();

    _viz.drawLine(_playWidth, 0, _playWidth, _viz.getHeight(), 0x666666);
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
    const float contentH = 1160.0f;

    Rectangle panelBounds = {static_cast<float>(_playWidth), 0.0f, static_cast<float>(SIDEBAR_WIDTH), static_cast<float>(_viz.getHeight())};
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
        reiniciar();
    if (GuiButton({px + panelW / 2 + 6, y, panelW / 2 - 6, 36}, "#143# Limpar"))
        limpar();
    y += 50;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 22;

    drawSlider(px, y, panelW, "Bacterias", &_configBacterias, 0.0f, 3000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Comida", &_configComida, 0.0f, 2000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Veneno", &_configVeneno, 0.0f, 1000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Velocidade", &_velocidadeMs, 1.0f, 200.0f, "ms");
    y += 60;

    GuiLine({px, y, panelW, 10}, nullptr);
    y += 8;
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x999999, "Caracteristicas das bacterias");
    y += 26;

    drawSlider(px, y, panelW, "Energia inicial", &_energiaInicial, 10.0f, 2000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Vida maxima (idade)", &_vidaMaxima, 20.0f, 5000.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Valor da comida", &_valorComida, 1.0f, 200.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Dano do veneno", &_danoVeneno, 1.0f, 200.0f, "");
    y += 54;

    drawSlider(px, y, panelW, "Limite reproducao", &_limiteReproducao, 50.0f, 3000.0f, "");
    y += 60;

    if (GuiButton({px, y, panelW, 32}, "#68# Centrar vista"))
        _viz.resetCamera(static_cast<float>(_playWidth), static_cast<float>(_viz.getHeight()));
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

    snprintf(buf, sizeof(buf), "Vivas: %d", static_cast<int>(_bacterias.size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0x00FF00, buf);
    y += 25;

    snprintf(buf, sizeof(buf), "Comida no mapa: %d", static_cast<int>(_comida.size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0xFFFF00, buf);
    y += 25;

    snprintf(buf, sizeof(buf), "Veneno no mapa: %d", static_cast<int>(_veneno.size()));
    _viz.drawText(static_cast<int>(px), static_cast<int>(y), 0xAA00FF, buf);
    y += 30;

    EndScissorMode();
}

void Simulation::tick()
{
    sincronizarPopulacao();

    float dt = _velocidadeMs / 1000.0f;
    _acumulador += GetFrameTime();
    while (_acumulador >= dt)
    {
        if (!_pausado)
            update();
        _acumulador -= dt;
    }

    _viz.beginFrame();
    draw();
    drawUI();
    _viz.endFrame();
}
