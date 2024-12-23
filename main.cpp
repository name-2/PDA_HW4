#include "inc/info.hpp"

GridMap gridMap;
CostSettings costSettings;

bool PrintInfo = true;

// 讀取 gridMap.gmp
bool readGridMap(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }

    if (PrintInfo)
        cout << "Reading file " << filename << endl;

    string line, token;
    int chip_num = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        ss >> token;

        if (token == ".ra") {
            getline(file, line);
            stringstream data(line);
            data >> gridMap.ArealowerLeftX >> gridMap.ArealowerLeftY >> gridMap.Areawidth >> gridMap.Areaheight;
        } else if (token == ".g") {
            getline(file, line);
            stringstream data(line);
            data >> gridMap.Gcellwidth >> gridMap.Gcellheight;
        } else if (token == ".c") {
            chip_num++;
            if (chip_num == 1) {
                getline(file, line);
                stringstream data(line);
                data >> gridMap.chip1.lowerLeftX >> gridMap.chip1.lowerLeftY >> gridMap.chip1.width >> gridMap.chip1.height;
                gridMap.chip1.lowerLeftX += gridMap.ArealowerLeftX;
                gridMap.chip1.lowerLeftY += gridMap.ArealowerLeftY;
            } else if (chip_num == 2) {
                getline(file, line);
                stringstream data(line);
                data >> gridMap.chip2.lowerLeftX >> gridMap.chip2.lowerLeftY >> gridMap.chip2.width >> gridMap.chip2.height;
                gridMap.chip2.lowerLeftX += gridMap.ArealowerLeftX;
                gridMap.chip2.lowerLeftY += gridMap.ArealowerLeftY;
            }
        } else if (token == ".b") {
            while (1) {
                getline(file, line);
                if (line.empty())
                    break;
                stringstream data(line);
                int bumpIdx, x, y;
                data >> bumpIdx >> x >> y;
                if (chip_num == 1) {
                    gridMap.chip1.bumps.emplace_back(bumpIdx, Coords{x + gridMap.chip1.lowerLeftX, y + gridMap.chip1.lowerLeftY});
                } else if (chip_num == 2) {
                    gridMap.chip2.bumps.emplace_back(bumpIdx, Coords{x + gridMap.chip2.lowerLeftX, y + gridMap.chip2.lowerLeftY});
                }
            }
        }
    }
    gridMap.GcellNumX = gridMap.Areawidth / gridMap.Gcellwidth;
    gridMap.GcellNumY = gridMap.Areaheight / gridMap.Gcellheight;
    for (int i = 0; i < gridMap.chip1.bumps.size(); i++) {
        netInfo net;
        net.netID = gridMap.chip1.bumps[i].first;
        net.source = gridMap.chip1.bumps[i].second;
        net.sink = gridMap.chip2.bumps[i].second;
        gridMap.nets.emplace_back(net);
    }
    file.close();
    if (PrintInfo)
        cout << "Reading file " << filename << " done." << endl;
    return true;
}

// 讀取 GCell.gcl
bool readGCell(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }
    if (PrintInfo)
        cout << "Reading file " << filename << endl;

    string line, token;
    while (getline(file, line)) {
        stringstream ss(line);
        ss >> token;
        int gcell_num = 0;

        if (token == ".ec") { // Edge capacity
            while (getline(file, line) && !line.empty()) {
                stringstream data(line);
                GCell gcell;
                data >> gcell.leftEdgeCapacity >> gcell.bottomEdgeCapacity;
                gcell.x = (gcell_num % gridMap.GcellNumX) * gridMap.Gcellwidth + gridMap.ArealowerLeftX;
                gcell.y = (gcell_num / gridMap.GcellNumX) * gridMap.Gcellheight + gridMap.ArealowerLeftY;
                gridMap.gCells[make_pair(gcell_num % gridMap.GcellNumX, gcell_num / gridMap.GcellNumX)] = gcell;
                gcell_num++;
            }
        }
    }
    file.close();
    if (PrintInfo)
        cout << "Reading file " << filename << " done." << endl;
    return true;
}

// 讀取 cost.cst
bool readCostSettings(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }
    int layer = 0;
    gridMap.OverflowPenalty = 0;

    if (PrintInfo)
        cout << "Reading file " << filename << endl;
    string line, token;
    while (getline(file, line)) {
        stringstream ss(line);
        ss >> token;

        if (token == ".alpha") {
            ss >> token;
            costSettings.alpha = stod(token);
        } else if (token == ".beta") {
            ss >> token;
            costSettings.beta = stod(token);
        } else if (token == ".gamma") {
            ss >> token;
            costSettings.gamma = stod(token);
        } else if (token == ".delta") {
            ss >> token;
            costSettings.delta = stod(token);
        } else if (token == ".v") {
            getline(file, line);
            costSettings.viaCost = stod(line);
        } else if (token == ".l") {
            layer++;
            if (layer == 1) {
                for (int i = 0; i < gridMap.GcellNumY; ++i) {
                    getline(file, line);
                    stringstream data(line);
                    int j = 0;
                    double cost;
                    while (data >> token) {
                        cost = stod(token);
                        gridMap.gCells[make_pair(j++, i)].layer1Cost = cost;
                        gridMap.OverflowPenalty = max(gridMap.OverflowPenalty, cost);
                    }
                }
            } else if (layer == 2) {
                for (int i = 0; i < gridMap.GcellNumY; ++i) {
                    getline(file, line);
                    stringstream data(line);
                    int j = 0;
                    double cost;
                    while (data >> token) {
                        cost = stod(token);
                        gridMap.gCells[make_pair(j++, i)].layer2Cost = cost;
                        gridMap.OverflowPenalty = max(gridMap.OverflowPenalty, cost);
                    }
                }
            }
        }
    }
    file.close();
    if (PrintInfo)
        cout << "Reading file " << filename << " done." << endl;
    return true;
}

