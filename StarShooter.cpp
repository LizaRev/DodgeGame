#include <ncurses.h>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;
const int STAR_COUNT = 10;

struct Bullet { int x, y; bool active; };
struct Star { int x, y; bool alive; int colorPair; int respawnTimer; };

int main() {
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    curs_set(0);

    start_color();

    // Кольори: яскраві символи на чорному фоні
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);    // зірка жовта
    init_pair(2, COLOR_CYAN, COLOR_BLACK);      // зірка блакитна
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);   // корабель фіолетовий

    int shipX = WIDTH / 2;
    vector<Bullet> bullets;
    vector<Star> stars;
    int score = 0;

    // Ініціалізація зірок (жовті та блакитні)
    for (int i = 0; i < STAR_COUNT; i++) {
        int color = (rand() % 2) + 1; // 1 або 2
        stars.push_back({rand() % WIDTH, rand() % 5, true, color, 0});
    }

    while (true) {
        int key = getch();
        if (key == 'a' || key == 'A')  shipX = max(1, shipX - 2);
        if (key == 'd' || key == 'D')  shipX = min(WIDTH - 2, shipX + 2);
        if (key == 'e' || key == 'E')  bullets.push_back({shipX, HEIGHT - 4, true});

        // Рух куль
        for (auto &b : bullets) {
            if (b.active) {
                b.y--;
                if (b.y < 0)  b.active = false;
            }
        }

        // Попадання по зірках
        for (auto &b : bullets) {
            if (!b.active) continue;
            for (auto &s : stars) {
                if (s.alive && b.y == s.y && b.x == s.x) {
                    s.alive = false;
                    s.respawnTimer = 3;
                    b.active = false;
                    score++;
                    break;
                }
            }
        }

        // Рух зірок вниз
        for (auto &s : stars) {
            if (s.alive) {
                s.y++;

                // Якщо впала на корабель
                if (s.y >= HEIGHT - 3 && abs(s.x - shipX) <= 1) {
                    score++;
                    s.alive = false;
                    s.respawnTimer = 3;
                }

                // Якщо дійшла дна
                if (s.y >= HEIGHT) {
                    s.alive = false;
                    s.respawnTimer = 3;
                }
            } else {
                // Відлік до відродження
                if (s.respawnTimer > 0) {
                    s.respawnTimer--;
                    if (s.respawnTimer == 0) {
                        s.y = 0;
                        s.x = rand() % WIDTH;
                        s.alive = true;
                        s.colorPair = (rand() % 2) + 1; // жовта або блакитна
                    }
                }
            }
        }

        // Малюємо екран
        clear();
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                bool printed = false;
                // Кулі
                for (auto &b : bullets) {
                    if (b.active && b.x == x && b.y == y) {
                        mvaddch(y, x, '|');
                        printed = true;
                        break;
                    }
                }
                // Зірки
                if (!printed) {
                    for (auto &s : stars) {
                        if (s.alive && s.x == x && s.y == y) {
                            attron(COLOR_PAIR(s.colorPair) | A_BOLD);
                            mvaddch(y, x, 'X');
                            attroff(COLOR_PAIR(s.colorPair) | A_BOLD);
                            printed = true;
                            break;
                        }
                    }
                }
                if (!printed) mvaddch(y, x, '.');
            }
        }

        // Корабель
        attron(COLOR_PAIR(3) | A_BOLD);
        mvaddch(HEIGHT - 3, shipX, 'O');
        mvaddch(HEIGHT - 2, shipX - 1, '/');
        mvaddch(HEIGHT - 2, shipX, '|');
        mvaddch(HEIGHT - 2, shipX + 1, '\\');
        mvaddch(HEIGHT - 1, shipX - 1, '/');
        mvaddch(HEIGHT - 1, shipX + 1, '\\');
        attroff(COLOR_PAIR(3) | A_BOLD);

        mvprintw(HEIGHT, 0, "Score: %d", score);

        refresh();
        this_thread::sleep_for(chrono::milliseconds(250));
    }

    endwin();
    return 0;
}
