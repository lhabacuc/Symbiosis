NAME = symbiosis

MLX_DIR = minilibx-linux
MLX_LIB = $(MLX_DIR)/libmlx.a

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -I include -I $(MLX_DIR)

SRC = src/main.cpp \
      src/SerVivo.cpp \
      src/Bacteria.cpp \
      src/Visualizer.cpp \
      src/Simulation.cpp \
      src/Mapa.cpp

OBJ = $(SRC:.cpp=.o)

LDFLAGS = -L $(MLX_DIR) -lmlx -L/usr/lib -lXext -lX11 -lm

all: $(NAME)

$(MLX_LIB):
	$(MAKE) -C $(MLX_DIR)

$(NAME): $(MLX_LIB) $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
