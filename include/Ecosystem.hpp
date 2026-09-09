#ifndef ECOSYSTEM_HPP
# define ECOSYSTEM_HPP

# include <vector>
# include "Bacteria.hpp"
# include "Mapa.hpp"
# include "GradeEspacial.hpp"

struct Item
{
    int x;
    int y;
};

struct EcosystemConfig
{
    float configBacterias;
    float ultimoConfigBacterias;
    float configComida;
    float configVeneno;
    float velocidadeMs;

    float energiaInicial;
    float vidaMaxima;
    float valorComida;
    float danoVeneno;
    float limiteReproducao;

    bool  gregarismo;
    float raioSocial;
    float custoSolidao;

    float temperaturaAmbiente;
};

class Ecosystem
{
    private:
        Mapa                     _mapa;
        int                      _playWidth;
        int                      _playHeight;

        std::vector<Bacteria>    _bacterias;
        std::vector<Item>        _comida;
        std::vector<Item>        _veneno;

        GradeEspacial            _gradeComida;
        GradeEspacial            _gradeVeneno;
        GradeEspacial            _gradeBacterias;
        bool                     _desenhoDetalhado;

        std::vector<ZonaTemperatura> _zonasTemperatura;

        EcosystemConfig          _config;

        void spawnComida(int n);
        void spawnVeneno(int n);
        void spawnBacterias(int n);
        void resolverSobreposicoes();
        float temperaturaEm(int x, int y) const;

    public:
        explicit Ecosystem(const Mapa &mapa);

        EcosystemConfig &config();
        const EcosystemConfig &config() const;

        int getPlayWidth() const;
        int getPlayHeight() const;
        bool desenhoDetalhado() const;

        const std::vector<Bacteria> &getBacterias() const;
        const std::vector<Item> &getComida() const;
        const std::vector<Item> &getVeneno() const;
        const std::vector<ZonaTemperatura> &getZonasTemperatura() const;

        void sincronizarPopulacao();
        void carregarDeMapa();
        void reiniciar();
        void limpar();
        void update();
};

#endif
