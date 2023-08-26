#include <iostream>
#include <list>

#include <SFML/Graphics.hpp>

#include "noiseGenerator.cpp"
#include "tinycolormap.hpp"

#include "Boat.cpp"
#include "ShaderManager.cpp"

class App
{
private:
    sf::RenderWindow _window;
    // Create a texture and a sprite for the shader
    sf::RenderTexture _rawGameTexture;
    ShaderManager _shaderManager;

    noiseGenerator _noiseGenerator;

    int _fps;
    sf::Vector2<int> _t_window_size;
    int _px_tSize;
    sf::Vector2<int> _px_window_size;

    sf::Time _msFrameDuration;
    sf::Clock _clock;
    int _tick;

    tinycolormap::ColormapType _colorMap;
    sf::CircleShape _triangleShape;

    float _noiseScale;
    float _noiseSpeed;
    std::vector<float> _windDirection;
    std::vector<float> _windForce;

    sf::Vector2i _t_positionShift;
    std::vector<Boat *> _boats;

public:
    // i3 (63tx71t)
    App() : _noiseGenerator(-1), _fps(30), _t_window_size(31, 35), _px_tSize(30),
            _px_window_size(_t_window_size.x * _px_tSize, _t_window_size.y * _px_tSize),
            _msFrameDuration(sf::milliseconds(1000 / _fps)), _tick(0),
            _triangleShape(_px_tSize / 2, 3), _noiseScale(0.03), _noiseSpeed(1 / 8.0),
            _t_positionShift(0, 0)
    {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;
        _window.create(sf::VideoMode(_px_window_size.x, _px_window_size.y), "Windy", sf::Style::Default, settings);
        _window.setMouseCursorVisible(false); // hide the cursor

        // Shader stuff
        // _tex.create(_px_window_size.x, _px_window_size.y);
        // _spr = sf::Sprite(_tex);
        if (!sf::Shader::isAvailable())
        {
            std::cout << "Shader not available\n";
        }
        _shaderManager.loadShaders(_rawGameTexture, _px_window_size);

        // Drawing stuff
        // _colorMap = tinycolormap::ColormapType(_noiseGenerator.randint(0, 14));
        _colorMap = tinycolormap::ColormapType::Viridis;
        _triangleShape.setOrigin(_px_tSize / 2, _px_tSize / 2);

        // Init game state
        updateWind();
        for (size_t i = 0; i < 0; i++)
        {
            addBoat();
        }
    }
    ~App(){};

    void addBoat()
    {
        sf::Vector2i px_pos(
            _noiseGenerator.randint(3, _t_window_size.x - 6),
            _noiseGenerator.randint(3, _t_window_size.y - 6));
        px_pos *= _px_tSize;
        _boats.push_back(new Boat(_px_tSize, px_pos));
    }

    int run()
    {
        while (handle_loop())
        {
            // std::cout << "----- tour " << _tick << std::endl;
            // if (_boats.size() == 1)
            // {
            //     updatePositionShift();
            // }
            updateWind();
            std::vector<int> to_delete_idxs;
            for (size_t i = 0; i < _boats.size(); i++)
            {
                if (_boats[i]->update(_t_window_size, _windDirection, _windForce) == 0)
                {
                    to_delete_idxs.push_back(i);
                }
            }

            draw();
            for (auto &idx : to_delete_idxs)
            {
                delete _boats[idx];
                _boats.erase(_boats.begin() + idx);
                addBoat();
            }
        }
        return 1;
    }

    void updateWind()
    {
        _windDirection = _noiseGenerator.getSimplexNoiseArray(
            _noiseScale, _tick * _noiseSpeed, _t_positionShift, _t_window_size, 360, 2.1, FastNoiseLite::FractalType_Ridged);
        _windForce = _noiseGenerator.getSimplexNoiseArray(
            _noiseScale, _tick * _noiseSpeed + 1000, _t_positionShift, _t_window_size, 1, 1.2, FastNoiseLite::FractalType_FBm);
    }

