//
//  CUScene2.h
//  Cornell University Game Library (CUGL)
//
//  This module provides support for the root node of a (2d) scene graph. After
//  much debate, we have decided to decouple this from the application class
//  (which is different than Cocos2d). However, scenes are still permitted
//  to contain controller code. They are in a sense a "subapplication".
//
//  This class uses our standard shared-pointer architecture.
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
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty. In no event will the authors be held liable for any damages
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
#ifndef __CU_SCENE_2_H__
#define __CU_SCENE_2_H__
#include <cugl/core/math/cu_math.h>
#include <cugl/core/CUScene.h>
#include <cugl/scene2/CUSceneNode2.h>
#include <cugl/graphics/CUSpriteBatch.h>

namespace cugl {

    /**
     * The classes to construct a 2-d scene graph.
     *
     * Even though this is an optional package, this is one of the core features
     * of CUGL. These classes provide basic UI support (including limited Figma)
     * support. Any 2-d game will make extensive use of these classes. And
     * even 3-d games may use these classes for the HUD overlay.
     */
    namespace scene2 {

/**
 * This class provides the root node of a two-dimensional scene graph.
 *
 * The Scene2 class is very similar to {@link scene2::SceneNode} and shares many
 * methods in common. The major differences are that it has no parent and it has
 * no position (so it cannot be transformed). Instead, the Scene2 is defined by
 * an attached {@link OrthographicCamera}.
 *
 * Rendering happens by traversing the the scene graph using an "Pre-Order" tree
 * traversal algorithm ( https://en.wikipedia.org/wiki/Tree_traversal#Pre-order ).
 * That means that parents are always draw before (and behind children). The
 * children of each sub tree are ordered sequentially.
 *
 * Scenes do support optional z-ordering. This is not a true depth value, as
 * depth filtering is incompatible with alpha compositing. However, it does
 * provide a way to dynamically reorder how siblings are composed.
 */
class Scene2 : public Scene {
#pragma mark Values
protected:
    /** The sprite batch for rendering this scene */
    std::shared_ptr<graphics::SpriteBatch> _batch;
    /** The array of internal nodes */
    std::vector<std::shared_ptr<scene2::SceneNode>> _children;
    /** The default tint for this scene */
    Color4 _color;
    
    /** The blending equation for this scene */
    GLenum _blendEquation;
    /** The source factor for the blend function */
    GLenum _srcFactor;
    /** The destination factor for the blend function */
    GLenum _dstFactor;
    
#pragma mark -
#pragma mark Constructors
public:
    /**
     * Creates a new degenerate Scene2 on the stack.
     *
     * The scene has no camera and must be initialized.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate an object on
     * the heap, use one of the static constructors instead.
     */
    Scene2();
    
    /**
     * Deletes this scene, disposing all resources
     */
    ~Scene2() { dispose(); }
    
    /**
     * Disposes all of the resources used by this scene.
     *
     * A disposed Scene2 can be safely reinitialized. Any children owned by this
     * scene will be released. They will be deleted if no other object owns them.
     */
    virtual void dispose() override;
    
    /**
     * Initializes a Scene to fill the entire screen.
     *
     * @return true if initialization was successful.
     */
    virtual bool init() override;
    
    /**
     * Initializes a Scene with the given size hint.
     *
     * Scenes are designed to fill the entire screen. If you want a scene that
     * is only part of the screen, that should be implemented with a specific
     * scene graph. However, the size of that screen can vary from device to
     * device. To make scene design easier, designs are typically locked to a
     * dimension: width or height.
     *
     * This is the purpose of the size hint. If either of the values of hint
     * are non-zero, then the scene will lock that dimension to that particular
     * size. If both are non-zero, it will choose its dimension according to the
     * device orientation. Landscape will be height, while portrait will pick
     * width. Devices with no orientation will always priortize height over
     * width.
     *
     * @param hint      The size hint
     *
     * @return true if initialization was successful.
     */
    virtual bool initWithHint(Size hint) override;
    
