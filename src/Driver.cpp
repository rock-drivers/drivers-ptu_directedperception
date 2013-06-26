/**
 * Implementation for the FLIR Pan-Tilt Unit Driver.
 * @file Driver.cpp
 * @author Remus Claudiu Dumitru <r.dumitru@jacobs-university.de>
 */

//==============================================================================
// Includes
//==============================================================================
#include "Driver.h"
using namespace ptu;


#include <iostream>
#include <stdexcept>
#include <cmath>

using boost::lexical_cast;

//==============================================================================
// Static members initialization
//==============================================================================
const int ptu::Driver::DEFAULT_BAUDRATE    = 9600;
const int ptu::Driver::MAX_PACKET_SIZE     = 8192;
const float ptu::Driver::DEGREEPERTICK     = 0.051432698;
const float ptu::Driver::DEGREEPERSECARC = 0.0002778;

//==============================================================================
// Implementation
//==============================================================================


void Driver::initialize() {   

	//set response mode of the device to short (easier parsing) mode.
	write("FT ");
        readAns();

        // get the pan resolution
        write(Cmd::getResolution(PAN));
        mPanResolutionDeg = getQuery<float>(readAns()) * DEGREEPERSECARC;
        LOG_INFO_S << "Pan resolution is " << mPanResolutionDeg << " deg/position";

        // get the tilt resolution
        write(Cmd::getResolution(TILT));
        mTiltResolutionDeg = getQuery<float>(readAns()) * DEGREEPERSECARC;
        LOG_INFO_S << "Tilt resolution is " << mTiltResolutionDeg << " deg/position";

        // get min/max pan in rad
        write(Cmd::getMinPos(PAN));
        int min_pan = getQuery<int>(readAns());

        write(Cmd::getMaxPos(PAN));
        int max_pan = getQuery<int>(readAns());

        mMinPanRad = float(min_pan) * mPanResolutionDeg * M_PI / 180.0;
        mMaxPanRad = float(max_pan) * mPanResolutionDeg * M_PI / 180.0;

        LOG_INFO_S << "Pan limits (rad): " <<  mMinPanRad << " to " << mMaxPanRad;
	     
        // get min/max tilt in rad
        write(Cmd::getMinPos(TILT));
        int min_tilt = getQuery<int>(readAns());

        write(Cmd::getMaxPos(TILT));
        int max_tilt = getQuery<int>(readAns());

        mMinTiltRad = float(min_tilt) * mTiltResolutionDeg * M_PI / 180.0;
        mMaxTiltRad = float(max_tilt) * mTiltResolutionDeg * M_PI / 180.0;

        LOG_INFO_S << "Tilt limits (rad): " <<  mMinTiltRad << " to " << mMaxTiltRad;
}


void Driver::write(const std::string& msg) {
    
    writePacket(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size());
}


std::string Driver::readAns() {

    uint8_t buffer[MAX_PACKET_SIZE];
    size_t bufferSize = MAX_PACKET_SIZE;
    size_t packetSize;

    packetSize = readPacket(buffer, bufferSize);
   
    if ( packetSize < 2) 
        throw std::runtime_error("answer must be at least of size 2");

    else if (buffer[0] != Cmd::SUCC_BEG[0] && buffer[0] != Cmd::ERR_BEG[0])
        throw std::runtime_error("answer must always start with " + Cmd::SUCC_BEG + 
                " or " + Cmd::ERR_BEG);

    else if (buffer[0] == Cmd::ERR_BEG[0])
        throw std::runtime_error("error in command, reply: " + 
                std::string(reinterpret_cast<const char*>(buffer), packetSize));


    std::string reply(reinterpret_cast<const char*>(buffer), packetSize);
    LOG_DEBUG_S << "readAns: " << reply;
    return reply;
}
    

int Driver::extractPacket(const uint8_t* buffer, size_t size) const {
    

    // possible beginnings of a packet
    std::string begs = Cmd::SUCC_BEG + Cmd::ERR_BEG;

    // the current packet
    std::string packet(reinterpret_cast<const char*>(buffer), size);

    // find the beginning and the end of a packet
    size_t beg = packet.find_first_of(begs);
    size_t end = packet.find_first_of(Cmd::DELIM_CR);

    // see the documentation of extractPacket for further details
    if (beg == std::string::npos && end == std::string::npos) {
        return (-size);
    } else if (beg != 0 && beg != std::string::npos) {
        return (-beg);
    } else if (beg == 0 && end == std::string::npos) {
        return 0;
    } else if (beg == 0 && end != std::string::npos) {
        return end + 1;
    } else {
        throw std::runtime_error("extractPacket: invalid branch taken");
    }

    return 0;
}

Driver::Driver() :
        iodrivers_base::Driver(MAX_PACKET_SIZE),
        mPanResolutionDeg(1.0),
        mTiltResolutionDeg(1.0)
{}

Driver::~Driver() {
    if (this->isValid()) {
        this->close();
    }
}

//TODO check if offset parameter is really usefull here, since PO and PP seem to 
//     have always the same answer.
int Driver::getPos(Axis axis, bool offset) {

    write(Cmd::getPos(axis, offset));

    return getQuery<int>(readAns());
}

float Driver::getPosDeg(Axis axis, bool offset) {

    return float(getPos(axis, offset)) * DEGREEPERTICK;
}

float Driver::getPosRad(Axis axis, bool offset) {

    return getPosDeg(axis, offset) / 180.0 * M_PI;
}


//set position as degree value.
bool Driver::setPosDeg(const Axis &axis, const bool &offset, const float &val, 
                       const bool &awaitCompletion){
    
    int ticks = round(val / DEGREEPERTICK);
    setPos(axis, offset, ticks, awaitCompletion);
    
    return true;
}
bool Driver::setPosRad(const Axis &axis, const bool &offset, const float &val, 
                       const bool &awaitCompletion){

    return setPosDeg(axis, offset, val / M_PI * 180.0, awaitCompletion);
}


bool Driver::setPos(const Axis& axis, const bool& offset, const int& val, 
                    const bool& awaitCompletion) {

    write(Cmd::setPos(val, axis, offset));

    readAns();

    if (awaitCompletion){
	
	//set awaitCompletion mode      
	LOG_DEBUG_S << "setPos: with await completion.";
	write(Cmd::awaitPosCmdCompletion());
	
	//check if command was set successfully.
	readAns();
    }

    return true;
}


void Driver::setSpeed(Axis axis, int speed) {
    
    write(Cmd::setDesiredSpeed(speed,axis));
   
    readAns(); 
}

void Driver::setSpeedDeg(Axis axis, float speed) {
    
    int speed_pos_s;

    if ( axis == TILT )
        speed_pos_s = speed / mTiltResolutionDeg;
    else if (axis == PAN )
        speed_pos_s = speed / mPanResolutionDeg;

    setSpeed(axis,speed_pos_s);
}

void Driver::setSpeedRad(Axis axis, float speed) {

    setSpeedDeg(axis, speed * 180.0 / M_PI);
}

void Driver::setHalt() {

    write(Cmd::haltPosCmd(true,true));
    readAns();
}
