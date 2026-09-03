#ifndef MAPA_HPP
# define MAPA_HPP

# include <string>
# include <vector>
# include <utility>

class Mapa
{
    private:
        int                                 _width;
        int                                 _height;
        std::vector<std::pair<int, int> >   _bacterias;
        std::vector<std::pair<int, int> >   _comida;
        std::vector<std::pair<int, int> >   _veneno;

        float                                _bacteriaVida;
        float                                _bacteriaVidaMaxima;

        std::pair<int, int> gerarPosicao(const std::string &modo, int raio);
        void gerarBloco(std::vector<std::pair<int, int> > &alvo, int count, const std::string &modo, int raio);

    public:
        Mapa(const std::string &path);

        int getWidth() const;
        int getHeight() const;
        const std::vector<std::pair<int, int> > &getBacterias() const;
        const std::vector<std::pair<int, int> > &getComida() const;
        const std::vector<std::pair<int, int> > &getVeneno() const;
        float getBacteriaVida() const;
        float getBacteriaVidaMaxima() const;
};

#endif
