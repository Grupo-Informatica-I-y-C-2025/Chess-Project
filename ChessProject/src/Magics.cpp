
#include "board.h"
#include <mutex>
#include <unordered_set>
#include <unordered_map>



// Funci�n para calcular calidad del n�mero m�gico
int Board::calculateMagicQuality(int sq, Bitboard magic, Bitboard mask, int bits, bool isBishop) {
	unordered_map<Bitboard, Bitboard> attack_table;
	int collisions = 0;
	const int num_patterns = 1 << countBits(mask);

	for (int i = 0; i < num_patterns; ++i) {
		Bitboard blockers = indexToBitboard(i, mask);
		Bitboard attacks = isBishop ? getBishopAttacksSlow(sq, blockers)
			: getRookAttacksSlow(sq, blockers);
		const int idx = (blockers * magic) >> (64 - bits);

		if (attack_table.count(idx) && attack_table[idx] != attacks) {
			collisions++;
		}
		attack_table[idx] = attacks;
	}
	return collisions;
}
// Funci�n para guardar los mejores n�meros m�gicos
void Board::saveBestMagic(int sq, Bitboard magic, int quality, bool isBishop) {
	ofstream out("magics.log", ios::app);
	out << (isBishop ? "Bishop" : "Rook")
		<< " Square: " << sq
		<< " Magic: 0x" << hex << magic << dec
		<< " Quality: " << quality
		<< " Bits: " << (isBishop ? bishopIndexBits[sq] : rookIndexBits[sq])
		<< "\n";
}

// Static RNG for magic generation (seeded once)
static std::mt19937_64 magicRng(std::random_device{}());
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

std::mutex outputMutex; // Para proteger recursos de salida compartidos

// Funci�n para guardar n�meros m�gicos en el archivo si es m�s peque�o
void saveMagicIfSmaller(int sq, Bitboard magic, bool isBishop) {
	const std::string filename = isBishop ? "bishop_magics.txt" : "rook_magics.txt";

	std::fstream file(filename, std::ios::in | std::ios::out);

	// Si no existe, lo creamos y llenamos con 0xffffffffffffffffULL
	if (!file.is_open()) {
		file.open(filename, std::ios::out);
		for (int i = 0; i < 64; ++i)
			file << "0xffffffffffffffffULL\n";
		file.close();
		file.open(filename, std::ios::in | std::ios::out);
	}

	// Leer todos los valores existentes
	std::vector<Bitboard> magics(64, UINT64_MAX);
	std::string line;
	for (int i = 0; i < 64 && std::getline(file, line); ++i) {
		std::stringstream ss(line);
		ss >> std::hex >> magics[i];
	}

	// Si el nuevo es mejor (m�s peque�o), actualizamos
	if (magic < magics[sq]) {
		magics[sq] = magic;

		// Volvemos al principio del archivo y reescribimos todo
		file.clear();
		file.seekp(0);
		for (int i = 0; i < 64; ++i)
			file << "0x" << std::hex << magics[i] << "ULL\n";

		std::lock_guard<std::mutex> lock(outputMutex);
		std::cout << "Updated square " << sq << " with better magic: 0x" << std::hex << magic << "ULL\n";
	}

	file.close();
}


// Funci�n para generar m�gicos
void Board::generateMagicsForSquares(bool isBishop) {
	std::cout << (isBishop ? "Bishop" : "Rook") << " Magics:\n";
	std::vector<std::thread> threads;

	for (int sq = 0; sq < 64; ++sq) {
		//if(sq != 29)continue; // Solo para el cuadrado 2
		threads.emplace_back([&, sq]() {
			Bitboard magic = findMagic(sq, isBishop);
			saveMagicIfSmaller(sq, magic, isBishop);
		});
	}

	for (auto& t : threads) t.join();
}

Bitboard Board::findMagic(int sq, bool isBishop) {
	Bitboard mask = isBishop ? getBishopMask(sq) : getRookMask(sq);
	int bits = countBits(mask);
	int tableSize = 1 << bits;
	uint64_t attempts = 0;

	while (true) {
		++attempts;
		Bitboard magic = magicRng() & magicRng() & magicRng();

		if (countBits(magic & mask) < bits / 2) continue;

		if (validateMagic(sq, magic, mask, bits, tableSize, isBishop)) {
			std::lock_guard<std::mutex> lock(outputMutex);
			cout << "Square " << sq << " magic found after " << attempts << " attempts\n";
			return magic;
		}
	}
}

