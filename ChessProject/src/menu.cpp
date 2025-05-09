#include "menu.h"
#include <iostream>

// Constructor
Menu::Menu() : selectedMode(-1), isActive(true) {}

// Agregar un elemento al menú
void Menu::AddItem(const std::string& label, int mode) {
    float yOffset = -0.2f * items.size(); // Espaciado vertical entre botones
    items.push_back({ label, mode, -0.5f, 0.5f + yOffset, 1.0f, 0.15f });
}

// Dibujar el menú
void Menu::Draw() {
    if (!isActive) return;

    for (const auto& item : items) {
        // Dibujar el botón
        glColor3f(0.2f, 0.2f, 0.8f); // Color del botón
        glBegin(GL_QUADS);
        glVertex2f(item.x, item.y);
        glVertex2f(item.x + item.width, item.y);
        glVertex2f(item.x + item.width, item.y - item.height);
        glVertex2f(item.x, item.y - item.height);
        glEnd();

        // Dibujar el texto
        glColor3f(1.0f, 1.0f, 1.0f); // Color del texto
        glRasterPos2f(item.x + 0.05f, item.y - 0.1f);
        for (char c : item.label) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }
}

// Manejar clics del ratón
void Menu::HandleMouseClick(int x, int y) {
    if (!isActive) return;

    // Convertir coordenadas de pantalla a coordenadas del mundo
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float worldX = (float(x) / windowWidth) * 2.0f - 1.0f;
    float worldY = 1.0f - (float(y) / windowHeight) * 2.0f;

    for (const auto& item : items) {
        if (worldX >= item.x && worldX <= item.x + item.width &&
            worldY <= item.y && worldY >= item.y - item.height) {
            selectedMode = item.mode;
            isActive = false; // Desactivar el menú después de seleccionar
            std::cout << "Modo seleccionado: " << selectedMode << std::endl;
            break;
        }
    }
}

// Obtener el modo seleccionado
int Menu::GetSelectedMode() const {
    return selectedMode;
}

// Verificar si el menú está activo
bool Menu::IsActive() const {
    return isActive;
}

// Desactivar el menú
void Menu::Deactivate() {
    isActive = false;
}
