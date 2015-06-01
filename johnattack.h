#ifndef JOHNATTACK_H
#define JOHNATTACK_H

#include "johnhandler.h"

#include <QObject>

class JohnAttack : public JohnHandler
{
public:
    JohnAttack();
    ~JohnAttack();

    void start();
    void stop();
};

#endif // JOHNATTACK_H
