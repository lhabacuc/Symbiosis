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

    public:
        Mapa(const std::string &path);

        int getWidth() const;
        int getHeight() const;
        const std::vector<std::pair<int, int> > &getBacterias() const;
        const std::vector<std::pair<int, int> > &getComida() const;
};

#endif
