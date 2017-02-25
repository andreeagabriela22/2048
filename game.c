#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

int board[4][4];        //tabla de joc
int bpredictiv[4][4];   //tabla folosita pentru prezicerea celei mai bune miscari
int scor = 0;           //scorul jucatorului
int rows,cols;          //numarul de linii si coloane disponibile in fereastra
int joc_inceput = 0;    //indicatorul de joc inceput

void rotire() {
    //va realiza rotirea tablei de joc, in sens trigonometric, cu 90 grade

    int i,j;
    int board_aux[4][4];

    //crearea tablei noi, rotite
    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            board_aux[j][i] = board[i][3-j];
        }
    }

    //actualizarea tablei de joc
    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            board[i][j] = board_aux[i][j];
        }
    }

}

void miscare(int k) {
    //va realiza o miscare pe tabla de joc, pe linia k, intotdeauna inspre stanga

    int i,j;
    int index;
    int increment_index;
    int modificare;

    index = 0;
    while (index<3) {
        i = index;
        modificare = 0;

        if (board[k][i] == 0) { //se translateaza toate elementele din vector
            board[k][i] = board[k][i+1];
            increment_index = 1; //presupunem ca va trebui sa incrementam index, pentru a nu intra intr-o bucla infinita
            for (j=i+1;j<3;j++) {
                if (board[k][j+1]>0) {
                    increment_index = 0;
                }
                board[k][j] = board[k][j+1];
            }
            board[k][3] = 0; //ultimul element devine 0
            if (increment_index) {
                index++; //se trece la un nou index
            }
        } else {
            for (j=i+1;j<4;j++) {
                if ((board[k][i] != board[k][j]) && (board[k][j]>0)) {
                    break; //nu se poate face nici o adaugare
                }
                if ((board[k][i] == board[k][j]) && (modificare==0)) { //se face o adaugare
                    board[k][i] = board[k][i] + board[k][j];
                    board[k][j] = 0;
                    scor = scor + board[k][i];
                    modificare = 1;
                }
            }
            index++; //se trece la un nou index
        }
    }
}

void rotire_predictiv() {
    //va realiza rotirea tablei predictive, in sens trigonometric, cu 90 grade

    int i,j;
    int board_aux[4][4];

    //crearea tablei noi, rotite
    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            board_aux[j][i] = bpredictiv[i][3-j];
        }
    }

    //actualizarea tablei de joc
    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            bpredictiv[i][j] = board_aux[i][j];
        }
    }

}

int miscare_predictiv(int k) {
    //va realiza o miscare pe tabla predictiva, pe linia k, intotdeauna inspre stanga

    int i,j;
    int index;
    int increment_index;
    int miscari_valide = 0; //contor miscari valide
    int celule_eliberate = 0; //contor celule eliberate
    int modificare;

    index = 0;
    while (index<3) {
        i = index;
        modificare = 0;

        if (bpredictiv[k][i] == 0) { //se translateaza toate elementele din vector
            miscari_valide++;
            bpredictiv[k][i] = bpredictiv[k][i+1];
            increment_index = 1; //presupunem ca va trebui sa incrementam index, pentru a nu intra intr-o bucla infinita
            for (j=i+1;j<3;j++) {
                if (bpredictiv[k][j+1]>0) {
                    increment_index = 0;
                }
                bpredictiv[k][j] = bpredictiv[k][j+1];
            }
            bpredictiv[k][3] = 0; //ultimul element devine 0
            if (increment_index) {
                index++; //se trece la un nou index
            }
        } else {
            for (j=i+1;j<4;j++) {
                if ((bpredictiv[k][i] != bpredictiv[k][j]) && (bpredictiv[k][j]>0)) {
                    break; //nu se poate face nici o adaugare
                }
                if ((bpredictiv[k][i] == bpredictiv[k][j]) && (modificare==0)) { //se face o adaugare
                    bpredictiv[k][i] = bpredictiv[k][i] + bpredictiv[k][j];
                    bpredictiv[k][j] = 0;
                    celule_eliberate++;
                    miscari_valide++;
                    modificare = 1;
                }
            }
            index++; //se trece la un nou index
        }
    }

    if (celule_eliberate>0) { //se pot elibera celule
        return celule_eliberate;
    } else { //nu se pot elibera celule
        if (miscari_valide==0) {
            return 101; //numar mai mare decat maximul posibil de celule eliberate, semnal ca nu exista miscari valide pe linia respectiva
        } else {
            return 0;
        }
    }

}

