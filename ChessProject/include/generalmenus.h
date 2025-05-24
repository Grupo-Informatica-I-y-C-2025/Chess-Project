#pragma once

#include <string>
#include <vector>
#include <functional>
#include "freeglut.h"
#include "gltools.h"
#include "boardGL.h"


enum Palette{
    AZUL_CLARO_BRILLANTE=0,
    AZUL_OSCURO=1,
    AZUL_MAS_OSCURO=2,
    AZUL_BRILLANTE=3,
    AZUL_MEDIO=4,
    AZUL_SUAVE=5,
    AZUL_INCLUSO_MAS_OSCURO=6


};

void Color(int i){
    switch (i){
        case AZUL_CLARO_BRILLANTE:glColor4f(0.6f, 0.6f, 1.0f, 0.9f);break;
        case AZUL_OSCURO :glColor4f(0.15f, 0.15f, 0.3f, 0.9f);break;
        case AZUL_MAS_OSCURO:  glColor4f(0.1f, 0.1f, 0.2f, 0.9f);break;
        case AZUL_BRILLANTE:glColor4f(0.4f, 0.4f, 0.9f, 0.9f);break;
        case AZUL_MEDIO:glColor4f(0.3f, 0.3f, 0.7f, 0.9f);break;
        case AZUL_SUAVE:glColor4f(0.25f, 0.25f, 0.45f, 0.8f);break;
        case AZUL_INCLUSO_MAS_OSCURO:glColor4f(0.15f, 0.15f, 0.35f, 0.8f);break;
    }
}


class GeneralMenu {

public:
	enum class Option:int {
        CLASSIC=0,
        CLASSIC_KRAMNIK=1,
        DEMI=2,
        DEMI_KRAMNIK=3,
        SILVERBULLET=4,
        SILVERBULLET_KRAMNIK=5,
		RESUME=6,
		RESTART=7,
        SAVE=8,
		MAIN_MENU=9,
        PLAY=10,
        LOAD=11,
        EXIT=12,
		NONE=13,
        PVP=14,
        BOT=15,
        WHITE=16,
        BLACK=17
	};

	// Inicializar el menú
    void initialize() {
        
    }
	
	// Display the menu
	void render() const;
	
	// Process user input for menu navigation
	void handleInput(int key);
	
	// Get the currently selected option
	Option getSelectedOption() const;
	
	// Reset to default state
	void reset();
	
	// Is the menu currently active/visible
	bool isActive() const;
	
	// Show/hide the menu
	void setActive(bool active);
	

 
	bool active;
	int selectedIndex;

       //customs
	std::vector<std::string> optionTexts;
	std::vector<Option> options;

    std::string title;
    Palette borde_exterior;
    Palette fondo_panel_sup;
    Palette fondo_panel_inf;
    Palette boton_seleccionado_sup;
    Palette boton_seleccionado_inf;
    Palette boton_sup;
    Palette boton_inf;

};


class PrincipalMenu: public GeneralMenu{
public:
     PrincipalMenu(){
        // Configurar las opciones del menú
        optionTexts.clear();
        options.clear();
        
        optionTexts.push_back("Jugar");
        options.push_back(Option::PLAY);
        
        optionTexts.push_back("Cargar");
        options.push_back(Option::LOAD);

        optionTexts.push_back("Salir");
        options.push_back(Option::EXIT);

        title = "MENU PRINCIPAL";

        borde_exterior = AZUL_CLARO_BRILLANTE;

        fondo_panel_sup = AZUL_OSCURO;
        fondo_panel_inf = AZUL_MAS_OSCURO;

        boton_seleccionado_sup = AZUL_BRILLANTE;
        boton_seleccionado_inf = AZUL_MEDIO;

        boton_sup = AZUL_SUAVE;
        boton_inf = AZUL_INCLUSO_MAS_OSCURO;


        selectedIndex = 0;
        active = true; // Asegurarse de que comienza inactivo
    }
};

