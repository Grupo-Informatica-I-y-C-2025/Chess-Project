#include "Game.h"
#include "generator.h"

//mover a boardGL
void Game::activate( int xcell_sel, int ycell_sel)
{
	////Activar casillas -- parte propia	
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < M)) {
		if (board->selected(ycell_sel, xcell_sel))
		{
			board->unselectAll();
			movement.sourceSquare = -1, movement.targetSquare = -1;
		}
		else if (board->BitboardGetColor(ycell_sel, xcell_sel) == board->BitboardGetTurn())
		{
			board->selectCell(ycell_sel, xcell_sel);

			if(!board->isKramnik())movement.sourceSquare = -1;
			registerCall(xcell_sel, ycell_sel);
		}
		else if (board->BitboardGetColor(ycell_sel, xcell_sel) == !board->BitboardGetTurn() || board->BitboardGetColor(ycell_sel, xcell_sel) == 2) {
			board->unselectAll();
			registerCall(xcell_sel, ycell_sel);
		}
	}

}
void Game::registerCall(int xcell_sel, int ycell_sel) {
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < M)) {
		if (movement.sourceSquare<0)
		{
			movement.sourceSquare = xcell_sel * 8 + ycell_sel;
		}
		else {
			movement.targetSquare = xcell_sel * 8 + ycell_sel;
		}
	}
}

bool Game::playTurn(int difficulty =1) {
	bool turn = board->BitboardGetTurn();
	
	if (!game_over && turn == bot && !pvp/*&& !generated*/)movement = bot4.botMove(turn,difficulty,*board);
		//generated = true;
	

	if (movement.sourceSquare >= 0 && movement.targetSquare >= 0) {
		
		if (Generator::isLegalmove(movement,*board)) {
			Generator::makeMove(movement,*board);
			//generated = false;
			board->moveHistory.push_back(movement);

			scanEndGame(!turn);
			if (!game_over && board->scanChecks(!turn)) cout << "\ncheck";

			movement.sourceSquare = -1, movement.targetSquare = -1;
			return true;
		}
		else cout << "\nInvalid move";
		//reset
		movement.sourceSquare = -1, movement.targetSquare = -1;
	}
	return false;
}

void Game::scanEndGame(bool color) {
	vector<Move> moves = Generator::generateAllMoves(color,*board);
	if (moves.size() == 0) {
		if (board->scanChecks(color)) {
			cout << "checkmate"<<endl;
			game_over = true;
		}
		else {
			cout << "draw"<<endl;
			game_over = true;
		}
	}
	if (board->scanRepetition()) {
		cout << "draw by repetition";
		game_over = true;
	}
	if (board->moveCount == 499) {
		game_over = true;
		cout << "override stack memory";
	}
	cout<<endl;
}

void Game::saveGame() {
	std::string dirPath = "./saved_games/";

	if (!FileUtils::directoryExists(dirPath)) {
		if (!FileUtils::createDirectory(dirPath)) {
			std::cerr << "Error al crear directorio.\n";
			return;
		}
	}

	int nextNum = getNextSaveNumber();
	std::string filename = "partida" + std::to_string(nextNum) + ".chess";

	if (board->saveGame(filename)) {
		std::cout << "Partida guardada como " << filename << "!\n";
	}
	else {
		std::cerr << "Error al guardar.\n";
	}
}


void Game::loadSavedGame(int choice) {
	std::string dirPath = "./saved_games/";

	if (!FileUtils::directoryExists(dirPath)) {
		std::cerr << "No hay partidas guardadas.\n";
		return;
	}

	std::vector<int> saves;

	for (const auto& filename : FileUtils::listDirectory(dirPath)) {
		if (filename.find("partida") == 0 && filename.size() > 13 && filename.substr(filename.size() - 6) == ".chess") {
			try {
				int num = std::stoi(filename.substr(7, filename.size() - 13));
				saves.push_back(num);
			}
			catch (...) {
				// Ignorar errores
			}
		}
	}

	if (saves.empty()) {
		std::cerr << "No hay partidas guardadas.\n";
		return;
	}

	std::sort(saves.begin(), saves.end());

	std::cout << "Partidas guardadas:\n";
	for (int num : saves) {
		std::cout << " - Partida " << num << "\n";
	}

	if (std::find(saves.begin(), saves.end(), choice) == saves.end()) {
		std::cerr << "N�mero inv�lido.\n";
		return;
	}

	std::string filename = "partida" + std::to_string(choice) + ".chess";

	if (board->loadGame(filename)) {
		std::cout << "Partida " << choice << " cargada exitosamente.\n";
	}
	else {
		std::cerr << "Error al cargar la partida.\n";
	}
}


