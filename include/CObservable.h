#pragma once

#include <functional>

class CObservable
{
public:
    /*
     * Observable class
     * This class allow for registering a callback that will be called
     * when the class call notify().
     */
    CObservable();
    ~CObservable();

    /*
     * Add a callback to be called when notified
     * 
     * @param : callback function
     */
    void registerCallback( const std::function<void()>& callback );

    /*
    * Add a callback to be called when notified
    *
    * @param : callback function
    */
    void unregisterCallback( const std::function<void()>& callback );

protected:
    /*
     * Should only be used by the class that herit from this class.
     * This will trigger all callback registed
     */
    void notify() const;

private:

    /*
     * Vector containing all callback to be called when notified
     */
    std::vector<std::function<void()>> m_vectorCallback;
};

