COMPILADOR = gcc
CFLAGS = -Iinclude -g

#Cria uma pasta chamada 'objects' que sera o diretorio dos arquivos objeto (.o)
OBJ_DIR = objects

#'FONTES' referencia todos os arquivos .c do projeto
FONTES = $(shell find source -type f -name "*.c")

#Gera arquivos objetos de todos os arquivos de FONTES
OBJ = $(FONTES:source/%.c=$(OBJ_DIR)/%.o)

#'CABECALHOS' referencia todos os arquivos .h do projeto
CABECALHOS = $(shell find source -type f -name "*.h")

#Regra de compilacao dos .c
$(OBJ_DIR)/%.o: source/%.c $(CABECALHOS)
	@mkdir -p $(dir $@)
	$(COMPILADOR) $(CFLAGS) -c $< -o $@

# Linkagem
TETRIS: $(OBJ)
	$(COMPILADOR) -o $@ $^

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR) TETRIS