std::vector<int> Game::listSavedGames()  {
	std::vector<int> saves;
	std::string dirPath = "./saved_games/";

	// Verificar si el directorio existe
	if (!FileUtils::directoryExists(dirPath)) return saves;

	// Iterar sobre los archivos en el directorio
	for (const auto& filename : FileUtils::listDirectory(dirPath)) {
		// No necesitamos .path().filename().string(), ya es std::string

		// Filtrar archivos que coincidan con "partidaX.chess"
		if (filename.find("partida") == 0 && filename.size() > 13 && filename.substr(filename.size() - 6) == ".chess") {
			// Extraer el n�mero X del nombre
			std::string numStr = filename.substr(7, filename.size() - 13); // "partida" (7) y ".chess" (6)
			try {
				int num = std::stoi(numStr);
				saves.push_back(num);
			}
			catch (...) {
				continue; // Ignorar n�meros inv�lidos
			}
		}
	}

	// Ordenar los n�meros
	std::sort(saves.begin(), saves.end());
	return saves;
}


int Game::getNextSaveNumber()  {
	std::vector<int> saves;
	std::string dirPath = "./saved_games/";

	if (!FileUtils::directoryExists(dirPath))
		return 1; // Si no existe, empezar con 1

	for (const auto& filename : FileUtils::listDirectory(dirPath)) {
		if (filename.find("partida") == 0 && filename.size() > 13 && filename.substr(filename.size() - 6) == ".chess") {
			try {
				int num = std::stoi(filename.substr(7, filename.size() - 13)); // "partida" (7) y ".chess" (6)
				saves.push_back(num);
			}
			catch (...) {
				// Ignorar errores
			}
		}
	}

	return saves.empty() ? 1 : (*std::max_element(saves.begin(), saves.end()) + 1);
}


int Game::LoadType(int choice) {
std::string dirPath = "./saved_games/";

	if (!FileUtils::directoryExists(dirPath)) {
		std::cerr << "No hay partidas guardadas.\n";
		return -1;
	}

	std::vector<int> saves;

	for (const auto& filename : FileUtils::listDirectory(dirPath)) {
		if (filename.find("partida") == 0 && filename.size() > 13 && filename.substr(filename.size() - 6) == ".chess") {
			try {
				int num = std::stoi(filename.substr(7, filename.size() - 13));
				saves.push_back(num);
			}
			catch (...) {
				// Ignorar errores
			}
		}
	}

	if (saves.empty()) {
		std::cerr << "No hay partidas guardadas.\n";
		return -1;
	}

	std::sort(saves.begin(), saves.end());

	if (std::find(saves.begin(), saves.end(), choice) == saves.end()) {
		std::cerr << "N�mero inv�lido.\n";
		return -1;
	}

	std::string filename = "partida" + std::to_string(choice) + ".chess";

	std::ifstream file("./saved_games/" + filename);
	if (!file.is_open()) {
		std::cerr << "Error: No se pudo abrir el archivo\n";
		return -1;
	}

	// Leer tipo de tablero
	std::string variant;
	std::getline(file,variant);
	//cout<<stoi(variant)<<endl;
	if(0>stoi(variant) || stoi(variant)>5){
		std::cerr << "Error: El tipo de tablero no es valido\n";
		file.close();
		return -1;
	}
	file.close();
	return stoi(variant);
}
