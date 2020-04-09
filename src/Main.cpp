#include <Siv3D.hpp>

#define int_main() void Main()

constexpr const int left_board_pos = 70;
constexpr const int top_board_pos = 50;
constexpr const int cell_width = 60;
constexpr const int board_width = cell_width * 8;
constexpr const int stone_size = 25;

constexpr const int BLACK_STONE = 1;
constexpr const int WHITE_STONE = -1;

class OthelloGame{
private:
    int64 black_board, white_board;
    bool black_turn;
    
public:
    OthelloGame() : black_board(0x0000001008000000),
                    white_board(0x0000000810000000),
                    black_turn(true) {}
    
    Array<Array<int>> getBorad(){
        Array<Array<int>> a(8, Array<int>(8));
        
        for(int i = 0; i < 64; i++){
            if(black_board & (1LL << i)){
                a[7 - (i / 8)][7 - (i % 8)] = BLACK_STONE;
            }
            if(white_board & (1LL << i)){
                a[7 - (i / 8)][7 - (i % 8)] = WHITE_STONE;
            }
        }
        
        return a;
    }
    
    bool putStone(int y, int x){
        if(x < 0 || x >= 8 || y < 0 || y >= 8) return false;
        
        if(black_turn){
            int64 bit = 1LL << ((8 * (7 - y)) + (7 - x));
            if(white_board & bit) return false;
            black_board |= bit;
        }
        else{
            int64 bit = 1LL << ((8 * (7 - y)) + (7 - x));
            if(black_board & bit) return false;
            white_board |= bit;
        }
        
        black_turn = !black_turn;
        
        return true;
    }
};

void showBoard(OthelloGame &othello){
    auto board = othello.getBorad();
    
    // 盤を描画
    Rect(left_board_pos, top_board_pos, board_width, board_width).draw(Palette::Darkgreen);
    for(int i = 0; i < 9; i++){
        // 盤の枠線
        Line(left_board_pos + i * cell_width, top_board_pos, left_board_pos + i * cell_width, top_board_pos + board_width).draw(1, Palette::Black);
        Line(left_board_pos, top_board_pos + i * cell_width, left_board_pos + board_width, top_board_pos + i * cell_width).draw(1, Palette::Black);
    }
    
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            // 黒石を描画
            if(board[i][j] == BLACK_STONE){
                Circle(left_board_pos + cell_width / 2 + j * cell_width, top_board_pos + cell_width / 2 + i * cell_width, stone_size).draw(Palette::Black);
            }
            // 白石を描画
            if(board[i][j] == WHITE_STONE){
                Circle(left_board_pos + cell_width / 2 + j * cell_width, top_board_pos + cell_width / 2 + i * cell_width, stone_size).draw(Palette::White);
            }
        }
    }
}

int_main() {
    Scene::SetBackground(Palette::Pink);
    
    OthelloGame othello;
    
    while (System::Update()) {
        showBoard(othello);
        
        if(MouseL.down()){
            Vec2 cur_pos = Cursor::Pos();
            if(cur_pos.x < left_board_pos || cur_pos.y < top_board_pos) break;
            int x = (cur_pos.x - left_board_pos) / cell_width;
            int y = (cur_pos.y - top_board_pos) / cell_width;
            othello.putStone(y, x);
        }
    }
}
