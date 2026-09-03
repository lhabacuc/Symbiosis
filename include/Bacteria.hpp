#ifndef BACTERIA_HPP
# define BACTERIA_HPP

# include <iostream>
# include "SerVivo.hpp"

class Bacteria : public SerVivo
{
    float geneVelocidade;
    float geneRaioVisao;
    float genePreferencia;

    static constexpr float LIMITE_REPRODUCAO = 100.0f;

    public:
        Bacteria(int x, int y);
        Bacteria(const Bacteria &other, float genePreference);
        Bacteria(int posX, int posY, float maeVelocidade, float maeVisao, float maePreferencia);
        void viver();
        void envelhecer(float custoMultiplicador = 1.0f);
        void moverAleatorio();
        void moverPara(int tx, int ty);
        int getPasso() const;
        float getRaioVisaoPixels() const;
        Bacteria divide();
        float getPreference() const;
        bool podeSeDividir() const;
        float getGeneVelocidade() const;
        float getGeneRaioVisao() const;
        float getGenePreferencia() const;
        void interagirComItem(TipoItem item);
        void setPreference(float genePreference);
};

#endif