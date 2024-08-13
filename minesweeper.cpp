#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <fstream>
#include <ncurses.h>
#include <random>
#include <ncurses.h>

using namespace std;

void print_board(vector < vector <char> > board) {
    int num_rows = board.size();
    int num_cols = board[0].size();
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0 ; j < num_cols; j++) {
            cout << board[i][j];
        }
        cout << '\n';
    }
}

void print_board(vector < vector <int> > flags) {
    int num_rows = flags.size();
    int num_cols = flags[0].size();
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            cout << flags[i][j];
        }
        cout << '\n';
    }
}

int mine_counter(vector < vector <char> > board, int row, int col) {
    int max_row = board.size();
    int max_col = board[0].size();
    int count = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int a = 2 * (row + i) + 1;
            int b = 4 * (col + j) + 2;
            if (a > -1 && b > -1 && a < max_row && b < max_col && board[a][b] == 'o') {
                count++;
            }
        }
    }
    return count;
}

vector<int> difficulty(string s) {
    vector<int> array;
    if (s == "Easy") {
        array.push_back(9);
        array.push_back(9);
        array.push_back(10);
    }
    if (s == "Medium") {
        array.push_back(16);
        array.push_back(16);
        array.push_back(40);
    }
    if (s == "Hard") {
        array.push_back(16);
        array.push_back(30);
        array.push_back(99);
    }
    return array;
}

void fill_horizontal(vector < vector <char> > &board) {
    int rows = board.size();
    int cols = board[0].size();
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j += 2) {
            board[j][i] = '-';
        } 
    }
}

void fill_vertical(vector <vector <char> > &board) {
    int rows = board.size();
    int cols = board[0].size();
    for (int j = 0; j < (cols - 1) / 4; j++) {
        for (int i = 1; i < rows; i += 2) {
            board[i][4 * j] = '|';
            board[i][4 * j + 1] = ' ';
            board[i][4 * j + 2] = ' ';
            board[i][4 * j + 3] = ' ';
            board[i][4 * j + 4] = '|';
        } 
    }
}

void initialise_mines(vector <vector <char> > &board, int num_of_mines, vector <vector <int> > &mines) {
    int up_row = board.size();
    int up_col = board[0].size();
    up_row = (up_row - 1) / 2;
    up_col = (up_col - 1) / 4;
    do {
        random_device rd;
        mt19937 gen(rd());
        int upper_bound_row = up_row;
        int upper_bound_col = up_col;
        uniform_int_distribution<int> distribution_row(0, up_row - 1);
        uniform_int_distribution<int> distribution_col(0, up_col - 1);
        int random_row = distribution_row(gen);
        int random_col = distribution_col(gen);
        if (mines[random_row][random_col] == 0) {
            board[2 * random_row + 1][4 * random_col + 2] = 'o';
            num_of_mines--;
            mines[random_row][random_col] = 1;
        }
    } while(num_of_mines > 0);
}

void number_the_board(vector <vector <char> > &board) {
    int rows = board.size();
    int cols = board[0].size();
    for (int i = 0; i < (rows - 1) / 2; i++) {
        for (int j = 0; j < (cols - 1) / 4; j++) {
            if (board[2 * i + 1][4 * j + 2] == ' ') {
                int temp = mine_counter(board, i, j);
                char num = '0' + temp;
                if (temp != 0) {
                    board[2 * i + 1][4 * j + 2] = num;
                }
            }
        }
    }
} 

void massive_clear(vector <vector <char> > board, vector < vector <int> > &clear_flag, int row, int col, int max_row, int max_col, int *clear_count, vector < vector <int> > mine_flag) {
    if (clear_flag[row][col] == 0 && board[2 * row + 1][4 * col + 2] != 'o' && mine_flag[row][col] == 0) {
        clear_flag[row][col] = 1;
        (*clear_count)++;
        if (board[2 * row + 1][4 * col + 2] == ' ') {
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    if (row + i > -1 && row + i < max_row && col + j > -1 && col + j < max_col) {
                        massive_clear(board, clear_flag, row + i, col + j, max_row, max_col, clear_count, mine_flag);
                    }
                }
            }
        }
    }
}

