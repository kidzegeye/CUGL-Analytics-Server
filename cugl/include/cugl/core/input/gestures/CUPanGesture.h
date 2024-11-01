//
//  CUPanGesture.h
//  Cornell University Game Library (CUGL)
//
//  This class provides basic support for multifinger pan gestures. Unlike the
//  CoreGesture device, this will always detect a pan, even when other gestures
//  are active. Futhermore, it is not limited to two-finger pans. It can detect
//  detect any pan of two or more fingers.
//
//  This class is a singleton and should never be allocated directly.  It
//  should only be accessed via the Input dispatcher.
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
#ifndef __CU_PAN_INPUT_H__
#define __CU_PAN_INPUT_H__
#include <cugl/core/input/CUInput.h>
#include <cugl/core/math/CUVec2.h>

namespace cugl {

#pragma mark -
#pragma mark PanEvent

/**
 * This simple class is a struct to hold a pan event.
 *
 * A pan event is a gesture with duration. Hence this event stores
 * information about the start of the event, as well as the current
 * status of the event.
 */
class PanEvent {
public:
    /** The starting time of the gesture */
    Timestamp start;
    /** The current time of the gesture */
    Timestamp now;
    /** The initial position of the pan */
    Vec2 origPosition;
    /** The current position of the pan */
    Vec2 currPosition;
    /** The change since the last generated event */
    Vec2 delta;
    /** The number of fingers involved in this pan */
    Uint32 fingers;
    
    /**
     * Constructs a new pan event with the default values
     */
    PanEvent() : fingers(0) {}
    
    /**
     * Constructs a new pan event with the given values
     *
     * @param start     The initial timestamp
     * @param position  The starting location
     * @param fingers   The number of fingers uses
     */
    PanEvent(const Timestamp& start, const Vec2 position, Uint32 fingers) {
        this->start = start;
        this->now = start;
        origPosition = position;
        currPosition = position;
        this->fingers = fingers;
    }
    
    /**
     * Constructs a new pan event that is a copy of the given one
     *
     * @param event     The event to copy
     */
    PanEvent(const PanEvent& event) {
        start = event.start;
        now  = event.now;
        origPosition = event.origPosition;
        currPosition = event.origPosition;
        delta = event.delta;
        fingers = event.fingers;
    }
    
    /**
     * Clears the contents of this pan event
     */
    void clear() {
        origPosition.setZero();
        currPosition.setZero();
        delta.setZero();
        fingers = 0;
    }
};


#pragma mark -
#pragma mark PanGesture

/**
 * This class is an input device recognizing pan events.
 *
 * A pan is a gesture where two or more fingers are moved together across the
 * touch device. If the pan is done very quickly, it is often called a swipe.
 * Most UX designers assign different meanings to pans, depending on the
 * number of fingers involved. For example, on Apple devices, two fingers
 * is used to scroll while three moves to a new page.
 *
 * This input device is a touch device that supports multitouch gestures.
 * This is often the screen itself, but this is not always guaranteed.  For
 * example, the trackpad on MacBooks support pans.  For that reason, we
 * cannot guarantee that the touches scale with the display.  Instead, all
 * gesture information is normalized, with the top left corner of the touch
 * device being (0,0) and the lower right being (1,1).
 *
 * If you know that the touch device is the screen, and would like to measure
 * the pan in screen coordinates, you should set the screen attribute to
 * true with {@link setTouchScreen}.  In this case, the pan distance will
 * be scaled to the display.  In those cases where the device is known to be 
 * the screen (Android, iOS devices), this value starts out as true.
 *
 * As with most devices, we provide support for both listeners and polling
 * the mouse. Polling the device will query the touch screen at the start of
 * the frame, but it may miss those case in there are multiple pan updates
 * in a single animation frame.
 *
 * Listeners are guaranteed to catch all changes in the pan position, as long as
 * they are detected by the OS.  However, listeners are not called as soon as
 * the event happens.  Instead, the events are queued and processed at the start
 * of the animation frame, before the method {@link Application#update(float)}
 * is called.
 */
class PanGesture : public InputDevice {
public:
#pragma mark Listener
    /**
     * @typedef Listener
     *
     * This type represents a listener for a pan in the {@link PanGesture} class.
     *
     * In CUGL, listeners are implemented as a set of callback functions, not as
     * objects. This allows each listener to implement as much or as little
     * functionality as it wants. A listener is identified by a key which should
     * be a globally unique unsigned int.
     *
     * While pan listeners do not traditionally require focus like a keyboard does,
     * we have included that functionality. While only one listener can have focus
     * at a time, all listeners will receive input from the PanInput device.
     *
     * The function type is equivalent to
     *
     *      std::function<void(const PanEvent& event, bool focus)>
     *
     * @param event     The touch event for this pan event
     * @param focus     Whether the listener currently has focus
     */
    typedef std::function<void(const PanEvent& event, bool focus)> Listener;
    
protected:
    /** Whether or not this input device is a touch screen */
    bool  _screen;
    /** Whether or not there is an active pan being processed */
    bool  _active;
    /** Whether or not the pan is sensitive to the number of fingers */
    bool  _fingery;
    /** The distance threshold for generating a pan event */
    float _threshold;
    /** The pan event data (stored whether or not there is an event) */
    PanEvent _event;
    
