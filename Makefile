# Makefile

# Compilador
CXX = mpic++
CXXFLAGS = -std=c++11 -Wall

# Directorios de salida
OBJDIR = obj
BINDIR = bin

# Nombre del ejecutable
TARGET = $(BINDIR)/main

# Archivos fuente y de cabecera
SRCS = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

# Archivos objeto
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

# Regla para compilar el proyecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(OBJS) | $(BINDIR)
	@$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar los archivos objeto
$(OBJDIR)/%.o: %.cpp $(HEADERS) | $(OBJDIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para crear el directorio de objetos
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Regla para crear el directorio de binarios
$(BINDIR):
	@mkdir -p $(BINDIR)

# Regla para limpiar el proyecto
clean:
	@echo "Cleaning up..."
	@rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
