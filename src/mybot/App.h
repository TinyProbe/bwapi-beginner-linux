#ifndef _APP_H_
#define _APP_H_

#include <BWAPI/AIModule.h>

class App {
    BWAPI::AIModule *bot_;

public:
    App();
    ~App();

    void run();

private:
    void loop();

};

#endif