    /** The current finger positions */
    std::unordered_map<Sint64, Vec2> _fingers;
    /** The number of fingers updated this pass */
    size_t _updated;
    
    /** The set of listeners called whenever a pan begins */
    std::unordered_map<Uint32, Listener> _beginListeners;
    /** The set of listeners called whenever a pan ends */
    std::unordered_map<Uint32, Listener> _finishListeners;
    /** The set of listeners called whenever a pan is moved */
    std::unordered_map<Uint32, Listener> _motionListeners;
    
    
#pragma mark Constructor
    /**
     * Creates and initializes a new pan input device.
     *
     * WARNING: Never allocate a pan input device directly.  Always use the
     * {@link Input#activate()} method instead.
     */
    PanGesture();
    
    /**
     * Deletes this input device, disposing of all resources
     */
    virtual ~PanGesture() {}
    
    /**
     * Initializes this device, acquiring any necessary resources
     *
     * @return true if initialization was successful
     */
    bool init() { return initWithName("Pan Gesture"); }
    
    /**
     * Unintializes this device, returning it to its default state
     *
     * An uninitialized device may not work without reinitialization.
     */
    virtual void dispose() override;
    
    
#pragma mark Device Attributes
public:
    /**
     * Returns true if this device is a touch screen.
     *
     * This device is not guaranteed to be a touch screen.  For example, the
     * trackpad on MacBooks support pans. We do try to make our best guess
     * about whether or not a device is a touch screen, but on some devices
     * this may need to be set manually.
     *
     * If this value is true, all pan information will scale with the display.
     * Otherwise, the pan will be normalized to a unit square, where the
     * top left corner of the touch device is (0,0) and the lower right is
     * (1,1). You may want to set this value to false for true cross-platform
     * gesture support.
     *
     * @return true if this device is a touch screen.
     */
    bool isTouchScreen() const { return _screen; }
    
    /**
     * Sets whether this device is a touch screen.
     *
     * This device is not guaranteed to be a touch screen.  For example, the
     * trackpad on MacBooks support pans. We do try to make our best guess
     * about whether or not a device is a touch screen, but on some devices
     * this may need to be set manually.
     *
     * If this value is true, all pan information will scale with the display.
     * Otherwise, the pan will be normalized to a unit square, where the
     * top left corner of the touch device is (0,0) and the lower right is
     * (1,1). You may want to set this value to false for true cross-platform
     * gesture support.
     *
     * @param flag  Whether this device is a touch screen.
     */
    void setTouchScreen(bool flag);
    
    /**
     * Returns true if changing the number of fingers interrupts the pan
     *
     * This only affects the listener interface.  When using the polling 
     * interface, it is up to the application to determine when a pan starts
     * and stops.
     *
     * @return true if changing the number of fingers interrupts the pan
     */
    bool isFingerSensitive() const { return _fingery; }

