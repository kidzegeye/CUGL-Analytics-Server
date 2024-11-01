//
//  CUObjLoader.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides a specific implementation of the Loader class to load
//  OBJ models. An OBJ asset is identified by its .obj, any associated .mtl
//  files and any associated textures. Hence there are reasons to load an OBJ
//  asset multiple times, though this is rare.
//
//  As with all of our loaders, this loader is designed to be attached to an
//  asset manager.  In addition, this class uses our standard shared-pointer
//  architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 7/3/24 (CUGL 3.0 reorganization)
//
#include <cugl/scene3/CUObjLoader.h>
#include <cugl/scene3/CUObjParser.h>
#include <cugl/core/assets/CUAssetManager.h>
#include <cugl/graphics/loaders/CUTextureLoader.h>
#include <cugl/core/util/CUFiletools.h>
#include <cugl/core/CUApplication.h>

using namespace cugl;
using namespace cugl::scene3;
using namespace cugl::graphics;

#pragma mark Support Functions
/** What the source name is if we do not know it */
#define UNKNOWN_SOURCE  "<unknown>"
/** The default min filter */
#define UNKNOWN_MINFLT  "linear"
/** The default mage filter */
#define UNKNOWN_MAGFLT  "linear"
/** The default wrap rule */
#define UNKNOWN_WRAP    "clamp"

/**
 * Returns the OpenGL enum for the given min filter name
 *
 * This function converts JSON directory entries into OpenGL values. If the
 * name is invalid, it returns GL_NEAREST.
 *
 * @param name  The JSON name for the min filter
 *
 * @return the OpenGL enum for the given min filter name
 */
static GLuint decodeMinFilter(const std::string name) {
    if (name == "nearest") {
        return GL_NEAREST;
    } else if (name == "linear") {
        return GL_LINEAR;
    } else if (name == "nearest-nearest") {
        return GL_NEAREST_MIPMAP_NEAREST;
    } else if (name == "linear-nearest") {
        return GL_LINEAR_MIPMAP_NEAREST;
    } else if (name == "nearest-linear") {
        return GL_NEAREST_MIPMAP_LINEAR;
    } else if (name == "linear-linear") {
        return GL_LINEAR_MIPMAP_LINEAR;
    }
    return GL_NEAREST;
}

/**
 * Returns the OpenGL enum for the given mag filter name
 *
 * This function converts JSON directory entries into OpenGL values. If the
 * name is invalid, it returns GL_LINEAR.
 *
 * @param name  The JSON name for the mag filter
 *
 * @return the OpenGL enum for the given mag filter name
 */
static GLuint decodeMagFilter(const std::string name) {
    if (name == "nearest") {
        return GL_NEAREST;
    }
    return GL_LINEAR;
}

/**
 * Returns the OpenGL enum for the given texture wrap name
 *
 * This function converts JSON directory entries into OpenGL values. If the
 * name is invalid, it returns GL_CLAMP_TO_EDGE.
 *
 * @param name  The JSON name for the texture wrap
 *
 * @return the OpenGL enum for the given texture wrap name
 */
static GLuint decodeWrap(const std::string name) {
    if (name == "clamp") {
        return GL_CLAMP_TO_EDGE;
    } else if (name == "repeat") {
        return GL_REPEAT;
    } else if (name == "mirrored") {
        return GL_MIRRORED_REPEAT;
    }
    return GL_CLAMP_TO_EDGE;
}

/**
 * Updates a texture info object with data from the given json
 *
 * If the json is nullptr, this function ensures that the texture path is
 * relative (not absolute) but makes no other changes.
 *
 * @param info  The texture information
 * @param json  The JSON with the texture data
 */
static void updateTextureInfo(const std::shared_ptr<TextureInfo>& info, JsonValue* json) {
    if (info == nullptr) {
        return;
    }
    
    if (json == nullptr) {
        std::string root = Application::get()->getAssetDirectory();
        info->path = info->path.substr(root.size());
        return;
    }
    
    if (json->isString()) {
        info->path = json->asString();
        return;
    }
    
    info->path = json->getString("file",info->path);
    info->minflt = decodeMinFilter(json->getString("minfilter",UNKNOWN_MINFLT));
    info->magflt = decodeMagFilter(json->getString("magfilter",UNKNOWN_MAGFLT));
    info->wrapS  = decodeWrap(json->getString("wrapS",UNKNOWN_WRAP));
    info->wrapT  = decodeWrap(json->getString("wrapT",UNKNOWN_WRAP));
    info->mipmaps = json->getBool("mipmaps",false);
}

