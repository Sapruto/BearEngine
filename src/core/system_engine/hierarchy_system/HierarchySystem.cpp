/*#include "HierarchySystem.h"

void AddGameObject(GameObject* newGameObject){
    gameObjects.push_back(newGameObject);
}
void AddCommunication(GameObject* parent, GameObject* child){
    Communication communication;
    for(int i = 0; i < gameObjects.size(); i++){
        if(parent == gameObjects[i]){
            communication.parent = i;
        }
        else if(child == gameObjects[i]){
            communication.child = i;
        }
    }
}

void RemoveGameObject(GameObject* gameObjectToDelete){
    gameObjects.remove_if(gameObjectToDelete);
}

GameObject* GetParent(GameObject* gameObject){

}
std::vector<GameObject*> GetParents(GameObject* gameObject){

}

GameObject* GetChild(GameObject* gameObject){

}
std::vector<GameObject*> GetChilds(GameObject* gameObject){

}*/