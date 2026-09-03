#include "Simulation.hpp"
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <algorithm>

static Simulation *g_instanciaAtual = nullptr;

Simulation::Simulation(Visualizer &viz, const Mapa &mapa)
    : _viz(viz), _playWidth(mapa.getWidth()), _playHeight(mapa.getHeight()),
      _pausado(false), _tickDelayUs(50000)
{
    _configBacterias = static_cast<int>(mapa.getBacterias().size());
    _configComida = static_cast<int>(mapa.getComida().size());
    _configVeneno = static_cast<int>(mapa.getVeneno().size());

    const std::vector<std::pair<int, int> > &bacterias = mapa.getBacterias();
    for (size_t i = 0; i < bacterias.size(); i++)
    {
        Bacteria b(bacterias[i].first, bacterias[i].second);
        b.setEnergy(60.0f);
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

    construirUI();
    g_instanciaAtual = this;
}

void Simulation::construirUI()
{
    int px = _playWidth + 15;
    int y = 40;
    const int rowH = 46;

    _botoes.push_back({px, y, px + 210, y + 30, ACAO_PAUSA, "Pausar / Continuar"});
    y += rowH;
    _botoes.push_back({px, y, px + 100, y + 30, ACAO_RESET, "Reiniciar"});
    _botoes.push_back({px + 110, y, px + 210, y + 30, ACAO_LIMPAR, "Limpar"});
    y += rowH + 20;

    _botoes.push_back({px, y, px + 30, y + 30, ACAO_BACTERIA_MENOS, "-"});
    _botoes.push_back({px + 180, y, px + 210, y + 30, ACAO_BACTERIA_MAIS, "+"});
    y += rowH;

    _botoes.push_back({px, y, px + 30, y + 30, ACAO_COMIDA_MENOS, "-"});
    _botoes.push_back({px + 180, y, px + 210, y + 30, ACAO_COMIDA_MAIS, "+"});
    y += rowH;

    _botoes.push_back({px, y, px + 30, y + 30, ACAO_VENENO_MENOS, "-"});
    _botoes.push_back({px + 180, y, px + 210, y + 30, ACAO_VENENO_MAIS, "+"});
    y += rowH;

    _botoes.push_back({px, y, px + 30, y + 30, ACAO_VELOCIDADE_MENOS, "-"});
    _botoes.push_back({px + 180, y, px + 210, y + 30, ACAO_VELOCIDADE_MAIS, "+"});
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
        b.setEnergy(60.0f);
        _bacterias.push_back(b);
    }
}

void Simulation::reiniciar()
{
    _bacterias.clear();
    _comida.clear();
    _veneno.clear();
    spawnBacterias(_configBacterias);
    spawnComida(_configComida);
    spawnVeneno(_configVeneno);
    _pausado = false;
}

void Simulation::limpar()
{
    _bacterias.clear();
    _comida.clear();
    _veneno.clear();
    _pausado = true;
}

void Simulation::handleClick(int x, int y)
{
    for (size_t i = 0; i < _botoes.size(); i++)
    {
        const Botao &b = _botoes[i];
        if (x < b.x1 || x > b.x2 || y < b.y1 || y > b.y2)
            continue;

        switch (b.acao)
        {
            case ACAO_PAUSA:
                _pausado = !_pausado;
                break;
            case ACAO_RESET:
                reiniciar();
                break;
            case ACAO_LIMPAR:
                limpar();
                break;
            case ACAO_BACTERIA_MENOS:
                _configBacterias = std::max(0, _configBacterias - 10);
                break;
            case ACAO_BACTERIA_MAIS:
                _configBacterias += 10;
                break;
            case ACAO_COMIDA_MENOS:
                _configComida = std::max(0, _configComida - 10);
                break;
            case ACAO_COMIDA_MAIS:
                _configComida += 10;
                break;
            case ACAO_VENENO_MENOS:
                _configVeneno = std::max(0, _configVeneno - 5);
                break;
            case ACAO_VENENO_MAIS:
                _configVeneno += 5;
                break;
            case ACAO_VELOCIDADE_MENOS:
                _tickDelayUs = std::min(200000, _tickDelayUs + 10000);
                break;
            case ACAO_VELOCIDADE_MAIS:
                _tickDelayUs = std::max(0, _tickDelayUs - 10000);
                break;
        }
        return;
    }
}

