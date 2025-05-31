#pragma once
#include <ixwebsocket/IXWebSocket.h>

class WebSocketManager {
public:
    static WebSocketManager* getInstance();
    
    void connect();
    void sendMessage(const std::string& message);
    void sendLevelData(const std::string& levelData);
    ix::WebSocket& getWebSocket();

private:
    WebSocketManager();
    ~WebSocketManager();
    
    ix::WebSocket m_webSocket;
    static WebSocketManager* s_instance;
};