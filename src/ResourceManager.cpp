#include <ResourceManager.hpp>

bool ResourceManager::load_texture_from_file(const std::string& path, bool change_if_exists)
{
    if (m_textures.count(path) && !change_if_exists)
    {
        return false;
    }
    sf::Texture loaded_texture;
    if (loaded_texture.loadFromFile(path))
    {
        loaded_texture.setSmooth(true);  // TODO: add options to make it possible to modify
        // texture
        loaded_texture.generateMipmap();
        m_textures[path] = std::move(loaded_texture);
        return true;
    }
    return false;
}

const sf::Texture& ResourceManager::get_texture(const std::string& texture_name) const
{
    return m_textures.at(texture_name);
}