class ModeSelectionMenu: public GeneralMenu{
public:
    ModeSelectionMenu(){
        // Configurar las opciones del menú
        optionTexts.clear();
        options.clear();
        
        optionTexts.push_back("Classic");
        options.push_back(Option::CLASSIC);
        
        optionTexts.push_back("Classic kramnik");
        options.push_back(Option::CLASSIC_KRAMNIK);

        optionTexts.push_back("Demi");
        options.push_back(Option::DEMI);
        
        optionTexts.push_back("Demi kramnik");
        options.push_back(Option::DEMI_KRAMNIK);

        optionTexts.push_back("Silverbullet");
        options.push_back(Option::SILVERBULLET);

        optionTexts.push_back("Silverbullet kramnik");
        options.push_back(Option::SILVERBULLET_KRAMNIK);

        title = "MODE SELECTION";

        borde_exterior = AZUL_CLARO_BRILLANTE;

        fondo_panel_sup = AZUL_OSCURO;
        fondo_panel_inf = AZUL_MAS_OSCURO;

        boton_seleccionado_sup = AZUL_BRILLANTE;
        boton_seleccionado_inf = AZUL_MEDIO;

        boton_sup = AZUL_SUAVE;
        boton_inf = AZUL_INCLUSO_MAS_OSCURO;


        selectedIndex = 0;
        active = false; // Asegurarse de que comienza inactivo
    }

};
class InGameMenu : public GeneralMenu {


public:

    InGameMenu(){
        // Configurar las opciones del menú
        optionTexts.clear();
        options.clear();
        
        optionTexts.push_back("Continuar");
        options.push_back(Option::RESUME);
        
        optionTexts.push_back("Reiniciar");
        options.push_back(Option::RESTART);

        optionTexts.push_back("Guardar");
        options.push_back(Option::SAVE);
        
        optionTexts.push_back("Salir");
        options.push_back(Option::EXIT);

        title = "PAUSA";

        borde_exterior = AZUL_CLARO_BRILLANTE;

        fondo_panel_sup = AZUL_OSCURO;
        fondo_panel_inf = AZUL_MAS_OSCURO;

        boton_seleccionado_sup = AZUL_BRILLANTE;
        boton_seleccionado_inf = AZUL_MEDIO;

        boton_sup = AZUL_SUAVE;
        boton_inf = AZUL_INCLUSO_MAS_OSCURO;


        selectedIndex = 0;
        active = false; // Asegurarse de que comienza inactivo
    }



};


class EndGameMenu : public GeneralMenu {
    std::string resultText;

public:

    EndGameMenu() {
        // Configurar las opciones del menú
        optionTexts.clear();
        options.clear();
        
        optionTexts.push_back("Reiniciar");
        options.push_back(Option::RESTART);
        
        optionTexts.push_back("Salir");
        options.push_back(Option::EXIT);
        
        // Preparar texto del resultado

        borde_exterior = AZUL_CLARO_BRILLANTE;

        fondo_panel_sup = AZUL_OSCURO;
        fondo_panel_inf = AZUL_MAS_OSCURO;

        boton_seleccionado_sup = AZUL_BRILLANTE;
        boton_seleccionado_inf = AZUL_MEDIO;

        boton_sup = AZUL_SUAVE;
        boton_inf = AZUL_INCLUSO_MAS_OSCURO;

        selectedIndex = 0;
        active = false;
    }
    
    //customs
    void setWinner(bool whiteWins){ title = whiteWins ? "¡BLANCAS GANAN!" : "¡NEGRAS GANAN!";}
};

class ExitMenu : public GeneralMenu {
    std::string resultText;

public:

    ExitMenu() {
        // Configurar las opciones del menú
        optionTexts.clear();
        options.clear();
        
        optionTexts.push_back("Menu Principal");
        options.push_back(Option::MAIN_MENU);

        optionTexts.push_back("Salir");
        options.push_back(Option::EXIT);
    
        
        // Preparar texto del resultado

        title = "SALIR";

        borde_exterior = AZUL_CLARO_BRILLANTE;

        fondo_panel_sup = AZUL_OSCURO;
        fondo_panel_inf = AZUL_MAS_OSCURO;

        boton_seleccionado_sup = AZUL_BRILLANTE;
        boton_seleccionado_inf = AZUL_MEDIO;

        boton_sup = AZUL_SUAVE;
        boton_inf = AZUL_INCLUSO_MAS_OSCURO;

        selectedIndex = 0;
        active = false;
    }
    
};

class BotMenu : public GeneralMenu {
    std::string resultText;

public:

    BotMenu() {
        // Configurar las opciones del menú
        optionTexts.clear();
        options.clear();
        
        optionTexts.push_back("player vs player");
        options.push_back(Option::PVP);

        optionTexts.push_back("player vs bot");
        options.push_back(Option::BOT);
    
        
        // Preparar texto del resultado

        title = "MODO DE JUEGO";

        borde_exterior = AZUL_CLARO_BRILLANTE;

        fondo_panel_sup = AZUL_OSCURO;
        fondo_panel_inf = AZUL_MAS_OSCURO;

        boton_seleccionado_sup = AZUL_BRILLANTE;
        boton_seleccionado_inf = AZUL_MEDIO;

        boton_sup = AZUL_SUAVE;
        boton_inf = AZUL_INCLUSO_MAS_OSCURO;

        selectedIndex = 0;
        active = false;
    }
    
};
class ColorMenu : public GeneralMenu {
    std::string resultText;

public:

