#include "my_player.h"
#include "iostream"
#include <windows.h>

const int MAX_SEQUENCE_LENGTH = 5;
int capacity = 10;
Mark m;

struct Sequence {
    char marks[MAX_SEQUENCE_LENGTH + 1]; /* последовательность */
    char without_spaces[MAX_SEQUENCE_LENGTH + 1]; /* последовательность без пробелов*/
    Point start; /* начальная координата последовательности*/
    Point end; /* конечная координата последовательности*/
    Point left; /* первая клетка не входящая в последовательность слева*/
    Point right; /* первая клетка не входящая в последовательность справа*/
    Point gap; /* разрыв внутри последовательности*/
    char left_edge; /* значения поля слева от последовательности*/
    char right_edge; /* значения поля справа от последовательности*/
};

int value;
Sequence* sequences = nullptr;
int seqCount = 0;

int x = 0;
int o = 0;

const char* order[] = {
    "X XXX",
    "XX XX",
    "XXX X",
    "XXXX",
    "O OOO",
    "OO OO",
    "OOO O",
    "OOOO",
    "O OO",
    "OO O",
    "XX X",
    "X XX",
    "OOO",
    "XXX",
    "O O",
    "X X",
    "OO",
    "XX",
};

const char* order_0[] = {
    "O OOO",
    "OO OO",
    "OOO O",
    "OOOO",
    "X XXX",
    "XX XX",
    "XXX X",
    "XXXX",
    "XX X",
    "X XX",
    "OO O",
    "O OO",
    "XXX",
    "OOO",
    "X X",
    "O O",
    "XX",
    "OO",
};

int getOrderIndex(const char* str, const char* c[]) {
    for (int i = 0; i < 18; i++) {
        if (strcmp(str, c[i]) == 0) {
            return i;
        }
    }

    return 18; 
}

int compareSequences(const Sequence& seqA, const Sequence& seqB, const char* c[]) {
    int indexA = getOrderIndex(seqA.marks, c);
    int indexB = getOrderIndex(seqB.marks, c);
    return indexB - indexA;
}

void bubbleSortSequences(Sequence* sequences, int size, Mark& m) {
    if (m == Mark::Zero) {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (compareSequences(sequences[j], sequences[j + 1], order_0) < 0) {
                    Sequence temp = sequences[j];
                    sequences[j] = sequences[j + 1];
                    sequences[j + 1] = temp;
                }
            }
        }
    }
    else {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (compareSequences(sequences[j], sequences[j + 1], order) < 0) {
                    Sequence temp = sequences[j];
                    sequences[j] = sequences[j + 1];
                    sequences[j + 1] = temp;
                }
            }
        }
    }
}

