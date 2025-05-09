#pragma once
#include "freeglut.h"
#include <string>
#include <vector>

class Menu {
private:
    struct MenuItem {
        std::string label;
        int mode;
        float x, y, width, height; // Posición y tamaño del botón
    };

    std::vector<MenuItem> items;
    int selectedMode;
    bool isActive;

public:
    Menu();
    void AddItem(const std::string& label, int mode);
    void Draw();
    void HandleMouseClick(int x, int y);
    int GetSelectedMode() const;
    bool IsActive() const;
    void Deactivate();
};