#pragma mark -
#pragma mark Asset Loading
/**
 * Loads the portion of the texture that is safe to load outside the main thread.
 *
 * For best performance, all OBJ textures should be loaded in a previous
 * pass. However, if that is not the case, then this method can load any
 * additional textures that are needed. This method uses the preload pass
 * of {@link TextureLoader}.
 *
 * @param info      The texture information
 * @param loader    The texture loader for the parent asset manager
 */
void ObjLoader::preloadTexture(const std::shared_ptr<TextureInfo>& info,
                               const std::shared_ptr<TextureLoader>& loader) {
    if (info == nullptr || loader == nullptr) {
        return;
    }
    
    auto texture = loader->get(info->name);
    if (texture == nullptr) {
        SDL_Surface* surface = loader->preload(info->path);
        if (surface != nullptr) {
            _surfaces[info->name] = surface;
        }
    }
}

/**
 * Creates a texture from the given information.
 *
 * For best performance, all OBJ textures should be loaded in a previous
 * pass. However, if that is not the case, then this method can load any
 * additional textures that are needed. This method uses the materialize
 * pass of {@link TextureLoader}.
 *
 * @param info      The texture information
 * @param loader    The texture loader for the parent asset manager
 */
std::shared_ptr<Texture> ObjLoader::materializeTexture(const std::shared_ptr<TextureInfo>& info,
                                                       const std::shared_ptr<TextureLoader>& loader) {
    if (info == nullptr || loader == nullptr) {
        return nullptr;
    }

    auto result = loader->get(info->path);
    if (result == nullptr) {
        auto it = _surfaces.find(info->name);
        if (it != _surfaces.end()) {
            // This deallocates surface
            loader->materialize(info->name, it->second, nullptr);
            result = loader->get(info->name);
            _surfaces.erase(it);
            
            // Do the settings
            if (result != nullptr) {
                result->setMagFilter(info->magflt);
                result->setMinFilter(info->magflt);
                result->setWrapS(info->wrapS);
                result->setWrapT(info->wrapT);
                if (info->mipmaps) {
                    result->buildMipMaps();
                }
            }
        }
    }
    
    return result;
}


/**
 * Loads the portion of this asset that is safe to load outside the main thread.
 *
 * It is not safe to create an OpenGL buffer in a separate thread. However,
 * it is safe to create a {@link ObjModel}, so long as it does not have a
 * graphics buffer. Hence this method does the maximum amount of work that
 * can be done in asynchronous OBJ loading.
 *
 * If the MTL file has any associated textures, this method will create a
 * thread-safe instance using {@link TextureLoader#preload}. Those textures
 * will be materialized the same time the OBJ mesh is materialized.
 *
 * @param source    The pathname to the asset
 *
 * @return the incomplete OBJ model
 */
std::shared_ptr<ObjModel> ObjLoader::preload(const std::string key, const std::string source) {
    
    // Make sure we reference the asset directory
    bool absolute = cugl::filetool::is_absolute(source);
    CUAssertLog(!absolute, "This loader does not accept absolute paths for assets");

    std::string root = Application::get()->getAssetDirectory();
    std::string prefix = cugl::filetool::split_path(source).first;
    prefix.push_back(cugl::filetool::path_sep);
    std::string path = root+source;

    std::shared_ptr<ModelInfo> info = _parser->parseObj(source,path,false);
    std::shared_ptr<ObjModel> result = ObjModel::allocWithInfo(info,0);
    if (result == nullptr) {
        return nullptr;
    }
    
    // Make sure we have a texture loader
    auto texloader = std::dynamic_pointer_cast<TextureLoader>(_manager->access<Texture>());

    // Get the materials.
    for(auto it = info->libraries.begin(); it != info->libraries.end(); ++it) {
        auto lib = _parser->parseMtl(it->first,root+prefix+it->first);

        // Now get the textures
        if (lib != nullptr) {
            it->second = lib;
            for(auto jt = lib->materials.begin(); jt != lib->materials.end(); ++jt) {
                auto mtlinfo = jt->second;
                auto material = Material::allocWithInfo(mtlinfo,false);
                
                auto kt = _matast.find(key);
                if (kt == _matast.end()) {
                    _matast.try_emplace(key);
                }
                _matast[key][mtlinfo->name] = mtlinfo;
                auto mt = _materials.find(key);
                if (mt == _materials.end()) {
                    _materials.try_emplace(key);
                }
                _materials[key][mtlinfo->name] = material;

                updateTextureInfo(mtlinfo->map_Ka,nullptr);
                preloadTexture(mtlinfo->map_Ka,texloader);
                
                updateTextureInfo(mtlinfo->map_Kd,nullptr);
                preloadTexture(mtlinfo->map_Kd,texloader);
                
                updateTextureInfo(mtlinfo->map_Ks,nullptr);
                preloadTexture(mtlinfo->map_Ks,texloader);
                
                updateTextureInfo(mtlinfo->map_Kn,nullptr);
                preloadTexture(mtlinfo->map_Kn,texloader);
            }
        }
    }
    
    return result;
}

