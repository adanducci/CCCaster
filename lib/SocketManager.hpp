#pragma once

#include <unordered_set>


class Socket;


class SocketManager
{
public:

    // Check for socket events
    void check ( uint64_t timeout );

    // Add / remove / clear socket instances
    void add ( Socket *socket );
    void remove ( Socket *socket );
    void clear();

    // Initialize / deinitialize socket manager
    void initialize();
    void deinitialize();
    bool isInitialized() const { return initialized; }

    // Check if a socket is still allocated
    bool isAllocated ( Socket *socket ) const
    {
        return ( allocatedSockets.find ( socket ) != allocatedSockets.end() );
    }

    // Get the singleton instance
    static SocketManager& get();

private:

    // Sets of active and allocated socket instances
    std::unordered_set<Socket *> activeSockets, allocatedSockets;

    // Flag to indicate the set of allocated sockets has changed
    bool changed = false;

    // Flag to indicate if initialized
    bool initialized = false;

    // Private constructor, etc. for singleton class
    SocketManager();
    SocketManager ( const SocketManager& );
    const SocketManager& operator= ( const SocketManager& );
};