int getAllSequences(const GameView& game) {
    int sequenceCount = 0;

    Boundary set = game.get_settings().field_size;

    struct {
        int dx;
        int dy;
    } dirs[] = { {1, 0}, {0, 1}, {1, 1}, {-1, 1} };

    for (int i = set.min.x; i <= set.max.x; i++) {
        for (int j = set.min.y; j <= set.max.y; j++) {
            Point p(i, j);
            Mark m = game.get_state().field->get_value(p);

            if (m == Mark::None) continue;

            for (int d_ix = 0; d_ix < 4; ++d_ix) {
                int dx = dirs[d_ix].dx, dy = dirs[d_ix].dy;
                int step;
                Point start = p, end = p;
                
                Sequence seq;
                seq.left_edge = ' ';
                seq.right_edge = ' ';

                int marksIndex = 0;
                int letterIndex = 0;
                bool spaceUsed = false;
                int sequenceLength = 0;
                int maxSequenceLength = 0;

                Sequence tempSeq;

                for (step = 0; step < game.get_settings().win_length; ++step) {
                    Point p2 = Point{ p.x + step * dx, p.y + step * dy };
                    if (!set.is_within(p2)) break;
                    Mark m2 = game.get_state().field->get_value(p2);

                    if (m2 == Mark::None && !spaceUsed && sequenceLength > 0) {
                        tempSeq.marks[marksIndex++] = ' ';
                        spaceUsed = true;
                        tempSeq.gap = p2;
                        continue;
                    }
                    else if (m2 != m && m2 != Mark::None) {
                        break;
                    }
                    else if (m2 == Mark::None && spaceUsed) {
                        break;
                    }

                    tempSeq.marks[marksIndex++] = (m2 == Mark::Cross) ? 'X' : 'O';
                    int count = 0;

                    for (int b = 0; b < marksIndex; b++) {
                        if (tempSeq.marks[b] != ' ') {
                            tempSeq.without_spaces[count] = tempSeq.marks[b];
                            count++;
                        }
                    }

                    tempSeq.without_spaces[count + 1] = '\0';

                    end = p2;
                    sequenceLength++;
                }

                if (sequenceLength > maxSequenceLength) {
                    maxSequenceLength = sequenceLength;
                    if (tempSeq.marks[marksIndex - 1] == ' ') tempSeq.marks[marksIndex - 1] = '\0';
                    else tempSeq.marks[marksIndex] = '\0';
                    tempSeq.start = start;
                    tempSeq.end = end;

                    Point left_edge_point = Point{ start.x - dx, start.y - dy };

                    if (set.is_within(left_edge_point)) {
                        Mark left_edge_mark = game.get_state().field->get_value(left_edge_point);
                        tempSeq.left = left_edge_point;

                        if (left_edge_mark == Mark::Cross) {
                            tempSeq.left_edge = 'X';
                        }
                        else if (left_edge_mark == Mark::Zero) {
                            tempSeq.left_edge = 'O';
                        }
                        else {
                            tempSeq.left_edge = ' ';
                        }
                    }

                    Point right_edge_point = Point{ end.x + dx, end.y + dy };

                    if (set.is_within(right_edge_point)) {
                        Mark right_edge_mark = game.get_state().field->get_value(right_edge_point);
                        tempSeq.right = right_edge_point;
                        if (right_edge_mark == Mark::Cross) {
                            tempSeq.right_edge = 'X';
                        }
                        else if (right_edge_mark == Mark::Zero) {
                            tempSeq.right_edge = 'O';
                        }
                        else {
                            tempSeq.right_edge = ' ';
                        }
                    }

                    if (maxSequenceLength > 1) {
                        if (tempSeq.left_edge == tempSeq.marks[0]) continue;

                        if (sequenceCount >= value) {
                            int v = value * 2;
                            Sequence* s = new Sequence[v];

                            for (int i = 0; i < value; i++) {
                                s[i] = sequences[i];
                            }

                            s[value] = tempSeq;

                            delete[] sequences;

                            sequences = s;
                            value = v;
                        }

                        sequences[sequenceCount++] = tempSeq;
                    }
                }
            }
        }
    }

    seqCount = sequenceCount;
    return sequenceCount;
}

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void BasicObserver::notify(const GameView& game, const Event& event) {
   if (event.get_type() == MoveEvent::TYPE) {
       if (game.get_state().number_of_moves < 2 && sequences == nullptr) {
           value = 10;
           sequences = new Sequence[value];
       }
        return;
    }
   /* if (event.get_type() == PlayerJoinedEvent::TYPE) {
        auto& data = get_data<PlayerJoinedEvent>(event);
        _out << "Player '" << data.name << "' joined as ";
        _print_mark(data.mark) << '\n';
        return;
    }*/
    if (event.get_type() == GameStartedEvent::TYPE) {
        _out << "Game started\n";
        return;
    }
    if (event.get_type() == WinEvent::TYPE) {
        auto& data = get_data<WinEvent>(event);
        _out << "Player playing ";
        _print_mark(data.winner_mark) << " has won\n";

        if (data.winner_mark == Mark::Cross) {
            x++;
        }
        else {
            o++;
        }

        return;
    }
    if (event.get_type() == DrawEvent::TYPE) {
        auto& data = get_data<DrawEvent>(event);
        _out << "Draw happened, reason: '" << data.reason << "'\n";
        return;
    }
    if (event.get_type() == DisqualificationEvent::TYPE) {
        auto& data = get_data<DisqualificationEvent>(event);
        _out << "Player playing ";
        _print_mark(data.player_mark) << " was disqualified, reason: '"
            << data.reason << "'\n";

        if (data.player_mark == Mark::Cross) {
            o++;
        }
        else {
            x++;
        }

        return;
    }
}