int check_directie(int directie) {
    //intoarce numarul de celule eliberate pe o directie

    int celule_eliberate = 0;
    int miscare_valida = 0; //presupunem ca miscarea nu este valida
    int i,j;

    //se initializeaza tabla predictiva cu tabla de joc curenta
    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            bpredictiv[i][j] = board[i][j];
        }
    }

    //verificare directie
    switch (directie) {
        //stanga
        case 1: break;
        //sus
        case 2: rotire_predictiv(); break;
        //dreapta
        case 3: rotire_predictiv(); rotire_predictiv(); break;
        //jos
        case 4: rotire_predictiv(); rotire_predictiv(); rotire_predictiv(); break;
    }

    //efectuare miscare
    for (i=0;i<4;i++) {
        j = miscare_predictiv(i);
        if (j==101) {
            //miscarea nu este valida
        } else {
            celule_eliberate = celule_eliberate + j;
            miscare_valida = 1; //miscarea este valida
        }
    }

    if (miscare_valida) {
        return celule_eliberate; //se intorc numarul de celule eliberate
    } else {
        return 101; //miscarea s-a dovedit ca nu este valida si se intoarce un numar mai mare decat maximul posibil de celule eliberate
    }
}

int directie_optima() {
    //identifica directia optima si intoarce valoarea ei sau 101 daca nu exista miscari

    int max = -1;
    int directie = 0;
    int i,j;

    for (i=1;i<=4;i++) {
        j = check_directie(i); //verifica directie
        //printw("directie %d -> %d\n",i,j);
        if (j<101) { //directie valida
            if (j>max) {
                max = j;
                directie = i;
            }
        }
    }

    if (directie>0) {
        return directie;
    } else {
        return 101; //nu mai exista o directie valida - GAME OVER
    }
}

int check_win() {
    //verifica daca exista pe tabla de joc valoarea 2048, care reprezinta ca ati castigat

    int i,j;
    int rezultat = 0;

    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            if (board[i][j] == 2048) {
                rezultat = 1;
                break;
            }
        }
    }

    return rezultat;
}

void afisare_game_win() {
    //afisare YOU WON

    int j;

    clear();

    j=1;
    attron(A_REVERSE|A_BOLD);
    mvprintw(j,(cols-30)/2, "YOU WON");
    attroff(A_REVERSE|A_BOLD);

    j=3;
    mvprintw(j,(cols-30)/2, "SCOR OBTINUT : %d",scor);

    refresh();

    getch();
}

void afisare_game_over() {
    //afisare GAME OVER

    int j;

    clear();

    j=1;
    attron(A_REVERSE|A_BOLD);
    mvprintw(j,(cols-30)/2, "GAME OVER");
    attroff(A_REVERSE|A_BOLD);

    j=3;
    mvprintw(j,(cols-30)/2, "SCOR OBTINUT : %d",scor);

    refresh();

    getch();
}

void afisare() {
    //afiseaza layout-ul jocului pe ecran

    int i,j;
    time_t rawtime;
    struct tm * timeinfo;

    clear();

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    j=1;
    attron(A_REVERSE|A_BOLD);
    mvprintw(j,(cols-30)/2, "2048");
    attroff(A_REVERSE|A_BOLD);

    j=3;
    mvprintw(j,(cols-30)/2, "[ %d.%d.%d %02d:%02d ]",timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min);
    j++;
    mvprintw(j,(cols-30)/2, "[ SCOR : %d ]",scor);

    j=6;
    for (i=0;i<4;i++) {
        mvprintw(j,(cols-30)/2,"+------+------+------+------+");
        j++;
        mvprintw(j,(cols-30)/2,"| %-4d | %-4d | %-4d | %-4d |",board[i][0],board[i][1],board[i][2],board[i][3]);
        j++;
    }
    mvprintw(j,(cols-30)/2,"+------+------+------+------+");

    j++;
    j++;

    /*
    mvprintw(j,(cols-30)/2,"INCERCATI SA ATINGETI CEL MAI");
    j++;
    mvprintw(j,(cols-30)/2,"MARE SCOR PRIN ALATURAREA DE");
    j++;
    mvprintw(j,(cols-30)/2,"VALORI IDENTICE");
    j++;
    j++;
    */

    mvprintw(j,(cols-30)/2,"COMENZI VALIDE");
    j++;
    j++;

    mvprintw(j,(cols-30)/2,"SAGEATA SUS - alaturare valori in sus");
    j++;
    mvprintw(j,(cols-30)/2,"SAGEATA JOS - alaturare valori in jos");
    j++;
    mvprintw(j,(cols-30)/2,"SAGEATA STANGA - alaturare valori in stanga");
    j++;
    mvprintw(j,(cols-30)/2,"SAGEATA DREAPTA - alaturare valori in dreapta");
    j++;
    mvprintw(j,(cols-30)/2,"Q - meniu principal");

    refresh();
}

