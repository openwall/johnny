#include "johnattack.h"

JohnAttack::JohnAttack()
{

}

JohnAttack::~JohnAttack()
{

}

void JohnAttack::start()
{
    JohnHandler::exec();
}

void JohnAttack::stop()
{
    JohnHandler::terminate();
}

