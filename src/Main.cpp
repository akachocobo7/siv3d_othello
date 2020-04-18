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
    const int SEARCH_LV = 5;
    const int FINAL_STAGE_NUM = 51;
    const int64 VALUE_PLACE[64] = {
        150, -10, 10,  5,  5, 10, -10, 150,
        -10,-200,  2,  1,  1,  2,-200, -10,
         10,   2,  0,  1,  1,  0,   2,  10,
          5,   1,  1,  0,  0,  1,   1,   5,
          5,   1,  1,  0,  0,  1,   1,   5,
         10,   2,  0,  1,  1,  0,   2,  10,
        -10,-200,  2,  1,  1,  2,-200, -10,
        150, -10, 10,  5,  5, 10, -10, 150
    };
    const int64 INF = 1e18;
    
    int64 black_board, white_board;
    bool black_turn;
    bool isAIcolorWhite;
    
    
    // posで指定した場所に石を置き、revで指定した場所の石をひっくり返す
    void revStone(const int64 pos, const int64 rev){
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
    
    // 一手戻す
    void undo(const int64 pos, const int64 rev){
        black_turn = !black_turn;
        
        if(black_turn){
            black_board ^= pos | rev;
            white_board ^= rev;
        }
        else{
            white_board ^= pos | rev;
            black_board ^= rev;
        }
    }
    
    // posの場所に石を置いたとき、石が返る位置のbitを立てて返す
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
    
    int64 negaMax(const int64 depth, const bool is_before_put, const int64 alpha, const int64 beta, bool first_hand = true){
        if(depth == 0) return -boardValue();

        int64 max_value = alpha;
        bool is_put = false;
        int64 best_pos = 0;
        
        for(int64 bit = 0x8000000000000000; bit != 0; bit = (bit >> 1) & 0x7fffffffffffffff){
            if(int64 rev = canPut(bit); rev != 0){
                is_put = true;
                revStone(bit, rev);
                int64 tmp = -negaMax(depth - 1, true, -beta, -max_value, false);
                undo(bit, rev);

                if(tmp >= beta) return tmp;
                if(tmp > max_value){
                    max_value = tmp;
                    best_pos = bit;
                }
            }
        }

        if(is_put){
            if(first_hand) return best_pos;
            return max_value;
        }
        else if(!is_before_put) return -boardValue();
        else{
            black_turn = !black_turn;
            int64 tmp = -negaMax(depth - 1, false, -beta, -max_value, false);
            black_turn = !black_turn;
            return tmp;
        }
    }
    
    // 盤面の価値を返す
    int64 boardValue(){
        int64 value = 0;
        const int64 VSP = 1, VCP = 40, VFS = 45;    // 評価値の重み
        
        const int stone_count = countStone(BLACK_STONE) + countStone(WHITE_STONE);

        if(stone_count >= FINAL_STAGE_NUM){
            value += countStone(WHITE_STONE) - countStone(BLACK_STONE);
        }
        else{
            value += valueStonePlace() * VSP;
            value += valueCanPut() * VCP;
            value += valueFixedStone() *  VFS;
        }

        if(!isAIcolorWhite) value = -value;    // AIが黒なら反転

        return value;
    }
    
    // オセロの石の位置によって価値を計算
    int64 valueStonePlace(){
        int64 value = 0, c = 0;

        if(isAIcolorWhite){
            if(countStone(WHITE_STONE) >= 1) c = 0;  // 全滅対策
            else if(countStone(BLACK_STONE) == 0) c = -50000;    // 相手を全滅させたら評価値大アップ
            else c = 50000;
        }
        else{
            if(countStone(BLACK_STONE) >= 1) c = 0;
            else if(countStone(WHITE_STONE) == 0) c = -50000;
            else c = 50000;
        }

        int64 b = black_board, w = white_board;
        for(int i = 63; i >= 0; i--){
            value += (b & 1) * VALUE_PLACE[i];
            value -= (w & 1) * VALUE_PLACE[i];

            b >>= 1;
            w >>= 1;
        }

        return -(value + c);
    }

    // 石の置ける位置の数によって価値を計算
    int64 valueCanPut(){
        int64 value = 0;

        for(int64 bit = 0x8000000000000000; bit != 0; bit = (bit >> 1) & 0x7fffffffffffffff){
            if(canPut(bit)) value++;
        }

        if(black_turn) return -value;
        else return value;
    }
    
    // 確定石の数によって価値を計算
    int64 valueFixedStone(){
        int64 bit;
        int64 value = 0;

        if((black_board | white_board) & 0x8000000000000000){
            if(black_board & 0x8000000000000000){
                bit = 0x8000000000000000;
                while(black_board & bit){
                    value += 1;
                    bit = (bit >> 1) & 0x7fffffffffffffff;
                }
                bit = 0x8000000000000000;
                while(black_board & bit){
                    value += 1;
                    bit = (bit >> 8) & 0x00ffffffffffffff;
                }
            }
            else{
                bit = 0x8000000000000000;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit >> 1) & 0x7fffffffffffffff;
                }
                bit = 0x8000000000000000;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit >> 8) & 0x00ffffffffffffff;
                }
            }
        }
        if((black_board | white_board) & 0x0100000000000000){
            if(black_board & 0x0100000000000000){
                bit = 0x0100000000000000;
                while(black_board & bit){
                    value += 1;
                    bit = (bit << 1) & 0xfffffffffffffffe;
                }
                bit = 0x0100000000000000;
                while(black_board & bit){
                    value += 1;
                    bit = (bit >> 8) & 0x00ffffffffffffff;
                }
            }
            else{
                bit = 0x0100000000000000;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit << 1) & 0xfffffffffffffffe;
                }
                bit = 0x0100000000000000;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit >> 8) & 0x00ffffffffffffff;
                }
            }
        }
        if((black_board | white_board) & 0x0000000000000080){
            if(black_board & 0x0000000000000080){
                bit = 0x0000000000000080;
                while(black_board & bit){
                    value += 1;
                    bit = (bit >> 1) & 0x7fffffffffffffff;
                }
                bit = 0x0000000000000080;
                while(black_board & bit){
                    value += 1;
                    bit = (bit << 8) & 0xffffffffffffff00;
                }
            }
            else{
                bit = 0x0000000000000080;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit >> 1) & 0x7fffffffffffffff;
                }
                bit = 0x0000000000000080;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit << 8) & 0xffffffffffffff00;
                }
            }
        }
        if((black_board | white_board) & 0x0000000000000001){
            if(black_board & 0x0000000000000001){
                bit = 0x0000000000000001;
                while(black_board & bit){
                    value += 1;
                    bit = (bit << 1) & 0xfffffffffffffffe;
                }
                bit = 0x0000000000000001;
                while(black_board & bit){
                    value += 1;
                    bit = (bit << 8) & 0xffffffffffffff00;
                }
            }
            else{
                bit = 0x0000000000000001;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit << 1) & 0xfffffffffffffffe;
                }
                bit = 0x0000000000000001;
                while(white_board & bit){
                    value -= 1;
                    bit = (bit << 8) & 0xffffffffffffff00;
                }
            }
        }

        return -value;
    }
    
