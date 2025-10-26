#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#define NUM_ROWS 8
#define NUM_COLS 8
#define NUM_SQUARES (NUM_ROWS * NUM_COLS)
#define coord_hash(row, col) (row * NUM_COLS + col)
#define row_in_bounds(row) ((row) >= 0 && (row) < NUM_ROWS)
#define col_in_bounds(col) ((col) >= 0 && (col) < NUM_COLS)
#define coord_in_bounds(row, col) (row_in_bounds(row) && col_in_bounds(col))

typedef uint8_t PieceType;
#define PIECE_TYPE_NONE         0u
#define PIECE_TYPE_PAWN_WHITE   1u
#define PIECE_TYPE_KNIGHT_WHITE 2u
#define PIECE_TYPE_BISHOP_WHITE 3u
#define PIECE_TYPE_ROOK_WHITE   4u
#define PIECE_TYPE_QUEEN_WHITE  5u
#define PIECE_TYPE_KING_WHITE   6u
#define PIECE_TYPE_PAWN_BLACK   7u
#define PIECE_TYPE_KNIGHT_BLACK 8u
#define PIECE_TYPE_BISHOP_BLACK 9u
#define PIECE_TYPE_ROOK_BLACK   10u
#define PIECE_TYPE_QUEEN_BLACK  11u
#define PIECE_TYPE_KING_BLACK   12u
#define NUM_PIECE_TYPES         13u

#define PieceType_is_white(piece) ((piece) >= PIECE_TYPE_PAWN_WHITE && (piece) <= PIECE_TYPE_KING_WHITE)
#define PieceType_is_black(piece) ((piece) >= PIECE_TYPE_PAWN_BLACK && (piece) <= PIECE_TYPE_KING_BLACK)

struct PieceState {
    PieceType type;
};

struct Context {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *sprite_sheet;
    uint64_t highlight_state;
    SDL_FRect sprite_srcrects[NUM_PIECE_TYPES];
    union {
        struct PieceState grid[NUM_ROWS][NUM_COLS];
        struct PieceState flattened[NUM_ROWS * NUM_COLS];
    } piece_state;
    struct { float pos_x, pos_y; } mouse_down_coords;
    struct { int row, col; } last_moved;
} global_context = {};

void _context_init_sprites() {
    global_context.sprite_sheet = IMG_LoadTexture(global_context.renderer, "pieces_sprite_sheet.png");
    if (global_context.sprite_sheet) {
        float texture_width, texture_height;
        SDL_GetTextureSize(global_context.sprite_sheet, &texture_width, &texture_height);
        float src_width = texture_width / 4.0f;
        float src_height = texture_width / 4.0f;
        global_context.sprite_srcrects[PIECE_TYPE_PAWN_WHITE  ] = (SDL_FRect){.x = 0.0f            , .y = src_height * 0.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_KNIGHT_WHITE] = (SDL_FRect){.x = src_width       , .y = src_height * 0.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_BISHOP_WHITE] = (SDL_FRect){.x = src_width * 2.0 , .y = src_height * 0.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_ROOK_WHITE  ] = (SDL_FRect){.x = src_width * 3.0 , .y = src_height * 0.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_QUEEN_WHITE ] = (SDL_FRect){.x = 0.0f            , .y = src_height * 1.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_KING_WHITE  ] = (SDL_FRect){.x = src_width       , .y = src_height * 1.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_PAWN_BLACK  ] = (SDL_FRect){.x = 0.0f            , .y = src_height * 2.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_KNIGHT_BLACK] = (SDL_FRect){.x = src_width       , .y = src_height * 2.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_BISHOP_BLACK] = (SDL_FRect){.x = src_width * 2.0 , .y = src_height * 2.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_ROOK_BLACK  ] = (SDL_FRect){.x = src_width * 3.0 , .y = src_height * 2.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_QUEEN_BLACK ] = (SDL_FRect){.x = 0.0f            , .y = src_height * 3.0f, .w = src_width, .h = src_height};
        global_context.sprite_srcrects[PIECE_TYPE_KING_BLACK  ] = (SDL_FRect){.x = src_width       , .y = src_height * 3.0f, .w = src_width, .h = src_height};
    }
}

