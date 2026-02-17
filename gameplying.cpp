#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

const int INF = 1000000000;

//åÇáİäßÔä ÈÓ ÚÔÇä ÇØÈÚ ÇáÍÇáå
void printPiles(const vector<int>& v) {
    for (int i = 0; i < v.size(); i++) {
        cout << v[i];
        if (i + 1 < v.size()) cout << " ";
    }
}

//åæä ÈÔæİ ÇĞÇ ÇááÚÈÉ ÎáÕÊ æáÇ áÇ
bool terminal_test(const vector<int>& st) {
    //áæ ßáåã ÇŞá ãä 3 ãÚäÇåÇ ãÇ ãäŞÏÑ äŞÓã æáÇ æÇÍÏ
    for (int x : st) {
        if (x >= 3) return false;
    }
    return true;
}

//ÇáŞíãÉ áãÇ äæÕá áäåÇíå
int utility(bool maxTurn) {
    //áæ ÏæÑ max ãÇ ÚäÏå ÍÑßÇÊ íÎÓÑ
    // áæ ÏæÑ min æãÇ ÚäÏå ÍÑßÇÊ íÎÓÑ min (íÚäí ÇáİæÒmax )
    return maxTurn ? -1 : 1;
}

//ÈÌíÈ ßá ÇáÍÑßÇÊ ÇáããßäÉ (íÚäí ßíİ äŞÓã ÇáåíÈÇÊ(
vector<vector<int>> expansion(const vector<int>& st) {
    vector<vector<int>> nextStates;

    for (int i = 0; i < st.size(); i++) {
        int h = st[i];

        //äÌÑÈ äŞÓã ÇáåíÈ áÌÒÆíä ãÔ ãÊÓÇæííä
        for (int a = 1; a <= h / 2; a++) {
            int b = h - a;
            if (a == b) continue; // áÇÒã íßæäæÇ ãÎÊáİíä

            vector<int> ns = st;
            ns.erase(ns.begin() + i);

            ns.push_back(a);
            ns.push_back(b);


            sort(ns.begin(), ns.end());


            if (find(nextStates.begin(), nextStates.end(), ns) == nextStates.end())
                nextStates.push_back(ns);
        }
    }

    return nextStates;
}

int minimize(const vector<int>& st, int alpha, int beta, int& cut, int& visited);
int maximize(const vector<int>& st, int alpha, int beta, int& cut, int& visited);

//ÏÇáÉ ÇáãÇßÓ (Çááí ÈÏæ íÍÇæá íÑÈÍ)
int maximize(const vector<int>& st, int alpha, int beta, int& cut, int& visited) {
    visited++;

    if (terminal_test(st)) return utility(true);

    int best = -INF;
    auto kids = expansion(st);

    for (auto& ch : kids) {
        int val = minimize(ch, alpha, beta, cut, visited);
        if (val > best) best = val;

        //ÇĞÇ İí ŞÕ İÑÚ
        if (best >= beta) {
            cut++;
            return best;
        }

        // äÍÏË alpha
        if (best > alpha) alpha = best;
    }

    return best;
}

//ÏÇáÉ Çáãíä (Çááí ÈÏæ íÎÓÑ max)
int minimize(const vector<int>& st, int alpha, int beta, int& cut, int& visited) {
    visited++;

    if (terminal_test(st)) return utility(false);

    int best = INF;
    auto kids = expansion(st);

    for (auto& ch : kids) {
        int val = maximize(ch, alpha, beta, cut, visited);
        if (val < best) best = val;

        // áæ ÕÇÑÊ ŞÕ İÑÚ
        if (best <= alpha) {
            cut++;
            return best;
        }

        //äÍÏË beta
        if (best < beta) beta = best;
    }

    return best;
}

//åæä ÇáÔÛá ßáå ÈÕíÑ
void solve(const vector<int>& root) {
    int prun = 0;     // ÚÏÏ ÇáŞÕ
    int eval = 0;     // ÚÏÏ ÇáÍÇáÇÊ íáí ÒÑäÇåÇ
    int alpha = -INF;
    int beta = INF;


    eval++;
    auto succ = expansion(root);


    cout << "Successors :" << endl;



    int bestVal = -INF;
    vector<int> bestState;

    for (auto& ch : succ) {
        int a2 = alpha, b2 = beta;

        int v = minimize(ch, a2, b2, prun, eval);

        printPiles(ch);
        cout << ", value=" << (v > 0 ? "+" : "") << v << endl;

        if (v > bestVal) {
            bestVal = v;
            bestState = ch;
        }

        if (v > alpha) alpha = v;
    }

    cout << "The best successor is ";
    printPiles(bestState);
    cout << endl;

    cout << "The number of prunings = " << prun << endl;
    cout << "The number of states evaluated = " << eval << endl;

    cout << "The best successor state from this state is ";
    printPiles(bestState);
    cout << endl;
}

int main() {
    cout << "Enter current position :";

    string line;
    getline(cin, line);

    stringstream ss(line);
    vector<int> arr;
    int x;
    while (ss >> x) arr.push_back(x);

    vector<int> st;
    for (int i = 1; i < arr.size(); i++) st.push_back(arr[i]);

    sort(st.begin(), st.end());

    solve(st);

    return 0;
}
