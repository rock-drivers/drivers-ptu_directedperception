/**
  * Definition of FLIR Pan-Tilt Unit commands.
  * @file Cmd.cpp
  * @author Remus Claudiu Dumitru <r.dumitru@jacobs-university.de>
  * @date Wed Apr 11 CEST 2012
  */

//==============================================================================
// Includes
//==============================================================================
#include "Cmd.h"
using namespace ptu;

#include <iomanip>

#include <stdexcept>
using namespace std;

#include <boost/current_function.hpp>
using namespace boost;

//==============================================================================
// Public static members initialization
//==============================================================================
const string Cmd::DELIM_CR        = "\r";
const string Cmd::DELIM_SP        = " ";

const string Cmd::SUCC_BEG        = "*";
const string Cmd::ERR_BEG         = "!";

const string Cmd::SUCC_CMD      = "*\r";

//==============================================================================
// Private static methods implementation
//==============================================================================
void Cmd::appendAxis(const Axis& axis, const string currName, stringstream& ss) {
    if (axis == PAN) {
        ss << "P";
    } else if (axis == TILT) {
        ss << "T";
    } else {
        throw std::runtime_error(currName + ": invalid axis value");
    }
}

//==============================================================================
// Public static methods implementation
//==============================================================================
string Cmd::getPos(const Axis& axis, const bool& offset) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    if (offset == true) {
        msg << "O";
    } else {
        msg << "P";
    }

    msg << DELIM_SP;
    return msg.str();
}

string Cmd::setPos(const int& val, const Axis& axis, const bool& offset)
{
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    if (offset == true) {
        msg << "O";
    } else {
        msg << "P";
    }

    msg << val << DELIM_SP;
    return msg.str();
}

string Cmd::getResoultion(const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "R" << DELIM_SP;
    return msg.str();
}

string Cmd::getMinPos(const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);
    msg << "N" << DELIM_SP;

    return msg.str();
}

string Cmd::getMaxPos(const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);
    msg << "X" << DELIM_SP;

    return msg.str();
}

string Cmd::getPanLimitMode() {
    stringstream msg;
    msg << "L" << DELIM_SP;

    return msg.str();
}

string Cmd::setPanLimitMode(const bool& val) {
    stringstream msg;
    msg << "L";

    if (val == true) {
        msg << "E";
    } else {
        msg << "D";
    }

    msg << DELIM_SP;
    return msg.str();
}

string Cmd::enableImmediatePosExec() {
    return "I" + DELIM_SP;
}

string Cmd::enableSlavedPosExec() {
    return "S" + DELIM_SP;
}

string Cmd::awaitPosCmdCompletion() {
    return "A" + DELIM_SP;
}

string Cmd::haltPosCmd(const bool& pan, const bool& tilt) {
    stringstream msg;

    if (pan == true && tilt == false) {
        msg << "HP";
    } else if (pan == false && tilt == true) {
        msg << "HT";
    } else if (pan == true && tilt == true) {
        msg << "H";
    }

    msg << DELIM_SP;

    if (pan == false && tilt == false) {
        msg.clear();
    }

    return msg.str();
}

string Cmd::autoScan(const int& panPos1, const int& panPos2) {
    stringstream msg;
    msg << "M"<< panPos1 << "," << panPos2 << DELIM_SP;
    return msg.str();
}

string Cmd::autoScan(const int& panPos1, const int& panPos2,
            const int& tiltPos1, const int& tiltPos2)
{
    stringstream msg;
    msg << "M" << panPos1  << "," << panPos2 << "," << tiltPos1 << "," << tiltPos2 << DELIM_SP;
    return msg.str();
}

string Cmd::lastAutoScan() {
    return "M" + DELIM_SP;
}

string Cmd::getAutoScanAtPowerUp() {
    return "MQ" + DELIM_SP;
}

string Cmd::setAutoScanAtPowerUp(const bool& val) {
    stringstream msg;
    msg << "M";

    if (val == true) {
        msg << "E";
    } else {
        msg << "D";
    }

    msg << DELIM_SP;
    return msg.str();
}

string Cmd::preset(const int& index, const PresetAction& action) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    if (index < 0 && index > 32) {
            throw std::runtime_error(currName + ": index must be between 0 and 32");
    }

    stringstream msg;
    msg << "X";
    if (action == SET) {
        msg << "S";
    } else if (action == GOTO) {
        msg << "G";
    } else if (action == CLEAR) {
        msg << "C";
    } else {
        throw std::runtime_error(currName + ": invalid axis value");
    }

    msg << DELIM_SP;
    return msg.str();
}

string Cmd::getDesiredSpeed(const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "S" << DELIM_SP;
    return msg.str();
}

string Cmd::setDesiredSpeed(const int& val, const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "S" << val << DELIM_SP;
    return msg.str();
}

string Cmd::getCurrentSpeed(const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "D" << DELIM_SP;
    return msg.str();
}

string Cmd::setDesiredDeltaSpeed(const int& val, const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "D" << val << DELIM_SP;
    return msg.str();
}

string Cmd::getDesiredAccel(const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "A" << DELIM_SP;
    return msg.str();
}

string Cmd::setDesiredAccel(const int& val, const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "A" << val << DELIM_SP;
    return msg.str();
}

string Cmd::getDesiredBaseSpeed(const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "B" << DELIM_SP;
    return msg.str();
}

string Cmd::setDesiredBaseSpeed(const int& val, const Axis& axis) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    msg << "B" << val << DELIM_SP;
    return msg.str();
}

string Cmd::getSpeedLimit(const Axis& axis, const AxisLimit& limit) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    if (limit == UPPER) {
        msg << "U";
    } else if (limit == LOWER) {
        msg << "L";
    } else {
        throw std::runtime_error(currName + ": invalid limit value");
    }

    msg << DELIM_SP;
    return msg.str();
}

string Cmd::setSpeedLimit(const int& val, const Axis& axis, const AxisLimit& limit) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    appendAxis(axis, currName, msg);

    if (limit == UPPER) {
        msg << "U";
    } else if (limit == LOWER) {
        msg << "L";
    } else {
        throw std::runtime_error(currName + ": invalid limit value");
    }

    msg << val << DELIM_SP;
    return msg.str();
}

string Cmd::getCtrlMode() {
    return "C" + DELIM_SP;
}

string Cmd::setCtrlMode(const CtrllMode& mode) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    stringstream msg;
    msg << "C";

    if (mode == INDEP) {
        msg << "I";
    } else if (mode == PURE) {
        msg << "V";
    } else {
        throw std::runtime_error(currName + ": invalid speed control mode");
    }

    msg << DELIM_SP;
    return msg.str();
}

