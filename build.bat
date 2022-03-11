@echo off
echo Build Start
g++ main.cpp tigr.c slider.cpp button.cpp -o build/main -lopengl32 -lgdi32 -lWinmm
echo Build Complete
