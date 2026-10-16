#include <climits>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

enum Terrain { PLAINS = 0, RIVER = 1, MOUNTAIN = 2 };

struct Cell {
    int region;
    int type;
    int trackOwner;
    int instability;
    bool inked;
    vector<pair<int, int>> connections;
};

struct Town {
    int id;
    int x;
    int y;
    vector<int> desired;
};

struct Region {
    bool hasTown = false;
    vector<int> cells;
};

struct Game {
    int myId;
    int width;
    int height;
    int myScore;
    int foeScore;
    vector<Cell> cells;
    vector<Town> towns;
    vector<Region> regions;

    Cell& at(int x, int y) { return cells[y * width + x]; }

    void readInit() {
        cin >> myId >> width >> height;
        cells.assign(width * height, Cell());
        for (int i = 0; i < width * height; i++) {
            cin >> cells[i].region >> cells[i].type;
            if (cells[i].region >= (int)regions.size())
                regions.resize(cells[i].region + 1);
            regions[cells[i].region].cells.push_back(i);
        }
        int townCount;
        cin >> townCount;
        towns.resize(townCount);
        for (Town& t : towns) {
            string desired;
            cin >> t.id >> t.x >> t.y >> desired;
            if (desired != "x") {
                stringstream ss(desired);
                string id;
                while (getline(ss, id, ','))
                    t.desired.push_back(stoi(id));
            }
            regions[at(t.x, t.y).region].hasTown = true;
        }
    }

    void readTurn() {
        cin >> myScore >> foeScore;
        for (Cell& c : cells) {
            string conns;
            int inked;
            cin >> c.trackOwner >> c.instability >> inked >> conns;
            c.inked = inked;
            c.connections.clear();
            if (conns == "x")
                continue;
            stringstream ss(conns);
            string pair;
            while (getline(ss, pair, ',')) {
                size_t dash = pair.find('-');
                c.connections.push_back({stoi(pair.substr(0, dash)), stoi(pair.substr(dash + 1))});
            }
        }
    }

    bool isActive(int from, int to) {
        const Town& t = towns[from];
        for (const auto& conn : at(t.x, t.y).connections)
            if (conn.first == from && conn.second == to)
                return true;
        return false;
    }
};

int main() {
    Game game;
    game.readInit();

    while (true) {
        game.readTurn();
        vector<string> actions;

        int bestDist = INT_MAX;
        const Town* from = nullptr;
        const Town* to = nullptr;
        for (const Town& t : game.towns) {
            for (int id : t.desired) {
                const Town& o = game.towns[id];
                int dist = abs(t.x - o.x) + abs(t.y - o.y);
                if (!game.isActive(t.id, id) && dist < bestDist) {
                    bestDist = dist;
                    from = &t;
                    to = &o;
                }
            }
        }
        if (from)
            actions.push_back("AUTOPLACE " + to_string(from->x) + " " + to_string(from->y) + " "
                              + to_string(to->x) + " " + to_string(to->y));

        int foe = 1 - game.myId;
        int target = -1;
        int mostFoeTracks = 0;
        for (int r = 0; r < (int)game.regions.size(); r++) {
            if (game.regions[r].hasTown || game.cells[game.regions[r].cells[0]].inked)
                continue;
            int foeTracks = 0;
            for (int i : game.regions[r].cells)
                foeTracks += game.cells[i].trackOwner == foe;
            if (foeTracks > mostFoeTracks) {
                mostFoeTracks = foeTracks;
                target = r;
            }
        }
        if (target >= 0)
            actions.push_back("DISRUPT " + to_string(target));

        if (actions.empty())
            actions.push_back("WAIT");

        string line;
        for (size_t i = 0; i < actions.size(); i++)
            line += (i ? ";" : "") + actions[i];
        cout << line << endl;
    }
}
