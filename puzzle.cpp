#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <sstream>
using namespace std;


class Board {
public:
    vector<vector<int>> tiles;
    int blankrow, blankcol;
    int cost , heur;
    string movesmade;
    vector<string> moveslist;

    Board() : cost(0), heur(0) 
    {

        tiles.resize(4, vector<int>(2, 0)); 
    
    }

    Board(const string& s) : cost(0), heur(0) {
        tiles.resize(4, vector<int>(2, 0));
        int idx = 0;
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 2; c++) {
                tiles[r][c] = s[idx] - '0';
                if (tiles[r][c] == 0) { blankrow = r; blankcol = c; }
                idx++;
            }
    }

    string tostr() const {
        string out;
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 2; c++)
                out += to_string(tiles[r][c]);
        return out;
    }

    string torowstr() const {
        return tostr();
    }

    void show() const {
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 2; c++) {
                cout << tiles[r][c];
                if (c == 0) cout << " ";
            }
            cout << endl;
        }
    }

    bool operator==(const Board& b) const {
        return tiles == b.tiles;
    }


    int f() const
    {
        return cost + heur; 
    }
};

struct BoardHash {
    size_t operator()(const Board& b) const 
    { 
        return hash<string>()(b.tostr());

    }
};

struct BoardCmp {
    bool operator()(const Board& a, const Board& b) const {
        if (a.f() != b.f()) return a.f() > b.f();
        return a.cost > b.cost;
    }
};

class Solver {
    Board goal;

    int mandist(int val ,int r,int c) const {
        if (val == 0) return 0;
        int gr = (val - 1) / 2;
        int gc = (val - 1) % 2;
        return abs(r - gr) + abs(c - gc);
    }

    int heuristic(const Board& b) const {
        int tot = 0;
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 2; c++)
                tot += mandist(b.tiles[r][c], r, c);

        for (int r = 0; r < 4; r++) {
            int l = b.tiles[r][0], ri = b.tiles[r][1];
            if (l != 0 && ri != 0) {
                int gl = (l - 1) / 2, gr = (ri - 1) / 2;
                if (gl == r && gr == r) {
                    int gcL = (l - 1) % 2, gcR = (ri - 1) % 2;
                    if (gcL == 0 && gcR == 1 && l > ri) tot += 2;
                    else if (gcL == 1 && gcR == 0 && ri > l) tot += 2;
                }
            }
        }
        return tot;
    }

    bool valid(int r, int c) const {
        return r >= 0 && r < 4 && c >= 0 && c < 2; 
    }

    vector<Board> nextboards(const Board& b) const {
        
        vector<Board> nexts;

        int dirs[4][2] = { {0,-1},{0,1},{-1,0},{1,0} };
        string moves[4] = { "L" ,"R", "U" ,"D" };


        for (int d = 0; d < 4; d++) {
            int nr = b.blankrow + dirs[d][0], nc = b.blankcol + dirs[d][1];
            if (valid(nr, nc)) {
                Board nb = b;
                swap(nb.tiles[b.blankrow][b.blankcol], nb.tiles[nr][nc]);

                nb.blankrow = nr; nb.blankcol = nc;
                nb.cost = b.cost + 1;
                nb.heur = heuristic(nb);
                nb.movesmade = b.movesmade + moves[d] + "\n";
                nb.moveslist = b.moveslist;
                nb.moveslist.push_back(moves[d]);
                nexts.push_back(nb);
            }
        }
        for (int r = 0; r < 4; r++) {
            Board nb = b;
            int t1 = nb.tiles[r][0], t2 = nb.tiles[r][1];
            swap(nb.tiles[r][0], nb.tiles[r][1]);
            if (nb.blankrow == r) nb.blankcol = (nb.blankcol == 0 ? 1 : 0);


            nb.cost = b.cost + 2;
            nb.heur = heuristic(nb);
            string mv = "E " + to_string(t1) + " " + to_string(t2);
            nb.movesmade = b.movesmade + mv + "\n";
            nb.moveslist = b.moveslist;
            nb.moveslist.push_back(mv);
            nexts.push_back(nb);
        }
        return nexts;
    }