    ColorMenu() {
        // Configurar las opciones del menú
        optionTexts.clear();
        options.clear();
        
        optionTexts.push_back("BLANCO");
        options.push_back(Option::WHITE);

        optionTexts.push_back("NEGRO");
        options.push_back(Option::BLACK);
    
        
        // Preparar texto del resultado

        title = "MODO DE JUEGO";

        borde_exterior = AZUL_CLARO_BRILLANTE;

        fondo_panel_sup = AZUL_OSCURO;
        fondo_panel_inf = AZUL_MAS_OSCURO;

        boton_seleccionado_sup = AZUL_BRILLANTE;
        boton_seleccionado_inf = AZUL_MEDIO;

        boton_sup = AZUL_SUAVE;
        boton_inf = AZUL_INCLUSO_MAS_OSCURO;

        selectedIndex = 0;
        active = false;
    }
    
};

class LoadMenu : public GeneralMenu {
std::vector<int> save_list;

public:

    LoadMenu() {
        
        // Customs
         title = "CARGAR PARTIDA";

        borde_exterior = AZUL_CLARO_BRILLANTE;

        fondo_panel_sup = AZUL_OSCURO;
        fondo_panel_inf = AZUL_MAS_OSCURO;

        boton_seleccionado_sup = AZUL_BRILLANTE;
        boton_seleccionado_inf = AZUL_MEDIO;

        boton_sup = AZUL_SUAVE;
        boton_inf = AZUL_INCLUSO_MAS_OSCURO;

        selectedIndex = 0;
        active = false;
    }
    
    void refresh(){
        // Configurar las opciones del menú
        optionTexts.clear();
        options.clear();
        
        save_list = Game::listSavedGames();

        for(auto& save: save_list){
            
            std::string entry = "Partida "+ std::to_string(save);
            optionTexts.push_back(entry);
            options.push_back((PrincipalMenu::Option)save);
            
        }
    }
};



