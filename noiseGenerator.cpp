#include <random>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "SimplexNoise.h"
#include "FastNoiseLite.h"
// https://github.com/Auburn/FastNoiseLite

class noiseGenerator
{
private:
    int _seed;
    std::random_device _dev;
    std::mt19937 _rng;

    SimplexNoise _noise;
    FastNoiseLite _fnoise;

public:
    noiseGenerator(int seed) : _noise(1, 1, 2, 0.5)
    {
        if (seed < 0)
        {
            _rng.seed(_dev());
            _seed = randint(0, 100000);
        }
        else
        {
            _rng.seed(seed);
            _seed = seed;
        }
        std::cout << "Seed: " << _seed << std::endl;
        _fnoise.SetSeed(_seed);
        _fnoise.SetFrequency(0.02);
        _fnoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    };
    ~noiseGenerator(){};
    int randint(int min, int max)
    {
        std::uniform_int_distribution<std::mt19937::result_type> tmp(min, max);
        return tmp(_rng);
    }
    std::vector<float> getSimplexNoiseArray(
        float scale,
        float z_idx,
        sf::Vector2i shift, sf::Vector2i t_window_size,
        int factor, float clamp_size,
        FastNoiseLite::FractalType fractalType)
    {
        _fnoise.SetFractalType(fractalType);
        scale = 1.0; // remove scale (needed by older lib)
        std::vector<float> noiseData(t_window_size.x * t_window_size.y);
        int index = 0;
        for (int y = shift.y; y < t_window_size.y + shift.y; y++)
        {
            for (int x = shift.x; x < t_window_size.x + shift.x; x++)
            {
                // auto tmp = _noise.noise((float)scale * x, (float)scale * y, z_idx);
                auto tmp = _fnoise.GetNoise((float)scale * x, (float)scale * y, z_idx);
                tmp *= clamp_size; // To cutoff the non used parts of the non uniform noise
                noiseData[index++] = std::clamp((tmp + 1) / 2, 0.f, 1.f) * factor;
            }
        }
        return noiseData;
    }
};