/**
 * Loads the portion of this asset that is safe to load outside the main thread.
 *
 * It is not safe to create an OpenGL buffer in a separate thread. However,
 * it is safe to create a {@link ObjModel}, so long as it does not have a
 * graphics buffer. Hence this method does the maximum amount of work that
 * can be done in asynchronous OBJ loading.
 *
 * If the MTL file has any associated textures, this method will create a
 * thread-safe instance using {@link TextureLoader#preload}. Those textures
 * will be materialized the same time the OBJ mesh is materialized.
 *
 * @param json  The JSON entry specifying the asset
 *
 * @return the incomplete OBJ model
 */
std::shared_ptr<ObjModel> ObjLoader::preload(const std::shared_ptr<JsonValue>& json) {
    if (json->isString()) {
        return preload(json->key(),json->asString());
    } else if (!json->has("file")) {
        return nullptr;
    }
    
    std::string key = json->key();
    std::string source = json->getString("file");
    
    // Make sure we reference the asset directory
    bool absolute = cugl::filetool::is_absolute(source);
    CUAssertLog(!absolute, "This loader does not accept absolute paths for assets");

    std::string root = Application::get()->getAssetDirectory();
    std::string prefix = cugl::filetool::split_path(source).first;
    prefix.push_back(cugl::filetool::path_sep);
    std::string path = root+source;

    std::shared_ptr<ModelInfo> info = _parser->parseObj(key,path,false);
    std::shared_ptr<ObjModel> result = ObjModel::allocWithInfo(info,0);
    if (result == nullptr) {
        return nullptr;
    }
    
    // Make sure we have a texture loader
    auto texloader = std::dynamic_pointer_cast<TextureLoader>(_manager->access<Texture>());

    JsonValue* mtljson = nullptr;
    if (json->has("mtls")) {
        mtljson = json->get("mtls").get();
    }
    
    // Get the materials.
    for(auto it = info->libraries.begin(); it !=info->libraries.end(); ++it) {
        JsonValue* child = nullptr;
        std::string path = root+prefix+it->first;
        if (mtljson != nullptr && mtljson->has(it->first)) {
            child = mtljson->get(it->first).get();
            path = child->getString("file",it->first);
        }
        
        auto lib = _parser->parseMtl(it->first,path);

        // Now get the textures
        if (lib != nullptr) {
            it->second = lib;
            if (child != nullptr && child->has("textures")) {
                child = child->get("textures").get();
            }
            
            if (texloader != nullptr) {
                for(auto jt = lib->materials.begin(); jt != lib->materials.end(); ++jt) {
                    auto mtlinfo = jt->second;
                    auto material = Material::allocWithInfo(mtlinfo,false);
    
                    auto kt = _matast.find(key);
                    if (kt == _matast.end()) {
                        _matast.try_emplace(key);
                    }
                    _matast[key][mtlinfo->name] = mtlinfo;
                    auto mt = _materials.find(key);
                    if (mt == _materials.end()) {
                        _materials.try_emplace(key);
                    }
                    _materials[key][mtlinfo->name] = material;
                    
                    JsonValue* textinfo;
                    textinfo = mtlinfo->map_Ka == nullptr ? nullptr : child->get(mtlinfo->map_Ka->name).get();
                    updateTextureInfo(mtlinfo->map_Ka,textinfo);
                    preloadTexture(mtlinfo->map_Ka,texloader);
                    
                    textinfo = mtlinfo->map_Kd == nullptr ? nullptr : child->get(mtlinfo->map_Kd->name).get();
                    updateTextureInfo(mtlinfo->map_Kd,textinfo);
                    preloadTexture(mtlinfo->map_Kd,texloader);
                    
                    textinfo = mtlinfo->map_Ks == nullptr ? nullptr : child->get(mtlinfo->map_Ks->name).get();
                    updateTextureInfo(mtlinfo->map_Ks,textinfo);
                    preloadTexture(mtlinfo->map_Ks,texloader);
                    
                    textinfo = mtlinfo->map_Kn == nullptr ? nullptr : child->get(mtlinfo->map_Kn->name).get();
                    updateTextureInfo(mtlinfo->map_Kn,textinfo);
                    preloadTexture(mtlinfo->map_Kn,texloader);
                }
            }
        }
    }
    
    return result;

}