// Dibujar el menú
void GeneralMenu::render() const {
    if (!active) return;
    
    // Guardar la matriz de proyección y modelo actual
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Deshabilitar iluminación para el menú 2D
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Habilitar transparencia
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Fondo semitransparente con efecto viñeta
    glBegin(GL_QUADS);
    // Centro más transparente que los bordes
    glColor4f(0.0f, 0.0f, 0.1f, 0.5f);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(-0.5f, -0.5f);
    
    // Bordes más oscuros
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(-0.5f, 0.5f);
    
    glVertex2f(0.5f, 0.5f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(0.5f, -0.5f);
    
    glVertex2f(0.5f, -0.5f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(-0.5f, -0.5f);
    
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-0.5f, 0.5f);
    glEnd();
    
    // Dibujar panel del menú
    float menuWidth = 0.7f;
    float menuHeight = optionTexts.size() * 0.15f + 0.2f;
    float menuX = -menuWidth / 2.0f;
    float menuY = 0.4f;
    
    // Borde exterior con efecto de brillo
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    Color(borde_exterior);
    glVertex2f(menuX - 0.01f, menuY + 0.01f);
    glVertex2f(menuX + menuWidth + 0.01f, menuY + 0.01f);
    glVertex2f(menuX + menuWidth + 0.01f, menuY - menuHeight - 0.01f);
    glVertex2f(menuX - 0.01f, menuY - menuHeight - 0.01f);
    glEnd();
    
    // Fondo del panel con degradado
    glBegin(GL_QUADS);
    Color(fondo_panel_sup);
    glVertex2f(menuX, menuY);
    glVertex2f(menuX + menuWidth, menuY);
    Color(fondo_panel_inf);
    glVertex2f(menuX + menuWidth, menuY - menuHeight);
    glVertex2f(menuX, menuY - menuHeight);
    glEnd();
    
    // Título del menú
    glColor3f(0.8f, 0.8f, 1.0f);
    glRasterPos2f(menuX + menuWidth * 0.35f, menuY - 0.08f);
    for (char c : title) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    // Línea separadora bajo el título
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor4f(0.5f, 0.5f, 0.8f, 0.8f);
    glVertex2f(menuX + 0.05f, menuY - 0.12f);
    glVertex2f(menuX + menuWidth - 0.05f, menuY - 0.12f);
    glEnd();
    
    // Dibujar botones con efectos modernos
    float startY = menuY - 0.17f;
    for (size_t i = 0; i < optionTexts.size(); i++) {
        float itemY = startY - i * 0.15f;
        float itemX = menuX + 0.05f;
        float itemWidth = menuWidth - 0.1f;
        float itemHeight = 0.12f;
        
        // Sombra del botón
        if (i == static_cast<size_t>(selectedIndex)) {
            glBegin(GL_QUADS);
            glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
            glVertex2f(itemX + 0.006f, itemY - 0.006f);
            glVertex2f(itemX + itemWidth + 0.006f, itemY - 0.006f);
            glVertex2f(itemX + itemWidth + 0.006f, itemY - itemHeight - 0.006f);
            glVertex2f(itemX + 0.006f, itemY - itemHeight - 0.006f);
            glEnd();
        }
        
        // Color del botón con efecto de selección
        glBegin(GL_QUADS);
        if (i == static_cast<size_t>(selectedIndex)) {
            // Gradiente para el botón seleccionado
            Color(boton_seleccionado_sup); // Azul brillante arriba
            glVertex2f(itemX, itemY);
            glVertex2f(itemX + itemWidth, itemY);
            Color(boton_seleccionado_inf); // Azul medio abajo
            glVertex2f(itemX + itemWidth, itemY - itemHeight);
            glVertex2f(itemX, itemY - itemHeight);
        } else {
            // Gradiente para botones normales
            Color(boton_sup); // Azul suave arriba
            glVertex2f(itemX, itemY);
            glVertex2f(itemX + itemWidth, itemY);
            Color(boton_inf); // Azul más oscuro abajo
            glVertex2f(itemX + itemWidth, itemY - itemHeight);
            glVertex2f(itemX, itemY - itemHeight);
        }
        glEnd();
        
        // Borde del botón
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
        if (i == static_cast<size_t>(selectedIndex)) {
            glColor4f(0.7f, 0.7f, 1.0f, 0.9f); // Borde más brillante para seleccionado
        } else {
            glColor4f(0.5f, 0.5f, 0.9f, 0.7f); // Borde normal
        }
        glVertex2f(itemX, itemY);
        glVertex2f(itemX + itemWidth, itemY);
        glVertex2f(itemX + itemWidth, itemY - itemHeight);
        glVertex2f(itemX, itemY - itemHeight);
        glEnd();
        
        // Texto del botón
        if (i == static_cast<size_t>(selectedIndex)) {
            // Efecto de brillante para texto seleccionado
            glColor3f(1.0f, 1.0f, 1.0f);
        } else {
            glColor3f(0.8f, 0.8f, 0.95f);
        }
        
        // Centrar el texto en el botón
        void* font = GLUT_BITMAP_HELVETICA_18;
        int textWidth = 0;
        for (char c : optionTexts[i]) {
            textWidth += glutBitmapWidth(font, c);
        }
        
        float centeredX = itemX + (itemWidth - textWidth/500.0f) / 2.0f;
        glRasterPos2f(centeredX, itemY - itemHeight * 0.6f);
        
        for (char c : optionTexts[i]) {
            glutBitmapCharacter(font, c);
        }
    }
    
    // Restaurar estados GL
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    // Restaurar las matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Procesar entrada
void GeneralMenu::handleInput(int key) {
    if (!active) return;
    
    switch (key) {
        case GLUT_KEY_UP:
            // Mover selección hacia arriba
            if (selectedIndex > 0)
                selectedIndex--;
            else
                selectedIndex = optionTexts.size() - 1;
            break;
            
        case GLUT_KEY_DOWN:
            // Mover selección hacia abajo
            if (selectedIndex < static_cast<int>(optionTexts.size()) - 1)
                selectedIndex++;
            else
                selectedIndex = 0;
            break;
        
        case 27: // ESC
            // Cerrar menú
            active = false;
            break;
            
        case 13: // ENTER
            // Seleccionar opción actual (procesada en OnKeyboardDown)
            break;
    }
}

// Obtener la opción seleccionada
GeneralMenu::Option GeneralMenu::getSelectedOption() const {
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(options.size()))
        return options[selectedIndex];
    return Option::NONE;
}

// Reiniciar a estado por defecto
void GeneralMenu::reset() {
    active = false;
    selectedIndex = 0;
}

// ¿Está el menú activo?
bool GeneralMenu::isActive() const {
    return active;
}

// Activar/desactivar el menú
void GeneralMenu::setActive(bool isActive) {
    active = isActive;
}
