OUTPUT_DIR = ./out

test:
	make build;
	make run;

build:
	make dirs
	gcc -c main.c -o $(OUTPUT_DIR)/main.o;
	gcc $(OUTPUT_DIR)/main.o -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -o $(OUTPUT_DIR)/main;

run:
	$(OUTPUT_DIR)/main;

clean:
	-rm -rf $(OUTPUT_DIR)

dirs:
	mkdir -p $(OUTPUT_DIR)
