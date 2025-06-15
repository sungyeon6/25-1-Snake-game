#include <ncurses.h>
int main()
{
    WINDOW *window1;
    WINDOW *window2;
    WINDOW *window3;
    initscr();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    mvprintw(1, 1, "Press Spacebar/Enter to move on");
    refresh();
    getch();

    // window1: 뱀이 움직이는 공간
    window1 = newwin(24, 60, 3, 3);
    wbkgd(window1, COLOR_PAIR(1));
    wattron(window1, COLOR_PAIR(1));
    mvwprintw(window1, 1, 1, "Space for snake");
    wborder(window1, '*', '*', '*', '*', '@', '@', '@', '@');
    wrefresh(window1);
    getch();

    // window2: 점수 표시하는 곳
    window2 = newwin(10, 35, 3, 65);
    wbkgd(window2, COLOR_PAIR(1));
    wattron(window2, COLOR_PAIR(1));
    mvwprintw(window2, 1, 2, "Score Board");
    mvwprintw(window2, 4, 2, "B: Current Length / Max Length");
    mvwprintw(window2, 5, 2, "+: Acquired Growth Items");
    mvwprintw(window2, 6, 2, "-: Acquired Poison Items");
    mvwprintw(window2, 7, 2, "G: Number of Gate uses");
    wborder(window2, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wrefresh(window2);
    getch();

    // window3: 미션 표시하는 곳
    window3 = newwin(10, 35, 14, 65);
    wbkgd(window3, COLOR_PAIR(1));
    wattron(window3, COLOR_PAIR(1));
    mvwprintw(window3, 1, 1, "Mission");
    mvwprintw(window3, 4, 1, "B: 10  ( )");
    mvwprintw(window3, 5, 1, "+: 5   ( )");
    mvwprintw(window3, 6, 1, "-: 2   ( )");
    mvwprintw(window3, 7, 1, "G: 1   (v)");
    wborder(window3, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wrefresh(window3);
    getch();

    delwin(window1);
    endwin();
    return 0;
}
