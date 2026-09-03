#include "Simulation.hpp"
#include <cstdlib>
#include <unistd.h>
#include <iostream>

Simulation::Simulation(Visualizer &viz, const Mapa &mapa) : _viz(viz)
{
    const std::vector<std::pair<int, int> > &bacterias = mapa.getBacterias();
    _bacterias.reserve(bacterias.size());
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
}

void Simulation::spawnComida(int n)
{
    for (int i = 0; i < n; i++)
    {
        Item c;
        c.x = rand() % _viz.getWidth();
        c.y = rand() % _viz.getHeight();
        _comida.push_back(c);
    }
}

void Simulation::spawnVeneno(int n)
{
    for (int i = 0; i < n; i++)
    {
        Item v;
        v.x = rand() % _viz.getWidth();
        v.y = rand() % _viz.getHeight();
        _veneno.push_back(v);
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

    if (_comida.size() < 80)
        spawnComida(2);
    if (_veneno.size() < 20 && rand() % 4 == 0)
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
    {
        int x = _comida[i].x;
        int y = _comida[i].y;
        _viz.putPixel(x, y, 0xFFFF00);
        _viz.putPixel(x + 1, y, 0xFFFF00);
        _viz.putPixel(x, y + 1, 0xFFFF00);
        _viz.putPixel(x + 1, y + 1, 0xFFFF00);
    }

    for (size_t i = 0; i < _veneno.size(); i++)
    {
        int x = _veneno[i].x;
        int y = _veneno[i].y;
        _viz.putPixel(x, y, 0xAA00FF);
        _viz.putPixel(x + 1, y, 0xAA00FF);
        _viz.putPixel(x, y + 1, 0xAA00FF);
        _viz.putPixel(x + 1, y + 1, 0xAA00FF);
    }

    for (size_t i = 0; i < _bacterias.size(); i++)
    {
        Bacteria &b = _bacterias[i];
        int intensidade = (int)(b.getEnergy() * 2.0f);
        if (intensidade > 255)
            intensidade = 255;
        if (intensidade < 50)
            intensidade = 50;
        int color = (intensidade << 8);

        int x = b.getX();
        int y = b.getY();
        for (int dy = 0; dy < 4; dy++)
            for (int dx = 0; dx < 4; dx++)
                _viz.putPixel(x + dx, y + dy, color);
    }
    _viz.render();
}

void Simulation::tick()
{
    usleep(50000); // ~20 frames por segundo, para a simulacao ser visivel
    update();
    draw();
    std::cout << "bacterias vivas: " << _bacterias.size() << " | comida: " << _comida.size()
              << " | veneno: " << _veneno.size() << std::endl;
}

int Simulation::loopCallback(void *param)
{
    Simulation *sim = static_cast<Simulation *>(param);
    sim->tick();
    return 0;
}
