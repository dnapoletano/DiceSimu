#include <iostream>
#include <random>
#include <sstream>

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

struct Point {
  int y,x;
};


class Dice {
private:
  int _value;

public:
  Dice() {}
  void GetFigure(int number) {
    switch (number) {
    case 1:
      one();
      break;
    case 2:
      two();
      break;
    case 3:
      three();
      break;
    case 4:
      four();
      break;
    case 5:
      five();
      break;
    case 6:
      six();
      break;
    default:
      break;
    }


  }
  void RollDice(std::uniform_int_distribution<> &_distr,
                std::mt19937 &_eng) {
    for (size_t i{0}; i < 100; ++i) {


      const int number = _distr(_eng);
    }
  }

  void one()
  {
    std::cout << " -----"  << "\x1b[B" << "\x1b[6D";
    std::cout << "|     |" << "\x1b[B" << "\x1b[7D";
    std::cout << "|  O  |" << "\x1b[B" << "\x1b[7D";
    std::cout << "|     |" << "\x1b[B" << "\x1b[7D";
    std::cout << " -----"  << "\n";
  }
  void two()
  {
    std::cout << " -----"  << "\x1b[B" << "\x1b[6D";
    std::cout << "|    O|" << "\x1b[B" << "\x1b[7D";
    std::cout << "|     |" << "\x1b[B" << "\x1b[7D";
    std::cout << "|O    |" << "\x1b[B" << "\x1b[7D";
    std::cout << " -----" << "\n";
  }
  void three()
  {
    std::cout << " -----"  << "\x1b[B" << "\x1b[6D";
    std::cout << "|    O|" << "\x1b[B" << "\x1b[7D";
    std::cout << "|  O  |" << "\x1b[B" << "\x1b[7D";
    std::cout << "|O    |" << "\x1b[B" << "\x1b[7D";
    std::cout << " -----" << "\n";
  }
  void four()
  {
    std::cout << " -----"  << "\x1b[B" << "\x1b[6D";
    std::cout << "|O   O|" << "\x1b[B" << "\x1b[7D";
    std::cout << "|     |" << "\x1b[B" << "\x1b[7D";
    std::cout << "|O   O|" << "\x1b[B" << "\x1b[7D";
    std::cout << " -----" << "\n";
  }
  void five()
  {
    std::cout << " -----"  << "\x1b[B" << "\x1b[6D";
    std::cout << "|O   O|" << "\x1b[B" << "\x1b[7D";
    std::cout << "|  O  |" << "\x1b[B" << "\x1b[7D";
    std::cout << "|O   O|" << "\x1b[B" << "\x1b[7D";
    std::cout << " -----" << "\n";
  }
  void six()
  {
    std::cout << " -----"  << "\x1b[B" << "\x1b[6D";
    std::cout << "|O   O|" << "\x1b[B" << "\x1b[7D";
    std::cout << "|O   O|" << "\x1b[B" << "\x1b[7D";
    std::cout << "|O   O|" << "\x1b[B" << "\x1b[7D";
    std::cout << " -----" << "\n";
  }
};

void _enable_raw_mode() {
  termios term;
  tcgetattr(0, &term);
  term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(0, TCSANOW, &term);
}

void _disable_raw_mode() {
  termios term;
  tcgetattr(0, &term);
  term.c_lflag |= ICANON | ECHO;
  tcsetattr(0, TCSANOW, &term);
}

char _getch() {
  struct termios old_attr, new_attr;
  tcgetattr(STDIN_FILENO, &old_attr);

  new_attr = old_attr;
  new_attr.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
  char c = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
  return c;
}

void clear_screen() {
  std::cout << "\x1b[?25l";
  std::cout << "\x1b[2J";
}

Point get_curs_position() {
  std::cout << "\x1b[6n";
  char c = _getch();
  c = _getch(); /// consume special characters
  c = _getch();

  Point curs_position = {.y=0, .x=0};
  std::string pos;
  while (c != ';') {
    c = _getch();
    if (c >= '0' && c <= '9')
      pos.push_back(c);
  }
  curs_position.y = std::stoi(pos);
  pos = "";
  while (c != 'R') {
    c = _getch();
    if (c >= '0' && c <= '9')
      pos.push_back(c);
  }
  curs_position.x = std::stoi(pos);
  return curs_position;
}

void set_curs_position(const Point p) {
  std::cout << "\x1b[" + std::to_string(p.y) + ";"+std::to_string(p.x)+"H";
}

void set_cols_rows(int &cols, int &rows) {
  struct winsize size;
  ioctl(1, TIOCGWINSZ, &size);
  cols = size.ws_col;
  rows = size.ws_row;
}

void draw_screen_divisor(const int rows, const int xpos) {
  for (int i{1}; i < rows; ++i) {
    set_curs_position({i, xpos});
    std::cout << "|";
  }
}

void draw_init_screen(const int ypos, const int xpos) {
    set_curs_position({ypos, xpos});
    std::cout << "\x1b[48;5;105m" << "Press r to play, q to quit" << "\x1b[0m";
}

void clear_init_screen(const int ypos, const int xpos) {
    set_curs_position({ypos, xpos});
    std::cout << "                          ";
}


bool _kbhit() {
  _enable_raw_mode();

  int bytes_waiting;
  ioctl(0, FIONREAD, &bytes_waiting);
  bool hit = (bytes_waiting > 0);

  _disable_raw_mode();
  tcflush(0, TCIFLUSH);
  return hit;
}

std::string get_key(const bool wait = false) {
  if(not wait && not _kbhit()) return "";
  char input = _getch();
  switch (input) {
  case 'q':
    return "q";
    break;
  default:
    return "";
  }
}


int main(int argc, char** argv) {
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<> distr(1, 6);
  Dice d{};

  int cols, rows;
  set_cols_rows(cols, rows);
  Point ScreenCentre = {static_cast<int>(rows / 2), static_cast<int>(cols / 2)};


  clear_screen();

  draw_init_screen(ScreenCentre.y, ScreenCentre.x-13);

  const size_t ndice = (argc > 1) ? std::atoi(argv[1]) : 2;

  while (true) {
    char c = _getch();
    if (c == 'r') {
      clear_init_screen(ScreenCentre.y, ScreenCentre.x - 13);
      if(ndice == 2)
        draw_screen_divisor(rows, ScreenCentre.x);
      for (int i{0}; i < 1000; ++i) {
        usleep(500);
        const int xoffset = (ndice == 2) ? 9 : 3;
        set_curs_position({ScreenCentre.y - 3, ScreenCentre.x - xoffset});
        d.GetFigure(distr(eng));
        if(ndice == 2){
          set_curs_position({ScreenCentre.y - 3, ScreenCentre.x + 3});
          d.GetFigure(distr(eng));
        }
      }
    } else if (c == 'q') {
      break;
    }
  }
  std::system("reset");
  return 0;
}
