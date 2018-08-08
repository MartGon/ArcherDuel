#include "TextureRenderer.h"
#include "GameObject.h"
#include "RendererManager.h"

TextureRenderer::TextureRenderer() : Component()
{
	texture = Texture();
}

TextureRenderer::TextureRenderer(const char* path) : Component() 
{
	texture = Texture(path, RendererManager::renderer);
	tPath = std::string(path);
}

TextureRenderer::TextureRenderer(Texture texture) : Component()
{
	this->texture = texture;
	tPath = std::string(texture.path);
}

void TextureRenderer::update()
{
	int x = gameObject->transform.position.x;
	int y = gameObject->transform.position.y;

	texture.scale = gameObject->transform.scale;

	texture.render(x, y);
}