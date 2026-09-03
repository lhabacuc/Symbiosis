#include "Simulation.hpp"
#include <cstdlib>

Simulation::Simulation(Visualizer &viz, int nBacterias) : _viz(viz)
{
    _bacterias.reserve(nBacterias);
    for (int i = 0; i < nBacterias; i++)
    {
        int x = rand() % _viz.getWidth();
        int y = rand() % _viz.getHeight();
        _bacterias.push_back(Bacteria(x, y));
    }
}

void Simulation::update()
{
    for (size_t i = 0; i < _bacterias.size(); i++)
        _bacterias[i].viver();

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
        _viz.putPixel(x, y, color);
        _viz.putPixel(x + 1, y, color);
        _viz.putPixel(x, y + 1, color);
        _viz.putPixel(x + 1, y + 1, color);
    }
}

void Simulation::tick()
{
    update();
    draw();
}

int Simulation::loopCallback(void *param)
{
    Simulation *sim = static_cast<Simulation *>(param);
    sim->tick();
    return 0;
}