void _context_init_board_state() {
    _Static_assert(NUM_ROWS > 7);
    _Static_assert(NUM_COLS > 7);

    global_context.piece_state.grid[0][0].type = PIECE_TYPE_ROOK_BLACK;
    global_context.piece_state.grid[0][1].type = PIECE_TYPE_KNIGHT_BLACK;
    global_context.piece_state.grid[0][2].type = PIECE_TYPE_BISHOP_BLACK;
    global_context.piece_state.grid[0][3].type = PIECE_TYPE_QUEEN_BLACK;
    global_context.piece_state.grid[0][4].type = PIECE_TYPE_KING_BLACK;
    global_context.piece_state.grid[0][5].type = PIECE_TYPE_BISHOP_BLACK;
    global_context.piece_state.grid[0][6].type = PIECE_TYPE_KNIGHT_BLACK;
    global_context.piece_state.grid[0][7].type = PIECE_TYPE_ROOK_BLACK;

    global_context.piece_state.grid[1][0].type = PIECE_TYPE_PAWN_BLACK;
    global_context.piece_state.grid[1][1].type = PIECE_TYPE_PAWN_BLACK;
    global_context.piece_state.grid[1][2].type = PIECE_TYPE_PAWN_BLACK;
    global_context.piece_state.grid[1][3].type = PIECE_TYPE_PAWN_BLACK;
    global_context.piece_state.grid[1][4].type = PIECE_TYPE_PAWN_BLACK;
    global_context.piece_state.grid[1][5].type = PIECE_TYPE_PAWN_BLACK;
    global_context.piece_state.grid[1][6].type = PIECE_TYPE_PAWN_BLACK;
    global_context.piece_state.grid[1][7].type = PIECE_TYPE_PAWN_BLACK;

    global_context.piece_state.grid[6][0].type = PIECE_TYPE_PAWN_WHITE;
    global_context.piece_state.grid[6][1].type = PIECE_TYPE_PAWN_WHITE;
    global_context.piece_state.grid[6][2].type = PIECE_TYPE_PAWN_WHITE;
    global_context.piece_state.grid[6][3].type = PIECE_TYPE_PAWN_WHITE;
    global_context.piece_state.grid[6][4].type = PIECE_TYPE_PAWN_WHITE;
    global_context.piece_state.grid[6][5].type = PIECE_TYPE_PAWN_WHITE;
    global_context.piece_state.grid[6][6].type = PIECE_TYPE_PAWN_WHITE;
    global_context.piece_state.grid[6][7].type = PIECE_TYPE_PAWN_WHITE;

    global_context.piece_state.grid[7][0].type = PIECE_TYPE_ROOK_WHITE;
    global_context.piece_state.grid[7][1].type = PIECE_TYPE_KNIGHT_WHITE;
    global_context.piece_state.grid[7][2].type = PIECE_TYPE_BISHOP_WHITE;
    global_context.piece_state.grid[7][3].type = PIECE_TYPE_QUEEN_WHITE;
    global_context.piece_state.grid[7][4].type = PIECE_TYPE_KING_WHITE;
    global_context.piece_state.grid[7][5].type = PIECE_TYPE_BISHOP_WHITE;
    global_context.piece_state.grid[7][6].type = PIECE_TYPE_KNIGHT_WHITE;
    global_context.piece_state.grid[7][7].type = PIECE_TYPE_ROOK_WHITE;    
}

bool context_initialise() {
    static int const WINDOW_DIMENSION_DEFAULT = 512;

    if (SDL_CreateWindowAndRenderer(
        "Chess",
        WINDOW_DIMENSION_DEFAULT,
        WINDOW_DIMENSION_DEFAULT,
        SDL_WINDOW_RESIZABLE,
        &global_context.window,
        &global_context.renderer
    )) {
        SDL_SetWindowAspectRatio(global_context.window, 1.0f, 1.0f);
        SDL_SetRenderDrawBlendMode(global_context.renderer, SDL_BLENDMODE_BLEND);
        _context_init_sprites();
        _context_init_board_state();
    } else {
        SDL_Log("Error in context_initialise: failed to create window and renderer. SDL error: %s", SDL_GetError());
    }

    return global_context.window && global_context.sprite_sheet;
}

void context_deinitialise() {
    SDL_DestroyWindow(global_context.window);   // Also destroys renderer and all associated textures
}

