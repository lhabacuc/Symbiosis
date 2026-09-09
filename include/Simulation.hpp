#ifndef SIMULATION_HPP
# define SIMULATION_HPP

# include <string>
# include "Visualizer.hpp"
# include "Mapa.hpp"
# include "Ecosystem.hpp"

class Simulation
{
    public:
        static constexpr int SIDEBAR_WIDTH = 320;

    private:
        Visualizer   &_viz;
        Ecosystem     _ecosystem;

        bool          _pausado;
        float         _acumulador;

        float         _scrollX;
        float         _scrollY;

        void aplicarEstilo();
        void draw();
        void drawUI();
        void drawSlider(float x, float y, float w, const char *label, float *value, float minV, float maxV, const char *suffix);

    public:
        Simulation(Visualizer &viz, const Mapa &mapa);

        void tick();
};

#endif
