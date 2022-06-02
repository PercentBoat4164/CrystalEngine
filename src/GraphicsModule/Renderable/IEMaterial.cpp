#include "IEMaterial.hpp"

#include <memory>

#include "IERenderEngine.hpp"
#include "IEMesh.hpp"

IEMaterial::IEMaterial(IERenderEngine *engineLink) {
	create(engineLink);
}

void IEMaterial::setAPI(const IEAPI &API) {
	if (API.name == IE_RENDER_ENGINE_API_NAME_OPENGL) {
		_create = &IEMaterial::_openglCreate;
		_loadFromDiskToRAM = &IEMaterial::_openglLoadFromDiskToRAM;
		_loadFromRAMToVRAM = &IEMaterial::_openglLoadFromRAMToVRAM;
		_unloadFromVRAM = &IEMaterial::_openglUnloadFromVRAM;
		_unloadFromRAM = &IEMaterial::_openglUnloadFromRAM;
	} else if (API.name == IE_RENDER_ENGINE_API_NAME_VULKAN) {
		_create = &IEMaterial::_vulkanCreate;
		_loadFromDiskToRAM = &IEMaterial::_vulkanLoadFromDiskToRAM;
		_loadFromRAMToVRAM = &IEMaterial::_vulkanLoadFromRAMToVRAM;
		_unloadFromVRAM = &IEMaterial::_vulkanUnloadFromVRAM;
		_unloadFromRAM = &IEMaterial::_vulkanUnloadFromRAM;
	}
}


std::function<void(IEMaterial &)> IEMaterial::_create{nullptr};

void IEMaterial::create(IERenderEngine *engineLink) {
	linkedRenderEngine = engineLink;
	_create(*this);
}

void IEMaterial::_openglCreate() {

}

void IEMaterial::_vulkanCreate() {

}


std::function<void(IEMaterial &, const std::string &, const aiScene *, uint32_t)> IEMaterial::_loadFromDiskToRAM{nullptr};

void IEMaterial::loadFromDiskToRAM(const std::string &directory, const aiScene *scene, uint32_t index) {
	_loadFromDiskToRAM(*this, directory, scene, index);
}

void IEMaterial::_openglLoadFromDiskToRAM(const std::string &directory, const aiScene *scene, uint32_t index) {

}

void IEMaterial::_vulkanLoadFromDiskToRAM(const std::string &directory, const aiScene *scene, uint32_t index) {
	aiMaterial *material = scene->mMaterials[index];

	// find all textures in scene including embedded textures
	textureCount = 0;
	uint32_t thisCount;
	/**@todo Build a system that tracks duplicate textures and only keeps one copy in memory.*/
	for (int i = 0; i < textureTypes.size(); ++i) {
		thisCount = material->GetTextureCount(textureTypes[i].second);
		if (thisCount == 0) {
			textureTypes.erase(textureTypes.begin() + i--);
		}
		textureCount += thisCount;
	}
	linkedRenderEngine->textures.reserve(linkedRenderEngine->textures.size() + textureCount);

	aiString texturePath{};
	std::string data{};
	aiTexture *texture;
	uint32_t textureIndex{0};

	// load all textures despite embedded state
	for (std::pair<uint32_t *, aiTextureType> textureType: textureTypes) {
		while (texturePath.length == 0 && textureIndex < textureCount) {
			material->GetTexture(textureType.second, textureIndex++, &texturePath);
		}
		texture = const_cast<aiTexture *>(scene->GetEmbeddedTexture(texturePath.C_Str()));
		if (texture == nullptr || texture->mHeight != 0) {  // is the texture not an embedded texture?
			texture = new aiTexture;
			texture->mFilename = directory + "/" + texturePath.C_Str();
			texture->mHeight = 1;  // flag texture as not embedded
		}
		*textureType.first = linkedRenderEngine->textures.size();
		linkedRenderEngine->textures.push_back(std::make_shared<IETexture>(linkedRenderEngine, new IETexture::CreateInfo));
		linkedRenderEngine->textures[*textureType.first]->loadFromDiskToRAM(texture);
	}
}


std::function<void(IEMaterial &)> IEMaterial::_loadFromRAMToVRAM{nullptr};

void IEMaterial::loadFromRAMToVRAM() {
	_loadFromRAMToVRAM(*this);
}

void IEMaterial::_openglLoadFromRAMToVRAM() {

}

void IEMaterial::_vulkanLoadFromRAMToVRAM() {
	for (std::pair<uint32_t *, aiTextureType> textureType: textureTypes) {
		linkedRenderEngine->textures[*textureType.first]->loadFromRAMToVRAM();
	}
}


std::function<void(IEMaterial &)> IEMaterial::_unloadFromVRAM{nullptr};

void IEMaterial::unloadFromVRAM() {
	_unloadFromVRAM(*this);
}

void IEMaterial::_openglUnloadFromVRAM() {

}

void IEMaterial::_vulkanUnloadFromVRAM() {
	for (std::pair<uint32_t *, aiTextureType> textureType: textureTypes) {
//		linkedRenderEngine->textures[*textureType.first];
	}
}


/**@todo Keep two numbers of materials owning texture in IETexture. Decrease the appropriate number whenever a material is unloaded from RAM or VRAM. Delete texture from appropriate memory type when number is zero.*/

std::function<void(IEMaterial &)> IEMaterial::_unloadFromRAM{nullptr};

void IEMaterial::unloadFromRAM() {
	_unloadFromRAM(*this);
}

void IEMaterial::_openglUnloadFromRAM() {

}

void IEMaterial::_vulkanUnloadFromRAM() {
	for (std::pair<uint32_t *, aiTextureType> textureType: textureTypes) {
//		linkedRenderEngine->textures[*textureType.first];
	}
}