public:
    OthelloGame(bool isAIcolorWhite = true) : black_board(0x0000000810000000),
                    white_board(0x0000001008000000),
                    black_turn(true),
                    isAIcolorWhite(isAIcolorWhite) {}
    
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
    
    void init(bool isAIcolorWhite = true){
        black_board = 0x0000000810000000;
        white_board = 0x0000001008000000;
        black_turn = true;
        this->isAIcolorWhite = isAIcolorWhite;
    }
    
    // (y, x) を bit座標に変換
    int64 vec2bit(int y, int x){
        return 1LL << ((8 * (7 - y)) + (7 - x));
    }
    
    // bit座標を (y, x) に変換
    Vec2 bit2vec(int64 bit){
        for(int i = 0; i < 64; i++){
            if(bit & (1LL << (63 - i))){
                return Vec2(i % 8, i / 8);
            }
        }
        return Vec2(-1, -1);
    }
    
    // 黒(または白)石の数を数える
    int countStone(const int stone){
        return __builtin_popcountll((stone == BLACK_STONE) ? black_board : white_board);
    }

    // (y, x) に石を置く、置けた場合はtrueを返す
    bool putStone(const int y, const int x){
        const int64 pos = vec2bit(y, x);
        if(const int64 rev = canPut(pos); rev != 0){
            revStone(pos, rev);
            return true;
        }
        
        return false;
    }
    
    // AIが石を置く。置いた石の座標 (y, x) を返す
    Vec2 AI(){
        Vec2 res(-1, -1);
        for(bool again = true; /* 相手がパスならtrue */ again; ){
            again = false;
            
            // 打てないならパス
            bool pass = true;
            for(int64 bit = 0x8000000000000000; bit != 0; bit = (bit >> 1) & 0x7fffffffffffffff){
                if(canPut(bit)){
                    pass = false;
                    break;
                }
            }
            if(pass){
                black_turn = !black_turn;
                return res;
            }
            
            int stone_sum = countStone(BLACK_STONE) + countStone(WHITE_STONE);
            if(stone_sum >= 64) return res;
            int64 pos = negaMax(stone_sum >= FINAL_STAGE_NUM ? 64 - stone_sum : SEARCH_LV, true, -INF, INF);
            int64 rev = canPut(pos);
            revStone(pos, rev);
            
            res = bit2vec(pos);
            
            // 相手が打てないならもう一度AIのターン
            for(int64 bit = 0x8000000000000000; bit != 0; bit = (bit >> 1) & 0x7fffffffffffffff){
                if(canPut(bit)) return res;
            }
            black_turn = !black_turn;
            again = true;
        }
        
        return res;
    }

    bool canPut(int y, int x){
        return canPut(vec2bit(y, x)) != 0;
    }
};