void Simulation::update()
{
    for (size_t i = 0; i < _bacterias.size(); i++)
        _bacterias[i].viver();

    for (size_t i = 0; i < _bacterias.size(); i++)
    {
        for (size_t j = 0; j < _comida.size();)
        {
            int dx = _bacterias[i].getX() - _comida[j].x;
            int dy = _bacterias[i].getY() - _comida[j].y;
            if (dx > -8 && dx < 8 && dy > -8 && dy < 8)
            {
                _bacterias[i].interagirComItem(FOOD);
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
                _bacterias[i].interagirComItem(POISON);
                _veneno.erase(_veneno.begin() + j);
            }
            else
                j++;
        }
    }

    if (static_cast<int>(_comida.size()) < _configComida)
        spawnComida(2);
    if (static_cast<int>(_veneno.size()) < _configVeneno && rand() % 4 == 0)
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
        if (_bacterias[i].podeSeDividir())
        {
            _bacterias[i].setEnergy(_bacterias[i].getEnergy() / 2.0f);
            _bacterias.push_back(_bacterias[i].divide());
        }
    }
}

void Simulation::draw()
{
    _viz.clear();

    for (size_t i = 0; i < _comida.size(); i++)
        _viz.drawRect(_comida[i].x, _comida[i].y, 4, 4, 0xFFFF00);

    for (size_t i = 0; i < _veneno.size(); i++)
        _viz.drawRect(_veneno[i].x, _veneno[i].y, 4, 4, 0xAA00FF);

    for (size_t i = 0; i < _bacterias.size(); i++)
    {
        Bacteria &b = _bacterias[i];
        int intensidade = (int)(b.getEnergy() * 2.0f);
        if (intensidade > 255)
            intensidade = 255;
        if (intensidade < 50)
            intensidade = 50;
        int color = (intensidade << 8);
        _viz.drawRect(b.getX(), b.getY(), 4, 4, color);
    }

    _viz.drawRect(_playWidth, 0, 2, _viz.getHeight(), 0x444444);

    drawUI();

    _viz.render();
}

void Simulation::drawUI()
{
    for (size_t i = 0; i < _botoes.size(); i++)
    {
        const Botao &b = _botoes[i];
        int cor = 0x336699;
        if (b.acao == ACAO_LIMPAR)
            cor = 0x993333;
        else if (b.acao == ACAO_RESET)
            cor = 0x339933;
        _viz.drawRect(b.x1, b.y1, b.x2 - b.x1, b.y2 - b.y1, cor);
    }
}

void Simulation::tick()
{
    usleep(static_cast<useconds_t>(_tickDelayUs));
    if (!_pausado)
        update();
    draw();

    int px = _playWidth + 15;
    _viz.drawText(px, 25, 0xFFFFFF, "SYMBIOSIS - CONTROLOS");
    _viz.drawText(px + 15, 60, 0xFFFFFF, _pausado ? "PAUSADO" : "A CORRER");
    _viz.drawText(px + 15, 106, 0xFFFFFF, "Reiniciar");
    _viz.drawText(px + 125, 106, 0xFFFFFF, "Limpar");

    int y = 40 + 46 + 46 + 20;
    std::string linha;

    linha = "Bacterias: " + std::to_string(_configBacterias);
    _viz.drawText(px + 40, y + 20, 0xFFFFFF, linha.c_str());
    y += 46;

    linha = "Comida: " + std::to_string(_configComida);
    _viz.drawText(px + 40, y + 20, 0xFFFFFF, linha.c_str());
    y += 46;

    linha = "Veneno: " + std::to_string(_configVeneno);
    _viz.drawText(px + 40, y + 20, 0xFFFFFF, linha.c_str());
    y += 46;

    linha = "Velocidade: " + std::to_string(_tickDelayUs / 1000) + "ms";
    _viz.drawText(px + 40, y + 20, 0xFFFFFF, linha.c_str());
    y += 46 + 30;

    linha = "Vivas: " + std::to_string(_bacterias.size());
    _viz.drawText(px, y, 0x00FF00, linha.c_str());
    y += 25;
    linha = "Comida no mapa: " + std::to_string(_comida.size());
    _viz.drawText(px, y, 0xFFFF00, linha.c_str());
    y += 25;
    linha = "Veneno no mapa: " + std::to_string(_veneno.size());
    _viz.drawText(px, y, 0xAA00FF, linha.c_str());
}

int Simulation::loopCallback(void *param)
{
    Simulation *sim = static_cast<Simulation *>(param);
    sim->tick();
    return 0;
}

int Simulation::mouseCallback(int button, int x, int y, void *param)
{
    Simulation *sim = static_cast<Simulation *>(param);
    if (button == 1)
        sim->handleClick(x, y);
    return 0;
}
