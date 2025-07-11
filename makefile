build:
	g++ -c src/main.cpp -IC:\SFML-2.5.1\include -DSFML_STATIC -o bin/main.o

	g++ bin/main.o -o bin/main -LC:\SFML-2.5.1\lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 -lfreetype