int nr_aleator() {
    //genereaza un numar aleator: 2 sau 4

    int rezultat;

    srand((unsigned) time(NULL)); //genereaza un numar petru rand()
    int p = (rand() % 2) + 1; // va intoarce 1 sau 2

    if (p==1) {
        rezultat = 2;
    }

    if (p==2) {
        rezultat = 4;
    }

    return rezultat;
}

void adaugare_valoare() {
    //adaugare valoare aleatoare dupa efectuarea unei miscari valide

    int arr_poz[20];
    int i,j,n;
    int poz;
    int index;

    //identificare pozitii cu valoarea 0
    n = -1;
    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            if (board[i][j] == 0) {
                n++;
                arr_poz[n] = (i*4) + j;
            }
        }
    }

    srand((unsigned) time(NULL));
    index = rand() % (n+1);

    poz = arr_poz[index]; //va intoarce pozitia corespunzatoare

    while (board[(int) (poz/4)][(int) (poz%4)] != 0) {
        index = rand() % (n+1);
        poz = arr_poz[index]; //va intoarce pozitia corespunzatoare
    }

    board[(int) (poz/4)][(int) (poz%4)] = nr_aleator();
}

void initializare_tabla_joc() {
    //initializarea tablei de joc

    int i,j;
    int poz1, poz2;

    for (i=0;i<4;i++) {
        for (j=0;j<4;j++) {
            board[i][j] = 0;
        }
    }

    srand((unsigned) time(NULL));

    poz1 = rand() % 16; //va intoarce un intreg intre 0 si 15
    poz2 = rand() % 16; //va intoarce un intreg intre 0 si 15

    while (poz1 == poz2) {
        //ne asiguram ca avem pozitii diferite
        poz2 = rand() % 16; //va intoarce un intreg intre 0 si 15
    }

    board[(int) (poz1/4)][(int) (poz1%4)] = nr_aleator();
    board[(int) (poz2/4)][(int) (poz2%4)] = nr_aleator();

}

void joc(int continuare); //header-ul functiei joc

void meniu() {
    //afisare meniu principal

    int ch;
    char *optiuni_meniu[] = {"NEW GAME","RESUME","QUIT"}; //optiuni pentru meniul principal
    int n_optiuni = sizeof(optiuni_meniu) / sizeof(char *); //numarul de optiuni meniu principal
    int selectie = 1; //selectia initiala meniu principal
    int alegere = 0;
    int i;
    int vpoz;

    getmaxyx(stdscr, rows, cols); //citeste numarul maxim de randuri si coloane disponibile in fereastra

    clear();

    //afisare nume aplicatie
    init_pair(1, COLOR_RED, COLOR_YELLOW);
    attron(COLOR_PAIR(1)|A_BOLD);
    mvprintw(2, (cols/2)-4, "********");
    mvprintw(3, (cols/2)-4, "* 2048 *");
    mvprintw(4, (cols/2)-4, "********");
    attroff(COLOR_PAIR(1)|A_BOLD);

    //navigatie prin optiunile de meniu
    while (1) {

         //afisare optiuni meniu
         for (i=0;i<n_optiuni;i++) {
            if (selectie == i+1) { //selectia curenta
                attron(A_STANDOUT);
            }

            vpoz = 6 + (i+1)*2;

            if ((i==1) && (joc_inceput == 1)) { //pentru optiunea RESUME
                mvprintw(vpoz, (cols/2)-4, "%s", optiuni_meniu[i]);
            }

            if ((i>1) && (joc_inceput == 0)) { //pentru optiunile dupa RESUME
                vpoz = vpoz - 2;
            }

            if (i!=1) { //pentru alte optiuni in afara de RESUME
                mvprintw(vpoz, (cols/2)-4, "%s", optiuni_meniu[i]);
            }

            if (selectie == i+1) { //selectia curenta
                attroff(A_STANDOUT);
            }
        }

        refresh();

        ch = getch();

        switch (ch) {
            case KEY_DOWN:
                selectie++;
                if ((selectie == 2) && (joc_inceput == 0)) {
                    //se sare peste selectia RESUME
                    selectie++;
                }
                if (selectie>n_optiuni) {
                    selectie = 1;
                }
                break;
            case KEY_UP:
                selectie--;
                if ((selectie == 2) && (joc_inceput == 0)) {
                    //se sare peste selectia RESUME
                    selectie--;
                }
                if (selectie<1) {
                    selectie = n_optiuni;
                }
                break;
            case 10:
                alegere = selectie;
        }

        if (alegere != 0) { //a fost facuta o alegere
            break;
        }
    }

    switch (alegere) {
        case 1:
            //incepere joc nou
            joc(0);
            break;
        case 2:
            //intoarcere la joc
            joc(1);
            break;
        case 3:
            //iesire din joc
            break;
    }

    refresh();

}

