#ifndef SIMULATION_HPP
# define SIMULATION_HPP

# include <vector>
# include "Bacteria.hpp"
# include "Visualizer.hpp"
# include "Mapa.hpp"

struct Comida
{
    int x;
    int y;
};

class Simulation
{
    private:
        Visualizer              &_viz;
        std::vector<Bacteria>    _bacterias;
        std::vector<Comida>      _comida;

        void spawnComida(int n);
        void update();
        void draw();

    public:
        Simulation(Visualizer &viz, const Mapa &mapa);

        void tick();
        static int loopCallback(void *param);
};

#endif
