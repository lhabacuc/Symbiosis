#ifndef SIMULATION_HPP
# define SIMULATION_HPP

# include <vector>
# include "Visualizer.hpp"
# include "Mapa.hpp"
# include "Ecosystem.hpp"
# include "UIPanel.hpp"

class Simulation
{
    public:
        static constexpr int SIDEBAR_WIDTH = 320;

    private:
        Visualizer   &_viz;
        Ecosystem     _ecosystem;
        UIPanel       _ui;

        bool             _pausado;
        float            _acumulador;
        ModoVisualizacao _modoVisualizacao;

        void draw();
        void desenharMapaDensidade(const std::vector<Bacteria> &bacterias);

    public:
        Simulation(Visualizer &viz, const Mapa &mapa);

        void tick();
};

#endif
