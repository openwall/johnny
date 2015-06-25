#ifndef JOHNATTACK_H
#define JOHNATTACK_H

#include "johnhandler.h"

#include <QObject>

class JohnAttack : public JohnHandler
{
    Q_OBJECT
    
public:
    JohnAttack();
    ~JohnAttack();

    // Nothing much to do here for now, there will be soon
    void start();
    void stop();
};

#endif // JOHNATTACK_H
