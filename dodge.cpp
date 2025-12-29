#include <ncurses.h>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <algorithm>

using namespace std;


const int WIDTH = 50;
const int HEIGHT = 22;
const int START_LIVES = 3;


struct Player {
    int x;
    int lives;
};

struct Enemy {
    int x, y;
    int color;
};


void initNcurses() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);   
    init_pair(2, COLOR_RED, COLOR_BLACK);     
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);  
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK); 
    init_pair(5, COLOR_CYAN, COLOR_BLACK);   
    init_pair(6, COLOR_WHITE, COLOR_BLACK);   
}


void drawBorder() {
    attron(COLOR_PAIR(5));
    for (int x = 0; x < WIDTH; x++) {
        mvaddch(0, x, '-');
        mvaddch(HEIGHT - 1, x, '-');
    }
    for (int y = 0; y < HEIGHT; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, WIDTH - 1, '|');
    }
    attroff(COLOR_PAIR(5));
}


void drawUI(int score, int lives, int speed) {
    attron(COLOR_PAIR(6));
    mvprintw(HEIGHT, 2, "Score: %d", score);
    mvprintw(HEIGHT, 18, "Lives: %d", lives);
    mvprintw(HEIGHT, 32, "Speed: %d", speed);
    mvprintw(HEIGHT + 1, 2, "A/D or <- -> move | P pause | Q quit");
    attroff(COLOR_PAIR(6));
}

int main() {
    srand(time(nullptr));
    initNcurses();

    Player player;
    player.x = WIDTH / 2;
    player.lives = START_LIVES;

    vector<Enemy> enemies;

    int score = 0;
    int speed = 120;
    int spawnRate = 6; 
    bool paused = false;

    
    while (true) {
        clear();

       
        int ch = getch();
        if (ch == 'q') break;

        if (ch == 'p') paused = !paused;

        if (!paused) {
            if ((ch == 'a' || ch == KEY_LEFT) && player.x > 1)
                player.x--;
            if ((ch == 'd' || ch == KEY_RIGHT) && player.x < WIDTH - 2)
                player.x++;
        }

        if (paused) {
            drawBorder();
            mvprintw(HEIGHT / 2, WIDTH / 2 - 3, "PAUSE");
            refresh();
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }

       
        if (rand() % spawnRate == 0) {
            enemies.push_back({ rand() % (WIDTH - 2) + 1, 1, (rand() % 3) + 2 });
        }

       
        for (auto &e : enemies)
            e.y++;

        
        for (size_t i = 0; i < enemies.size(); i++) {
            if (enemies[i].y == HEIGHT - 2 &&
                enemies[i].x == player.x) {
                player.lives--;
                enemies.erase(enemies.begin() + i);
                break;
            }
        }

        
        enemies.erase(
            remove_if(enemies.begin(), enemies.end(),
                [](Enemy &e) { return e.y >= HEIGHT - 1; }),
            enemies.end()
        );

       
        if (score % 50 == 0 && speed > 40)
            speed--;

       
        if (score % 50 == 0 && spawnRate > 1)
            spawnRate--;

        
        if (player.lives <= 0) {
            clear();
            mvprintw(HEIGHT / 2 - 1, WIDTH / 2 - 5, "GAME OVER");
            mvprintw(HEIGHT / 2, WIDTH / 2 - 7, "Score: %d", score);
            mvprintw(HEIGHT / 2 + 2, WIDTH / 2 - 12,
                     "Press any key to exit");
            refresh();

            nodelay(stdscr, FALSE);
            getch();
            endwin();
            return 0;
        }

        
        drawBorder();

        for (auto &e : enemies) {
            attron(COLOR_PAIR(e.color));
            mvaddch(e.y, e.x, '#');
            attroff(COLOR_PAIR(e.color));
        }

        attron(COLOR_PAIR(1));
        mvaddch(HEIGHT - 2, player.x, '@');
        attroff(COLOR_PAIR(1));

        drawUI(score, player.lives, speed);

        score++;
        refresh();
        this_thread::sleep_for(chrono::milliseconds(speed));
    }

    endwin();
    return 0;
}
