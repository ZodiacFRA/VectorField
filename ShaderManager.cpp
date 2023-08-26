class ShaderManager
{
public:
    // Shader stuff
    sf::ContextSettings _contextSettings;
    const int BLUR_RADIUS_VALUES[4] = {250, 180, 125, 55};
    float blur_weight;
    sf::RenderStates shaderStates;
    // no blendmode! we make our own - assemble.frag
    sf::Shader luminescenceShader;
    sf::RenderTexture luminescenceRender;
    sf::Shader blurShader;
    std::array<sf::RenderTexture, 1> blurRenders;
    sf::Shader assembleShader;
    sf::RenderTexture assembleRender;
    sf::Shader multiplyShader;
    // Pixelate shader
    sf::Shader pixelateShader;

    ShaderManager(/* args */){};
    ~ShaderManager(){};

    sf::Sprite applyShaders(sf::RenderTexture &rawGameTexture)
    {
        // apply luminescence
        shaderStates.shader = &luminescenceShader;
        luminescenceRender.clear();
        luminescenceRender.draw(sf::Sprite(rawGameTexture.getTexture()), shaderStates);
        luminescenceRender.display();

        // apply two pass gaussian blur n times to simulate gaussian blur.
        shaderStates.shader = &blurShader;
        for (size_t i = 0; i < blurRenders.size(); ++i)
        {
            blurShader.setUniform("blur_radius", BLUR_RADIUS_VALUES[i]);

            blurRenders[i].clear();
            blurRenders[i].draw(sf::Sprite(luminescenceRender.getTexture()));
            blurRenders[i].display();

            // vertical blur
            blurShader.setUniform("blur_direction", sf::Glsl::Vec2(1.0, 0.0));
            blurRenders[i].draw(sf::Sprite(blurRenders[i].getTexture()), shaderStates);
            blurRenders[i].display();

            // horizontal blur
            blurShader.setUniform("blur_direction", sf::Glsl::Vec2(0.0, 1.0));
            blurRenders[i].draw(sf::Sprite(blurRenders[i].getTexture()), shaderStates);
            blurRenders[i].display();
        }

        // load blurRenders[0] into assembleRender so we can add the other blurs ontop of it
        shaderStates.shader = &multiplyShader;
        multiplyShader.setUniform("multiply", blur_weight);
        assembleRender.clear();
        assembleRender.draw(sf::Sprite(blurRenders[0].getTexture()), shaderStates);
        assembleRender.display();

        // adding the rest ontop creating a final blur
        shaderStates.shader = &assembleShader;
        assembleShader.setUniform("add_weight", blur_weight);
        for (size_t i = 1; i < blurRenders.size(); ++i)
        {
            assembleShader.setUniform("add_texture", blurRenders[i].getTexture());
            assembleRender.draw(sf::Sprite(assembleRender.getTexture()), shaderStates);
            assembleRender.display();
        }

        // final result; scene + blur
        assembleShader.setUniform("add_weight", 1.0f);
        assembleShader.setUniform("add_texture", assembleRender.getTexture());
        assembleRender.draw(sf::Sprite(rawGameTexture.getTexture()), shaderStates);
        assembleRender.display();

        return sf::Sprite(assembleRender.getTexture());
    }

    void loadShaders(sf::RenderTexture &rawGameTexture, sf::Vector2i px_window_size)
    {
        luminescenceShader.loadFromFile("./Shaders/luminescence.frag", sf::Shader::Fragment);
        luminescenceShader.setUniform("texture", sf::Shader::CurrentTexture);
        luminescenceShader.setUniform("threshold", 0.4f);

        blurShader.loadFromFile("./Shaders/boxblur.frag", sf::Shader::Fragment);
        blurShader.setUniform("texture", sf::Shader::CurrentTexture);
        blurShader.setUniform("texture_inverse", 1.0f / px_window_size.x);

        assembleShader.loadFromFile("./Shaders/assemble.frag", sf::Shader::Fragment);
        assembleShader.setUniform("texture", sf::Shader::CurrentTexture);

        multiplyShader.loadFromFile("./Shaders/multiply.frag", sf::Shader::Fragment);
        multiplyShader.setUniform("texture", sf::Shader::CurrentTexture);

        _contextSettings.antialiasingLevel = 2;

        rawGameTexture.create(px_window_size.x, px_window_size.y, _contextSettings);
        luminescenceRender.create(px_window_size.x, px_window_size.y, _contextSettings);
        assembleRender.create(px_window_size.x, px_window_size.y, _contextSettings);

        // Adding multiple boxblurs with different radii looks really nice! in this case 4 layers
        for (size_t i = 0; i < blurRenders.size(); ++i)
        {
            blurRenders[i].create(px_window_size.x, px_window_size.y, _contextSettings);
        }
        blur_weight = blurRenders.empty() ? 0.0 : 1.0 / blurRenders.size();

        // From https://coding-experiments.blogspot.com/2010/06/pixelation.html
        pixelateShader.loadFromFile("./Shaders/pixelateHex.vert", sf::Shader::Fragment);
        pixelateShader.setUniform("tex", sf::Shader::CurrentTexture);
        // pixelateShader.setUniform("resolution", 2048);
    }
};
