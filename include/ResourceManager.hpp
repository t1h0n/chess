#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <map>
#include <string>

class ResourceManager
{
public:
    bool load_texture_from_file(const std::string& path, bool change_if_exists = false);
    /**
     * @warning throws if texture is inaccessible
     */
    const sf::Texture& get_texture(const std::string& path) const;

private:
    std::map<std::string, sf::Texture> m_textures;
};