    /**
     * Sets whether changing the number of fingers interrupts the pan
     *
     * This only affects the listener interface.  When using the polling
     * interface, it is up to the application to determine when a pan starts
     * and stops.
     *
     * @param flag  Whether changing the number of fingers interrupts the pan
     */
    void setFingerSensitive(bool flag) { _fingery = flag; }

    
#pragma mark Data Polling
    /**
     * Returns true if the device is in the middle of an active pan.
     *
     * If the device is not an in active pan, all other polling methods
     * will return the default value.
     *
     * @return true if the device is in the middle of an active pan.
     */
    bool isActive() const { return _active; }
    
    /**
     * Returns the change in the pan position since the last animation frame.
     *
     * @return the change in the pan position since the last animation frame.
     */
    const Vec2 getDelta() const { return _active ? _event.delta : Vec2::ZERO; }
    
    /**
     * Returns the cumulative pan vector since the gesture began.
     *
     * @return the cumulative pan vector since the gesture began.
     */
    const Vec2 getPan() const;
    
    /**
     * Returns the current position of the pan.
     *
     * This position is computed as the centroid of all of the fingers involved
     * in the pan.
     *
     * @return the current position of the pan.
     */
    const Vec2 getPosition() const { return _active ? _event.currPosition : Vec2::ZERO; }
    
    /**
     * Returns the number of fingers involved in the pan gesture.
     *
     * This value may change over the course of the pan gesture. However,
     * there are always guaranteed to be at least two fingers.
     *
     * @return the number of fingers involved in the pan gesture.
     */
    int getFingers() const { return _active ? _event.fingers : 0; }

    
#pragma mark Listeners
    /**
     * Requests focus for the given identifier
     *
     * Only a listener can have focus.  This method returns false if key
     * does not refer to an active listener
     *
     * @param key   The identifier for the focus object
     *
     * @return false if key does not refer to an active listener
     */
    virtual bool requestFocus(Uint32 key) override;
    
    /**
     * Returns true if key represents a listener object
     *
     * An object is a listener if it is a listener for any of the three actions:
     * pan begin, pan end, or pan change.
     *
     * @param key   The identifier for the listener
     *
     * @return true if key represents a listener object
     */
    bool isListener(Uint32 key) const;
    
    /**
     * Returns the pan begin listener for the given object key
     *
     * This listener is invoked when pan crosses the distance threshold.
     *
     * If there is no listener for the given key, it returns nullptr.
     *
     * @param key   The identifier for the listener
     *
     * @return the pan begin listener for the given object key
     */
    const Listener getBeginListener(Uint32 key) const;
    
    /**
     * Returns the pan end listener for the given object key
     *
     * This listener is invoked when all (but one) fingers in an active pan
     * are released.
     *
     * If there is no listener for the given key, it returns nullptr.
     *
     * @param key   The identifier for the listener
     *
     * @return the pan end listener for the given object key
     */
    const Listener getEndListener(Uint32 key) const;
    
    /**
     * Returns the pan change listener for the given object key
     *
     * This listener is invoked when the pan position changes.
     *
     * @param key   The identifier for the listener
     *
     * @return the pan change listener for the given object key
     */
    const Listener getChangeListener(Uint32 key) const;
    
    /**
     * Adds a pan begin listener for the given object key
     *
     * There can only be one listener for a given key.  If there is already
     * a listener for the key, the method will fail and return false.  You
     * must remove a listener before adding a new one for the same key.
     *
     * This listener is invoked when pan crosses the distance threshold.
     *
     * @param key       The identifier for the listener
     * @param listener  The listener to add
     *
     * @return true if the listener was succesfully added
     */
    bool addBeginListener(Uint32 key, Listener listener);
    
