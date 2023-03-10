#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <vector>

using namespace std;

class PerlinNoise {
    private:
        int row;
        int col;
        vector<vector<float>> seed;

        void generateSeed();
        vector<vector<float>> normalize(vector<vector<float>> grid);

    public:
        PerlinNoise(int row, int col);
        vector<vector<float>> generateNoise(int octaves);
};

#endif