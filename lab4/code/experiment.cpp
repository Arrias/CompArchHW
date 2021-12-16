#include "util.h"
#include <omp.h>

const int L = 10; // сколько больших файлов обрабатывается
vector<int> chunks = {1, 8, 256};

int main() {
    auto type = omp_sched_dynamic;

    string fl = "../samples/pikachu.ppm";
    ifstream in(fl);
    Tab orig;
    orig.init(in);
    vector<Tab> others(L);

    for (int i = 0; i < L; ++i) {
        others[i].init(orig);
    }

    for (int chunk : chunks) {
        omp_set_schedule(type, chunk);

        for (int thr = 1; thr <= 7;  ++thr) {
            omp_set_num_threads(thr);

            double now = clock();

#pragma omp parallel for
            for (int i = 0; i < L; ++i) {
                others[i].customize();
            }

            // cout << thr << " ";
            cout << (clock() - now) /  10000. << endl;
        }

        cout << "\n\n";
    }

}