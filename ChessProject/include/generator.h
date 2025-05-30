#pragma once 

class Board;
class Game;

class Generator{
friend class Game;

public:
    static void updateAttackMap( Board&);

    static Bitboard calculateAttackMap(bool ,const Board&) ;
	static Bitboard generateAttacksFrom(int , bool ,const Board&) ;
	static Bitboard generateMovesFrom(int, bool,const Board&) ;
	static Bitboard getMovementMap(bool,const Board&);

    static vector<Move> generateAllMoves(bool, Board&);
	static vector<Move> generateQuiescientMoves(bool, Board&);
	static vector<Move> generateCheckMoves(bool, Board&);
    static vector<Move> generateCaptureMoves(bool, Board&);
    static void orderMoves(vector<Move>&, Board&);

	static MoveResult makeMove( Move&, Board&);
	static void undoMove( Board&);
	static MoveType determineMoveType(const Move&,const Board& );
    static bool isLegalmove(Move&, Board&);

private:
    static Bitboard generatePawnMoves(int,bool,const Board&);
	static Bitboard generateRookMoves(int,bool,const Board&) ;
	static Bitboard generateBishopMoves(int , bool ,const Board&) ;
	static Bitboard generateKnightMoves(int , bool ,const Board&) ;
	static Bitboard generateQueenMoves(int , bool ,const Board&) ;
	static Bitboard generateKingMoves(int,bool ,const Board&) ;
	static Bitboard generateCastlingMoves(bool ,const Board&) ;


};
