#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

struct Player {
  std::string name;
  int score = 0;
  bool is_turn = false;
  bool update_from_previous_turn = false;
};


struct Point {
  int y,x;
};


class Dice {
private:
  int _value;

public:
  Dice() {}
  void GetFigure(int number) {
    _value = number;
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

  int GetValue() {return _value;}

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

void draw_player_score(const Player &player, const int ypos, const int xpos) {
  std::string color_modifier = player.is_turn ? "\x1b[48;5;172m" : "";
  std::string reset_color = player.is_turn ? "\x1b[0m": "";

  const int textwidth = player.name.size() + 4;
  const int scorepos = textwidth / 2;

  set_curs_position({ypos+2, xpos + scorepos});
  std::cout << "         "<< reset_color;
  /// y,x are the coordinates of the upper right corner of the text box
  set_curs_position({ypos, xpos});
  std::cout << color_modifier << "  " << player.name << "  ";
  set_curs_position({ypos+2, xpos + scorepos});
  std::cout << player.score << reset_color;
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

void read_score(Player &player_c, Player &player_o,
                const int value, const int ypos,
                const int xpos) {
  set_curs_position({ypos, xpos});
  std::cout << "\x1b[48;5;105m" << "Colpito (c), mancato (x)" << "\x1b[0m";
  while (true) {
    const char c = _getch();
    if (c == 'c') {
      player_c.score = player_c.score + 20;
      if (value == 6) {
        player_c.score = player_c.score + 30;
      }
      break;
    } else if (c == 'x') {
      player_c.score = player_c.score - 5;
      if (player_o.update_from_previous_turn) {
        player_o.score = player_o.score + 10;
        player_o.update_from_previous_turn = false;
      }
      break;
    }
  }
  set_curs_position({ypos, xpos});
  std::cout << "                        ";
  if (value == 1) {
    /// check the next turn if the other player scores or misses
    player_c.update_from_previous_turn = true;
  } else if (value == 2) {
     set_curs_position({ypos, xpos-4});
     std::cout << "\x1b[48;5;105m" << "Prima partita? (y) (n)" << "\x1b[0m";
     while (true) {
       const char c = _getch();
       if (c == 'y') {
         player_o.score = player_o.score - 20;
         break;
       } else if (c == 'n') {
         break;
       }
     }
     set_curs_position({ypos, xpos-4});
     std::cout << "                                ";
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

  draw_init_screen(ScreenCentre.y, ScreenCentre.x - 13);
  Player p1{.name = (argc > 2) ? argv[2] : "player1", .score = 0};
  Player p2{.name = (argc > 3) ? argv[3] : "player2", .score = 0};

  const size_t ndice = (argc > 1) ? std::atoi(argv[1]) : 2;

  int turn_count = 0;
  while (true) {
    p1.is_turn = p2.is_turn = false;
    char c = _getch();
    if (c == 'r') {
      if(turn_count == 0)
        clear_init_screen(ScreenCentre.y, ScreenCentre.x - 13);
      else
        clear_init_screen(ScreenCentre.y + 25, ScreenCentre.x - 13);

      Player &current_player = (turn_count++ % 2 == 0) ? p1 : p2;
      Player &other_player   = (&current_player == &p1) ? p2 : p1;
      current_player.is_turn = true;
      other_player.is_turn = false;

      draw_player_score(p1, 10, 10);
      draw_player_score(p2, 10,cols - p2.name.size() - 2 - 10);

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

      read_score(current_player, other_player, d.GetValue(),
                 ScreenCentre.y+25, ScreenCentre.x - 13);
      draw_player_score(p1, 10, 10);
      draw_player_score(p2, 10, cols - p2.name.size() - 2 - 10);
    } else if (c == 'q') {
      break;
    }
    draw_init_screen(ScreenCentre.y+25, ScreenCentre.x - 13);
  }
  std::system("reset");
  return 0;
}
