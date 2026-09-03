#ifndef SIMULATION_HPP
# define SIMULATION_HPP

# include <vector>
# include <string>
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
    public:
        static constexpr int SIDEBAR_WIDTH = 320;
        static constexpr int PANEL_MIN_HEIGHT = 940;

    private:
        Visualizer              &_viz;
        int                      _playWidth;
        int                      _playHeight;

        std::vector<Bacteria>    _bacterias;
        std::vector<Item>        _comida;
        std::vector<Item>        _veneno;

        bool                     _pausado;

        // populacao / ambiente (aplicados em tempo real)
        float                    _configBacterias;
        float                    _ultimoConfigBacterias;
        float                    _configComida;
        float                    _configVeneno;
        float                    _velocidadeMs;

        // caracteristicas das bacterias (aplicados em tempo real)
        float                    _energiaInicial;
        float                    _vidaMaxima;
        float                    _valorComida;
        float                    _danoVeneno;
        float                    _limiteReproducao;

        float                    _acumulador;

        void aplicarEstilo();
        void sincronizarPopulacao();

        void spawnComida(int n);
        void spawnVeneno(int n);
        void spawnBacterias(int n);
        void reiniciar();
        void limpar();

        void update();
        void draw();
        void drawUI();
        void drawSlider(float x, float y, float w, const char *label, float *value, float minV, float maxV, const char *suffix);

    public:
        Simulation(Visualizer &viz, const Mapa &mapa);

        void tick();
};

#endif
