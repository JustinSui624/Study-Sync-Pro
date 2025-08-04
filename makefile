build:
	g++ -c src/main.cpp -IC:\SFML-2.5.1\include -I../vcpkg/installed/x64-mingw-dynamic/include -DSFML_STATIC -o bin/main.o
	g++ -c src/TextBox.cpp -IC:\SFML-2.5.1\include -I../vcpkg/installed/x64-mingw-dynamic/include -DSFML_STATIC -o bin/TextBox.o
	g++ -c src/Application.cpp -IC:\SFML-2.5.1\include -I../vcpkg/installed/x64-mingw-dynamic/include -DSFML_STATIC -o bin/Application.o
	g++ bin/main.o bin/TextBox.o bin/Application.o -o bin/StudySyncPro.exe -LC:\SFML-2.5.1\lib -L../vcpkg/installed/x64-mingw-dynamic/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lpq -lws2_32 -lwsock32 -lopengl32 -lwinmm -lgdi32 -lfreetype

clean:
	del /Q bin\*.o bin\*.exe