void print_blue(vector <vector <char> > board, vector <vector <int> > mine_flag) {
    int num_rows = board.size();
    int num_cols = board[0].size();
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (i % 2 == 1 && j % 4 != 0 && mine_flag[i / 2][j / 4] == 1) {
                attron(COLOR_PAIR(2));
                mvaddch(i, j, ' ');
                attroff(COLOR_PAIR(2));
            }
            else {
                attron(COLOR_PAIR(3));
                mvaddch(i, j, board[i][j]);
                attron(COLOR_PAIR(3));
            }
        }
    }
}

void win(int clear_count, int actual, int num_rows, vector <vector <char> > board, vector <vector <int> > mine_flag) {
    if (clear_count == actual) {
        print_blue(board, mine_flag);
        mvprintw(2 * num_rows + 4, 0, "You win!\n");
        refresh();
        getch();
        endwin();
        exit(0);
    }
}

void print_green(vector < vector <char> > board, vector < vector <int> > flags, int mine_count, int clear_count, vector < vector <int> > mine_flag) {
    int num_rows = board.size();
    int num_cols = board[0].size();
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (i % 2 == 1 && j % 4 != 0 && flags[i / 2][j / 4] == 0 && mine_flag[i / 2][j / 4] == 0) {
                attron(COLOR_PAIR(1));
                mvaddch(i, j, ' ');
                attroff(COLOR_PAIR(1));
            }
            else if (i % 2 == 1 && j % 4 != 0 && flags[i / 2][j / 4] == 0 && mine_flag[i / 2][j / 4] == 1) {
                attron(COLOR_PAIR(2));
                mvaddch(i, j, ' ');
                attroff(COLOR_PAIR(2));
            }
            else {
                attron(COLOR_PAIR(3));
                mvaddch(i, j, board[i][j]);
                attron(COLOR_PAIR(3));
            }
        }
    }
    mvprintw(num_rows/2, num_cols + 5, "                   ", mine_count);
    mvprintw(num_rows/2 + 1, num_cols + 5, "                     ", clear_count);
    mvprintw(num_rows/2, num_cols + 5, "Remaining mines: %d", mine_count);
    mvprintw(num_rows/2 + 1, num_cols + 5, "Remaining spcaes: %d", clear_count);
    refresh();
}