void DebugObserver::notify(const GameView& game, const Event& event) {
    if (game.get_state().number_of_moves < 2 && sequences == nullptr) {
        value = 10;
        sequences = new Sequence[value];
    }

    if (event.get_type() == MoveEvent::TYPE) {
        auto& data = get_data<MoveEvent>(event);
        Boundary set = game.get_settings().field_size;

       /*for (int i = set.min.x; i <= set.max.x; i++) {
            for (int j = set.min.y; j <= set.max.y; j++) {
                Point p(i, j);
                Mark m = game.get_state().field->get_value(p);

                if (m == Mark::None) {
                    _out << "| ";
                }
                else if (m == Mark::Cross) {
                    if (data.point.x == i && data.point.y == j) {
                        setColor(2);
                        _out << "|X";
                        setColor(7);
                    }
                    else {
                        _out << "|X";
                    }
                }
                else {
                    if (data.point.x == i && data.point.y == j) {
                        setColor(2);
                        _out << "|O";
                        setColor(7);
                    }
                    else {
                        _out << "|O";
                    }
                }
            }

            _out << "\n";
        }*/

        /*for (int i = 0; i < seqCount; i++) {
            if (strlen(sequences[i].marks) >= 3) {
                std::cout << sequences[i].marks << std::endl;
            }
        } 
        
        std::cin.get();
*/
        return;
    }
}

std::ostream& BasicObserver::_print_mark(Mark m) {
    if (m == Mark::Cross) return _out << "X";
    if (m == Mark::Zero) return _out << "O";
    return _out << "?";
}

static field_index_t rand_int(field_index_t min, field_index_t max) {
    return min + rand() % (max - min + 1);
}

int mov = 0;

Point GoodPlayer::play(const GameView& game) {
    m = _my_mark;

    const bool is_first_move = game.get_state().number_of_moves == 0;
    /* sleep(1); */

    if (is_first_move) {
        const Boundary& b = game.get_settings().field_size;
        return Point {
            .x = rand_int(b.min.x, b.max.x),
            .y = rand_int(b.min.y, b.max.y),
        };
    }

    getAllSequences(game);

    Point result;

    if (use_adv(game, result)) {
        mov++;
        return result;
    }
    else {
        block_last_move(game, result);
        mov++;
        return result;
    }
}

void GoodPlayer::choose_side(Point& left, Point& right, Point& result, const Boundary& b, const GameView& game) {
    int count_left_e = 0;
    int count_left_f = 0;

    int count_right_e = 0;
    int count_right_f = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;

            if (b.is_within(Point(left.x + i, left.y + j))) {
                Mark val = game.get_state().field->get_value(Point(left.x + i, left.y + j));

                if (val == _my_mark) count_left_f += 1;
                else if (val == Mark::Zero) count_left_e += 1;
            }

            if (b.is_within(Point(right.x + i, right.y + j))) {
                Mark val = game.get_state().field->get_value(Point(right.x + i, right.y + j));

                if (val == _my_mark) count_right_f += 1;
                else if (val == Mark::Zero) count_right_e += 1;
            }
        }
    }

    if (count_left_f >= count_right_f && count_left_e <= count_right_e && b.is_within(left)) result = left;
    else if (b.is_within(right)) result = right;
}

void GoodPlayer::validate_move(int i, Point& result, const Boundary& b, const GameView& game) {
    Sequence s = sequences[i];

    if (!b.is_within(s.left)) {
        result = s.right;
        return;
    }
    else if (!b.is_within(s.right)) {
        result = s.left;
        return;
    };

    if (s.left_edge == s.right_edge) {
        choose_side(s.left, s.right, result, b, game);
    }
    else if (s.left_edge == ' ') {
        result = s.left;
    }
    else if (s.right_edge == ' ') {
        result = s.right;
    }
}

