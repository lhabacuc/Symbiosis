#include "SerVivo.hpp"

SerVivo::SerVivo() : _energy(50.0f), x(0), y(0), idade(0), idadeMaxima(100.0f) {}

SerVivo::SerVivo(int x, int y, float energy, int idadeMaxima) : _energy(energy), x(x), y(y), idade(0), idadeMaxima(idadeMaxima) {}

SerVivo::SerVivo(const SerVivo &other) : _energy(other._energy), x(other.x), y(other.y), idade(other.idade), idadeMaxima(other.idadeMaxima) {}

SerVivo::~SerVivo() {}

SerVivo &SerVivo::operator=(const SerVivo &other)
{
    if (this != &other)
    {
        _energy = other._energy;
        x = other.x;
        y = other.y;
        idade = other.idade;
        idadeMaxima = other.idadeMaxima;
    }
    return *this;
}

void SerVivo::setEnergy(float energy)
{
    _energy = energy;
}

void SerVivo::setIdade(int id)
{
    idade = id;
}

void SerVivo::setIdadeMaxima(int idMax)
{
    idadeMaxima = idMax;
}

bool SerVivo::estaMorto()
{
    return _energy <= 0.0f || idade >= idadeMaxima;
}

void SerVivo::interagirComItem(TipoItem item)
{
    switch (item)
    {
        case FOOD:
            setEnergy(getEnergy() + 5.0f);
            break;
        case POISON:
            setEnergy(getEnergy() - 5.0f);
            break;
        case NEUTRAL:
            break;
    }
}

float SerVivo::getEnergy() const
{
    return _energy;
}

int SerVivo::getX() const
{
    return x;
}

int SerVivo::getY() const
{
    return y;
}

float SerVivo::getIdade() const
{
    return idade;
}

int SerVivo::getIdadeMaxima() const
{
    return idadeMaxima;
}

void SerVivo::viver()
{
    _energy -= 1.0f;
}