/**
 * Creates an OpenGL buffer for the model, and assigns it the given key.
 *
 * This method finishes the asset loading started in {@link preload}. This
 * step is not safe to be done in a separate thread. Instead, it takes
 * place in the main CUGL thread via {@link Application#schedule}.
 *
 * Any additional textures loaded by the associated MTL will be materialized
 * at this time. This method will only work if all of the textures and
 * MTL files associated with this OBJ file are in the same directory as it.
 *
 * This method supports an optional callback function which reports whether
 * the asset was successfully materialized.
 *
 * @param key       The key to access the asset after loading
 * @param model     The model to materialize
 * @param callback  An optional callback for asynchronous loading
 */
void ObjLoader::materialize(const std::string key,
                            const std::shared_ptr<ObjModel>& model,
                            LoaderCallback callback) {
    if (model == nullptr) {
        return;
    }

    // Create the OpenGL buffers
    bool success = true;
    for(auto it = model->getMeshes().begin(); success && it != model->getMeshes().end(); ++it) {
        success = (*it)->createBuffer();
    }
    
    // Now we need to get the materials
    auto texloader = std::dynamic_pointer_cast<TextureLoader>(_manager->access<Texture>());
    auto matset = _materials.find(key);
    auto astset = _matast.find(key);
    
    for(auto it = model->getMeshes().begin(); success && it != model->getMeshes().end(); ++it) {
        auto mesh = *it;
        if (matset != _materials.end() && astset != _matast.end()) {
            auto material = matset->second.find(mesh->getMaterialName());
            auto matinfo  = astset->second.find(mesh->getMaterialName());
            if (material != matset->second.end()) {
                mesh->setMaterial(material->second);
                
                // See if any textures need to be materialized
                material->second->setAmbientMap(materializeTexture(matinfo->second->map_Ka, texloader));
                material->second->setDiffuseMap(materializeTexture(matinfo->second->map_Kd, texloader));
                material->second->setSpecularMap(materializeTexture(matinfo->second->map_Ks, texloader));
                material->second->setBumpMap(materializeTexture(matinfo->second->map_Kn, texloader));
            }
        }
    }

    if (callback != nullptr) {
        callback(key,success);
    }
    
    _assets[key] = model;
    _queue.erase(key);
}

/**
 * Internal method to support asset loading.
 *
 * This method supports either synchronous or asynchronous loading, as
 * specified by the given parameter. If the loading is asynchronous,
 * the user may specify an optional callback function.
 *
 * This method will split the loading across the {@link preload} and
 * {@link materialize} methods. This ensures that asynchronous loading
 * is safe.
 *
 * This method will only work if all of the textures and MTL files
 * associated with this OBJ file are in the same directory as it.
 *
 * @param key       The key to access the asset after loading
 * @param source    The pathname to the asset
 * @param callback  An optional callback for asynchronous loading
 * @param async     Whether the asset was loaded asynchronously
 *
 * @return true if the asset was successfully loaded
 */