bool GoodPlayer::use_adv(const GameView& game, Point& result) {
    const Boundary& b = game.get_settings().field_size;

    if (_my_mark == Mark::Zero) {
        Mark m = Mark::Zero;
        bubbleSortSequences(sequences, seqCount, m);
    }
    else {
        Mark m = Mark::Cross;
        bubbleSortSequences(sequences, seqCount, m);
    }

    for (int i = 0; i < seqCount; i++) {
        char* m = sequences[i].marks;
        if ((!b.is_within(sequences[i].left) && !b.is_within(sequences[i].right))) continue;
        
        if (_my_mark == Mark::Cross) {
                if (strcmp(sequences[i].marks, "XXXX") == 0 && (sequences[i].left_edge == ' ' || sequences[i].right_edge == ' ')) {
                    if (sequences[i].left_edge == ' ') {
                        result = sequences[i].left;
                        return true;
                    }
                    else {
                        result = sequences[i].right;
                        return true;
                    }
                }
                else if (strcmp(m, "XXX X") == 0 || strcmp(m, "X XXX") == 0 || strcmp(m, "XX XX") == 0) {
                    result = sequences[i].gap;
                    return true;
                }
                else if ((strcmp(m, "OOOO") == 0)
                    && (sequences[i].left_edge == ' ' || sequences[i].right_edge == ' ')) {

                    if (sequences[i].left_edge == ' ') result = sequences[i].left;
                    else result = sequences[i].right;

                    return true;
                }
                else if (strcmp(m, "OOO O") == 0 || strcmp(m, "O OOO") == 0 || strcmp(m, "OO OO") == 0) {
                    result = sequences[i].gap;
                    return true;
                }
                else if (strcmp(m, "OO O") == 0 || strcmp(m, "O OO") == 0) {
                    if (sequences[i].left_edge == ' ' && sequences[i].right_edge == ' ') {
                        result = sequences[i].gap;
                        return true;
                    }
                }
                else if ((strcmp(m, "XX X") == 0 || strcmp(m, "X XX") == 0) &&
                    (sequences[i].left_edge == ' ' && sequences[i].right_edge == ' ')) {
                    result = sequences[i].gap;
                    return true;
                }
                else if (strcmp(m, "OOO") == 0 && (sequences[i].left_edge == ' '
                    && sequences[i].right_edge == ' ')) {

                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "O O") == 0 && (sequences[i].left_edge == ' '
                    && sequences[i].right_edge == ' ')) {

                    result = sequences[i].gap;
                    return true;
                }
                else if (strcmp(m, "XXX") == 0 && (sequences[i].left_edge == ' '
                    && sequences[i].right_edge == ' ')) {

                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "X X") == 0 && (sequences[i].left_edge == ' '
                    && sequences[i].right_edge == ' ') &&
                    (b.is_within(sequences[i].left) || b.is_within(sequences[i].right))) {

                    if (b.is_within(sequences[i].left) && b.is_within(sequences[i].right)) {
                        choose_side(sequences[i].left, sequences[i].right, result, b, game);
                    }
                    else if (b.is_within(sequences[i].left)) {
                        result = sequences[i].left;
                    }
                    else result = sequences[i].right;

                    return true;
                }
                else if (strcmp(m, "OOO") == 0 && (sequences[i].left_edge == ' '
                    || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "XXX") == 0 && (sequences[i].left_edge == ' '
                    || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "O O") == 0 && (sequences[i].left_edge == ' '
                    || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "XX") == 0 && (sequences[i].left_edge == ' ' && sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "OO") == 0 && (sequences[i].left_edge == ' ' && sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "XX") == 0 && (sequences[i].left_edge == ' ' || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "XX") == 0 && (sequences[i].left_edge == ' ' || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
        }
        else {
                if (strcmp(sequences[i].marks, "OOOO") == 0 && (sequences[i].left_edge == ' ' || sequences[i].right_edge == ' ')) {
                    if (sequences[i].left_edge == ' ') {
                        result = sequences[i].left;
                        return true;
                    }
                    else {
                        result = sequences[i].right;
                        return true;
                    }
                }
                else if (strcmp(m, "OOO O") == 0 || strcmp(m, "O OOO") == 0 || strcmp(m, "OO OO") == 0) {
                    result = sequences[i].gap;
                    return true;
                } else if ((strcmp(m, "XXXX") == 0)
                    && (sequences[i].left_edge == ' ' || sequences[i].right_edge == ' ')) {

                    if (sequences[i].left_edge == ' ') result = sequences[i].left;
                    else result = sequences[i].right;

                    return true;
                } else if (strcmp(m, "XXX X") == 0 || strcmp(m, "X XXX") == 0 || strcmp(m, "XX XX") == 0) {
                    result = sequences[i].gap;
                    return true;
                } else if (strcmp(m, "XX X") == 0 || strcmp(m, "X XX") == 0) {
                    if (sequences[i].left_edge == ' ' && sequences[i].right_edge == ' ') {
                        result = sequences[i].gap;
                        return true;
                    }
                } else if ((strcmp(m, "OO O") == 0 || strcmp(m, "O OO") == 0) &&
                    (sequences[i].left_edge == ' ' && sequences[i].right_edge == ' ')) {
                    result = sequences[i].gap;
                    return true;
                } else if (strcmp(m, "XXX") == 0 && (sequences[i].left_edge == ' '
                    && sequences[i].right_edge == ' ')) {

                    validate_move(i, result, b, game);
                    return true;
                } else if (strcmp(m, "X X") == 0 && (sequences[i].left_edge == ' '
                    && sequences[i].right_edge == ' ')) {

                    result = sequences[i].gap;
                    return true;
                } else if (strcmp(m, "OOO") == 0 && (sequences[i].left_edge == ' '
                    && sequences[i].right_edge == ' ')) {

                    validate_move(i, result, b, game);
                    return true;
                } else if (strcmp(m, "O O") == 0 && (sequences[i].left_edge == ' '
                    && sequences[i].right_edge == ' ') &&
                    (b.is_within(sequences[i].left) || b.is_within(sequences[i].right))) {

                    if (b.is_within(sequences[i].left) && b.is_within(sequences[i].right)) {
                        choose_side(sequences[i].left, sequences[i].right, result, b, game);
                    }
                    else if (b.is_within(sequences[i].left)) {
                        result = sequences[i].left;
                    }
                    else result = sequences[i].right;

                    return true;
                } else if (strcmp(m, "XXX") == 0 && (sequences[i].left_edge == ' '
                    || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                } else if (strcmp(m, "OOO") == 0 && (sequences[i].left_edge == ' '
                    || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "X X") == 0 && (sequences[i].left_edge == ' '
                    || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "O O") == 0 && (sequences[i].left_edge == ' '
                    || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "OO") == 0 && (sequences[i].left_edge == ' ' && sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
                else if (strcmp(m, "XX") == 0 && (sequences[i].left_edge == ' ' && sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }

                else if (strcmp(m, "OO") == 0 && (sequences[i].left_edge == ' ' || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }

                else if (strcmp(m, "OO") == 0 && (sequences[i].left_edge == ' ' || sequences[i].right_edge == ' ')) {
                    validate_move(i, result, b, game);
                    return true;
                }
        }
    }
    return false;
}

bool GoodPlayer::block_last_move(const GameView& game, Point& result) {
    const Boundary& b = game.get_settings().field_size;

    struct {
        int dx;
        int dy;
    } dirs[] = { {1, 0}, {0, 1}, {1, 1}, {-1, 1} };

    bool found_best_point = false;
    int max_len = 0;
    Point best_point;
    Mark other_mark = game.get_state().field->get_value(_last_move);

    for (int d_ix = 0; d_ix < 4; ++d_ix) {
        int dx = dirs[d_ix].dx, dy = dirs[d_ix].dy;
        int len_f = 0, len_b = 0;
        bool can_close = false;
        Point point_to_close;

        for (Point p = _last_move; b.is_within(p) && len_f <= 5;
            p.x += dx, p.y += dy) {
            Mark val = game.get_state().field->get_value(p);

            if (val == other_mark) {
                len_f++;
            }
            else if (val == Mark::None) {
                can_close = true;
                point_to_close = p;
                break;
            }
            else {
                break;
            }
        }

        for (Point p = _last_move; b.is_within(p) && len_b <= 5;
            p.x -= dx, p.y -= dy) {
            Mark val = game.get_state().field->get_value(p);
            if (val == other_mark) {
                len_b++;
            }
            else if (val == Mark::None) {
                can_close = true;
                point_to_close = p;
                break;
            }
            else {
                break;
            }
        }

        if (!can_close)
            continue;
        int len = len_f + len_b - 1;

        if (len > max_len) {
            max_len = len;
            best_point = point_to_close;
            found_best_point = true;
        }
    }

    if (found_best_point) {
        result = best_point;
        return true;
    }

    return false;
}

void GoodPlayer::assign_mark(Mark player_mark) { _my_mark = player_mark; }

void GoodPlayer::notify(const GameView& view, const Event& event) {
    if (event.get_type() == MoveEvent::TYPE) {
        auto& data = get_data<MoveEvent>(event);
        if (data.mark != _my_mark) {
            _last_move = data.point;
        }
        return;
    }
}
