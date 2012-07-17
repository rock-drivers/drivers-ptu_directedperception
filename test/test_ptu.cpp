// \file test_ptu.cpp
#include <iostream>

#include <boost/program_options.hpp>

#include <Driver.h>
#include <Cmd.h>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {

    po::options_description desc("Options");
    desc.add_options()
        ("help", "show help")
        ("port,p", "port to connect to (by default it connects to localhost")
        ("query,q", "queries the properties of the ptu"); 

    ptu::Driver drv;
    drv.open("localhost");

    bool ret;
    // ret = drv.setPos(276, TILT, true);
    std::cout << ret << std::endl;

    drv.close();
    return 0;
}

