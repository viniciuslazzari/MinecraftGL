#include "perlin_noise.hpp"

#include <cstdlib>
#include <time.h>
#include <cmath>
#include <cstdio>

PerlinNoise::PerlinNoise(int row, int col){
    this->row = row;
    this->col = col;
}

void PerlinNoise::generateSeed(){
    int row = this->row;
    int col = this->col;

    srand(time(NULL));

    float seedPoint;

    for(int i = 0; i < row; i++){
        vector<float> temp;
        
        for(int j = 0; j < col; j++){
            seedPoint = (float) rand() / (float) RAND_MAX;
            temp.push_back(seedPoint);
        }

        this->seed.push_back(temp);
    }
}

vector<vector<float>> PerlinNoise::normalize(vector<vector<float>> grid){
    int row = this->row;
    int col = this->col;

    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            grid[i][j] = round(grid[i][j] * 20);
        } 
    }

    return grid;
}

vector<vector<float>> PerlinNoise::generateNoise(int octaves){
    generateSeed();

    int row = this->row;
    int col = this->col;

    vector<vector<float>> noise;

    float pointNoise, scale, scaleAcc;

    for(int i = 0; i < row; i++){
        vector<float> temp;

        for(int j = 0; j < col; j++){
            pointNoise = 0.0f;
            scale = 1.0f;
            scaleAcc = 0.0f;

            for(int o = 0; o < octaves; o++){
                int octaveSpacing = row / 2;

                int curPointX = (i / octaveSpacing) * octaveSpacing;
                int nextPointX = (curPointX + octaveSpacing) % row;

                int curPointY = (j / octaveSpacing) * octaveSpacing;
                int nextPointY = (curPointY + octaveSpacing) % row;

                float octaveIndexX = (float) (i - curPointX) / (float) octaveSpacing;
                float octaveIndexY = (float) (j - curPointY) / (float) octaveSpacing;

                float sampleA = (1.0f - octaveIndexX) * this->seed[curPointY][curPointX] + octaveIndexX * this->seed[curPointY][nextPointX];
                float sampleB = (1.0f - octaveIndexX) * this->seed[nextPointY][curPointX] + octaveIndexX * this->seed[nextPointY][nextPointX];

                scaleAcc += scale;
                pointNoise += (octaveIndexY * (sampleB - sampleA) + sampleA) * scale;
                scale /= 2.0f;
            }

            temp.push_back(pointNoise / scaleAcc);
        }

        noise.push_back(temp);
    }

    noise = normalize(noise);

    return noise;
};