    /**
     * Initializes a Scene with the given size hint.
     *
     * Scenes are designed to fill the entire screen. If you want a scene that
     * is only part of the screen, that should be implemented with a specific
     * scene graph. However, the size of that screen can vary from device to
     * device. To make scene design easier, designs are typically locked to a
     * dimension: width or height.
     *
     * This is the purpose of the size hint. If either of the values of hint
     * are non-zero, then the scene will lock that dimension to that particular
     * size. If both are non-zero, it will choose its dimension according to the
     * device orientation. Landscape will be height, while portrait will pick
     * width. Devices with no orientation will always priortize height over
     * width.
     *
     * @param width     The width size hint
     * @param height    The height size hint
     *
     * @return true if initialization was successful.
     */
    virtual bool initWithHint(float width, float height) override {
        return initWithHint(Size(width,height));
    }
    
#pragma mark -
#pragma mark Static Constructors
    /**
     * Returns a newly allocated Scene to fill the entire screen.
     *
     * @return a newly allocated Scene to fill the entire screen..
     */
    static std::shared_ptr<Scene2> alloc() {
        std::shared_ptr<Scene2> result = std::make_shared<Scene2>();
        return (result->init() ? result : nullptr);
    }

    /**
     * Returns a newly allocated Scene with the given size hint.
     *
     * Scenes are designed to fill the entire screen. If you want a scene that
     * is only part of the screen, that should be implemented with a specific
     * scene graph. However, the size of that screen can vary from device to
     * device. To make scene design easier, designs are typically locked to a
     * dimension: width or height.
     *
     * This is the purpose of the size hint. If either of the values of hint
     * are non-zero, then the scene will lock that dimension to that particular
     * size. If both are non-zero, it will choose its dimension according to the
     * device orientation. Landscape will be height, while portrait will pick
     * width. Devices with no orientation will always priortize height over
     * width.
     *
     * @param hint      The size hint
     *
     * @return a newly allocated Scene with the given size hint.
     */
    static std::shared_ptr<Scene2> allocWithHint(const Size hint) {
        std::shared_ptr<Scene2> result = std::make_shared<Scene2>();
        return (result->initWithHint(hint) ? result : nullptr);
    }
    
