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

enum Acao
{
    ACAO_PAUSA,
    ACAO_RESET,
    ACAO_LIMPAR,
    ACAO_BACTERIA_MENOS,
    ACAO_BACTERIA_MAIS,
    ACAO_COMIDA_MENOS,
    ACAO_COMIDA_MAIS,
    ACAO_VENENO_MENOS,
    ACAO_VENENO_MAIS,
    ACAO_VELOCIDADE_MENOS,
    ACAO_VELOCIDADE_MAIS
};

struct Botao
{
    int x1, y1, x2, y2;
    Acao acao;
    std::string label;
};

class Simulation
{
    public:
        static constexpr int SIDEBAR_WIDTH = 240;

    private:
        Visualizer              &_viz;
        int                      _playWidth;
        int                      _playHeight;

        std::vector<Bacteria>    _bacterias;
        std::vector<Item>        _comida;
        std::vector<Item>        _veneno;
        std::vector<Botao>       _botoes;

        bool                     _pausado;
        int                      _configBacterias;
        int                      _configComida;
        int                      _configVeneno;
        int                      _tickDelayUs;

        void construirUI();
        void handleClick(int x, int y);

        void spawnComida(int n);
        void spawnVeneno(int n);
        void spawnBacterias(int n);
        void reiniciar();
        void limpar();

        void update();
        void draw();
        void drawUI();

    public:
        Simulation(Visualizer &viz, const Mapa &mapa);

        void tick();
        static int loopCallback(void *param);
        static int mouseCallback(int button, int x, int y, void *param);
};

#endif
