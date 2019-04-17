#include <TVout.h>
#include <fontALL.h>

TVout TV;
uint8_t h_char, v_char;
uint32_t score, best;

// structure for the obstacle on the "road" and the player
struct character
{
   char sprite;
   int v_char;
   int h_char;
};

uint8_t const NB_OBSTACLE = 8;
character obstacles[NB_OBSTACLE];

character player;

void setup() {
    // Setup the random number generator
    Serial.begin(9600);
    randomSeed(analogRead(0));
    Serial.end();
    
    TV.begin(NTSC, 128, 96);
    TV.select_font(font4x6);

    h_char = TV.hres() / 4;
    v_char = TV.vres() / 6;

    best = 0; // highscore
    init_game();
}

void init_game() {
    // Create race track
    TV.clear_screen();    
    draw_border(35);
    
    // init score
    score = 0;

    // reset player position + draw player
    player.h_char = h_char / 2;
    player.v_char = v_char - 1;
    player.sprite = 88;
    draw_char(player.h_char, player.v_char, player.sprite);

    // generating initials obstacles
    generate_all_obstacles();
    draw_all_obstacles();
}

void draw_char(uint8_t h_char_pos, uint8_t v_char_pos, char c) {
    TV.print_char(h_char_pos*4, v_char_pos * 6, c);
}

void clear_char(uint8_t h_char_pos, uint8_t v_char_pos) {
    TV.print_char(h_char_pos*4, v_char_pos * 6, 32);
}

void move_player_up() {
    if (player.v_char > 1) {
        clear_char(player.h_char, player.v_char);
        player.v_char -= 1;
        draw_char(player.h_char, player.v_char, player.sprite);
    }
}

void move_player_down() {
    if (player.v_char < v_char - 1) {
        clear_char(player.h_char, player.v_char);
        player.v_char += 1;
        draw_char(player.h_char, player.v_char, player.sprite);
    }

}

void move_player_left() {
    if (player.h_char > 11) {
        clear_char(player.h_char, player.v_char);
        player.h_char -= 1;
        draw_char(player.h_char, player.v_char, player.sprite);

    }
}

void move_player_right() {
    if (player.h_char < 20) {
        clear_char(player.h_char, player.v_char);
        player.h_char += 1;
        draw_char(player.h_char, player.v_char, player.sprite);
    }
}

void generate_all_obstacles() {
    for (int i = 0; i < NB_OBSTACLE; i++){
        obstacles[i] = generate_obstacle(random(h_char / 2 - 5, h_char / 2 + 5), random(0, v_char - 1));
    }
}

void draw_all_obstacles() {
    for (int i = 0; i < NB_OBSTACLE; i++){
        draw_char(obstacles[i].h_char, obstacles[i].v_char, obstacles[i].sprite);
    }
}

void scroll_screen() {
    for (int i = 0; i < NB_OBSTACLE; i++){
        clear_char(obstacles[i].h_char, obstacles[i].v_char);
        obstacles[i].v_char += 1;
        draw_char(obstacles[i].h_char, obstacles[i].v_char, obstacles[i].sprite);
    }
}

character generate_obstacle(uint8_t h_char_pos, uint8_t v_char_pos) {
    character obs;
    obs.sprite = random(33, 48);
    obs.h_char = h_char_pos;
    obs.v_char = v_char_pos;

    return obs;
}

// Generate new obstacle if one goes offscreen
void renew_obstacle() {
    for (int i = 0; i < NB_OBSTACLE; i++) {
        if (obstacles[i].v_char >= v_char) {
            //delete *obstacles[i];
            obstacles[i] = generate_obstacle(random(h_char / 2 - 5, h_char / 2 + 5), 0);
            draw_char(obstacles[i].h_char, obstacles[i].v_char, obstacles[i].sprite);
        }
    }
}

// move the "player" automatically, sicne this is a self playing game.
void move_player() {
    bool moved = false;
    bool obs_front = false;
    bool obs_far_front = false; // 2 char in front
    bool obs_front_left = false;
    bool obs_front_right = false;
    bool obs_left = false;
    bool obs_right = false;
    bool obs_behind = false;

    // Checking if obstacles are near the player
    for (int i = 0; i < NB_OBSTACLE; i++) {
        // Check if there is an obstacle on the line in front of player (2 char ahead)
        if (obstacles[i].v_char == player.v_char - 2) {
            // Check if this obstacle is directly in front of player
            if (obstacles[i].h_char == player.h_char) {
                obs_far_front = true;
            }
        }

        // Check if there is an obstacle on the line in front of player
        if (obstacles[i].v_char == player.v_char - 1) {
            // Check if this obstacle is directly in front of player
            if (obstacles[i].h_char == player.h_char) {
                obs_front = true;
            }
            if (obstacles[i].h_char == player.h_char - 1) {
                obs_front_left = true;
            }
            if (obstacles[i].h_char == player.h_char + 1) {
                obs_front_right = true;
            }
        }

        // Check if there is an obstacle on the same line as player
        if (obstacles[i].v_char == player.v_char) {
            if (obstacles[i].h_char == player.h_char - 1) {
                obs_left = true;
            }
            if (obstacles[i].h_char == player.h_char + 1) {
                obs_right = true;
            }
        }
        // Check if there is an obstacle behind player
        if (obstacles[i].v_char == player.v_char + 1) {
            if (obstacles[i].h_char == player.h_char) {
                obs_behind = true;
            }
        }
    }
    
    // Deciding the next move
    if (obs_front) {
        if (! obs_front_left and ! obs_left and player.h_char > 11) {
            move_player_left();
        }
        else if (! obs_front_right and ! obs_right and player.h_char < 21) {
            move_player_right();
        }
        else if (! obs_behind and player.v_char < v_char - 1) {
            move_player_down();
        }
    }
    // basically we do what we want
    else if (! obs_front and ! obs_far_front and ! obs_front_left and ! obs_front_right and ! obs_left and ! obs_right and ! obs_behind) {
        switch (random(0,4)) {
            case 0:
                move_player_right();
                break;
            case 1:
                move_player_left();
                break;
            case 2:
                move_player_up();
                break;
            case 3:
                move_player_down();
                break;
        }
    }
}

void check_colision() {
    for (int i = 0; i < NB_OBSTACLE; i++) {
        if (obstacles[i].v_char == player.v_char) {
            if (obstacles[i].h_char == player.h_char){
                TV.set_cursor(11 * 4, 0); // 11 * 4 = 11 char from left side
                TV.write("Game");
                TV.set_cursor(11 * 4, 6); // 11 * 4 = 11 char from left side
                TV.write("Over");
                TV.delay(2000);

                if (score > best) {
                    best = score;
                }

                init_game();
            }
        }
    }
}

void increase_score() {
    char str[10];
    sprintf(str, "%u", ++score);
    TV.set_cursor(0, 0);
    TV.write(str);
    TV.set_cursor(0, 6);
    sprintf(str, "%u", best);
    TV.write(str);
}

void draw_border(char c) {
    for(int y=0;y < v_char; y++){
        for(int x=0; x < h_char; x++){
            if (x < h_char / 2 - 5 || x > h_char / 2 + 4) {
                TV.print_char(x*4,y*6, c); //#
            }
        }
    }
}

void loop() {
    increase_score();
    TV.delay(75);
    move_player();
    check_colision();
    scroll_screen();
    renew_obstacle();
    check_colision();
}