bool Board::validateMagic(int sq, Bitboard magic, Bitboard mask,
	int bits, int tableSize, bool isBishop) {
	int numPatterns = 1 << bits;
	vector<Bitboard> attackTable(tableSize, UINT64_MAX);

	for (int i = 0; i < numPatterns; ++i) {
		Bitboard blockers = indexToBitboard(i, mask);
		Bitboard attacks = isBishop
			? getBishopAttacksSlow(sq, blockers)
			: getRookAttacksSlow(sq, blockers);
		uint64_t idx = (blockers * magic) >> (64 - bits);
		if (idx >= static_cast<uint64_t>(tableSize)) return false;

		if (attackTable[idx] == UINT64_MAX)
			attackTable[idx] = attacks;
		else if (attackTable[idx] != attacks)
			return false;
	}
	return true;
}



// Convertir �ndice a patr�n de bloqueo
Bitboard Board::indexToBitboard(Bitboard index, Bitboard mask) {
	Bitboard blockers = 0;
	while (mask) {
		// Obtener el �ndice del bit m�s significativo en mask
		int msb = 63 - portable_clzll(mask);
		if (index & 1) blockers |= (1ULL << msb);
		index >>= 1;
		mask &= ~(1ULL << msb); // Eliminar el bit procesado de mask
	}
	return blockers;
}

