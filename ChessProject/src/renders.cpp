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



struct Vertex {
	float x, y, z;     // posición
	float nx, ny, nz;  // normal
	float u, v;        // coordenadas de textura
};

struct Model {
	std::vector<Vertex> vertices;
	GLuint textureID = 0;
};
struct CapturedPiece {
	Model* model;
	float x, y;
	float zOffset = 0.0f;
	float angle = 0.0f;
	float opacity = 1.0f;
	int framesLeft = 30; // dura medio segundo a 60fps
};
std::vector<CapturedPiece> capturedPieces;

Model model_pawn_w, model_pawn_b;
Model model_rook_w, model_rook_b;
Model model_knight_w, model_knight_b;
Model model_bishop_w, model_bishop_b;
Model model_queen_w, model_queen_b;
Model model_king_w, model_king_b;

GLuint loadTexture(const std::string& path) {
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Error al cargar textura: " << path << std::endl;
		return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
	return textureID;
}

Model loadOBJ(const std::string& path) {
	Model model;
	std::vector<float> temp_vertices, temp_normals, temp_texcoords;
	std::string mtlFile;

	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "No se pudo abrir: " << path << std::endl;
		return model;
	}

	std::string line;
	while (getline(file, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			temp_vertices.insert(temp_vertices.end(), { x, y, z });
		}
		else if (type == "vt") {
			float u, v;
			iss >> u >> v;
			temp_texcoords.insert(temp_texcoords.end(), { u, v });
		}
		else if (type == "vn") {
			float nx, ny, nz;
			iss >> nx >> ny >> nz;
			temp_normals.insert(temp_normals.end(), { nx, ny, nz });
		}
		else if (type == "f") {
			std::vector<std::string> tokens;
			std::string token;
			while (iss >> token) tokens.push_back(token);

			// fan triangulation (v0, v1, v2), (v0, v2, v3), ...
			for (size_t i = 1; i + 1 < tokens.size(); ++i) {
				std::vector<std::string> face = { tokens[0], tokens[i], tokens[i + 1] };
				for (const std::string& vstr : face) {
					unsigned int vi = 0, ti = 0, ni = 0;
					sscanf(vstr.c_str(), "%u/%u/%u", &vi, &ti, &ni);

					Vertex v;
					v.x = temp_vertices[(vi - 1) * 3 + 0];
					v.y = temp_vertices[(vi - 1) * 3 + 1];
					v.z = temp_vertices[(vi - 1) * 3 + 2];
					v.nx = temp_normals[(ni - 1) * 3 + 0];
					v.ny = temp_normals[(ni - 1) * 3 + 1];
					v.nz = temp_normals[(ni - 1) * 3 + 2];
					v.u = temp_texcoords[(ti - 1) * 2 + 0];
					v.v = 1.0f - temp_texcoords[(ti - 1) * 2 + 1];
					model.vertices.push_back(v);
				}
			}
		}
		else if (type == "mtllib") {
			iss >> mtlFile;
		}
	}

	if (!mtlFile.empty()) {
		std::ifstream mtl("models/" + mtlFile);
		if (mtl.is_open()) {
			std::string mtlLine;
			while (getline(mtl, mtlLine)) {
				std::istringstream mss(mtlLine);
				std::string mtype;
				mss >> mtype;
				if (mtype == "map_Kd") {
					std::string texFile;
					mss >> texFile;
					model.textureID = loadTexture("models/" + texFile);
					break;
				}
			}
		}
	}
	float minX = FLT_MAX, maxX = -FLT_MAX;
	float minY = FLT_MAX, maxY = -FLT_MAX;
	float minZ = FLT_MAX;

	for (const auto& v : model.vertices) {
		if (v.x < minX) minX = v.x;
		if (v.x > maxX) maxX = v.x;
		if (v.y < minY) minY = v.y;
		if (v.y > maxY) maxY = v.y;
		if (v.z < minZ) minZ = v.z;
	}

	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;
	float centerZ = minZ;

	for (auto& v : model.vertices) {
		v.x -= centerX;
		v.y -= centerY;
		v.z -= centerZ;
	}

	return model;
}
void drawModel(const Model& model) {
	if (model.textureID) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, model.textureID);
	}
	glBegin(GL_TRIANGLES);
	for (const auto& v : model.vertices) {
		glTexCoord2f(v.u, v.v);
		glNormal3f(v.nx, v.ny, v.nz);
		glVertex3f(v.x, v.y, v.z);
	}
	glEnd();
	if (model.textureID) {
		glDisable(GL_TEXTURE_2D);
	}
}
void initModels() 
{
	model_pawn_w = loadOBJ("models/12944_Stone_Chess_Pawn_Side_A_V2_L3.obj");
	model_pawn_b = loadOBJ("models/12950_Stone_Chess_Pawn_Side_B_v2_l3.obj");
	model_rook_w = loadOBJ("models/12941_Stone_Chess_Rook_Side_A_V2_l1.obj");
	model_rook_b = loadOBJ("models/12947_Stone_Chess_Rook_Side_B_v2_l1.obj");
	model_knight_w = loadOBJ("models/12943_Stone_Chess_Knight_Side_A_v2_l1.obj");
	model_knight_b = loadOBJ("models/12949_Stone_Chess_Knight_Side_B_V2_l1.obj");
	model_bishop_w = loadOBJ("models/12942_Stone_Chess_Bishop_V2_l1.obj");
	model_bishop_b = loadOBJ("models/12948_Stone_Chess_Bishop_Side_B_v2_l1.obj");
	model_queen_w = loadOBJ("models/12940_Stone_Chess_Queen_Side_A_V2_l1.obj");
	model_queen_b = loadOBJ("models/12946_Stone_Chess_Queen_Side_B_V2_l1.obj");
	model_king_w = loadOBJ("models/12939_Stone_Chess_King_Side_A_V2_l1.obj");
	model_king_b = loadOBJ("models/12945_Stone_Chess_King_Side_B_v2_l1.obj");
}
void BoardGL::drawCircle(float cx, float cy, float r, int num_segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy); // center of circle
	for (int ii = 0; ii <= num_segments; ii++) {
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments); // get the current angle
		float x = r * cosf(theta); // calculate the x component
		float y = r * sinf(theta); // calculate the y component
		glVertex2f(x + cx, y + cy); // output vertex
	}
	glEnd();
}
void BoardGL::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);  // Importante para iluminación correcta
    
    // Configuración de la luz principal (como una lámpara sobre el tablero)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Posición de la luz (x, y, z, 1.0 para luz posicional)
    GLfloat light_position[] = {4.0f, 10.0f, 10.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    // Propiedades de la luz
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_diffuse[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    // Atenuación para hacer más realista
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001f);

    // Configuración de materiales por defecto
    GLfloat mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    // Configuración de proyección
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 800 / 600.0f, 0.1, 50.0);
}
void BoardGL::registerCapture(int x, int y) {
	activeCaptures.push_back({ x, y, 0.0f });
}
void BoardGL::DrawGrid() {
	glLineWidth(1);
	GLTools::Color(gltools::BLACK);
	width = 0.25f;
	float distX = M * width;
	float distY = N * width;

	glDisable(GL_LIGHTING);
	for (int i = 0; i <= N; i++) {
		glBegin(GL_LINES);
		glVertex3f(0, i * width, -0.001f);
		glVertex3f(distX, i * width, -0.001f);
		glEnd();
	}
	for (int i = 0; i <= M; i++) {
		glBegin(GL_LINES);
		glVertex3f(i * width, 0, -0.001f);
		glVertex3f(i * width, distY, -0.001f);
		glEnd();
	}
	glEnable(GL_LIGHTING);
}

