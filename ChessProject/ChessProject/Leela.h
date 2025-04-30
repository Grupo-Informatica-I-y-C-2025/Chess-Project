#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "board.h"

class LC0 {

private:
    std::string uci_moves;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;

    void WriteToEngine(const std::string& cmd) {
        DWORD written;
        std::string full_cmd = cmd + "\n";
        WriteFile(hChildStd_IN_Wr, full_cmd.c_str(), full_cmd.size(), &written, NULL);
    }

    std::string ReadFromEngine() {
        CHAR buffer[128];
        DWORD read;
        std::string output;

        while (true) {
            BOOL success = ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &read, NULL);
            if (!success || read == 0) break;
            buffer[read] = '\0';
            output += buffer;
            if (output.find("bestmove") != std::string::npos) break;
        }

        return output;
    }

    Move StartLc0(const std::string& lc0_path, const std::string& weights_path, const std::string& moves) {
        SECURITY_ATTRIBUTES saAttr{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

        HANDLE hChildStd_OUT_Wr = NULL, hChildStd_IN_Rd = NULL;

        // Pipes
        CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0);
        SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);

        CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0);
        SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

        PROCESS_INFORMATION piProcInfo{};
        STARTUPINFOA siStartInfo{};
        siStartInfo.cb = sizeof(STARTUPINFOA);
        siStartInfo.hStdError = hChildStd_OUT_Wr;
        siStartInfo.hStdOutput = hChildStd_OUT_Wr;
        siStartInfo.hStdInput = hChildStd_IN_Rd;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

        std::string cmd = "\"" + lc0_path + "\" --weights=" + weights_path;
        BOOL success = CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

        CloseHandle(hChildStd_OUT_Wr);
        CloseHandle(hChildStd_IN_Rd);

        if (!success) {
            std::cerr << "Error al iniciar Lc0\n";
            return { -1, -1 };
        }

        // Comandos UCI
        WriteToEngine("uci");
        Sleep(1000);  // Esperar respuesta UCI
        WriteToEngine("isready");
        Sleep(1000);
        WriteToEngine("ucinewgame");

        // Insertar historial de movimientos UCI
        std::string position_cmd = "position startpos";
        if (!moves.empty()) {
            position_cmd += " moves " + moves;
        }
        WriteToEngine(position_cmd);
        WriteToEngine("go movetime 2000");

        std::string output = ReadFromEngine();
        std::cout << "Lc0 dice:\n" << output << std::endl;

        Move move= parseBestMove(output);

        // Cerrar proceso
        TerminateProcess(piProcInfo.hProcess, 0);
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
        return move;
    }
    Move parseBestMove(const std::string& output) {
        size_t pos = output.find("bestmove ");
        if (pos == std::string::npos) {
            std::cerr << "No se encontró bestmove en la salida" << std::endl;
            return Move{ -1, -1 }; // Movimiento inválido
        }

        std::string moveStr = output.substr(pos + 9, 4); // "e2e4" por ejemplo

        int sourceFile = moveStr[0] - 'a'; // 'e' -> 4
        int sourceRank = moveStr[1] - '1'; // '2' -> 1
        int targetFile = moveStr[2] - 'a';
        int targetRank = moveStr[3] - '1';

        int sourceSquare = sourceRank * 8 + sourceFile; // De (fila,columna) a 0-63
        int targetSquare = targetRank * 8 + targetFile;

        return Move{ sourceSquare, targetSquare };
    }

public:
    Move botMove() {
        std::string lc0_path = "./AI/lc0.exe";
        std::string weights_path = "./AI/t1-512x15x8h-distilled-swa-3395000.pb.gz";

        // Historial de movimientos en UCI (puedes modificar esta cadena)
        return StartLc0(lc0_path, weights_path, uci_moves);
    }

    char getFile(int file) {
        return 'a' + file;
    }

    char getRank(int rank) {
        return '1' + rank;
    }

    void recordMoves(Move move) {
        int source = move.sourceSquare;
        int target = move.targetSquare;

        char sourceFile = getFile(source % 8);
        char sourceRank = getRank(source / 8);
        char targetFile = getFile(target % 8);
        char targetRank = getRank(target / 8);

        uci_moves += " ";
        uci_moves += sourceFile;
        uci_moves += sourceRank;
        uci_moves += targetFile;
        uci_moves += targetRank;

        //std::cout << "Movimientos UCI hasta ahora: " << uci_moves << std::endl;
    }


};