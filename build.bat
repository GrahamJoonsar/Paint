@echo off
echo Build Start
g++ src/main.cpp src/tigr.c src/slider.cpp src/button.cpp -o build/main -lopengl32 -lgdi32 -lWinmm
echo Build Complete
