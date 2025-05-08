
#include "freeglut.h"
#include "gltools.h"
#include "boardGL.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cfloat>



//EN ESTE ARCHIVO VOY A HACER EL FONDO DEL JUEGO (SI PUEDO EN 3D), Y SI PUEDO LA ILUMINACIÓN
//TAMBIÉN VOY A HACER EL SONIDO DE LAS ANIMACIONES CUANDO UNA FICHA COMA A OTRA
// 
// 
//EN EL ARCHIVO DE BOARDGL TENGO QUE METER AL FINAL LAS TEXTURAS DE LAS CELDAS DEL TABLERO (LOSAS MARMOL)