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
    drv.openSerial("/dev/ttyS1", 9600);

    bool ret = false;
    //ret = drv.setPos(276, TILT, true);
    std::cout << "Bestimmung der aktuellen Position: " << std::endl;
 
    std::cout << "Antwort mit drv.getPos: " << std::endl;
    int answer = 0;
    //ask driver function for actual position
    ret = drv.getPos(ptu::PAN, false, answer);
    std::cout << "Antwort PAN: " << ret << ": " << answer << std::endl;
    ret = drv.getPos(ptu::TILT, false, answer);
    std::cout << "Antwort TILT: " << ret << ": " << answer << std::endl << std::endl;

 
    std::cout << "Antwort mit write + read: " <<  std::endl;
    std::string test = "PP ";
    drv.write(test, -1);
    std::string answ = "";
    drv.readAns(answ,500);
    std::cout << "Antwort auf PP (Pan): " << answ << "" << std::endl;
    test = "TP ";
    drv.write(test, -1);
    answ = "";
    drv.readAns(answ,500);
    std::cout << "Antwort auf TP (Tilt): " << answ << "" << std::endl << std::endl;


    //go to Position 0 with manual write method.
    //test = "PP0 "; 
    //drv.write(test, -1);
    //drv.readAns(answ,500);
    //std::cout << "Antwort von PTU: " << answ << std::endl;
    //tell PTU to finish current task before doing the next one. (manual write).
    //test = "A ";
    //drv.write(test,-1);
    //test = "PP-1000 ";
    //sleep(5000);
    //drv.write(test, -1);
    
    //user driver function to go to end position
    drv.setPos(ptu::PAN, false, -800);
    drv.setPos(ptu::TILT, false, 300);

    drv.close();
    return 0;
}