void joc(int continuare) {
    int forma_initiala;
    int ch; //tasta citita
    int alegere = 0;
    int i;

    clear();

    if (continuare==0) {
        //reinitializare joc
        initializare_tabla_joc();
        joc_inceput = 1;
        scor = 0;
    }

    halfdelay(50); //timeout 5 seconde pentru citirea unei comenzi de la tastatura

    while (1) {

        //afisare tabla de joc
        afisare();

        refresh();

        ch = getch(); //citire tasta

        if (ch == -1) { //nu s-a citit nimic de la tastatura in timpul alocat - se ia o decizie automata
            switch (directie_optima()) {
                //stanga
                case 1:
                    ch = KEY_LEFT;
                    break;
                //sus
                case 2:
                    ch = KEY_UP;
                    break;
                //dreapta
                case 3:
                    ch = KEY_RIGHT;
                    break;
                //jos
                case 4:
                    ch = KEY_DOWN;
                    break;
                case 101:
                    ch = 'Q';
                    alegere = 1; //iesire din joc - nu exista misari valide
                    break;
            }
        }

        forma_initiala = -1;

        switch (ch) {
            case KEY_LEFT:
                //nu este necesara rotirea
                if (check_directie(1) != 101) { //verificare directie valida
                    forma_initiala = 0;
                }
                break;
            case KEY_RIGHT:
                if (check_directie(3) != 101) { //verificare directie valida
                    rotire(); rotire();
                    forma_initiala = 2;
                }
                break;
            case KEY_UP:
                if (check_directie(2) != 101) { //verificare directie valida
                    rotire();
                    forma_initiala = 1;
                }
                break;
            case KEY_DOWN:
                if (check_directie(4) != 101) { //verificare directie valida
                    rotire(); rotire(); rotire();
                    forma_initiala = 3;
                }
                break;
            case 'q':
            case 'Q':
                alegere = 1;
                break;
        }

        if (forma_initiala>-1) {
            for (i=0;i<4;i++) {
                miscare(i);
            }

            switch (forma_initiala) {
                case 1:
                    rotire(); rotire(); rotire();
                    break;
                case 2:
                    rotire(); rotire();
                    break;
                case 3:
                    rotire();
                    break;
            }

            adaugare_valoare();
        }

        if (check_win() == 1) { //s-a atins valoarea 2048 - ati castigat
            break;
        }

        if (directie_optima() == 101) { //nu mai exista optiuni valide
            break;
        }

        if (alegere != 0) {
            break;
        }
    }

    cbreak(); //iesire din modul halfdelay()

    if ((check_win() == 1) && (joc_inceput==1)) { //GAME WON
        //afisare mesaj
        afisare_game_win();
        //reinitializare joc
        joc_inceput = 0;
        scor = 0;
    }

    if ((directie_optima() == 101) && (joc_inceput==1)) { //GAME OVER
        //afisare mesaj
        afisare_game_over();
        //reinitializare joc
        joc_inceput = 0;
        scor = 0;
    }

    meniu();

}

int main() {
    initscr();			        //incepe afisarea in mod curses
    start_color();              //permite utilizarea culorilor
    raw();                      //dezactiveaza buffer-ul pentru caracterele citite
    keypad(stdscr, TRUE);       //permite citirea caracterelor de la tastatura direct in cadrul programului
    noecho();                   //dezactiveaza afisarea caracterelor citite de la tastatura

    meniu();

    clear();

	endwin();			        //dezactiveaza afisarea in mod curses

    return 0;
}