void Board::initBishopMagics() {
	static const uint64_t Bmagic[64] = {
		  0x40040844404084ULL, 0x2004208a004208ULL,
  0x10190041080202ULL, 0x108060845042010ULL,
  0x581104180800210ULL,0x2112080446200010ULL,
  0x1080820820060210ULL,0x3c0808410220200ULL,
  0x4050404440404ULL,  0x21001420088ULL,
  0x24d0080801082102ULL,0x1020a0a020400ULL,
  0x40308200402ULL,   0x4011002100800ULL,
  0x401484104104005ULL,0x801010402020200ULL,
  0x400210c3880100ULL,0x404022024108200ULL,
  0x810018200204102ULL,0x4002801a02003ULL,
  0x85040820080400ULL,0x810102c808880400ULL,
  0xe900410884800ULL,0x8002020480840102ULL,
  0x220200865090201ULL,0x2010100a02021202ULL,
  0x152048408022401ULL,0x20080002081110ULL,
  0x4001001021004000ULL,0x800040400a011002ULL,
  0xe4004081011002ULL,0x1c004001012080ULL,
  0x8004200962a00220ULL,0x8422100208500202ULL,
  0x2000402200300c08ULL,0x8646020080080080ULL,
  0x80020a0200100808ULL,0x2010004880111000ULL,
  0x623000a080011400ULL,0x42008c0340209202ULL,
  0x209188240001000ULL,0x400408a884001800ULL,
  0x110400a6080400ULL,0x1840060a44020800ULL,
  0x90080104000041ULL,0x201011000808101ULL,
  0x1a2208080504f080ULL,0x8012020600211212ULL,
  0x500861011240000ULL,0x180806108200800ULL,
  0x4000020e01040044ULL,0x300000261044000aULL,
  0x802241102020002ULL,0x20906061210001ULL,
  0x5a84841004010310ULL,0x4010801011c04ULL,
  0xa010109502200ULL,  0x4a02012000ULL,
  0x500201010098b028ULL,0x8040002811040900ULL,
  0x28000010020204ULL,0x6000020202d0240ULL,
  0x8918844842082200ULL,0x4010011029020020ULL
	};

	for (int sq = 0; sq < 64; ++sq) {
		Bitboard mask = getBishopMask(sq);
		int      occCount = countBits(mask)+1;
		int      bits = occCount;
		int      size = 1 << bits;
		bishopMagics[sq] = Bmagic[sq];
		bishopIndexBits[sq] = bits;
		bishopMasks[sq] = mask;
		bishopAttacks[sq] = new Bitboard[size]();
		initBishopAttacks(sq);
	}
}
void Board::initBishopAttacks(int sq) {
	Bitboard mask = bishopMasks[sq];
	int      bits = bishopIndexBits[sq];
	int      occCount = bits;
	for (uint64_t i = 0; i < (1ULL << occCount); ++i) {
		Bitboard blockers = indexToBitboard(i, mask);
		Bitboard attacks = getBishopAttacksSlow(sq, blockers);
		uint64_t idx = (blockers * bishopMagics[sq]) >> (64 - bits);
		if (idx < (1u << bits))
			bishopAttacks[sq][idx] = attacks;
	}
}
void Board::initRookMagics() {
	// Valores m�gicos conocidos (pueden usarse como base)
	static const Bitboard Rmagic[64] = {
		0x8a80104000800020ULL, 0x140002000100040ULL,
  0x2801880a0017001ULL, 0x100081001000420ULL,
  0x200020010080420ULL, 0x3001c0002010008ULL,
  0x8480008002000100ULL, 0x2080088004402900ULL,
  0x800098204000ULL,    0x2024401000200040ULL,
  0x100802000801000ULL, 0x120800800801000ULL,
  0x208808088000400ULL, 0x2802200800400ULL,
  0x2200800100020080ULL,0x801000060821100ULL,
  0x80044006422000ULL,  0x100808020004000ULL,
  0x12108a0010204200ULL,0x140848010000802ULL,
  0x481828014002800ULL, 0x8094004002004100ULL,
  0x4010040010010802ULL,0x20008806104ULL,
  0x100400080208000ULL, 0x2040002120081000ULL,
  0x21200680100081ULL,  0x20100080080080ULL,
  0x2000a00200410ULL,   0x20080800400ULL,
  0x80088400100102ULL,  0x80004600042881ULL,
  0x4040008040800020ULL,0x440003000200801ULL,
  0x4200011004500ULL,   0x188020010100100ULL,
  0x14800401802800ULL,  0x2080040080800200ULL,
  0x124080204001001ULL, 0x200046502000484ULL,
  0x480400080088020ULL, 0x1000422010034000ULL,
  0x30200100110040ULL,  0x100021010009ULL,
  0x2002080100110004ULL,0x202008004008002ULL,
  0x20020004010100ULL,  0x2048440040820001ULL,
  0x101002200408200ULL, 0x40802000401080ULL,
  0x4008142004410100ULL,0x2060820c0120200ULL,
  0x1001004080100ULL,  0x20c020080040080ULL,
  0x2935610830022400ULL,0x44440041009200ULL,
  0x280001040802101ULL,0x2100190040002085ULL,
  0x80c0084100102001ULL,0x4024081001000421ULL,
  0x20030a0244872ULL,  0x12001008414402ULL,
  0x2006104900a0804ULL,0x1004081002402ULL
};

	for (int sq = 0; sq < 64; ++sq) {
		Bitboard mask = getRookMask(sq);
		int      occCount = countBits(mask) + 1;
		//cout <<"\n square:"<<sq << "   occCount: " << occCount << endl;
		int      bits = occCount;
		int      size = 1 << bits;
		rookMagics[sq] = Rmagic[sq];
		rookIndexBits[sq] = bits;
		rookMasks[sq] = mask;
		// zero-initialize table to avoid garbage
		rookAttacks[sq] = new Bitboard[size]();
		initRookAttacks(sq);
	}
}
void Board::initRookAttacks(int sq) {
	Bitboard mask = rookMasks[sq];
	int      bits = rookIndexBits[sq];
	int      occCount = bits;
	for (uint64_t i = 0; i < (1ULL << occCount); ++i) {
		Bitboard blockers = indexToBitboard(i, mask);
		Bitboard attacks = getRookAttacksSlow(sq, blockers);
		uint64_t idx = (blockers * rookMagics[sq]) >> (64 - bits);
		// ensure valid index
		if (idx < (1u << bits))
			rookAttacks[sq][idx] = attacks;
	}
}

Bitboard Board::getAttacksSlow(int sq, Bitboard blockers, bool isRook) {
	return isRook ? getRookAttacksSlow(sq, blockers)
		: getBishopAttacksSlow(sq, blockers);
}