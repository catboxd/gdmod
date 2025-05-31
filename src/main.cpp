#include <Geode/Geode.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/cocoa/CCGeometry.h>

#include <Geode/modify/LevelEditorLayer.hpp>

#include <Geode/binding/EditorUI.hpp>
#include <Geode/modify/EditorUI.hpp>

#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>


#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <matjson.hpp>
#include "include/manager.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>

#include <matjson/reflect.hpp>

struct Data {
	double blockX;
	double blockY;

	double moveX;
	double moveY;
};

struct Blocks {
    std::string command;
    Data data;
};

$on_mod(Loaded) {
    auto wsManager = WebSocketManager::getInstance();
    wsManager->connect();
};



class $modify(EditorUI) {
	void moveObject(GameObject* p0, CCPoint p1) {
		
		auto wsManager = WebSocketManager::getInstance();
		log::debug("moving");
		auto b = GJBaseGameLayer::get();
		auto ccc = LevelEditorLayer::get();
		// auto c = LevelEditorLayer::objectAtPosition(p1);
		// 
		
		// log::debug("{}",c->m_uID);
		Blocks datas = Blocks {
			.command= "MOVE_OBJECT",
			.data = Data {
				.blockX = p0->m_positionX,
				.blockY = p0->m_positionY,
				.moveX = p1.x,
				.moveY = p1.y
			}
		};

		log::debug("Moving object: {} x: {} y: {}", p0->m_uniqueID,p1.x,p1.y);
		EditorUI::moveObject(p0,p1);
		// auto xxx = CCPoint();
		// xxx.x = p1.x;
		// xxx.y = p1.y;


		matjson::Value value = datas;
		auto  testt = value.dump();
		wsManager->sendMessage(testt);

		// auto c = ccc->objectAtPosition(xxx);
		// if (c) {

		// 	log::debug("{}",c->m_uID);
		// 	log::debug("{}",xxx.x);
		// } else {
		// 	log::debug("no object {}", p0->m_positionY);
		// }

	}
};

class $modify(LevelEditorLayer) {

	// void removeObject(GameObject* target,bool p1){
	// 	log::info("BLOCK REMOVED!!!!");
	// 	this->removeObject(target,p1);
	// }
	bool init(GJGameLevel* p0, bool p1) {
		log::debug("[todo] websocket");
        auto wsManager = WebSocketManager::getInstance();
		auto bbb = matjson::makeObject({
				{ "command", "HOST" },
				{ "data", "YES" }
			}).dump();
		wsManager->sendMessage(bbb);
        // wsManager->sendMessage("Level editor initialized");

		return LevelEditorLayer::init(p0,p1);
	}

	void removeObject(GameObject* p0, bool p1) {
		auto test = GJBaseGameLayer::get();
		auto five = p0->getSaveString(test);
		log::debug("{}",five);
		log::debug("Removing object: {}", p0->m_uID);
		LevelEditorLayer::removeObject(p0,p1);
	}

	GameObject* createObject(int p0,CCPoint p1, bool p2) {

		// auto a = this->createObject(p0,p1,p2);
		log::debug("Block placed");
		// LevelEditorLayer::createObjectsFromString(b,false,false);
        auto wsManager = WebSocketManager::getInstance();
		//WebSocketManager:send(todo)
		
		// wsManager->sendMessage(aaa);
		auto blocks = LevelEditorLayer::createObject(p0,p1,p2);
		
		auto aaa = LevelEditorLayer::getLevelString();
		wsManager->sendLevelData(aaa);
		
		// LevelEditorLayer::removeAllObjects();
		log::info("{}",aaa);
		return blocks;
		
	}
	
};