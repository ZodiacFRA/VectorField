#include <iostream>
#include <math.h>

#include <SFML/Graphics.hpp>

class Boat
{
public:
    int _px_tSize;

    sf::CircleShape _boatShape;
    sf::Vector2i _px_boatPosition;
    std::vector<sf::Vector2i> _px_boatPositionArray;
    sf::Vector2f _boatVector;
    int _boatDirection;
    int _isDead;

    Boat(int px_t_Size, sf::Vector2i pos) : _px_tSize(px_t_Size),
                                            _boatShape(_px_tSize / 2, 3),
                                            _px_boatPosition(pos),
                                            _boatVector(0, 0),
                                            _boatDirection(0),
                                            _isDead(-1)
    {
        _boatShape.setOrigin(px_t_Size / 2, px_t_Size / 2);
        _boatShape.setFillColor(sf::Color::White);
        // _boatShape.setOutlineColor(sampleColormap(0));
        // _boatShape.setOutlineThickness(2.f);
        _boatShape.setScale(1, 2);
    };
    ~Boat(){};

    void draw(sf::RenderWindow &window, sf::Vector2i &t_positionShift)
    {
        _boatShape.setPosition(_px_boatPosition.x, _px_boatPosition.y);
        _boatShape.setRotation(_boatDirection);
        window.draw(_boatShape);
    }

    void drawTrail(sf::RenderWindow &window, sf::Vector2i &t_positionShift)
    {
        if (_px_boatPositionArray.size() < 2)
        {
            return;
        }
        // auto px_display_shift = t_positionShift * _px_tSize;
        // auto px_display_shift = t_positionShift * 0;
        for (size_t i = 0; i < _px_boatPositionArray.size() - 1; i++)
        {
            float tmp = 255 - (i / 1.5f);
            if (tmp <= 0)
            {
                tmp = 0;
            }
            sf::Color c(tmp, tmp, tmp);
            auto p1 = _px_boatPositionArray[i];
            auto p2 = _px_boatPositionArray[i + 1];
            sf::Vertex line[] =
                {
                    sf::Vertex(static_cast<sf::Vector2f>(p1), c),
                    sf::Vertex(static_cast<sf::Vector2f>(p2), c)};

            window.draw(line, 2, sf::Lines);
        }
    }

    int update(sf::Vector2<int> t_window_size, std::vector<float> windDirection, std::vector<float> windForce)
    {

        float boat_speed = 5;
        sf::Vector2i boat_tile(_px_boatPosition.x / _px_tSize, _px_boatPosition.y / _px_tSize);
        size_t tile_idx = t_window_size.x * boat_tile.y + boat_tile.x;
        if (tile_idx >= windDirection.size())
        {
            _isDead++;
            auto lastPos = _px_boatPositionArray[0];
            _px_boatPositionArray.insert(_px_boatPositionArray.begin(), lastPos);
            _px_boatPositionArray.pop_back();
            if (_isDead >= _px_boatPositionArray.size())
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        float localWindForce = windForce[tile_idx];
        localWindForce *= boat_speed;
        float localWindDir = windDirection[tile_idx];
        _boatDirection = localWindDir;
        localWindDir = localWindDir * (M_PI / 180);

        sf::Vector2i movementVector(sin(localWindDir) * localWindForce, -cos(localWindDir) * localWindForce);
        _boatVector = (_boatVector / 1.1f + static_cast<sf::Vector2f>(movementVector) / 4.0f);
        _px_boatPosition += static_cast<sf::Vector2i>(_boatVector);

        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        // {
        //     _px_boatPosition.x -= boat_speed;
        // }
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        // {
        //     _px_boatPosition.x += boat_speed;
        // }
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        // {
        //     _px_boatPosition.y -= boat_speed;
        // }
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        // {
        //     _px_boatPosition.y += boat_speed;
        // }

        _px_boatPositionArray.insert(_px_boatPositionArray.begin(), _px_boatPosition);
        if (_px_boatPositionArray.size() > 1024)
        {
            _px_boatPositionArray.pop_back();
        }
        return 1;
    }
};
