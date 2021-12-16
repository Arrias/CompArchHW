#include "util.h"

using namespace std;

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

        Tab tab;
        tab.init(in);

        auto clk = clock();
        tab.customize();
        cout << "Time (" << omp_get_max_threads() << " thread(s)): " << (double) (clock() - clk) / CLOCKS_PER_SEC * 1000. << " ms" << endl;
        tab.log(out);
    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    return 0;
}
