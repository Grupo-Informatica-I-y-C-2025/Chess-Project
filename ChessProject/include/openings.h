#include <fstream>
#include "nlohmann/json.hpp"
#include <string>
#include <unordered_map>

using json = nlohmann::json;

std::string indexToSquare(int index) {
    if (index < 0 || index > 63) return "??";
    
    char file = 'a' + (index % 8);
    int rank = (index / 8) + 1;
    return std::string(1, file) + std::to_string(rank);
}



// Función principal de conversión
std::string moveToUCI(const Move& move) {
    std::string uci;

    // 1. Convertir origen y destino
    uci = indexToSquare(move.sourceSquare) + indexToSquare(move.targetSquare);

    // 2. Manejar promoción
    if (move.moveType == PROMOTION) {
        switch (move.promotionPiece) {
            case QUEEN:  uci += 'q'; break;
            case ROOK:   uci += 'r'; break;
            case BISHOP: uci += 'b'; break;
            case KNIGHT: uci += 'n'; break;
            default: break; // No debería ocurrir
        }
    }

    return uci;
}

int squareToIndex(const std::string& square) {
    char file = square[0]; // Letra de la columna (ej: 'e')
    char rank = square[1]; // Número de la fila (ej: '2')

    int col = file - 'a';  // Convertir a 0-7
    int row = rank - '1';  // Convertir a 0-7 (filas no invertidas)

    // Si las filas están invertidas (fila 1 en ajedrez = índice 7):
    // row = 7 - (rank - '1');

    return row * 8 + col;
}

Move uciToMove(const std::string& uci, Board& board) {
    int from = squareToIndex(uci.substr(0, 2)); // Ej: "e2" -> 12
    int to = squareToIndex(uci.substr(2, 2));    // Ej: "e4" -> 32
    return {from, to, board.BitboardGetType(from), DEFAULT};
}

std::vector<std::string> getMoveHistory(const Board& board) {
    std::vector<std::string> history;
    for (const Move& m : board.moveHistory) {
        history.push_back(moveToUCI(m));
    }
    return history;
}



class OpeningBook {
private:
    struct Node {
        std::unordered_map<std::string, Node*> children;
        bool isEnd = false;
        ~Node() { // Destructor para liberar memoria
            for (auto& pair : children) delete pair.second;
        }
    };

    Node* root;

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;

        try {
            json openings = json::parse(file);

            for (const auto& [name, moves] : openings.items()) {
                Node* current = root;
                
                for (const auto& move : moves) {
                    std::string moveStr = move.get<std::string>();
                    
                    if (!current->children.count(moveStr)) {
                        current->children[moveStr] = new Node();
                    }
                    
                    current = current->children[moveStr];
                }
                current->isEnd = true; // Marcar fin de secuencia
            }
        } catch (const json::exception& e) {
            std::cerr << "Error cargando aperturas: " << e.what() << std::endl;
        }
    }

public:
    OpeningBook() : root(new Node()) {
        loadFromFile("openings.json");
    }

    ~OpeningBook() {
        delete root; // Liberar todo el árbol
    }

    std::string getNextMove(const std::vector<std::string>& moveHistory) {
        Node* current = root;
        
        for (const auto& move : moveHistory) {
            if (!current->children.count(move)) return "";
            current = current->children[move];
        }

        return current->children.empty() ? "" : current->children.begin()->first;
    }
};