void BoardGL::DrawBorder(){
    //Dibujar recuadro marrón
	glDisable(GL_LIGHTING);
	glColor3f(0.5f, 0.3f, 0.1f); // color marrón madera

	float margin = 0.05f;
	float outerWidth = M * width + margin * 2;
	float outerHeight = N * width + margin * 2;

	glPushMatrix();
	glTranslatef(center_x - outerWidth / 2.0f, center_y - outerHeight / 2.0f, +0.002f);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(outerWidth, 0, 0);
	glVertex3f(outerWidth, outerHeight, 0);
	glVertex3f(0, outerHeight, 0);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}


void BoardGL::DrawRecorder(){
    //Draw a transparent rectangle over the board to capture mouse input
	//with gluUnProject
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLTools::Color(0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(center_x, center_y, center_z);
	glRectf(M * width / 2.0f, N * width / 2.0f, -M * width / 2.0f, -N * width / 2.0f);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}
void BoardGL::DrawCell(int y, int x, int i) {
	float glx, gly;
	cell2center(y, x, glx, gly);
	int sq = y * 8 + x;
	int cellSel = 0;
	const ChessState* state = board->getState(i);
	int cellColor = board->BitboardGetColor(sq, *state);
	int cellType = board->BitboardGetType(sq, *state);
	if (i == 0) cellSel = board->selected(x, y);

	bool isWhiteSquare = (x + y) % 2 == 0;
	if (isWhiteSquare)
		glColor3f(0.9f, 0.9f, 0.9f);  // blanco suave
	else
		glColor3f(0.3f, 0.3f, 0.2f);  // gris oscuro

	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(glx - width / 2.0f, gly - width / 2.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(width, 0, 0);
	glVertex3f(width, width, 0);
	glVertex3f(0, width, 0);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);


	GLfloat mat_amb_diff[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
	
	// ==== DIBUJO DE PIEZAS ====
	GLTools::Color(gltools::WHITE);
	float scale = 0.035f;
	float pieceHeightOffset = -0.005f;

	Model* model = nullptr;
	float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;

	switch (cellType) {
	case PAWN:
		model = (cellColor == 0 ? &model_pawn_w : &model_pawn_b);
		//centerX = 1.16f; centerY = 1.26f; centerZ = 0.78f;
		break;
	case ROOK:
		model = (cellColor == 0 ? &model_rook_w : &model_rook_b);
		//centerX = 2.26f; centerY = 2.13f; centerZ = 2.19f;
		break;
	case KNIGHT:
		model = (cellColor == 0 ? &model_knight_w : &model_knight_b);
		//centerX = 1.91f; centerY = 1.81f; centerZ = 2.67f;
		break;
	case BISHOP:
		model = (cellColor == 0 ? &model_bishop_w : &model_bishop_b);
		//centerX = 2.38f; centerY = 2.34f; centerZ = 3.17f;
		break;
	case QUEEN:
		model = (cellColor == 0 ? &model_queen_w : &model_queen_b);
		//centerX = 2.53f; centerY = 2.46f; centerZ = 4.03f;
		break;
	case KING:
		model = (cellColor == 0 ? &model_king_w : &model_king_b);
		//centerX = 2.58f; centerY = 2.43f; centerZ = 4.51f;
		break;
	default:
		break;
	}

	if (model) {
		glPushMatrix();
		glTranslatef(glx , gly, pieceHeightOffset); // posición en celda
		glRotatef(180, 1, 0, 0); // orientación hacia arriba
		if (cellColor == 0) glRotatef(180, 0, 0, 1);
		if (cellColor == 1) glRotatef(-180, 0, 0, 1);// blancas hacia negras
		glScalef(scale, scale, scale);
		glTranslatef(-centerX, -centerY, -centerZ); // centrar el modelo
		drawModel(*model);
		glPopMatrix();
	}

	// Círculo de casilla atacada
	if (attackMap & 1ULL << sq) {
		GLTools::Color(gltools::GREY);
		glDisable(GL_LIGHTING);
		glPushMatrix();
		glTranslatef(glx, gly, -0.1f);
		drawCircle(0, 0, width / 7.0f, 50);
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
}