    /**
     * Adds a pan end listener for the given object key
     *
     * There can only be one listener for a given key.  If there is already
     * a listener for the key, the method will fail and return false.  You
     * must remove a listener before adding a new one for the same key.
     *
     * This listener is invoked when all (but one) fingers in an active pan
     * are released.
     *
     * @param key       The identifier for the listener
     * @param listener  The listener to add
     *
     * @return true if the listener was succesfully added
     */
    bool addEndListener(Uint32 key, Listener listener);
    
    /**
     * Adds a pan change listener for the given object key
     *
     * There can only be one listener for a given key.  If there is already
     * a listener for the key, the method will fail and return false.  You
     * must remove a listener before adding a new one for the same key.
     *
     * This listener is invoked when the pan position changes.
     *
     * @param key       The identifier for the listener
     * @param listener  The listener to add
     *
     * @return true if the listener was succesfully added
     */
    bool addChangeListener(Uint32 key, Listener listener);
    
    /**
     * Removes the pan begin listener for the given object key
     *
     * If there is no active listener for the given key, this method fails
     * and returns false.
     *
     * This listener is invoked when pan crosses the distance threshold.
     *
     * @param key   The identifier for the listener
     *
     * @return true if the listener was succesfully removed
     */
    bool removeBeginListener(Uint32 key);
    
    /**
     * Removes the pan end listener for the given object key
     *
     * If there is no active listener for the given key, this method fails
     * and returns false.
     *
     * This listener is invoked when all (but one) fingers in an active pan
     * are released.
     *
     * @param key   The identifier for the listener
     *
     * @return true if the listener was succesfully removed
     */
    bool removeEndListener(Uint32 key);
    
    /**
     * Removes the pan change listener for the given object key
     *
     * If there is no active listener for the given key, this method fails
     * and returns false.
     *
     * This listener is invoked when the pan position changes.
     *
     * @param key   The identifier for the listener
     *
     * @return true if the listener was succesfully removed
     */
    bool removeChangeListener(Uint32 key);
    
    
protected:
#pragma mark Input Device
    /**
     * Clears the state of this input device, readying it for the next frame.
     *
     * Many devices keep track of what happened "this" frame.  This method is
     * necessary to advance the frame.
     */
    virtual void clearState() override;
    
    /**
     * Processes an SDL_Event
     *
     * The dispatcher guarantees that an input device only receives events that
     * it subscribes to.
     *
     * @param event The input event to process
     * @param stamp The event timestamp in CUGL time
     *
     * @return false if the input indicates that the application should quit.
     */
    virtual bool updateState(const SDL_Event& event, const Timestamp& stamp) override;
    
    /**
     * Returns the scale/unscaled touch position.
     *
     * The value returned depends on the value of attribute _screen.  If this
     * attribute is false, the position is normalized to the unit square.
     * Otherwise it is scaled to the touch screen.
     *
     * @return the scale/unscaled touch position.
     */
    Vec2 getScaledPosition(float x, float y);
    
    /**
     * Determine the SDL events of relevance and store there types in eventset.
     *
     * An SDL_EventType is really Uint32.  This method stores the SDL event
     * types for this input device into the vector eventset, appending them
     * to the end. The Input dispatcher then uses this information to set up
     * subscriptions.
     *
     * @param eventset  The set to store the event types.
     */
    virtual void queryEvents(std::vector<Uint32>& eventset) override;

#pragma mark Internal Helpers
private:
    /**
     * Returns the centroid of the fingers
     *
     * The centroid is the average of all the fingers on the touch device.
     *
     * @return the centroid of the fingers
     */
    Vec2 computeCentroid() const;
    
    /**
     * Reinitializes the pan event for a new pan gesture.
     *
     * This method calls all of the begin listeners with the new
     * gesture information.
     *
     * @param stamp The initial timestamp of the new gesture
     */
    void startGesture(const Timestamp& stamp);

    /**
     * Finalizes the pan event, preparing for a new pan gesture.
     *
     * This method calls all of the end listeners with the final
     * gesture information.
     *
     * @param stamp The final timestamp of the gesture
     */
    void cancelGesture(const Timestamp& stamp);
    
    // Apparently friends are not inherited
    friend class Input;
};

}
#endif /* __CU_PAN_INPUT_H__ */
