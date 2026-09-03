#ifndef SIMULATION_HPP
# define SIMULATION_HPP

# include <vector>
# include "Bacteria.hpp"
# include "Visualizer.hpp"
# include "Mapa.hpp"

struct Item
{
    int x;
    int y;
};

class Simulation
{
    private:
        Visualizer              &_viz;
        std::vector<Bacteria>    _bacterias;
        std::vector<Item>        _comida;
        std::vector<Item>        _veneno;

        void spawnComida(int n);
        void spawnVeneno(int n);
        void update();
        void draw();

    public:
        Simulation(Visualizer &viz, const Mapa &mapa);

        void tick();
        static int loopCallback(void *param);
};

#endif
