#ifndef UIPANEL_HPP
# define UIPANEL_HPP

# include "Visualizer.hpp"
# include "Ecosystem.hpp"

enum class ModoVisualizacao
{
    NORMAL = 0,
    TERMICA,
    GENES,
    IDADE,
    DENSIDADE,
    TOTAL_MODOS
};

class UIPanel
{
    private:
        Visualizer   &_viz;
        int           _width;
        float         _scrollX;
        float         _scrollY;

        void drawSlider(float x, float y, float w, const char *label, float *value, float minV, float maxV, const char *suffix);

    public:
        UIPanel(Visualizer &viz, int width);

        void aplicarEstilo();
        void draw(Ecosystem &ecosystem, bool &pausado, ModoVisualizacao &modo);
};

#endif