void render_board() {
    static SDL_FColor const dark = {.r = 0.5f, .g = 0.3f, .b = 0.1f, .a = 1.0f};
    static SDL_FColor const light = {.r = 0.9f, .g = 0.8f, .b = 0.6f, .a = 1.0f};
    static SDL_FColor const highlight = {.r = 0.2f, .g = 0.5f, .b = 0.2f, .a = 0.5f};
    
    int board_width, board_height;
    SDL_GetWindowSize(global_context.window, &board_width, &board_height);
    float square_dimension = (board_height < board_width ? board_height : board_width) / 8.0f;

    SDL_FRect const light_squares[32] = {
        {.x = 0.0f,                    .y = 0.0f,                    .w = square_dimension, .h = square_dimension},
        {.x = 2.0f * square_dimension, .y = 0.0f,                    .w = square_dimension, .h = square_dimension},
        {.x = 4.0f * square_dimension, .y = 0.0f,                    .w = square_dimension, .h = square_dimension},
        {.x = 6.0f * square_dimension, .y = 0.0f,                    .w = square_dimension, .h = square_dimension},
        {.x = 1.0f * square_dimension, .y = 1.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 3.0f * square_dimension, .y = 1.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 5.0f * square_dimension, .y = 1.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 7.0f * square_dimension, .y = 1.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 0.0f,                    .y = 2.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 2.0f * square_dimension, .y = 2.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 4.0f * square_dimension, .y = 2.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 6.0f * square_dimension, .y = 2.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 1.0f * square_dimension, .y = 3.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 3.0f * square_dimension, .y = 3.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 5.0f * square_dimension, .y = 3.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 7.0f * square_dimension, .y = 3.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 0.0f,                    .y = 4.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 2.0f * square_dimension, .y = 4.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 4.0f * square_dimension, .y = 4.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 6.0f * square_dimension, .y = 4.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 1.0f * square_dimension, .y = 5.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 3.0f * square_dimension, .y = 5.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 5.0f * square_dimension, .y = 5.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 7.0f * square_dimension, .y = 5.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 0.0f,                    .y = 6.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 2.0f * square_dimension, .y = 6.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 4.0f * square_dimension, .y = 6.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 6.0f * square_dimension, .y = 6.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 1.0f * square_dimension, .y = 7.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 3.0f * square_dimension, .y = 7.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 5.0f * square_dimension, .y = 7.0f * square_dimension, .w = square_dimension, .h = square_dimension},
        {.x = 7.0f * square_dimension, .y = 7.0f * square_dimension, .w = square_dimension, .h = square_dimension},
    };
    
    SDL_SetRenderDrawColorFloat(global_context.renderer, dark.r, dark.g, dark.b, dark.a);
    SDL_RenderClear(global_context.renderer);
    SDL_SetRenderDrawColorFloat(global_context.renderer, light.r, light.g, light.b, light.a);
    SDL_RenderFillRects(global_context.renderer, light_squares, 32);

    SDL_FRect highlight_rects[27] = {};   // The maximum number of squares that a piece can control is 27 (queen in centre of empty board)
    int num_highlights = 0;
    uint64_t highlight_state = global_context.highlight_state;
    
    while (highlight_state) {
        SDL_assert(num_highlights < 27);
        int index = __builtin_ctzll(highlight_state);
        highlight_rects[num_highlights++] = (SDL_FRect){
            .x = square_dimension * (index % 8),
            .y = square_dimension * (index / 8),
            .w = square_dimension,
            .h = square_dimension,
        };
        highlight_state &= (highlight_state - 1u);  // clears least significant set bit
    }

    SDL_SetRenderDrawColorFloat(global_context.renderer, highlight.r, highlight.g, highlight.b, highlight.a);
    SDL_RenderFillRects(global_context.renderer, highlight_rects, num_highlights);

    SDL_FRect destrect = {.w = square_dimension, .h = square_dimension};
    for (int col = 0; col < 8; ++col) {
        destrect.x = col * square_dimension;
        for (int row = 0; row < 8; ++row) {
            destrect.y = row * square_dimension;
            SDL_FRect *srcrect = &global_context.sprite_srcrects[global_context.piece_state.grid[row][col].type];
            SDL_RenderTexture(global_context.renderer, global_context.sprite_sheet, srcrect, &destrect);
        }
    }

    SDL_RenderPresent(global_context.renderer);
}

