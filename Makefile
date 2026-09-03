NAME = symbiosis

RAYLIB_DIR = raylib/src
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -I include -I $(RAYLIB_DIR)

SRC = src/main.cpp \
      src/SerVivo.cpp \
      src/Bacteria.cpp \
      src/Visualizer.cpp \
      src/Simulation.cpp \
      src/Mapa.cpp

OBJ = $(SRC:.cpp=.o)

LDFLAGS = -L $(RAYLIB_DIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: $(NAME)

$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC

$(NAME): $(RAYLIB_LIB) $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
