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
    
    // posで指定した場所に石を置き、revで指定した場所の石をひっくり返す
    void putStone(const int64 pos, const int64 rev){
        if(black_turn){
            black_board ^= pos | rev;
            white_board ^= rev;
        }
        else{
            white_board ^= pos | rev;
            black_board ^= rev;
        }

        black_turn = !black_turn;
    }
    
    // posの場所に石を置いて、石が返った位置のbitを立てて返す
    int64 canPut(const int64 pos){
        if((black_board | white_board) & pos) return 0; //着手箇所が空白でない場合


        int64 rev = 0, r, mask;

        if(black_turn){     // 黒が打つ場合
            r = 0;
            mask = (pos >> 1) & 0x7f7f7f7f7f7f7f7f; //右方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){ //白石が連続する間
                r |= mask;
                mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
            }
            if(mask & black_board) rev |= r; //黒石があれば返す位置を確定する

            r = 0;
            mask = (pos >> 8) & 0x00ffffffffffffff; //下方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask >> 8) & 0x00ffffffffffffff;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos << 1) & 0xfefefefefefefefe; //左方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask << 1) & 0xfefefefefefefefe;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos << 8) & 0xffffffffffffff00; //上方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask << 8) & 0xffffffffffffff00;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos >> 9) & 0x007f7f7f7f7f7f7f; //右下方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos >> 7) & 0x00fefefefefefefe; //左下方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask >> 7) & 0x00fefefefefefefe;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos << 9) & 0xfefefefefefefe00; //左上方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask << 9) & 0xfefefefefefefe00;
            }
            if(mask & black_board) rev |= r;

            r = 0;
            mask = (pos << 7) & 0x7f7f7f7f7f7f7f00; //右上方向へ返せるかを調べる
            while(mask != 0 && (mask & white_board) != 0){
                r |= mask;
                mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
            }
            if(mask & black_board) rev |= r;
        }
        else{       // 白が打つ場合
            r = 0;
            mask = (pos >> 1) & 0x7f7f7f7f7f7f7f7f; //右方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){ //黒石が連続する間
                r |= mask;
                mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
            }
            if(mask & white_board) rev |= r; //白石があれば返す位置を確定する

            r = 0;
            mask = (pos >> 8) & 0x00ffffffffffffff; //下方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask >> 8) & 0x00ffffffffffffff;
            }
            if(mask & white_board) rev |= r;
            
            r = 0;
            mask = (pos << 1) & 0xfefefefefefefefe; //左方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask << 1) & 0xfefefefefefefefe;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos << 8) & 0xffffffffffffff00; //上方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask << 8) & 0xffffffffffffff00;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos >> 9) & 0x007f7f7f7f7f7f7f; //右下方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos >> 7) & 0x00fefefefefefefe; //左下方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask >> 7) & 0x00fefefefefefefe;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos << 9) & 0xfefefefefefefe00; //左上方向へ返せるかを調べる
            while(mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask << 9) & 0xfefefefefefefe00;
            }
            if(mask & white_board) rev |= r;

            r = 0;
            mask = (pos << 7) & 0x7f7f7f7f7f7f7f00; //右上方向へ返せるかを調べる
            while (mask != 0 && (mask & black_board) != 0){
                r |= mask;
                mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
            }
            if(mask & white_board) rev |= r;
        }

        return rev;

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

// (y, x) を bitに変換
int64 vec2bit(int y, int x){
    return 1LL << ((8 * (7 - y)) + (7 - x));
}

int_main() {
    Scene::SetBackground(Palette::Pink);
    
    OthelloGame othello;
    
    while (System::Update()) {
        showBoard(othello);
        
        if(MouseL.down()){
            Vec2 cur_pos = Cursor::Pos();
            if(cur_pos.x < left_board_pos || cur_pos.y < top_board_pos) continue;
            int x = (cur_pos.x - left_board_pos) / cell_width;
            int y = (cur_pos.y - top_board_pos) / cell_width;
            if(x < 0 || x >= 8 || y < 0 || y >= 8) continue;
            
            int64 pos = vec2bit(y, x);
            if(int64 rev = othello.canPut(pos)){
                othello.putStone(pos, rev);
            }
        }
    }
}
