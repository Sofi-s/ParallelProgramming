#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>
#include <string>

int main() {
    try {
        std::ifstream fileA("matrix_a.txt");
        std::ifstream fileB("matrix_b.txt");
        if (!fileA || !fileB)
            throw std::runtime_error("failed to open input files");

        int n = 0, nB = 0;
        if (!(fileA >> n) || !(fileB >> nB))
            throw std::runtime_error("failed to read matrix size");
        if (n != nB)
            throw std::runtime_error("size mismatch: " + std::to_string(n) +
                                     " vs " + std::to_string(nB));
        if (n <= 0)
            throw std::runtime_error("invalid size: " + std::to_string(n));

        std::cout << "Matrix size: " << n << "x" << n << "\n";

        std::vector<std::vector<double>> A(n, std::vector<double>(n));
        std::vector<std::vector<double>> B(n, std::vector<double>(n));
        std::vector<std::vector<double>> C(n, std::vector<double>(n, 0.0));

        for (auto& row : A)
            for (auto& x : row)
                if (!(fileA >> x))
                    throw std::runtime_error("failed to read matrix A");

        for (auto& row : B)
            for (auto& x : row)
                if (!(fileB >> x))
                    throw std::runtime_error("failed to read matrix B");

        fileA.close();
        fileB.close();

        std::cout << "Multiplying matrices...\n";
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n; k++)
                    C[i][j] += A[i][k] * B[k][j];

        auto end = std::chrono::high_resolution_clock::now();
        double time_sec = std::chrono::duration<double>(end - start).count();
        std::cout << "Execution time: " << time_sec << " seconds\n";

        {
            std::ofstream out("result.txt");
            if (!out)
                throw std::runtime_error("failed to open result file");

            out << n << '\n';
            for (auto& row : C) {
                for (double x : row)
                    out << std::fixed << std::setprecision(15) << x << ' ';
                out << '\n';
            }
            if (!out)
                throw std::runtime_error("failed to write result");
        }
        std::cout << "Result saved to result.txt\n";

        long long memory = 3LL * n * n * sizeof(double);
        long long operations = 2LL * n * n * n;

        std::cout << "\n========== METRICS ==========\n";
        std::cout << "Memory: " << memory / 1024 << " KB\n";
        std::cout << "Operations: " << operations << "\n";
        std::cout << "Performance: "
                  << (time_sec > 0 ? (operations / 1e9) / time_sec : 0.0)
                  << " GFLOPS\n";

        std::cout << "\nRunning verification...\n";
        std::system("py verify.py");

    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "Unknown error\n";
        return 1;
    }

    return 0;
}