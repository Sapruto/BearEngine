#include "include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls/Mechanical/Gravity.h"

void Gravity::ChangeBody(){
    acceleration += gravitationScale;

    body->GetCollider()->GetTransform()->position->y += acceleration * Time::GetInstance().DeltaTime();
}
void Gravity::ReactionOnEvent(){

}
void Gravity::GetStateFeature(){

}