    /**
     * Returns a newly allocated Scene with the given size hint.
     *
     * Scenes are designed to fill the entire screen. If you want a scene that
     * is only part of the screen, that should be implemented with a specific
     * scene graph. However, the size of that screen can vary from device to
     * device. To make scene design easier, designs are typically locked to a
     * dimension: width or height.
     *
     * This is the purpose of the size hint. If either of the values of hint
     * are non-zero, then the scene will lock that dimension to that particular
     * size. If both are non-zero, it will choose its dimension according to the
     * device orientation. Landscape will be height, while portrait will pick
     * width. Devices with no orientation will always priortize height over
     * width.
     *
     * @param width     The width size hint
     * @param height    The height size hint
     *
     * @return a newly allocated Scene with the given size hint.
     */
    static std::shared_ptr<Scene2> allocWithHint(float width, float height) {
        std::shared_ptr<Scene2> result = std::make_shared<Scene2>();
        return (result->initWithHint(width,height) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Attributes
    /**
     * Returns the tint color for this scene.
     *
     * During the render phase, this color will be applied to any child for
     * which hasRelativeColor() is true.
     *
     * @return the tint color for this scene.
     */
    Color4 getColor() { return _color; }
    
    /**
     * Sets the tint color for this scene.
     *
     * During the render phase, this color will be applied to any child for
     * which hasRelativeColor() is true.
     *
     * @param color  The tint color for this scene.
     */
    void setColor(Color4 color) { _color = color; }
    
    /**
     * Returns a string representation of this scene for debugging purposes.
     *
     * If verbose is true, the string will include class information. This
     * allows us to unambiguously identify the class.
     *
     * @param verbose Whether to include class information
     *
     * @return a string representation of this scene for debuggging purposes.
     */
    virtual std::string toString(bool verbose = false) const override;
    
#pragma mark -
#pragma mark Scene Graph
    /**
     * Returns the number of immediate children of this scene.
     *
     * @return The number of immediate children of this scene.
     */
    size_t getChildCount() const { return _children.size(); }
    
    /**
     * Returns the child at the given position.
     *
     * Children are not necessarily enumerated in the order that they are added.
     * For example, they may be resorted by their z-order. Hence you should
     * generally attempt to retrieve a child by tag or by name instead.
     *
     * @param pos   The child position.
     *
     * @return the child at the given position.
     */
    std::shared_ptr<scene2::SceneNode> getChild(unsigned int pos);
    
    /**
     * Returns the child at the given position.
     *
     * Children are not necessarily enumerated in the order that they are added.
     * Hence you should generally attempt to retrieve a child by tag or by name
     * instead.
     *
     * @param pos   The child position.
     *
     * @return the child at the given position.
     */
    const std::shared_ptr<scene2::SceneNode>& getChild(unsigned int pos) const;
    
    /**
     * Returns the child at the given position, typecast to a shared T pointer.
     *
     * This method is provided to simplify the polymorphism of a scene graph.
     * While all children are a subclass of type Node, you may want to access
     * them by their specific subclass. If the child is not an instance of
     * type T (or a subclass), this method returns nullptr.
     *
     * Children are not necessarily enumerated in the order that they are added.
     * Hence you should generally attempt to retrieve a child by tag or by name
     * instead.
     *
     * @param pos   The child position.
     *
     * @return the child at the given position, typecast to a shared T pointer.
     */
    template <typename T>
    inline std::shared_ptr<T> getChild(unsigned int pos) const {
        return std::dynamic_pointer_cast<T>(getChild(pos));
    }
    
    /**
     * Returns the (first) child with the given tag.
     *
     * If there is more than one child of the given tag, it returns the first
     * one that is found. Children are not necessarily enumerated in the order
     * that they are added. Hence it is very important that tags be unique.
     *
     * @param tag   An identifier to find the child node.
     *
     * @return the (first) child with the given tag.
     */
    std::shared_ptr<scene2::SceneNode> getChildByTag(unsigned int tag) const;
    
    /**
     * Returns the (first) child with the given tag, typecast to a shared T pointer.
     *
     * This method is provided to simplify the polymorphism of a scene graph.
     * While all children are a subclass of type Node, you may want to access
     * them by their specific subclass. If the child is not an instance of
     * type T (or a subclass), this method returns nullptr.
     *
     * If there is more than one child of the given tag, it returns the first
     * one that is found. Children are not necessarily enumerated in the order
     * that they are added. Hence it is very important that tags be unique.
     *
     * @param tag   An identifier to find the child node.
     *
     * @return the (first) child with the given tag, typecast to a shared T pointer.
     */
    template <typename T>
    inline std::shared_ptr<T> getChildByTag(unsigned int tag) const {
        return std::dynamic_pointer_cast<T>(getChildByTag(tag));
    }
    
    /**
     * Returns the (first) child with the given name.
     *
     * If there is more than one child of the given name, it returns the first
     * one that is found. Children are not necessarily enumerated in the order
     * that they are added. Hence it is very important that names be unique.
     *
     * @param name  An identifier to find the child node.
     *
     * @return the (first) child with the given name.
     */
    std::shared_ptr<scene2::SceneNode> getChildByName(const std::string name) const;
    
    /**
     * Returns the (first) child with the given name, typecast to a shared T pointer.
     *
     * This method is provided to simplify the polymorphism of a scene graph.
     * While all children are a subclass of type Node, you may want to access
     * them by their specific subclass. If the child is not an instance of
     * type T (or a subclass), this method returns nullptr.
     *
     * If there is more than one child of the given name, it returns the first
     * one that is found. Children are not necessarily enumerated in the order
     * that they are added. Hence it is very important that names be unique.
     *
     * @param name  An identifier to find the child node.
     *
     * @return the (first) child with the given name, typecast to a shared T pointer.
     */
    template <typename T>
    inline std::shared_ptr<T> getChildByName(const std::string name) const {
        return std::dynamic_pointer_cast<T>(getChildByName(name));
    }
    
    /**
     * Returns the list of the scene's immediate children.
     *
     * @return the list of the scene's immediate children.
     */
    std::vector<std::shared_ptr<scene2::SceneNode>> getChildren() { return _children; }
    
    /**
     * Returns the list of the scene's immediate children.
     *
     * @return the list of the scene's immediate children.
     */
    const std::vector<std::shared_ptr<scene2::SceneNode>>& getChildren() const { return _children; }
    
    /**
     * Adds a child to this scene.
     *
     * Children are not necessarily enumerated in the order that they are added.
     * Hence you should generally attempt to retrieve a child by tag or by name
     * instead.
     *
     * @param child A child node.
     */
    virtual void addChild(const std::shared_ptr<scene2::SceneNode>& child);
    
    /**
     * Adds a child to this scene with the given tag.
     *
     * Children are not necessarily enumerated in the order that they are added.
     * Hence you should generally attempt to retrieve a child by tag or by name
     * instead.
     *
     * @param child A child node.
     * @param tag   An integer to identify the node easily.
     */
    void addChildWithTag(const std::shared_ptr<scene2::SceneNode>& child, unsigned int tag) {
        addChild(child);
        child->setTag(tag);
    }
    
    /**
     * Adds a child to this scene with the given name.
     *
     * Children are not necessarily enumerated in the order that they are added.
     * Hence you should generally attempt to retrieve a child by tag or by name
     * instead.
     *
     * @param child A child node.
     * @param name  A string to identify the node.
     */
    void addChildWithName(const std::shared_ptr<scene2::SceneNode>& child, 
                          const std::string name) {
        addChild(child);
        child->setName(name);
    }
    
    /**
     * Swaps the current child child1 with the new child child2.
     *
     * If inherit is true, the children of child1 are assigned to child2 after
     * the swap; this value is false by default. The purpose of this value is
     * to allow transitions in the scene graph.
     *
     * This method is undefined if child1 is not a child of this scene.
     *
     * @param child1    The current child of this node
     * @param child2    The child to swap it with.
     * @param inherit   Whether the new child should inherit the children of child1.
     */
    void swapChild(const std::shared_ptr<scene2::SceneNode>& child1,
                   const std::shared_ptr<scene2::SceneNode>& child2, bool inherit=false);
    
    /**
     * Removes the child at the given position from this Scene.
     *
     * Removing a child alters the position of every child after it. Hence
     * it is unsafe to cache child positions.
     *
     * @param pos   The position of the child node which will be removed.
     */
    virtual void removeChild(unsigned int pos);
    
    /**
     * Removes a child from this Scene.
     *
     * Removing a child alters the position of every child after it. Hence
     * it is unsafe to cache child positions.
     *
     * If the child is not in this node, nothing happens.
     *
     * @param child The child node which will be removed.
     */
    void removeChild(const std::shared_ptr<scene2::SceneNode>& child);
    
    /**
     * Removes a child from the Scene by tag value.
     *
     * If there is more than one child of the given tag, it removes the first
     * one that is found. Children are not necessarily enumerated in the order
     * that they are added. Hence it is very important that tags be unique.
     *
     * @param tag   An integer to identify the node easily.
     */
    void removeChildByTag(unsigned int tag);
    
    /**
     * Removes a child from the Scene by name.
     *
     * If there is more than one child of the given name, it removes the first
     * one that is found. Children are not necessarily enumerated in the order
     * that they are added. Hence it is very important that names be unique.
     *
     * @param name  A string to identify the node.
     */
    void removeChildByName(const std::string name);
    
    /**
     * Removes all children from this Node.
     */
    virtual void removeAllChildren();
    
#pragma mark -
#pragma mark Scene Rendering
    /**
     * Returns the sprite batch for rendering this scene.
     *
     * Scene2 objects are rendered with a sprite batch by default. In particular
     * the method {@link #render} traverses the scene graph in a pre-order
     * traversal, calling {@link SceneNode#render} on each node (though this
     * behavior can be overridden.
     *
     * As sprite batches are fairly heavy-weight pipelines, we do not construct
     * a sprite batch for each scene. Instead a sprite batch has to be assigned
     * to the scene. If not sprite batch is assigned, nothing is drawn.
     *
     * @return the sprite batch for rendering this scene.
     */
    std::shared_ptr<graphics::SpriteBatch> getSpriteBatch() const { return _batch; }

    /**
     * Sets the sprite batch for rendering this scene.
     *
     * Scene2 objects are rendered with a sprite batch by default. In particular
     * the method {@link #render} traverses the scene graph in a pre-order
     * traversal, calling {@link SceneNode#render} on each node (though this
     * behavior can be overridden.
     *
     * As sprite batches are fairly heavy-weight pipelines, we do not construct
     * a sprite batch for each scene. Instead a sprite batch has to be assigned
     * to the scene. If not sprite batch is assigned, nothing is drawn.
     *
     * @param batch The sprite batch for rendering this scene.
     */
    void setSpriteBatch(const std::shared_ptr<graphics::SpriteBatch>& batch) {
        _batch = batch;
    }
    
    /**
     * Draws all of the children in this scene with the given SpriteBatch.
     *
     * This method with draw using {@link #getSpriteBatch}. If not sprite batch
     * has been assigned, nothing will be drawn.
     *
     * Rendering happens by traversing the the scene graph using an "Pre-Order"
     * tree traversal algorithm ( https://en.wikipedia.org/wiki/Tree_traversal#Pre-order ).
     * That means that parents are always draw before (and behind children).
     * To override this draw order, you should place an {@link OrderedNode}
     * in the scene graph to specify an alternative order.
     */
    virtual void render() override;
    
private:
#pragma mark -
#pragma mark Internal Helpers
    // Tightly couple with Node
    friend class SceneNode;
};

    }
}
#endif /* __CU_SCENE_2_H__ */
