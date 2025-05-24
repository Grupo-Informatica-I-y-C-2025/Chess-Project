#pragma once
#include "freeglut.h"
#include <iostream>
#include "boardGL.h"
#include "generalmenus.h"

class window{
    /////////////////////////////////
    // Global objects
    BoardGL* scene = nullptr;
    PrincipalMenu principalMenu;
    ModeSelectionMenu modeSelectionMenu;
    InGameMenu inGameMenu;
    EndGameMenu endGameMenu;
    LoadMenu loadMenu;
    ExitMenu exitMenu;
    BotMenu botMenu;
    ColorMenu colorMenu;


    int variant;
    int mode;

public:

    void setup(){
        //Initialize 3D models
        initModels();
    }

    void OnDraw(void) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Dibujar los menús en orden de prioridad
        if (principalMenu.isActive()) {
            principalMenu.render();
        } else if(modeSelectionMenu.isActive()){
            modeSelectionMenu.render();
        }else if (loadMenu.isActive()) {
            loadMenu.render();
        } else if (endGameMenu.isActive()) {
            endGameMenu.render();
        } else if (inGameMenu.isActive()) {
            inGameMenu.render();
        }
        else if (exitMenu.isActive()) {
            exitMenu.render();
        }
        else if (botMenu.isActive()) {
            botMenu.render();
        }
        else if (colorMenu.isActive()) {
            colorMenu.render();
        }
        else {
            // Si no hay menús activos, dibujar el juego
            scene->Draw();
        }


