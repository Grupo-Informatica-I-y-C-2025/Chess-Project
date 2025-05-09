
#include "board.h"
#include "generator.h"

void Board::printBoard(Bitboard board) const {
	cout << "\n\n";
	// Imprimir desde la fila superior (i=7) hasta la inferior (i=0)
	for (int i = 7; i >= 0; i--) {  // <- Cambiar el bucle a decreciente
		for (int j = 0; j < 8; j++) {
			// Calcular el bit correspondiente a la casilla (i, j)
			cout << ((board >> (i * 8 + j)) & 1);
		}
		cout << "\n";
	}
}
int Board::getVariant()const {
	return gametype/2;
}
void Board::initBitboards() {
	memset(&currentState, 0, sizeof(ChessState));  // Todos los bits a 0
}
void Board::setBitboards() {

	initBitboards();
	currentState.turn = WHITE; // El blanco comienza

	switch (getVariant()) {
	case 0:
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 6, PAWN, BLACK);
		}
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 1, PAWN, WHITE);
		}
		// Torres
		BitboardSetPiece(0, 7, ROOK, BLACK);    // a8
		BitboardSetPiece(7, 7, ROOK, BLACK);    // h8
		BitboardSetPiece(0, 0, ROOK, WHITE);    // a1
		BitboardSetPiece(7, 0, ROOK, WHITE);    // h1
		// Alfiles*/
		BitboardSetPiece(2, 7, BISHOP, BLACK);  // c8
		BitboardSetPiece(5, 7, BISHOP, BLACK);  // f8
		BitboardSetPiece(2, 0, BISHOP, WHITE);  // c1
		BitboardSetPiece(5, 0, BISHOP, WHITE);  // f1
		// Caballos
		BitboardSetPiece(1, 7, KNIGHT, BLACK);  // b8
		BitboardSetPiece(6, 7, KNIGHT, BLACK);  // g8
		BitboardSetPiece(1, 0, KNIGHT, WHITE);  // b1
		BitboardSetPiece(6, 0, KNIGHT, WHITE);  // g1
		// Reinas
		BitboardSetPiece(3, 7, QUEEN, BLACK);   // d8
		BitboardSetPiece(3, 0, QUEEN, WHITE);   // d1
		// Reyes
		BitboardSetPiece(4, 7, KING, BLACK);    // e8
		BitboardSetPiece(4, 0, KING, WHITE);    // e1
		currentState.castlingRights = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE;
		break;
	case 1:
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 6, PAWN, BLACK);
		}
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 1, PAWN, WHITE);
		}
		// Torres
		BitboardSetPiece(3, 7, ROOK, BLACK);
		BitboardSetPiece(3, 0, ROOK, WHITE);
		// Alfiles
		BitboardSetPiece(1, 7, BISHOP, BLACK);
		BitboardSetPiece(1, 0, BISHOP, WHITE);
		// Caballos
		BitboardSetPiece(2, 7, KNIGHT, BLACK);
		BitboardSetPiece(2, 0, KNIGHT, WHITE);
		// Reyes
		BitboardSetPiece(0, 7, KING, BLACK);
		BitboardSetPiece(0, 0, KING, WHITE);
		break;
	case 2:
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 3, PAWN, BLACK);
		}
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 1, PAWN, WHITE);
		}
		// Reinas
		BitboardSetPiece(1, 4, QUEEN, BLACK);
		BitboardSetPiece(1, 0, QUEEN, WHITE);
		// Torres
		BitboardSetPiece(3, 4, ROOK, BLACK);
		BitboardSetPiece(3, 0, ROOK, WHITE);
		BitboardSetPiece(0, 4, ROOK, BLACK);
		BitboardSetPiece(0, 0, ROOK, WHITE);
		// Reyes
		BitboardSetPiece(2, 4, KING, BLACK);
		BitboardSetPiece(2, 0, KING, WHITE);

		break;
	}

	initCached();
}


int Board::piecePunctuation(int type) const{
	switch (type)
	{
	case PAWN:

		return 100;
		break;

	case ROOK:

		return 500;
		break;

	case BISHOP:

		return 300;
		break;

	case KNIGHT:

		return 300;
		break;

	case QUEEN:

		return 900;
		break;

	case KING:

		return 0;
		break;


	case EMPTY_CELL:

		return 0;
		break;

	default:

		return 0;
		break;
	}
}




Bitboard Board::coordToBit(int x, int y) const {
	return 1ULL << (y * 8 + x); // Ej: (2,3) ? 1 << 26
}
void Board::removePiece(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Buscar en todos los tipos y colores
	for (int color = 0; color < 2; color++) {
		for (int type = 0; type < 6; type++) { // Saltar EMPTY_CELL
			if (currentState.pieces[color][type] & mask) {
				currentState.pieces[color][type] &= ~mask;
				currentState.occupancy &= ~mask;
				return;
			}
		}
	}
}

