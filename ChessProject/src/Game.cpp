#include "Game.h"


//mover a boardGL
void Game::unselectAll() {
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			board->unselectCell(i,j);
		}
	}
}
void Game::activate( int xcell_sel, int ycell_sel)
{
	////Activar casillas -- parte propia	
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < M)) {
		if (board->selected(ycell_sel, xcell_sel))
		{
			board->unselectCell(ycell_sel, xcell_sel);
			movement.sourceSquare = -1, movement.targetSquare = -1;
		}
		else if (board->BitboardGetColor(ycell_sel, xcell_sel) == board->BitboardGetTurn())
		{
			unselectAll();
			board->selectCell(ycell_sel, xcell_sel);
			//board->printBoard(board->generateMovesFrom(board->coordToPos(ycell_sel, xcell_sel),board->BitboardGetColor(ycell_sel, xcell_sel)));
			if(!board->isKramnik())movement.sourceSquare = -1;
			registerCall(xcell_sel, ycell_sel);
		}
		else if (board->BitboardGetColor(ycell_sel, xcell_sel) == !board->BitboardGetTurn() || board->BitboardGetColor(ycell_sel, xcell_sel) == 2) {
			unselectAll();
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

bool Game::playTurn() {
	bool turn = board->BitboardGetTurn();
	if (!game_over && turn == bot && !generated) {
		movement = bot3.botMove(turn);
		//cout << "\nmove :" << movement.sourceSquare % 8 << movement.sourceSquare / 8 << " " << movement.targetSquare % 8 << movement.targetSquare / 8;
		generated = true;
	}
	if (!game_over && turn == player && !generated) {
		movement = bot4.botMove(turn);
		//cout << "\nmove :" << movement.sourceSquare % 8 << movement.sourceSquare / 8 << " " << movement.targetSquare % 8 << movement.targetSquare / 8;
		generated = true;
	}

	if (movement.sourceSquare >= 0 && movement.targetSquare >= 0) {
		movement.pieceType = board->BitboardGetType(movement.sourceSquare);
		if (board->isLegalmove(movement)) {
			board->makeMove(movement);
			generated = false;
			leela.recordMoves(movement);
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
	vector<Move> moves = board->generateAllMoves(color);
	if (moves.size() == 0) {
		if (board->scanChecks(color)) {
			cout << "\ncheckmate";
			game_over = true;
			return;
		}
		else {
			cout << "\ndraw";
			game_over = true;
			return;
		}
	}
	if (board->scanRepetition()) {
		cout << "\ndraw by repetition";
		game_over = true;
	}
	if (board->moveCount == 499) {
		game_over = true;
		cout << "\noverride stack memory";
	}
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


void Game::loadSavedGame() {
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

	int choice;
	std::cout << "Ingresa el número de la partida a cargar: ";
	std::cin >> choice;

	if (std::find(saves.begin(), saves.end(), choice) == saves.end()) {
		std::cerr << "Número inválido.\n";
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


std::vector<int> Game::listSavedGames() const {
	std::vector<int> saves;
	std::string dirPath = "./saved_games/";

	// Verificar si el directorio existe
	if (!FileUtils::directoryExists(dirPath)) return saves;

	// Iterar sobre los archivos en el directorio
	for (const auto& filename : FileUtils::listDirectory(dirPath)) {
		// No necesitamos .path().filename().string(), ya es std::string

		// Filtrar archivos que coincidan con "partidaX.chess"
		if (filename.find("partida") == 0 && filename.size() > 13 && filename.substr(filename.size() - 6) == ".chess") {
			// Extraer el número X del nombre
			std::string numStr = filename.substr(7, filename.size() - 13); // "partida" (7) y ".chess" (6)
			try {
				int num = std::stoi(numStr);
				saves.push_back(num);
			}
			catch (...) {
				continue; // Ignorar números inválidos
			}
		}
	}

	// Ordenar los números
	std::sort(saves.begin(), saves.end());
	return saves;
}


int Game::getNextSaveNumber() const {
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


void Game::loadFENPositions(const std::string& filename) {
	std::ifstream file(filename);
	std::string fen;
	while (std::getline(file, fen)) {
		fenPositions.push_back(fen);
	}
}

void Game::runSimulation(int numGames) {
	for (int i = 0; i < numGames && i < fenPositions.size(); ++i) {
		// Cargar posición FEN
		if (!board->decodeState(fenPositions[i])) {
			std::cerr << "Error cargando FEN: " << fenPositions[i] << std::endl;
			continue;
		}

		// Simular partida
		bool gameOver = false;
		while (!gameOver) {
			// Obtener turno actual
			bool currentTurn = board->BitboardGetTurn();

			// Bot realiza movimiento
			Move move = (currentTurn == WHITE) ? bot4.botMove(currentTurn) : bot1.botMove(currentTurn);
			MoveResult result = board->makeMove(move);

			if (!result.success) {
				std::cerr << "Movimiento inválido en partida " << i << std::endl;
				break;
			}

			// Verificar fin de partida
			if (board->scanCheckMate(!currentTurn)) {
				(currentTurn == WHITE) ? ++whiteWins : ++blackWins;
				gameOver = true;
			}
			else if (board->scanRepetition() || board->generateAllMoves(currentTurn).empty()) {
				++draws;
				gameOver = true;
			}
		}
		++totalGames;
	}
}

void Game::printStats() const {
	std::cout << "Resultados después de " << totalGames << " partidas:\n";
	std::cout << "- Bot1 (Blanco) victorias: " << whiteWins << "\n";
	std::cout << "- Bot2 (Negro) victorias: " << blackWins << "\n";
	std::cout << "- Empates: " << draws << "\n";
	std::cout << "Puntuación Bot1: " << (whiteWins * 100 / totalGames) << "%\n";
}
