#     https://github.com/shiftkey/desktop?tab=readme-ov-file#installation-via-package-manager
rm -r sfml-app
g++ manager.cpp -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -pthread  -lX11 
./sfml-app
