#include<bits/stdc++.h>
using namespace std;


class Ship {
public:
    string name;
    int size;
    char symbol;
    vector<bool> hits;

    Ship(const string& n, int s, char sym) : name(n), size(s), symbol(sym), hits(s, false) {}

    bool checkHit(int x, int y) const {
        for (auto& coord : getCoordinates(x, y)) {
            if (hits[coord.second]) {
                return true;
            }
        }
        return false;
    }

    void markHit(int x, int y) {
        for (auto& coord : getCoordinates(x, y)) {
            hits[coord.second] = true;
        }
    }

    bool isSunk() const {
        return all_of(hits.begin(), hits.end(), [](bool hit) { return hit; });
    }

    vector<pair<int, int>> getCoordinates(int x, int y) const {
        vector<pair<int, int>> coordinates;
        if (size > 1) {
            for (int i = 0; i < size; ++i) {
                coordinates.emplace_back(x + i, y);
            }
        } else {
            coordinates.emplace_back(x, y);
        }
        return coordinates;
    }
};

class Player {
public:
    string name;
    vector<vector<char>> board;
    vector<vector<char>> opponentBoard;
    vector<Ship> ships;

    Player(const string& n) : name(n), board(10, vector<char>(10, ' ')), opponentBoard(10, vector<char>(10, ' ')) {}

    void placeShip(const Ship& ship, const string& coordinates, char orientation) {
        int x = coordinates[0] - 'A';
        int y = stoi(coordinates.substr(1)) - 1;

        if (orientation == 'H') {
            int endX = x + ship.size - 1;
            int endY = y;
            if (endX >= 10 || endY >= 10) {
                throw invalid_argument("Invalid placement, ship out of bounds.");
            }
            for (int i = 0; i < ship.size; ++i) {
                if (board[y][x + i] != ' ') {
                    throw invalid_argument("Invalid placement, ships overlap.");
                }
            }
            for (int i = 0; i < ship.size; ++i) {
                board[y][x + i] = ship.symbol;
            }
        } else if (orientation == 'V') {
            int endX = x;
            int endY = y + ship.size - 1;
            if (endX >= 10 || endY >= 10) {
                throw invalid_argument("Invalid placement, ship out of bounds.");
            }
            for (int i = 0; i < ship.size; ++i) {
                if (board[y + i][x] != ' ') {
                    throw invalid_argument("Invalid placement, ships overlap.");
                }
            }
            for (int i = 0; i < ship.size; ++i) {
                board[y + i][x] = ship.symbol;
            }
        } else {
            throw invalid_argument("Invalid orientation, must be 'H' or 'V'.");
        }

        ships.push_back(ship);
        updateOpponentBoard(ship, coordinates, orientation);
    }

    void updateOpponentBoard(const Ship& ship, const string& coordinates, char orientation) {
        int x = coordinates[0] - 'A';
        int y = stoi(coordinates.substr(1)) - 1;

        for (auto& coord : ship.getCoordinates(x, y)) {
            opponentBoard[coord.second][coord.first] = ' ';
        }
    }

    string makeGuess(const string& guess) {
        int x = guess[0] - 'A';
        int y = stoi(guess.substr(1)) - 1;

        if (opponentBoard[y][x] != ' ') {
            throw invalid_argument("You've already guessed this coordinate.");
        }

        for (auto& ship : ships) {
            if (ship.checkHit(x, y)) {
                ship.markHit(x, y);
                opponentBoard[y][x] = 'H';
                if (ship.isSunk()) {
                    return "Hit and sunk " + ship.name + "!";
                } else {
                    return "Hit!";
                }
            }
        }

        opponentBoard[y][x] = '*';
        return "Miss!";
    }

    bool hasLost() const {
        return all_of(ships.begin(), ships.end(), [](const Ship& ship) { return ship.isSunk(); });
    }

    void printOwnBoard() const {
        cout << "\n" << name << "'s Board:\n";
        cout << "  1 2 3 4 5 6 7 8 9 10\n";
        for (int i = 0; i < 10; ++i) {
            cout << static_cast<char>('A' + i) << ' ';
            for (int j = 0; j < 10; ++j) {
                cout << board[i][j] << ' ';
            }
            cout << '\n';
        }
        cout << '\n';
    }

    void printOpponentBoard() const {
        cout << "\n" << name << "'s Opponent Board:\n";
        cout << "  1 2 3 4 5 6 7 8 9 10\n";
        for (int i = 0; i < 10; ++i) {
            cout << static_cast<char>('A' + i) << ' ';
            for (int j = 0; j < 10; ++j) {
                cout << (opponentBoard[i][j] == '*' || opponentBoard[i][j] == 'H' ? opponentBoard[i][j] : ' ') << ' ';
            }
            cout << '\n';
        }
        cout << '\n';
    }
};

class BattleshipGame {
public:
    void startGame() {
        for (auto& player : players) {
            setupBoard(player);
        }

        while (!isGameOver()) {
            for (auto& player : players) {
                playTurn(player);
            }
        }

        cout << "Game Over! " << getWinner().name << " wins.\n";
    }

private:
    vector<Player> players = {{"Player 1"}, {"Player 2"}};

    void setupBoard(Player& player) {
        cout << player.name << ", set up your board:\n";
        player.printOwnBoard();
        vector<Ship> ships;
        Ship s1("P1",5,'a');
        Ship s2("P2",5,'b');
        ships.push_back(s1);
        ships.push_back(s2);
        for (auto& ship : ships ){
            placeShip(player, ship);
        }
    }

    void placeShip(Player& player, const Ship& ship) {
        while (true) {
            try {
                string coordinates;
                char orientation;
                cout << "Place " << ship.name << " (size " << ship.size << "): ";
                cin >> coordinates;
                cout << "Enter orientation (H for horizontal, V for vertical): ";
                cin >> orientation;

                player.placeShip(ship, coordinates, orientation);
                break;
            } catch (const invalid_argument& e) {
                cerr << "Invalid input: " << e.what() << "\n";
            }
        }
    }

    void playTurn(Player& player) {
        cout << player.name << ", it's your turn:\n";
        player.printOwnBoard();
        player.printOpponentBoard();

        while (true) {
            try {
                string guess;
                cout << "Enter your guess coordinates: ";
                cin >> guess;
                string result = player.makeGuess(guess);
                cout << result << "\n";
                break;
            } catch (const invalid_argument& e) {
                cerr << "Invalid input: " << e.what() << "\n";
            }
        }

        cout << "Press Enter to continue...\n";
        cin.ignore();
        cin.get();
    }

    bool isGameOver() const {
        return any_of(players.begin(), players.end(), [](const Player& player) { return player.hasLost(); });
    }

    Player getWinner() const {
        for (const auto& player : players) {
            if (!player.hasLost()) {
                return player;
            }
        }
        throw logic_error("No winner found.");
    }
};

int main() {
    BattleshipGame game;
    game.startGame();

    return 0;
}