#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <algorithm>

using namespace std;

struct segment {
    uint8_t vals[3]{};

    uint8_t &operator[](int id) {
        return vals[id];
    }
};

vector<int> dimensions = {1, 3};
vector<string> fmts = {"P5", "P6"};

struct Tab {
    int dimension;
    int n, m;
    vector<vector<segment>> tab;

    Tab(ifstream &in) {
        string fmt;
        in >> fmt;
        if (fmt != "P6" && fmt != "P5") throw runtime_error("wrong file format: " + fmt + " is not correct format");
        dimension = dimensions[fmt == "P6"];
        string h, w;
        in >> h >> w;
        n = stoi(w);
        m = stoi(h);
        in >> w; // ignor 255
        tab.assign(n, vector<segment>(m));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                for (int dim = 0; dim < dimension; ++dim) {
                    in >> tab[i][j][dim];
                }
            }
        }
    }

    void customize() {
        uint8_t l_bound = UINT8_MAX, r_bound = 0;
        int iter = 0;

#pragma omp parallel for reduction(max:r_bound), reduction(min:l_bound), private(iter)
        for (iter = 0; iter < n * m; ++iter) {
            int i = iter / m;
            int j = iter % m;
            for (int dim = 0; dim < dimension; ++dim) {
                r_bound = r_bound > tab[i][j][dim] ? r_bound : tab[i][j][dim];
                l_bound = l_bound < tab[i][j][dim] ? l_bound : tab[i][j][dim];
            }
        }

        // f : [l_bound, r_bound] x [l_bound, r_bound] x [l_bound, r_bound] -> [0, 255] x [0, 255] x [0, 255]
        auto f = [&l_bound, &r_bound](int x) {
            return (1 << 8) * (x - l_bound + 1) / (r_bound - l_bound + 1) - 1;
        };

#pragma omp parallel for private(iter)
        for (iter = 0; iter < n * m; ++iter) {
            int i = iter / m;
            int j = iter % m;
            for (int dim = 0; dim < dimension; ++dim) {
                if (tab[i][j][dim] < l_bound) {
                    tab[i][j][dim] = f(tab[i][j][dim]);
                }
            }
        }
    }

    void log(ofstream &out) {
        out << fmts[dimension == 3] << "\n";
        out << to_string(m) << " " << to_string(n) << "\n";
        out << "255\n";
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                for (int dim = 0; dim < dimension; ++dim) {
                    out << tab[i][j][dim];
                }
            }
        }
    }
};

int main(int argc, char *argv[]) {
    try {
        if (argc < 4) {
            throw runtime_error("incorrect number of parameters");
        }

        ifstream in(argv[2]);
        ofstream out(argv[3]);

        int thr_num = stoi(argv[1]);
        if (thr_num < 0) throw runtime_error("incorrect number of threads");

        if (thr_num > 0) {
            omp_set_num_threads(thr_num);
        }

        Tab tab(in);

        auto clk = clock();
        tab.customize();
        cout << "Time (" << omp_get_max_threads() << " thread(s)): " << (double) (clock() - clk) / CLOCKS_PER_SEC << " ms" << endl;
        tab.log(out);
    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    return 0;
}