int Board::BitboardGetColor(int x, int y) const {
	Bitboard mask = coordToBit(x, y);
	int color = 2;

	for (int c = 0; c < 2; c++) {
		for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return c;
			}
		}
	}return color;
}
int Board::BitboardGetColor(int sq) const {
	Bitboard mask = 1ULL << sq;
	int color = 2;

	for (int c = 0; c < 2; c++) {
		for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return c;
			}
		}
	}return color;
}
int Board::BitboardGetColor(int sq, ChessState state) const {
	Bitboard mask = 1ULL << sq;
	int color = 2;

	for (int c = 0; c < 2; c++) {
		for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			if (state.pieces[c][t] & mask) {
				return c;
			}
		}
	}return color;
}
int Board::BitboardGetType(int x, int y) const {
	Bitboard mask = coordToBit(x, y);
	int type = 6;

	for (int c = 0; c < 2; ++c) {
		for (int t = 0; t < 6; ++t) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return t;
			}
		}
	}return type;
}
int Board::BitboardGetType(int sq) const {
	Bitboard mask = 1ULL << sq;
	int type = 6;

	for (int c = 0; c < 2; ++c) {
		for (int t = 0; t < 6; ++t) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return t;
			}
		}
	}return type;
}
int Board::BitboardGetType(int sq, ChessState state) const {
	Bitboard mask = 1ULL << sq;
	int type = 6;

	for (int c = 0; c < 2; ++c) {
		for (int t = 0; t < 6; ++t) { // EMPTY_CELL no se almacena
			if (state.pieces[c][t] & mask) {
				return t;
			}
		}
	}return type;
}
void Board::BitboardSetPiece(int x, int y, int pieceType, bool color) {
	Bitboard mask = coordToBit(x, y);

	// Limpiar cualquier pieza existente
	removePiece(x, y);

	// Actualizar bitboards espec�ficos
	currentState.pieces[color][pieceType] |= mask;
	currentState.occupancy |= mask;
}

Bitboard Board::attackersTo(int target, bool color) const {
	Bitboard attackers = 0;
	Bitboard targetBB = 1ULL << target;

	// Iterar sobre todos los tipos de piezas del color especificado
	for (int pt = PAWN; pt <= KING; ++pt) {
		Bitboard pieces = currentState.pieces[color][pt];
		while (pieces) {
			int sq = portable_ctzll(pieces);
			Bitboard attacks = Generator::generateAttacksFrom(sq, color,*this);
			if (attacks & targetBB) {
				attackers |= (1ULL << sq); // A�adir la casilla al bitboard de atacantes
			}
			pieces &= pieces - 1; // Eliminar el bit procesado
		}
	}
	return attackers;
}

void Board::selectCell(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Limpiar cualquier flag
	unselectCell(x, y);

	// Actualizar bitboards espec�ficos
	currentState.selected |= mask;

}
void Board::unselectCell(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Buscar en todos los tipos y colores
	if (currentState.selected & mask) {
		currentState.selected &= ~mask;

	}
}
void Board::unselectAll() {
	currentState.selected=0;
}
bool Board::selected(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	bool s = 0;
	if (currentState.selected & mask)s = 1;
	return s;
}

