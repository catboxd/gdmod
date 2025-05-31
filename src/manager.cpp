#include "include/manager.hpp"

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <Geode/Geode.hpp>
#include <matjson.hpp>

#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;


struct Data {
    int hunger;
    int health;
};

struct Blocks {
    std::string name;
    Data stats;
    bool registered;
};

WebSocketManager* WebSocketManager::s_instance = nullptr;

WebSocketManager* WebSocketManager::getInstance() {
    if (!s_instance) {
        s_instance = new WebSocketManager();
    }
    return s_instance;
}

WebSocketManager::WebSocketManager() {
    ix::initNetSystem();
    m_webSocket.setUrl("ws://127.0.0.1:5656");
    
    m_webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            auto json = matjson::parse(msg->str);
            if (!json.err()) {
                auto obj = json.unwrap();
                log::info("yes json");
                if (obj["command"] == "UPDATE_MAP") {
                    log::info("UPDATING MAP!");
                    auto test = LevelEditorLayer::get();
                    auto st =  obj["data"].dump();
                    
                    auto bbnn = gd::string(st);
                    log::info("{}",st);
                    
                    // test->removeAllObjects();
                    test->createObjectsFromString(bbnn,true,true);
                    // test->createObjectsFromSetup(bbnn);
                    // auto test =
                    // LevelEditorLayer::createObjectsFromString("test",false,false);
                }else if (obj["command"] == "MOVE_OBJECT")
                {
                    auto ccc = LevelEditorLayer::get();
                    auto editor = EditorUI::get();
                    auto st =  obj["data"].dump();
                    // auto x1 = obj["data"]["x"].asInt().unwrap();
                    auto x1 = obj["data"]["BlockX"].asDouble().unwrap();
                    auto x2 = obj["data"]["BlockY"].asDouble().unwrap();

                    auto xMove1 = obj["data"]["MoveX"].asDouble().unwrap();
                    auto xMove2 = obj["data"]["MoveY"].asDouble().unwrap();
                    log::debug("{}",x2);



                    auto xxx = CCPoint();
                    xxx.x = x1;
                    xxx.y = x2;

                    auto bbb = CCPoint();
                    bbb.x = xMove1;
                    bbb.y = xMove2;
                    
                    auto c = ccc->objectAtPosition(xxx);
                    if (c) {
                        log::debug("YES OBJECTT!!");
                        editor->moveObject(c,bbb);
                    }
                    // log::info("MOVING OBJECT! {}, {}",x1,x2);
                    // std::string s = st;
                    // std::string delimiter = ";";
                    // std::string x1 = s.substr(0, s.find(delimiter));
                    // std::string x2 = s.substr(s.find(delimiter) + 1); // Corrected to get the second part



                    // log::debug("{}",x2);
                    // 
                    // 
                    
                    /* code */
                }
                
            }
            log::info("Received: {}", msg->str);
        }
        else if (msg->type == ix::WebSocketMessageType::Open) {
            log::info("WebSocket connected!");
        }
        else if (msg->type == ix::WebSocketMessageType::Error) {
            log::error("WebSocket error: {}", msg->errorInfo.reason);
        }
    });
}

WebSocketManager::~WebSocketManager() {
    m_webSocket.stop();
}

void WebSocketManager::connect() {
    m_webSocket.start();
}

void WebSocketManager::sendMessage(const std::string& message) {
    m_webSocket.send(message);
}

void WebSocketManager::sendLevelData(const std::string& levelData) {
    matjson::Value json;
    json["command"] = "UPDATE_MAP";
    json["data"] = levelData;

    // Serialize and send
    auto bbb = matjson::makeObject({
            { "command", "UPDATE_MAP" },
            { "data", levelData }
        }).dump();


    m_webSocket.send(bbb);
}

ix::WebSocket& WebSocketManager::getWebSocket() {
    return m_webSocket;
}