bool ObjLoader::read(const std::string key, const std::string source,
                     LoaderCallback callback, bool async) {
    if (_assets.find(key) != _assets.end() || _queue.find(key) != _queue.end()) {
        return false;
    }

    bool success = true;
    if (_loader == nullptr || !async) {
        enqueue(key);
        std::shared_ptr<ModelInfo> info = _parser->parseObj(source, true);
        std::shared_ptr<ObjModel> model = ObjModel::allocWithInfo(info,3);
        success = (model != nullptr);
        if (success) {
            _assets[key] = model;
        }
        _queue.erase(key);
    } else {
        _loader->addTask([=](void) {
            this->enqueue(key);
            std::shared_ptr<ObjModel> model = this->preload(key,source);
            Application::get()->schedule([=](void){
                this->materialize(key,model,callback);
                return false;
            });
        });
    }
    
    return success;
}

/**
 * Internal method to support asset loading.
 *
 * This method supports either synchronous or asynchronous loading, as
 * specified by the given parameter. If the loading is asynchronous,
 * the user may specify an optional callback function.
 *
 * This method will split the loading across the {@link preload} and
 * {@link materialize} methods. This ensures that asynchronous loading
 * is safe.
 *
 * This version of read provides support for JSON directories. An OBJ
 * directory entry has the following values
 *
 *      "file":         The path to the OBJ file
 *      "mtls":         An object of key:value pairs defining MTL libraries
 *
 * The "mtls" entry is optional. For each MTL library, the key should match
 * the name of the MTL file referenced in the obj file. If there are any
 * missing MTL libraries (or the "mtls" entry is missing entirely), then
 * the loader will attempt to use the same directory as the OBJ file.
 *
 * An MTL entry is either a string (which is a reference to the path to the
 * MTL file) or a JSON object. Such a JSON object would have the following
 * values:
 *
 *      "file":         The path to the MTL file
 *      "textures":     An object of key:value pairs defining textures
 *
 * The "textures" entry is optional. For each texture, the key should match
 * the name of the texture in the MTL file. Any missing textures will
 * attempt to be loaded using the associated {@link TextureLoader}.
 *
 * The values for the texture entries should be strings or JSONs. If they
 * are string, they should be either be a key referencing a previously
 * loaded texture, or a path the texture file (the loader interprets it
 * as a path only if there is no key with that name). If it is a JSON, then
 * the JSON should follow the same rules as {@link TextureLoader}.
 *
 * @param json      The directory entry for the asset
 * @param callback  An optional callback for asynchronous loading
 * @param async     Whether the asset was loaded asynchronously
 *
 * @return true if the asset was successfully loaded
 */
bool ObjLoader::read(const std::shared_ptr<JsonValue>& json,
                     LoaderCallback callback, bool async) {
    std::string key = json->key();
    if (_assets.find(key) != _assets.end() || _queue.find(key) != _queue.end()) {
        return false;
    }

    bool success = true;
    if (_loader == nullptr || !async) {
        enqueue(key);
        std::shared_ptr<ModelInfo> info = _parser->parseJson(json);
        std::shared_ptr<ObjModel> model = ObjModel::allocWithInfo(info,3);
        success = (model != nullptr);
        if (success) {
            _assets[key] = model;
        }
        _queue.erase(key);
    } else {
        _loader->addTask([=](void) {
            this->enqueue(key);
            std::shared_ptr<ObjModel> model = this->preload(json);
            Application::get()->schedule([=](void){
                this->materialize(key,model,callback);
                return false;
            });
        });
    }
    
    return success;
}

/**
 * Unloads the asset for the given directory entry
 *
 * An asset may still be available if it is referenced by a smart pointer.
 * See the description of the specific implementation for how assets
 * are released.
 *
 * This method clears the internal buffers of any materials or textures
 * associated with this model.
 *
 * @param json      The directory entry for the asset
 *
 * @return true if the asset was successfully unloaded
 */
bool ObjLoader::purgeJson(const std::shared_ptr<JsonValue>& json) {
    std::string key = json->key();
    auto it = _assets.find(key);
    if (it == _assets.end()) {
        return false;
    }
    _assets.erase(it);
    
    auto jt = _materials.find(key);
    if (jt != _materials.end()) {
        _materials.erase(jt);
    }

    auto kt = _matast.find(key);
    if (kt != _matast.end()) {
        _matast.erase(kt);
    }

    return true;
}