int main() {
    string diff;
    initscr();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    curs_set(0);
    int rows1, cols1;
    getmaxyx(stdscr, rows1, cols1);
    int middleRow = rows1 / 2;
    int middleCol = cols1 / 2;
    mvprintw(middleRow, middleCol - 11, "Easy\tMedium\tHard");
    int cha;
    MEVENT Event;
    while(1) {
        cha = getch();
        if (cha == KEY_MOUSE) {
            if (getmouse(&Event) == OK) {
                int row = Event.y;
                int col = Event.x;
                if (Event.bstate & BUTTON1_CLICKED) {
                    if (row == middleRow) {
                        int temp = col - middleCol;
                        if (temp > -12 && temp < -7) {
                            diff = "Easy";
                            break;
                        }
                        else if (temp < 2 && temp > -6) {
                            diff = "Medium";
                            break;
                        }
                        else if (temp > 2 && temp < 7) {
                            diff = "Hard";
                            break;
                        }
                    }
                }
            }
        }
    }
    refresh();
    clear();
    endwin();
    vector <int> level = difficulty(diff);
    vector < vector <char> > board(2 * level[0] + 1, vector<char>(4 * level[1] + 1, ' '));
    int num_rows =  level[0];
    int num_cols =  level[1];
    int num_mines =  level[2];
    int not_mines = num_cols * num_rows - num_mines;
    vector < vector <int> > mines(level[0], vector<int> (level[1], 0));
    vector < vector <int> > clear_flag(level[0], vector<int> (level[1], 0));
    vector < vector <int> > mine_flag(level[0], vector<int> (level[1], 0));
    vector < vector <int> > mine_flag_ans(level[0], vector<int> (level[1], 0));
    int flags_used = 0;
    for (int i = 0; i < num_rows; i++) {
        fill_vertical(board);
        fill_horizontal(board);
    }
    initialise_mines(board, num_mines, mines);
    number_the_board(board);
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            if (board[2 * i + 1][4 * j + 2] == 'o') {
                mine_flag_ans[i][j] = 1;
            }
        }
    }
    int clear_count = 0;
    initscr();
    refresh();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    init_pair(2, COLOR_BLUE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_RED);
    init_pair(5, COLOR_PAIR(0), COLOR_BLACK);
    wbkgd(stdscr, COLOR_PAIR(5));
    noecho();
    curs_set(0);
    print_green(board, clear_flag, num_mines, num_cols * num_rows - clear_count, mine_flag);
    wrefresh(stdscr);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    int ch;
    MEVENT event;
    while ((ch = getch()) != 'q') {
        if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                int row = event.y;
                int col = event.x;
                if (row < 2 * num_rows + 1 && col < 4 * num_cols + 1 && clear_flag[row / 2][col / 4] == 0) { 
                    if (event.bstate & BUTTON1_CLICKED) {
                        if (row % 2 == 1 && col % 4 != 0) {
                            if (board[row][col] != 'o') {
                                massive_clear(board, clear_flag, row / 2, col / 4, num_rows, num_cols, &clear_count, mine_flag);
                                print_green(board, clear_flag, num_mines - flags_used, num_cols * num_rows - clear_count, mine_flag);
                            }
                            else {
                                mvprintw(0, 4 * num_cols + 3, "Oh is it");
                                int rows1 = board.size();
                                int cols1 = board[0].size();
                                for (int i = 0; i < rows1; i++) {
                                    for (int j = 0; j < cols1; j++) {
                                        if (i % 2 == 1 && j % 4 != 0 && mine_flag_ans[i / 2][j / 4] == 1) {
                                            attron(COLOR_PAIR(4));
                                            mvaddch(i + 2 * num_rows + 7, j, ' ');
                                            attroff(COLOR_PAIR(4));
                                        }
                                        else {
                                            attron(COLOR_PAIR(3));
                                            mvaddch(i + 2 * num_rows + 7, j, board[i][j]);
                                            attron(COLOR_PAIR(3));
                                        }
                                    }
                                }
                                col = 4 * (col / 4) + 2;
                                attron(COLOR_PAIR(4));
                                mvaddch(row, col - 1, ' ');
                                mvaddch(row, col, ' ');
                                mvaddch(row, col + 1, ' ');
                                attroff(COLOR_PAIR(4));
                                getch();
                                endwin();
                                exit(0);
                            }
                        }
                    }
                    else if (event.bstate & BUTTON3_CLICKED) {
                        if (row % 2 == 1 && col % 4 != 0 && flags_used < num_mines) {
                            mine_flag[row / 2][col / 4]++;
                            mine_flag[row / 2][col / 4] = mine_flag[row / 2][col / 4] % 2;
                            col = 4 * (col / 4) + 2;
                            if (mine_flag[row / 2][col / 4] == 1) {
                                attron(COLOR_PAIR(2));
                                mvaddch(row, col - 1, ' ');
                                mvaddch(row, col, ' ');
                                mvaddch(row, col + 1, ' ');
                                attroff(COLOR_PAIR(2));
                                flags_used++;
                            }
                            else {
                                attron(COLOR_PAIR(1));
                                mvaddch(row, col - 1, ' ');
                                mvaddch(row, col, ' ');
                                mvaddch(row, col + 1, ' ');
                                attroff(COLOR_PAIR(1));
                                flags_used--;
                            }
                            attron(COLOR_PAIR(3));
                            mvprintw(num_rows, 4 * num_cols + 6, "                   ", num_mines - flags_used);
                            mvprintw(num_rows, 4 * num_cols + 6, "Remaining mines: %d", num_mines - flags_used);
                            attron(COLOR_PAIR(3));
                        }
                    }
                }
            }
        }
        win(clear_count, not_mines, num_rows, board, mine_flag_ans);
    }
    endwin();
    return 0;
}
