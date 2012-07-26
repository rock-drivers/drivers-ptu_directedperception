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
    int int_answer = 0;
    float float_answer = 0;

    //Set positions in ticks
    std::cout << "Setting absolute position -800(P), 300(T) with drv.setPos and awaitCompletion" << std::endl  << std::endl;
    drv.setPos(ptu::PAN, false, -800, true);
    drv.setPos(ptu::TILT, false, 300, true);

    //Read positions
    std::cout << "Reading positions with drv.getPos and drv.getPosDeg:" << std::endl;
    float_answer = drv.getPosDeg(ptu::PAN, false);
    std::cout << "Answer PAN: " << float_answer << "° (";
    ret = drv.getPos(ptu::PAN, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl;
    float_answer = drv.getPosDeg(ptu::TILT, false);
    std::cout << "Answer TILT: " << float_answer << "° (";
    ret = drv.getPos(ptu::TILT, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl << std::endl;

    //Set positions in degrees
    std::cout << "Setting degree position 30°(P), -20°(T) with drv.setPosDeg and awaitCompletion" << std::endl  << std::endl;
    drv.setPosDeg(ptu::PAN, false, 30, true);
    drv.setPosDeg(ptu::TILT, false, -20, true);


    std::cout << "Set position in rad 0.7(P), -0.7(T)" << std::endl;
    drv.setPosRad(ptu::PAN,false, 0.7, true);
    drv.setPosRad(ptu::TILT,false, -0.7, true);
    std::cout << "Check position" << std::endl;
    float_answer = drv.getPosRad(ptu::PAN, false);
    std::cout << "Answer PAN: " << float_answer << "rad (";
    ret = drv.getPos(ptu::PAN, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl;
    float_answer = drv.getPosRad(ptu::TILT, false);
    std::cout << "Answer TILT: " << float_answer << "rad (";
    ret = drv.getPos(ptu::TILT, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl << std::endl;
    
    //Read positions
    std::cout << "Reading positions with drv.getPos and drv.getPosDeg:" << std::endl;
    float_answer = drv.getPosDeg(ptu::PAN, false);
    std::cout << "Answer PAN: " << float_answer << "° (";
    ret = drv.getPos(ptu::PAN, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl;
    float_answer = drv.getPosDeg(ptu::TILT, false);
    std::cout << "Answer TILT: " << float_answer << "° (";
    ret = drv.getPos(ptu::TILT, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl << std::endl;

    //Set relative positions in degrees
    std::cout << "Setting relative degree position +20°(P), -10°(T) with drv.setPosDeg and awaitCompletion" << std::endl      << std::endl;
    drv.setPosDeg(ptu::PAN, true, 20, true);
    drv.setPosDeg(ptu::TILT, true, -10, true);
     
    //Read positions
    std::cout << "Reading positions with drv.getPos and drv.getPosDeg:" << std::endl;
    float_answer = drv.getPosDeg(ptu::PAN, false);
    std::cout << "Answer PAN: " << float_answer << "° (";
    ret = drv.getPos(ptu::PAN, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl;
    float_answer = drv.getPosDeg(ptu::TILT, false);
    std::cout << "Answer TILT: " << float_answer << "° (";
    ret = drv.getPos(ptu::TILT, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl << std::endl;

    //Set relative positions in degrees
    std::cout << "Resetting through relative degree position -50°(P), +30°(T) with drv.setPosDeg and awaitCompletion" <<     std::endl      << std::endl;
    drv.setPosDeg(ptu::PAN, true, -50, true);
    drv.setPosDeg(ptu::TILT, true, 30, true);

    //Read positions
    std::cout << "Reading positions with drv.getPos and drv.getPosDeg:" << std::endl;
    float_answer = drv.getPosDeg(ptu::PAN, false);
    std::cout << "Answer PAN: " << float_answer << "° (";
    ret = drv.getPos(ptu::PAN, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl;
    float_answer = drv.getPosDeg(ptu::TILT, false);
    std::cout << "Answer TILT: " << float_answer << "° (";
    ret = drv.getPos(ptu::TILT, false, int_answer);
    std::cout << int_answer << " ticks)" << std::endl << std::endl;

    //Reset positions    
    std::cout << std::endl << "Setting position back to 0(P), 0(T) with drv.setPos" << std::endl  << std::endl;
    drv.setPos(ptu::PAN, false, 0);
    drv.setPos(ptu::TILT, false, 0);



    drv.close();
    return 0;
}