bool outputFile(const string &filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }
    if (PrintInfo)
        cout << "Writing file " << filename << endl;

    for (int i = 0; i < gridMap.nets.size(); i++) {
        file << "n" << gridMap.nets[i].netID << "\n";
        for (int j = 0; j < gridMap.nets[i].path.size(); j++) {
            pair<Coords, Coords> path = gridMap.nets[i].path[j];
            if (path.first.x == path.second.x) {
                if (j != 0)
                    file << "via\n";
                file << "M1 " << path.first.x << " " << path.first.y << " " << path.second.x << " " << path.second.y << endl;
            } else if (path.first.y == path.second.y) {
                file << "via\n";
                file << "M2 " << path.first.x << " " << path.first.y << " " << path.second.x << " " << path.second.y << endl;
            } else {
                cout << "Error: Path is not L-shape\n";
            }
            if (j == gridMap.nets[i].path.size() - 1)
                file << "via\n";
        }
        file << ".end" << endl;
    }
    file.close();
    if (PrintInfo)
        cout << "Writing file " << filename << " done." << endl;
    return true;
}

void Lshape() {
    for (int i = 0; i < gridMap.nets.size(); i++) {
        auto *cur_net = &gridMap.nets[i];
        cur_net->path.clear();
        cur_net->path.emplace_back(make_pair(Coords{cur_net->source.x, cur_net->source.y}, Coords{cur_net->source.x, cur_net->sink.y}));
        cur_net->path.emplace_back(make_pair(Coords{cur_net->source.x, cur_net->sink.y}, Coords{cur_net->sink.x, cur_net->sink.y}));
    }
}

// 主程式
int main(int argc, char *argv[]) {
    setbuf(stdout, NULL); // 關閉 stdout 緩衝區
    setbuf(stderr, NULL); // 關閉 stderr 緩衝區
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <gridMap.gmp> <GCell.gcl> <cost.cst> <output.lg>" << endl;
        return 1;
    }

    string gridMapFile = argv[1];
    string gCellFile = argv[2];
    string costSettingsFile = argv[3];
    string outputFileName = argv[4];
    if (PrintInfo)
        cout << "Reading files: " << gridMapFile << ", " << gCellFile << ", " << costSettingsFile << endl;

    // 讀取檔案
    if (!readGridMap(gridMapFile))
        return 1;
    if (!readGCell(gCellFile))
        return 1;
    if (!readCostSettings(costSettingsFile))
        return 1;
    Lshape();
    if (!outputFile(outputFileName))
        return 1;

    // 測試輸出
    // cout << "Routing Area: (" << gridMap.ArealowerLeftX << "," << gridMap.ArealowerLeftY << ") Width=" << gridMap.Areawidth << " Height=" << gridMap.Areaheight << endl;

    // cout << "\nGcell Info: Width=" << gridMap.Gcellwidth << " Height=" << gridMap.Gcellheight << endl;

    // cout << "\nChip1 Info: (" << gridMap.chip1.lowerLeftX << ", " << gridMap.chip1.lowerLeftY
    //      << ") Width=" << gridMap.chip1.width << " Height=" << gridMap.chip1.height << endl;
    // for (const auto &bump : gridMap.chip1.bumps) {
    //     cout << "  Bump " << bump.first << ": (" << bump.second.first << ", " << bump.second.second << ")" << endl;
    // }

    // cout << "\nChip2 Info: (" << gridMap.chip2.lowerLeftX << ", " << gridMap.chip2.lowerLeftY
    //      << ") Width=" << gridMap.chip2.width << " Height=" << gridMap.chip2.height << endl;
    // for (const auto &bump : gridMap.chip2.bumps) {
    //     cout << "  Bump " << bump.first << ": (" << bump.second.first << ", " << bump.second.second << ")" << endl;
    // }

    // cout << "\nGCell Capacities:" << endl;
    // for (const auto &gcell : gridMap.gCells) {
    //     cout << "  GCell[" << gcell.first.first << "," << gcell.first.second << "]: Left=" << gcell.second.leftEdgeCapacity << ", Bottom=" << gcell.second.bottomEdgeCapacity << endl;
    // }

    // cout << "\nCost Settings:" << endl;
    // cout << "  Alpha: " << costSettings.alpha << endl;
    // cout << "  Beta: " << costSettings.beta << endl;
    // cout << "  Gamma: " << costSettings.gamma << endl;
    // cout << "  Delta: " << costSettings.delta << endl;
    // cout << "  Via Cost: " << costSettings.viaCost << endl;

    // cout << "\nLayer 1 Costs:" << endl;
    // for (int j = 0; j < gridMap.GcellNumY; j++) {
    //     for (int i = 0; i < gridMap.GcellNumX; i++) {
    //         cout << " " << gridMap.gCells[make_pair(i, j)].layer1Cost;
    //     }
    //     cout << endl;
    // }

    // cout << "\nLayer 2 Costs:" << endl;
    // for (int j = 0; j < gridMap.GcellNumY; j++) {
    //     for (int i = 0; i < gridMap.GcellNumX; i++) {
    //         cout << " " << gridMap.gCells[make_pair(i, j)].layer2Cost;
    //     }
    //     cout << endl;
    // }

    return 0;
}
