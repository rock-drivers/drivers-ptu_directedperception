#include "Driver.h"
#include "Cmd.h"
using namespace ptu;

#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    Driver drv;
    drv.open("localhost");

    bool ret;
    ret = drv.setPos(276, TILT, true);
    cout << ret << endl;

    drv.close();
    return 0;
}