void _highlight_pawn(int row, int col) {
    SDL_assert(coord_in_bounds(row, col));
    SDL_assert(global_context.piece_state.grid[row][col].type == PIECE_TYPE_PAWN_WHITE || global_context.piece_state.grid[row][col].type == PIECE_TYPE_PAWN_BLACK);
    SDL_assert(global_context.highlight_state == 0ull);

    bool is_white = global_context.piece_state.grid[row][col].type == PIECE_TYPE_PAWN_WHITE;
    int row_next = row + (is_white ? -1 : 1);

    if (row_in_bounds(row_next)) {
        bool can_advance = global_context.piece_state.grid[row_next][col].type == PIECE_TYPE_NONE;
        global_context.highlight_state |= ((uint64_t)can_advance) << coord_hash(row_next, col);

        if (can_advance) {
            int row_home = is_white ? 6 : 1;
            int row_double = is_white ? 4 : 3;
            bool can_double = row == row_home && global_context.piece_state.grid[row_double][col].type == PIECE_TYPE_NONE;
            global_context.highlight_state |= ((uint64_t)can_double) << coord_hash(row_double, col);
        }

        int row_en_passant = is_white ? 3 : 4;
        bool can_en_passant = row == row_en_passant;
        
        int col_positive = col + 1;
        PieceType target_positive = col_in_bounds(col_positive) ? global_context.piece_state.grid[row_next][col_positive].type : PIECE_TYPE_NONE;
        bool can_capture_positive = is_white ? PieceType_is_black(target_positive) : PieceType_is_white(target_positive);

        int col_negative = col - 1;
        PieceType target_negative = col_in_bounds(col_negative) ? global_context.piece_state.grid[row_next][col_negative].type : PIECE_TYPE_NONE;
        bool can_capture_negative = is_white ? PieceType_is_black(target_negative) : PieceType_is_white(target_negative);

        PieceType opposite_colour_pawn = (is_white ? PIECE_TYPE_PAWN_BLACK : PIECE_TYPE_PAWN_WHITE);
        bool can_en_passant_positive = can_en_passant && global_context.piece_state.grid[row][col_positive].type == opposite_colour_pawn;
        bool can_en_passant_negative = can_en_passant && global_context.piece_state.grid[row][col_negative].type == opposite_colour_pawn;

        global_context.highlight_state |= ((uint64_t)(can_capture_positive || can_en_passant_positive)) << coord_hash(row_next, col_positive);
        global_context.highlight_state |= ((uint64_t)(can_capture_negative || can_en_passant_negative)) << coord_hash(row_next, col_negative);
    }
}

void _highlight_knight(int row, int col) {
    SDL_assert(row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS);
    SDL_assert(global_context.piece_state.grid[row][col].type == PIECE_TYPE_KNIGHT_WHITE || global_context.piece_state.grid[row][col].type == PIECE_TYPE_KNIGHT_BLACK);
    SDL_assert(global_context.highlight_state == 0ull);

    bool is_white = global_context.piece_state.grid[row][col].type == PIECE_TYPE_KNIGHT_WHITE;

    int row_target = row - 2;
    int col_target = col - 1;
    if (coord_in_bounds(row_target, col_target)) {
        int index = coord_hash(row_target, col_target);
        PieceType target = global_context.piece_state.flattened[index].type;
        bool can_access = target == PIECE_TYPE_NONE || (is_white ? PieceType_is_black(target) : PieceType_is_white(target));
        global_context.highlight_state |= (uint64_t)can_access << index;
    }

    col_target = col + 1;
    if (coord_in_bounds(row_target, col_target)) {
        int index = coord_hash(row_target, col_target);
        PieceType target = global_context.piece_state.flattened[index].type;
        bool can_access = target == PIECE_TYPE_NONE || (is_white ? PieceType_is_black(target) : PieceType_is_white(target));
        global_context.highlight_state |= (uint64_t)can_access << index;
    }

    row_target = row - 1;
    col_target = col - 2;
    if (coord_in_bounds(row_target, col_target)) {
        int index = coord_hash(row_target, col_target);
        PieceType target = global_context.piece_state.flattened[index].type;
        bool can_access = target == PIECE_TYPE_NONE || (is_white ? PieceType_is_black(target) : PieceType_is_white(target));
        global_context.highlight_state |= (uint64_t)can_access << index;
    }

    col_target = col + 2;
    if (coord_in_bounds(row_target, col_target)) {
        int index = coord_hash(row_target, col_target);
        PieceType target = global_context.piece_state.flattened[index].type;
        bool can_access = target == PIECE_TYPE_NONE || (is_white ? PieceType_is_black(target) : PieceType_is_white(target));
        global_context.highlight_state |= (uint64_t)can_access << index;
    }

    row_target = row + 1;
    col_target = col - 2;
    if (coord_in_bounds(row_target, col_target)) {
        int index = coord_hash(row_target, col_target);
        PieceType target = global_context.piece_state.flattened[index].type;
        bool can_access = target == PIECE_TYPE_NONE || (is_white ? PieceType_is_black(target) : PieceType_is_white(target));
        global_context.highlight_state |= (uint64_t)can_access << index;
    }

    col_target = col + 2;
    if (coord_in_bounds(row_target, col_target)) {
        int index = coord_hash(row_target, col_target);
        PieceType target = global_context.piece_state.flattened[index].type;
        bool can_access = target == PIECE_TYPE_NONE || (is_white ? PieceType_is_black(target) : PieceType_is_white(target));
        global_context.highlight_state |= (uint64_t)can_access << index;
    }

    row_target = row + 2;
    col_target = col - 1;
    if (coord_in_bounds(row_target, col_target)) {
        int index = coord_hash(row_target, col_target);
        PieceType target = global_context.piece_state.flattened[index].type;
        bool can_access = target == PIECE_TYPE_NONE || (is_white ? PieceType_is_black(target) : PieceType_is_white(target));
        global_context.highlight_state |= (uint64_t)can_access << index;
    }

    col_target = col + 1;
    if (coord_in_bounds(row_target, col_target)) {
        int index = coord_hash(row_target, col_target);
        PieceType target = global_context.piece_state.flattened[index].type;
        bool can_access = target == PIECE_TYPE_NONE || (is_white ? PieceType_is_black(target) : PieceType_is_white(target));
        global_context.highlight_state |= (uint64_t)can_access << index;
    }
}

