build:
	g++ -c src/main.cpp -IC:\SFML-2.5.1\include -DSFML_STATIC -o bin/main.o
	g++ -c src/TextBox.cpp -IC:\SFML-2.5.1\include -DSFML_STATIC -o bin/TextBox.o
	g++ bin/main.o bin/TextBox.o -o bin/StudySyncPro -LC:\SFML-2.5.1\lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 -lfreetype