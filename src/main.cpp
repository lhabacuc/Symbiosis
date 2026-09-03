#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Bacteria.hpp"
#include "Visualizer.hpp"
#include "Simulation.hpp"
#include "Mapa.hpp"

int main(int argc, char **argv)
{
    srand((unsigned int)time(nullptr));

    std::string mapaPath = (argc > 1) ? argv[1] : "maps/default.sy";

    try
    {
        Mapa mapa(mapaPath);
        Visualizer visualizer(mapa.getWidth() + Simulation::SIDEBAR_WIDTH, mapa.getHeight(), "Symbiosis");
        Simulation simulation(visualizer, mapa);

        while (!visualizer.shouldClose())
            simulation.tick();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