string Board::encodeState(ChessState chess_state) const {
	string state;

	// Codificar las piezas en notaci�n FEN
	for (int y = N - 1; y >= 0; --y) {
		int emptyCount = 0;
		for (int x = 0; x < M; ++x) {
			int sq = x + y * 8;
			int color = BitboardGetColor(sq, chess_state);
			int type = BitboardGetType(sq, chess_state);
			if (type == EMPTY_CELL) {
				emptyCount++;
			}
			else {
				if (emptyCount > 0) {
					state += to_string(emptyCount);
					emptyCount = 0;
				}
				char c;
				if (color == WHITE) {
					switch (type) {
					case PAWN: c = 'P'; break;
					case KNIGHT: c = 'N'; break;
					case BISHOP: c = 'B'; break;
					case ROOK: c = 'R'; break;
					case QUEEN: c = 'Q'; break;
					case KING: c = 'K'; break;
					default: c = '?'; break;
					}
				}
				else {
					switch (type) {
					case PAWN: c = 'p'; break;
					case KNIGHT: c = 'n'; break;
					case BISHOP: c = 'b'; break;
					case ROOK: c = 'r'; break;
					case QUEEN: c = 'q'; break;
					case KING: c = 'k'; break;
					default: c = '?'; break;
					}
				}
				state += c;
			}
		}
		if (emptyCount > 0) {
			state += to_string(emptyCount);
		}
		if (y > 0) {
			state += '/';
		}
	}

	// Turno actual
	state += (chess_state.turn == WHITE) ? " w " : " b ";

	// Derechos de enroque
	string castling;
	if (chess_state.castlingRights & WHITE_KINGSIDE) castling += 'K';
	if (chess_state.castlingRights & WHITE_QUEENSIDE) castling += 'Q';
	if (chess_state.castlingRights & BLACK_KINGSIDE) castling += 'k';
	if (chess_state.castlingRights & BLACK_QUEENSIDE) castling += 'q';
	state += castling.empty() ? "-" : castling;

	// Casilla al paso
	state += ' ';
	if (chess_state.enPassant) {
		int sq = portable_ctzll(chess_state.enPassant);
		int x = sq % 8;
		int y = sq / 8;
		state += static_cast<char>('a' + x);
		state += to_string(y + 1);
	}
	else {
		state += '-';
	}

	return state;
}
bool Board::decodeState(const string& encoded) {
	istringstream iss(encoded);
	string boardStr, turnStr, castlingStr, enPassantStr;

	if (!getline(iss, boardStr, ' ') ||
		!getline(iss, turnStr, ' ') ||
		!getline(iss, castlingStr, ' ') ||
		!getline(iss, enPassantStr)) {
		return false;
	}

	// Reiniciar el tablero
	initBitboards();

	// Decodificar piezas
	int x = 0, y = N - 1;
	for (char c : boardStr) {
		if (c == '/') {
			y--;
			x = 0;
		}
		else if (isdigit(c)) {
			x += (c - '0');
		}
		else {
			int color = isupper(c) ? WHITE : BLACK;
			int type;
			switch (toupper(c)) {
			case 'P': type = PAWN; break;
			case 'N': type = KNIGHT; break;
			case 'B': type = BISHOP; break;
			case 'R': type = ROOK; break;
			case 'Q': type = QUEEN; break;
			case 'K': type = KING; break;
			default: return false;
			}
			if (x < M && y >= 0) {
				BitboardSetPiece(x, y, type, color);
				x++;
			}
		}
	}

	// Decodificar turno
	currentState.turn = (turnStr == "w") ? WHITE : BLACK;

	// Decodificar enroque
	currentState.castlingRights = 0;
	for (char c : castlingStr) {
		switch (c) {
		case 'K': currentState.castlingRights |= WHITE_KINGSIDE; break;
		case 'Q': currentState.castlingRights |= WHITE_QUEENSIDE; break;
		case 'k': currentState.castlingRights |= BLACK_KINGSIDE; break;
		case 'q': currentState.castlingRights |= BLACK_QUEENSIDE; break;
		}
	}

	// Decodificar al paso
	currentState.enPassant = 0;
	if (enPassantStr != "-") {
		int x = enPassantStr[0] - 'a';
		int y = enPassantStr[1] - '1';
		if (x >= 0 && x < M && y >= 0 && y < N) {
			currentState.enPassant = coordToBit(x, y);
		}
	}

	// Actualizar estado
	Generator::updateAttackMap(*this);
	return true;
}
const ChessState* Board::getState(int count) const {
	const ChessState* state = (count == 0) ? &currentState : &prevStates[moveCount - count];
	return state;
}
bool Board::saveGame(const std::string& filename) {
	std::string dirPath = "./saved_games/";

	// 1. Crear directorio (no falla si ya existe)
	FileUtils::createDirectory(dirPath);

	// 2. Ruta completa del archivo
	std::string fullPath = dirPath + filename;
	std::cout << "Ruta del archivo: " << fullPath << "\n";

	// 3. Abrir archivo
	std::ofstream file(fullPath);
	if (!file.is_open()) {
		std::cerr << "Error abriendo archivo\n";
		return false;
	}

	// 4. Verificar movimientos
	std::cout << "Movimientos a guardar: " << moveCount << "\n";
	if (moveCount == 0) {
		std::cerr << "No hay movimientos\n";
		return false;
	}

	// 5. Escribir datos
	try {
		for (int i = 0; i < moveCount; i++) {
			file << encodeState(prevStates[i]) << "\n";
			std::cout << "Guardado estado " << i << "\n";
		}
		file.close();
	}
	catch (const std::exception& e) {
		std::cerr << "Error al escribir: " << e.what() << "\n";
		return false;
	}

	return true;
}
bool Board::loadGame(const std::string& filename) {
	std::ifstream file("./saved_games/" + filename);
	if (!file.is_open()) {
		std::cerr << "Error: No se pudo abrir el archivo\n";
		return false;
	}

	// Limpiar estados previos
	moveCount = 0;
	currentState = ChessState{}; // Estado inicial vac�o

	std::string encodedState;
	int count = 0;
	while (std::getline(file, encodedState) && count < 500) {
		if (encodedState.empty()) continue; // Ignorar l�neas vac�as

		// Decodificar al estado actual temporalmente
		if (!decodeState(encodedState)) {
			std::cerr << "Error decodificando l�nea " << count << "\n";
			return false;
		}

		// Guardar el estado en el historial
		prevStates[count] = currentState;
		std::cout << "Estado cargado " << count << ": " << encodedState << "\n";
		count++;
	}

	// Restaurar el �ltimo estado v�lido
	if (count > 0) {
		moveCount = count;
		currentState = prevStates[count - 1];
		Generator::updateAttackMap(*this); // Actualizar ataques
	}
	else {
		std::cerr << "Archivo vac�o o inv�lido\n";
		return false;
	}

	file.close();
	return true;
}