        glutSwapBuffers();
    }

    void OnKeyboardDown(unsigned char key, int x_t, int y_t) {
        if (principalMenu.isActive()) {
                
            if (key == 13) { // ENTER
                PrincipalMenu::Option option = principalMenu.getSelectedOption();
                switch (option) {
                    case PrincipalMenu::Option::PLAY:
                        // Reiniciar juego
                        modeSelectionMenu.setActive(true);
                        principalMenu.setActive(false);
                        break;
                    case PrincipalMenu::Option::LOAD:
                        // Cargar partida
                        principalMenu.setActive(false);
                        loadMenu.refresh();
                        loadMenu.setActive(true);
                        break;
                    case PrincipalMenu::Option::EXIT:
                        // Salir del juego
                        exit(0);
                        break;
                }
            }
            principalMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (loadMenu.isActive()) {
                
            if (key == 13) { // ENTER
                int option = (int)loadMenu.getSelectedOption();
                int variant = Game::LoadType(option);
                if(variant==-1){
                    std::cout << "Error al cargar la partida." << std::endl;
                    return;
                }
                delete scene;
                scene = new BoardGL(variant,mode);
                scene->init();
                scene->game->loadSavedGame(option);
                loadMenu.setActive(false);
            }
            loadMenu.handleInput(key);
            glutPostRedisplay();
        }else if (exitMenu.isActive()) {
                
            if (key == 13) { // ENTER
                PrincipalMenu::Option option = exitMenu.getSelectedOption();
                switch (option) {
                    case EndGameMenu::Option::EXIT:
                        // Salir juego
                         exit(0);
                        break;
                    case EndGameMenu::Option::MAIN_MENU:
                        // Volver al menú principal
                        principalMenu.setActive(true);
                        exitMenu.setActive(false);
                        break;
                }
            }
            exitMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (modeSelectionMenu.isActive()) {
            variant = 0;    
            if (key == 13) { // ENTER
                PrincipalMenu::Option option = modeSelectionMenu.getSelectedOption();
                variant = (int)option;

                botMenu.setActive(true);
                modeSelectionMenu.setActive(false);
               /* switch (option) {
                    case PrincipalMenu::Option::CLASSIC:
                        // Reiniciar juego
                        delete scene;
                        scene = new BoardGL((int)PrincipalMenu::Option::CLASSIC);
                        scene->init();
                        modeSelectionMenu.setActive(false);
                        break;
                    case PrincipalMenu::Option::CLASSIC_KRAMNIK:
                        // Reiniciar juego
                        delete scene;
                        scene = new BoardGL((int)PrincipalMenu::Option::CLASSIC_KRAMNIK);
                        scene->init();
                        modeSelectionMenu.setActive(false);
                        break;
                   case PrincipalMenu::Option::DEMI:
                        // Reiniciar juego
                        delete scene;
                        scene = new BoardGL((int)PrincipalMenu::Option::DEMI);
                        scene->init();
                        modeSelectionMenu.setActive(false);
                        break;
                    case PrincipalMenu::Option::DEMI_KRAMNIK:
                        // Reiniciar juego
                        delete scene;
                        scene = new BoardGL((int)PrincipalMenu::Option::DEMI_KRAMNIK);
                        scene->init();
                        modeSelectionMenu.setActive(false);
                        break;
                    case PrincipalMenu::Option::SILVERBULLET:
                        // Reiniciar juego
                        delete scene;
                        scene = new BoardGL((int)PrincipalMenu::Option::SILVERBULLET);
                        scene->init();
                        modeSelectionMenu.setActive(false);
                        break;
                    case PrincipalMenu::Option::SILVERBULLET_KRAMNIK:
                        // Reiniciar juego
                        delete scene;
                        scene = new BoardGL((int)PrincipalMenu::Option::SILVERBULLET_KRAMNIK);
                        scene->init();
                        modeSelectionMenu.setActive(false);
                        break;
                }*/
            }
            modeSelectionMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (botMenu.isActive()) {
                
            if (key == 13) { // ENTER
                PrincipalMenu::Option option = botMenu.getSelectedOption();
                switch (option) {
                    case EndGameMenu::Option::PVP:
                         delete scene;
                        scene = new BoardGL(variant,2);
                        scene->init();
                        botMenu.setActive(false);
                        break;
                    case EndGameMenu::Option::BOT:
                        // Volver al menú principal
                        colorMenu.setActive(true);
                        exitMenu.setActive(false);
                        break;
                }
            }
            botMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (colorMenu.isActive()) {
                
            if (key == 13) { // ENTER
                mode = 2;
                PrincipalMenu::Option option = colorMenu.getSelectedOption();
                switch (option) {
                    case ColorMenu::Option::WHITE:
                        mode = false;
                        delete scene;
                        scene = new BoardGL(variant,mode);
                        scene->init();
                        botMenu.setActive(false);
                        break;
                    case ColorMenu::Option::BLACK:
                        mode = true;
                         delete scene;
                        scene = new BoardGL(variant,mode);
                        scene->init();
                        botMenu.setActive(false);
                        break;
                }
            }
            colorMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (endGameMenu.isActive()) {
            if (key == 13) { // ENTER
                EndGameMenu::Option option = endGameMenu.getSelectedOption();
                switch (option) {
                    case EndGameMenu::Option::RESTART:
                        // Reiniciar juego
                        delete scene;
                        scene = new BoardGL(variant,mode);//pantalla de reinicio de juego para la seleccion de juego
                        scene->init();
                        endGameMenu.setActive(false);
                        break;
                    case EndGameMenu::Option::EXIT:
                        // Volver al menú principal
                        exitMenu.setActive(true);
                        endGameMenu.setActive(false);
                        break;
                }
            }
            endGameMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (inGameMenu.isActive()) {
            if (key == 13) { // ENTER
                InGameMenu::Option option = inGameMenu.getSelectedOption();
                switch (option) {
                    case InGameMenu::Option::RESUME:
                        inGameMenu.setActive(false);
                        break;
                    case InGameMenu::Option::RESTART:
                        // Reiniciar juego
                        delete scene;
                        scene = new BoardGL(variant,mode);
                        scene->init();
                        inGameMenu.setActive(false);
                        break;
                    case InGameMenu::Option::SAVE: 
                        // Guardar partida
                        scene->game->saveGame();
                        break;
                    case InGameMenu::Option::EXIT:
                        // Volver al menú principal
                        exitMenu.setActive(true);
                        inGameMenu.setActive(false);
                        break;
                }
            }
            inGameMenu.handleInput(key);
            glutPostRedisplay();
        }
        else {
            // Si se presiona ESC o M durante el juego, mostrar el menú de pausa
            if (key == 27 || key == 'm' || key == 'M') {
                inGameMenu.setActive(!inGameMenu.isActive());
                glutPostRedisplay();
                return;
            }
            
            scene->KeyDown(key);
            glutPostRedisplay();
        }
    }

    void OnMouseClick(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            /*if (principalMenu.isActive()) {
                principalMenu.HandleMouseClick(x, y);
                if (!principalMenu.isActive()) {
                    // Initialize the game with the selected mode
                    int selectedMode = principalMenu.getSelectedOption();
                    
                    // Si se selecciona EXIT, salir
                    if (selectedMode == -1) {
                        exit(0);
                    }
                    
                    delete scene;
                    scene = new BoardGL(selectedMode);

                    scene->init();
                    
                    // Inicializar el menú de pausa
                    inGameMenu.initialize();
                }
            }
            else if (inGameMenu.isActive()) {
                // TODO: Implementar clics en el menú de pausa si se necesita
            }
            else {*/
                // Handle clicks on the board
                bool down = (state == GLUT_DOWN);
                int specialKey = glutGetModifiers();
                bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
                bool sKey = specialKey & GLUT_ACTIVE_SHIFT;
                scene->MouseButton(x, y, button, down, sKey, ctrlKey);
            //}
        }

        glutPostRedisplay();
    }

    void OnSpecialKeyDown(int key, int x, int y) {
        if (principalMenu.isActive()) {
            principalMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if(modeSelectionMenu.isActive()){
            modeSelectionMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if(loadMenu.isActive()){
            loadMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (endGameMenu.isActive()) {
            endGameMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (exitMenu.isActive()) {
            exitMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (botMenu.isActive()) {
            botMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (colorMenu.isActive()) {
            colorMenu.handleInput(key);
            glutPostRedisplay();
        }
        else if (!principalMenu.isActive() && inGameMenu.isActive()) {
            inGameMenu.handleInput(key);
            glutPostRedisplay();
        }
    }

    void OnTimer(int value) {
        if (!principalMenu.isActive() && !loadMenu.isActive() && !modeSelectionMenu.isActive() && !inGameMenu.isActive() && !botMenu.isActive() && !colorMenu.isActive() && !endGameMenu.isActive()) {
            // Verificar si hay jaque mate
            if (scene->game->isgame_over()) {
                // El color que NO tiene el turno es el ganador
                bool whiteWins = scene->game->getTurn();
                endGameMenu.setWinner(whiteWins);
                endGameMenu.setActive(true);
            }
            
            // Resto del código de OnTimer
            scene->OnTimer(value);
        }
    }
};