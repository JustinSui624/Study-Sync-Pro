build:
    g++ -c src/main.cpp -I./SFML-2.5.1/include -I./include -DSFML_STATIC -o bin/main.o
    g++ -c src/TextBox.cpp -I./SFML-2.5.1/include -I./include -DSFML_STATIC -o bin/TextBox.o
    g++ -c src/Application.cpp -I./SFML-2.5.1/include -I./include -DSFML_STATIC -o bin/Application.o
    g++ bin/main.o bin/TextBox.o bin/Application.o -o bin/StudySyncPro.exe -L./SFML-2.5.1/lib -L./lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lpq -lws2_32 -lwsock32 -lopengl32 -lwinmm -lgdi32 -lfreetype

clean:
    del /Q bin*.o bin*.exe
