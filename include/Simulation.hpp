#ifndef SIMULATION_HPP
# define SIMULATION_HPP

# include <vector>
# include <string>
# include "Bacteria.hpp"
# include "Visualizer.hpp"
# include "Mapa.hpp"
# include "GradeEspacial.hpp"

struct Item
{
    int x;
    int y;
};

class Simulation
{
    public:
        static constexpr int SIDEBAR_WIDTH = 320;

    private:
        Visualizer              &_viz;
        int                      _playWidth;
        int                      _playHeight;

        std::vector<Bacteria>    _bacterias;
        std::vector<Item>        _comida;
        std::vector<Item>        _veneno;

        GradeEspacial            _gradeComida;
        GradeEspacial            _gradeVeneno;
        GradeEspacial            _gradeBacterias;
        bool                     _desenhoDetalhado;

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

        bool                     _gregarismo;
        float                    _raioSocial;
        float                    _custoSolidao;

        float                    _acumulador;

        float                    _scrollX;
        float                    _scrollY;

        void aplicarEstilo();
        void sincronizarPopulacao();

        void spawnComida(int n);
        void spawnVeneno(int n);
        void spawnBacterias(int n);
        void reiniciar();
        void limpar();

        void update();
        void resolverSobreposicoes();
        void draw();
        void drawUI();
        void drawSlider(float x, float y, float w, const char *label, float *value, float minV, float maxV, const char *suffix);

    public:
        Simulation(Visualizer &viz, const Mapa &mapa);

        void tick();
};

#endif