void showBoard(OthelloGame &othello, Vec2 put_pos){
    auto board = othello.getBorad();
    
    // 盤を描画
    Rect(left_board_pos, top_board_pos, board_width).draw(Palette::Darkgreen);
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
    
    // 石を打った位置を色付け
    if(0 <= put_pos.y && put_pos.y < 8 && 0 <= put_pos.x && put_pos.x < 8){
        Rect(left_board_pos + cell_width * put_pos.x, top_board_pos + cell_width * put_pos.y, cell_width).draw(ColorF(1.0, 0.8, 0.0, 0.3)); // 半透明で黄色の正方形
    }
}

// ゲームが終わっているかどうか
bool isGameOver(OthelloGame &othello){
    // プレイヤーもAIも打てないなら終わり
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            if(othello.canPut(y, x)){
                return false;
            }
        }
    }
    
    return true;
}


int_main() {
    // ウィンドウを手動リサイズ可能にする
    Window::SetStyle(WindowStyle::Sizable);
    
    Window::SetTitle(U"othello");
    
    Scene::SetBackground(Palette::Pink);
    
    const Font font(32, Typeface::Bold); // サイズ32の太いフォントを生成
    const Font title_font(80);
    
    const Audio bgm(Resource(U"sound/PositiveHappy.mp3"));
    const Audio put_sound(Resource(U"sound/put.mp3"));
    const Audio title_sound(Resource(U"sound/title.mp3"));
    const Audio start_sound(Resource(U"sound/start.mp3"));
    const Audio gameover_sound(Resource(U"sound/end.mp3"));
    
    double volume = 0.0;
    bgm.setVolume(volume);
    
    const Texture othello_picture(Resource(U"picture/othello_game.png"));
    
    OthelloGame othello;
    
    while (System::Update()) {
        static bool titleScene = true;
        static Vec2 put_pos(-1, -1);
        static bool turn_AI = false;
        
        // 右側のメニュー
        if(SimpleGUI::Button(U"タイトル", Vec2(600, 250), 150)){
            bgm.stop();
            titleScene = true;
            title_sound.playOneShot();
        }
        if(SimpleGUI::Button(U"くろ", Vec2(600, 350), 150)){
            othello.init();
            put_pos = {-1, -1};
            bgm.play();
            titleScene = false;
            start_sound.playOneShot();
        }
        if(SimpleGUI::Button(U"しろ", Vec2(600, 420), 150)){
            othello.init(false);
            put_pos = {-1, -1};
            turn_AI = true;
            bgm.play();
            titleScene = false;
            start_sound.playOneShot();
        }
        
        // 音量調整
        if(SimpleGUI::Slider((volume > 0.0? U"BGM🔔" : U"BGM🔕"), volume, Vec2(50, 545))){
            bgm.setVolume(volume);
        }
        
        if(titleScene){
            title_font(U"オ セ ロ").draw(130, 100, Palette::Green);
            font(U"※効果音出ます").draw(260, 545, Palette::Black);
            
            othello_picture.scaled(0.3).draw(150, 250);
        }
        else{
            showBoard(othello, put_pos);
            
            if(turn_AI){
                put_pos = othello.AI();
                turn_AI = false;
                put_sound.playOneShot();
            }
            
            static bool sound_played = false; // 効果音を一回だけ鳴らす用のフラグ
            if(bool game_over = isGameOver(othello); game_over){
                int black_num = othello.countStone(BLACK_STONE);
                int white_num = othello.countStone(WHITE_STONE);
                
                if(black_num > white_num){
                    font(U"くろのかち！").draw(580, 200, Palette::Black);
                }
                else if(black_num < white_num){
                    font(U"しろのかち！").draw(580, 200, Palette::White);
                }
                else{
                    font(U"ひきわけ！").draw(590, 200, Palette::Green);
                }
                
                if(!sound_played){
                    gameover_sound.playOneShot();
                    sound_played = true;
                }
            }
            else{
                sound_played = false;
            }
            
            if(MouseL.down()){
                Vec2 cur_pos = Cursor::Pos();
                if(cur_pos.x < left_board_pos || cur_pos.y < top_board_pos) continue;
                int x = (cur_pos.x - left_board_pos) / cell_width;
                int y = (cur_pos.y - top_board_pos) / cell_width;
                if(x < 0 || x >= 8 || y < 0 || y >= 8) continue;
                
                if(othello.putStone(y, x)){
                    put_pos = {x, y};
                    turn_AI = true;
                    showBoard(othello, put_pos);
                    put_sound.playOneShot();
                }
            }
        }
    }
}
