#ifndef SIMULATION_HPP
# define SIMULATION_HPP

# include <vector>
# include "Bacteria.hpp"
# include "Visualizer.hpp"

class Simulation
{
    private:
        Visualizer          &_viz;
        std::vector<Bacteria> _bacterias;

        void update();
        void draw();

    public:
        Simulation(Visualizer &viz, int nBacterias);

        void tick();
        static int loopCallback(void *param);
};

#endif
