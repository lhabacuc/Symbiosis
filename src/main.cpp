#include <cstdlib>
#include <ctime>
#include "Bacteria.hpp"
#include "Visualizer.hpp"
#include "Simulation.hpp"

int main()
{
    srand((unsigned int)time(nullptr));

    Visualizer visualizer(800, 600, "Symbiosis");
    Simulation simulation(visualizer, 50);

    visualizer.loopHook(&Simulation::loopCallback, &simulation);
    visualizer.run();
    return 0;
}