void update_highlight_state(float mouse_up_pos_x, float mouse_up_pos_y) {
    int board_width, board_height;
    SDL_GetWindowSize(global_context.window, &board_width, &board_height);

    bool in_bounds = mouse_up_pos_x >= 0.0f && mouse_up_pos_x < board_width && mouse_up_pos_y >= 0.0f && mouse_up_pos_y < board_height;

    if (in_bounds) {
        float square_dimension = (board_height < board_width ? board_height : board_width) / 8.0f;
        int mouse_down_col = __builtin_floorf(global_context.mouse_down_coords.pos_x / square_dimension);
        int mouse_up_col = __builtin_floorf(mouse_up_pos_x / square_dimension);
        int mouse_down_row = __builtin_floorf(global_context.mouse_down_coords.pos_y / square_dimension);
        int mouse_up_row = __builtin_floorf(mouse_up_pos_y / square_dimension);

        if (mouse_down_col == mouse_up_col && mouse_down_row == mouse_up_row) {
            PieceType piece_type = global_context.piece_state.grid[mouse_up_row][mouse_up_col].type;
            SDL_assert(piece_type < NUM_PIECE_TYPES);
            global_context.highlight_state = 0llu;
            switch (piece_type) {
            case PIECE_TYPE_NONE:
                break;
            case PIECE_TYPE_PAWN_WHITE:
            case PIECE_TYPE_PAWN_BLACK:
                _highlight_pawn(mouse_up_row, mouse_up_col);
                break;
            case PIECE_TYPE_KNIGHT_WHITE:
            case PIECE_TYPE_KNIGHT_BLACK:
                _highlight_knight(mouse_up_row, mouse_up_col);
                break;
            case PIECE_TYPE_BISHOP_WHITE:
            case PIECE_TYPE_BISHOP_BLACK:
                // todo
                break;
            case PIECE_TYPE_ROOK_WHITE:
            case PIECE_TYPE_ROOK_BLACK:
                //todo
                break;
            case PIECE_TYPE_QUEEN_WHITE:
            case PIECE_TYPE_QUEEN_BLACK:
                //todo
                break;
            case PIECE_TYPE_KING_WHITE:
            case PIECE_TYPE_KING_BLACK:
                //todo
                break;
            default:
                __builtin_unreachable();
                break;
            }
        }
    }
}

int main() {
    bool exit = !context_initialise();
    while (!exit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
                exit = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                global_context.mouse_down_coords.pos_x = e.button.x;
                global_context.mouse_down_coords.pos_y = e.button.y;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                update_highlight_state(e.button.x, e.button.y);
                break;
            }
        }
        render_board();
    }
    context_deinitialise();
    return 0;
}