    void updatePositionShift()
    {
        sf::Vector2i t_move_zone_size = _t_window_size / 7;
        sf::Vector2i boat_tile = _boats[0]->_px_boatPosition / _px_tSize;
        // std::cout << _boats[0]->_px_boatPosition.x << " | " << _boats[0]->_px_boatPosition.y << std::endl;
        // std::cout << boat_tile.x << " | " << boat_tile.y << std::endl;
        if (boat_tile.x < t_move_zone_size.x)
        {
            int delta = t_move_zone_size.x - boat_tile.x;
            _t_positionShift.x -= delta;
            _boats[0]->_px_boatPosition.x += delta * _px_tSize;
        }
        else if (boat_tile.x > _t_window_size.x - t_move_zone_size.x)
        {
            int delta = boat_tile.x - (_t_window_size.x - t_move_zone_size.x);
            _t_positionShift.x += delta;
            _boats[0]->_px_boatPosition.x -= delta * _px_tSize;
        }
        if (boat_tile.y < t_move_zone_size.y)
        {
            int delta = t_move_zone_size.y - boat_tile.y;
            _t_positionShift.y -= delta;
            _boats[0]->_px_boatPosition.y += delta * _px_tSize;
        }
        else if (boat_tile.y > _t_window_size.y - t_move_zone_size.y)
        {
            int delta = boat_tile.y - (_t_window_size.y - t_move_zone_size.y);
            _t_positionShift.y += delta;
            _boats[0]->_px_boatPosition.y -= delta * _px_tSize;
        }
        // auto px_absolute_pos = _boats[0]->_px_boatPosition + _t_positionShift * _px_tSize;
        // _boats[0]->_px_boatPositionArray.insert(_boats[0]->_px_boatPositionArray.begin(), px_absolute_pos);
        // if (_boats[0]->_px_boatPositionArray.size() > 10000)
        // {
        //     _boats[0]->_px_boatPositionArray.pop_back();
        // }
    }

    void draw()
    {
        // First render the pixels to the _rawGameTexture, not to the _window
        // this way, we can apply the shaders at the end
        _rawGameTexture.clear();
        for (Boat *b : _boats)
        {
            b->drawTrail(_window, _t_positionShift);
        }
        for (int y = 0; y < _t_window_size.y; y++)
        {
            for (int x = 0; x < _t_window_size.x; x++)
            {
                _triangleShape.setPosition(
                    x * _px_tSize + _px_tSize / 2,
                    y * _px_tSize + _px_tSize / 2);
                float windForce = _windForce[_t_window_size.x * y + x];
                float triScale = 1.5;
                _triangleShape.setScale(windForce / 2 * triScale, windForce * triScale);

                float windDir = _windDirection[_t_window_size.x * y + x];
                _triangleShape.setRotation(windDir);
                _triangleShape.setFillColor(sampleColormap(windDir / 360.0));

                _rawGameTexture.draw(_triangleShape);
            }
        }
        // Update the texture with the draws
        _rawGameTexture.display();
        // Create a sprite with the _rawGameTexture
        // (in order to able to apply the shaders)
        sf::Sprite finalSprite;
        if (true)
        {
            finalSprite = _shaderManager.applyShaders(_rawGameTexture);
        }
        else
        {
            finalSprite = sf::Sprite(_rawGameTexture.getTexture());
        }
        // Now do the final draw on the window, with the vertex shaders if needed
        _window.clear(sf::Color::Black);
        if (true)
        {
            _window.draw(finalSprite, &_shaderManager.pixelateShader);
        }
        else
        {
            _window.draw(finalSprite);
        }
        for (Boat *b : _boats)
        {
            b->draw(_window, _t_positionShift);
        }
        _window.display();
    }

    sf::Color sampleColormap(float color_idx)
    {
        // https://github.com/yuki-koyama/tinycolormap/blob/master/include/tinycolormap.hpp
        const tinycolormap::Color color = tinycolormap::GetColor(color_idx, _colorMap);
        int r = 255 * color.r();
        int g = 255 * color.g();
        int b = 255 * color.b();
        sf::Color sfColor(r, g, b);
        return sfColor;
    }

    int handle_loop()
    {
        _window.display();
        if (!_window.isOpen())
        {
            return 0;
        }
        sf::Event event;
        while (_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                _window.close();
        }
        // clock and sleep here
        sf::Time elapsed = _clock.restart();
        sf::Time delayTime = _msFrameDuration - elapsed;
        // std::cout << "target len " << _msFrameDuration.asMilliseconds() << " elapsed " << elapsed.asMilliseconds() << " sleeping: " << delayTime.asMilliseconds() << std::endl;

        if (delayTime.asMilliseconds() > 0)
        {
            sf::sleep(delayTime);
        }
        // else
        // {
        //     std::cout << "Lagging " << -delayTime.asMilliseconds() << " ms behind" << std::endl;
        // }
        _window.clear();
        _tick++;
        return 1;
    }
};