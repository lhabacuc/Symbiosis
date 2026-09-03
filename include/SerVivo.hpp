#ifndef SERVIVO_HPP
# define SERVIVO_HPP

# include <iostream>
# include <cstdlib>
# include <vector>

enum TipoItem
{
    FOOD = 1,
    POISON = 2,
    NEUTRAL = 0
};

class SerVivo
{
    protected:
        float _energy;
        int x, y;
        int idade;
        int idadeMaxima;
    
    public:
        SerVivo();
        SerVivo(int x, int y, float energia = 50.0f, int idadeMaxima = 100);
        SerVivo(const SerVivo &other);
        virtual ~SerVivo();
        SerVivo &operator=(const SerVivo &other);
        void setEnergy(float energy);
        float getEnergy() const;
        int getX() const;
        int getY() const;
        float getIdade() const;
        int getIdadeMaxima() const;
        void setIdade(int id);
        void setIdadeMaxima(int idMax);
        virtual void viver() = 0;
        virtual void interagirComItem(TipoItem item) = 0;
        bool estaMorto();
};

#endif
