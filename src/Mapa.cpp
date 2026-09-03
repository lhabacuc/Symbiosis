#include "Mapa.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

Mapa::Mapa(const std::string &path) : _width(0), _height(0)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("nao foi possivel abrir o mapa: " + path);

    std::string linha;
    while (std::getline(file, linha))
    {
        if (linha.empty())
            continue;

        std::istringstream iss(linha);
        std::string tipo;
        iss >> tipo;

        if (tipo == "WIDTH")
            iss >> _width;
        else if (tipo == "HEIGHT")
            iss >> _height;
        else if (tipo == "BACTERIA")
        {
            int x, y;
            iss >> x >> y;
            _bacterias.push_back(std::make_pair(x, y));
        }
        else if (tipo == "COMIDA")
        {
            int x, y;
            iss >> x >> y;
            _comida.push_back(std::make_pair(x, y));
        }
    }

    if (_width <= 0 || _height <= 0)
        throw std::runtime_error("mapa invalido: WIDTH/HEIGHT em falta em " + path);
}

int Mapa::getWidth() const
{
    return _width;
}

int Mapa::getHeight() const
{
    return _height;
}

const std::vector<std::pair<int, int> > &Mapa::getBacterias() const
{
    return _bacterias;
}

const std::vector<std::pair<int, int> > &Mapa::getComida() const
{
    return _comida;
}
