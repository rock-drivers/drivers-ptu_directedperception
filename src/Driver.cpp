/**
 * Implementation for the FLIR Pan-Tilt Unit Driver.
 * @file Driver.cpp
 * @author Remus Claudiu Dumitru <r.dumitru@jacobs-university.de>
 * @date Wed Apr 11 13:25:02 CEST 2012
 */

//==============================================================================
// Includes
//==============================================================================
#include "Driver.h"
using namespace ptu;

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <base/logging.h>
#include <cmath>

#include <boost/current_function.hpp>
using namespace boost;

//==============================================================================
// Static members initialization
//==============================================================================
const int ptu::Driver::DEFAULT_BAUDRATE    = 9600;
const int ptu::Driver::MAX_PACKET_SIZE     = 8192;
const float ptu::Driver::DEGREEPERTICK     = 0.051432698;

//==============================================================================
// Implementation
//==============================================================================


bool Driver::openSerial(std::string const& port, int baudrate){
	bool retVal = false;
        retVal = iodrivers_base::Driver::openSerial(port, baudrate);
	
	//set response mode of the device to short (easier parsing) tarse mode.
	write("FT ");
	//check if set was done.
	std::string ans="",err="";
	readAns(ans);
	if(!validateAns(ans,err)){
		LOG_ERROR_S << "openSerial: error while changing echo mode: " << err;
		return false;
	}
	
	return retVal;
}


bool Driver::write(const std::string& msg, const int& timeout) {
    LOG_DEBUG_S << "write called with param: \"" << msg << "\"";
    try {
        if (timeout == -1) {
            writePacket(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size());
        } else {
            writePacket(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size(), timeout);
        }
    } catch (iodrivers_base::TimeoutError e) {
        std::cerr << "write: timeout error" <<std::endl;
        LOG_ERROR_S << "write: timeout error";
        return false;
    } catch (...) {
        std::cerr << "write: unknown error" << std::endl;
	LOG_ERROR_S << "write: unknown error";
        return false;
    }

    return true;
}

bool Driver::readAns(std::string& ans, int timeout) {

    uint8_t buffer[MAX_PACKET_SIZE];
    size_t bufferSize = MAX_PACKET_SIZE;
    size_t packetSize;

    try {
        if (-1 == timeout) {
	    LOG_DEBUG_S << "readAns called readPacket without timeout";
            packetSize = readPacket(buffer, bufferSize);
        } else {
	    LOG_DEBUG_S << "readAns called readPacket WITH timeout: " << timeout << ".";
            packetSize = readPacket(buffer, bufferSize, timeout);
        }
    } catch (iodrivers_base::TimeoutError e) {
        std::cerr << "readAns: timeout error" <<std::endl;
        return false;
    } catch (...) {
        std::cerr << "readAns : unknown error" <<std::endl;
        return false;
    }
	
    LOG_DEBUG_S << "readAns, device response string: " << ans;
    ans.assign(reinterpret_cast<const char*>(buffer), packetSize);
    return true;
}

bool Driver::validateAns(const std::string& ans, std::string& error) {
    if (ans.size() < 2) {
        error = "answer must be at least of size 2";
        return false;
    } else if (ans[0] != Cmd::SUCC_BEG[0] && ans[0] != Cmd::ERR_BEG[0]) {
        error = "answer must always start with " + Cmd::SUCC_BEG + " or " + Cmd::ERR_BEG;
        return false;
    } else if (ans[0] == Cmd::ERR_BEG[0]) {
        error = ans.substr(1, ans.size() - 2);
        return false;
    }

    return true;
}

int Driver::extractPacket(const uint8_t* buffer, size_t size) const {
    

    // possible beginnings of a packet
    std::string begs = Cmd::SUCC_BEG + Cmd::ERR_BEG;

    // the current packet
    std::string packet(reinterpret_cast<const char*>(buffer), size);

    // find the beginning and the end of a packet
    size_t beg = packet.find_first_of(begs);
    size_t end = packet.find_first_of(Cmd::DELIM_CR);

    //cout << beg << " " << end <<std::endl;

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
        _baudrate(DEFAULT_BAUDRATE)
{}

Driver::~Driver() {
    if (this->isValid()) {
        this->close();
    }
}

bool Driver::close() {
    iodrivers_base::Driver::close();
    return true;
}

//TODO check if offset parameter is really usefull here, since PO and PP seem to 
//     have always the same answer.
bool Driver::getPos(const Axis& axis, const bool& offset, int& pos) {

    bool ret = write(Cmd::getPos(axis, offset));
    if (!ret) return false;

    std::string ans;
    // TODO fix the timeout
    ret = readAns(ans, 10000);
    if (!ret) return false;

    std::string error;
    ret = validateAns(ans, error);
    if (!ret) {
        std::cerr << "getPos: " << error << std::endl;
	LOG_ERROR_S << "getPos: " << error;
        return false;
    }

    std::string toFind("* ");
    size_t found = ans.find(toFind);

    if (found == std::string::npos) {
        std::cerr << "getPos: invalid answer format" << std::endl;
	LOG_ERROR_S << "getPos: invalid answer format";
        return false;
    }

    std::stringstream ss;
    ss << ans.substr(found + toFind.size());
    ss >> pos;

    return true;
}


//get the position as degree.
float Driver::getPosDeg(const Axis& axis, const bool& offset){
    float retVal = 0;
    int tickPos = 0;
    getPos(axis,offset,tickPos);
    retVal = (float)tickPos * DEGREEPERTICK;
    return retVal;
}


//set position as degree value.
bool Driver::setPosDeg(const Axis &axis, const bool &offset, const float &val, 
                       const bool &awaitCompletion){
    
    //calculate tick value
    int ticks = round(val / DEGREEPERTICK);

    //setPos in ticks.
    setPos(axis, offset, ticks, awaitCompletion);
    
    return true;
}


bool Driver::setPos(const Axis& axis, const bool& offset, const int& val, 
                    const bool& awaitCompletion) {

    bool ret = write(Cmd::setPos(val, axis, offset));
    if (!ret) return false;

    std::string ans;
    // TODO fix the timeout
    ret = readAns(ans, 10000);
    if (!ret) return false;

    std::string error;
    ret = validateAns(ans, error);
    if (!ret) {
        std::cerr << "setPos: " << error << std::endl;
	LOG_ERROR_S << "setPos: " << error;
        return false;
    }

    if (awaitCompletion){
	
	//set awaitCompletion mode      
	LOG_DEBUG_S << "setPos: with await completion.";
	write(Cmd::awaitPosCmdCompletion());
	
	//check if command was set successfully.
	ret = readAns(ans, 10000);
	std::string err = "";
	if(!ret or !validateAns(ans, err)){
		LOG_ERROR_S << "failed to set await completion mode: " << err;
		//log error but do not answer with fail code.
	}
	
    }

    if (ans == Cmd::SUCC_CMD) {
        return true;
    } else {
        std::cerr << "setPos: failed to set position" << std::endl;
	LOG_ERROR_S << "setPos: failed to set position";
        return false;
    }

    return true;
}