public:
    Solver(const Board& g) : goal(g) {}

    struct Result { bool found; Board solution; int nodes; };

    Result solve(const Board& start) {
        priority_queue<Board, vector<Board>, BoardCmp> openset;
        unordered_set<string> closed;
        unordered_map<string, int> bestF;

        Board s = start;
        s.heur = heuristic(s);
        s.moveslist.clear();
        openset.push(s);
        bestF[s.tostr()] = s.f();
        int nodes = 0;

        while (!openset.empty()) {
            Board cur = openset.top();
            openset.pop();
            string cStr = cur.tostr();
            
            if (closed.find(cStr) != closed.end()) continue;
            closed.insert(cStr);
            nodes++;




            if (cur == goal) return { true,cur,nodes };

            for (Board nb : nextboards(cur)) {
                string nStr = nb.tostr();
                if (closed.find(nStr) != closed.end()) continue;
                if (bestF.find(nStr) == bestF.end() || nb.f() < bestF[nStr]) {
                    bestF[nStr] = nb.f();
                    openset.push(nb);
                }
            }
        }
        return { false ,s, nodes };
    }
};

struct Boards {
    Board start;
    Board goal; 
};

Boards readfile(const string& fname) {
    ifstream f(fname);
    if (!f.is_open()) { cerr << "Cant open file " << fname << endl; exit(1); }
    string s, g; getline(f, s); getline(f, g); f.close();
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    g.erase(remove(g.begin(), g.end(), ' '), g.end());
    if (s.length() != 8 || g.length() != 8) { cerr << "Invalid file format" << endl; exit(1); }
    return { Board(s), Board(g) };
}

int main() {
    cout << "ENTER NAME FILE containing initial and goal states :  ";
    string fname; getline(cin, fname);
    if (fname.length() >= 2 && fname[0] == '"' && fname[fname.length() - 1] == '"') fname = fname.substr(1, fname.length() - 2);

    Boards b = readfile(fname);

    Solver solver(b.goal);
    auto res = solver.solve(b.start);

    if (res.found) {

        cout << "Solution Found!" << endl;
        cout << "EXPANDED NODES :  " << res.nodes << endl;
       
        int l = res.solution.moveslist.size();
        
        cout << "SOLUTION LENGTH: " << l << endl;
        cout << "\n================== SOLUTION PATH ==============" << endl<< endl;
        cout << "INITIAL STATE : " << b.start.torowstr() << endl<<endl;
        cout << "_____________________________" << endl;

        Board curr = b.start;
        for (int i = 0; i < l; i++) {
            cout << "Move " << i + 1 << ": " << res.solution.moveslist[i] << endl;
           
            string mv = res.solution.moveslist[i];
            if (mv == "L" || mv == "R" || mv == "U" || mv == "D") {
                int nr = curr.blankrow, nc = curr.blankcol;
                if (mv == "L") nc--; else if (mv == "R") nc++; else if (mv == "U") nr--; else nr++;
                swap(curr.tiles[curr.blankrow][curr.blankcol], curr.tiles[nr][nc]);
                curr.blankrow = nr; curr.blankcol = nc;
            }
            else if (mv[0] == 'E') {
                stringstream ss(mv); string e, t1s, t2s; ss >> e >> t1s >> t2s;
                int t1 = stoi(t1s), t2 = stoi(t2s);
                for (int r = 0; r < 4; r++) {
                    bool h1 = (curr.tiles[r][0] == t1 || curr.tiles[r][1] == t1);
                    bool h2 = (curr.tiles[r][0] == t2 || curr.tiles[r][1] == t2);
                    if (h1 && h2) { swap(curr.tiles[r][0], curr.tiles[r][1]); if (curr.blankrow == r) curr.blankcol = (curr.blankcol == 0 ? 1 : 0); break; }
                }
            }
            
            curr.show();
            
            cout << "______________________________________" << endl;
        }
        cout << "GOAL STATE : " << b.goal.torowstr() << endl;
        cout << "Goal State Reached?   YES " << endl;
    }
    else {
        cout << "NO SULUTION " << endl;
        cout << "EXPANDED NODES : " << res.nodes << endl;
        cout << "Goal State Reached?  NO " << endl;
    }

    return 0;
}
