/**
  * Definition of FLIR Pan-Tilt Unit commands.
  * @file Cmd.h
  * @author Remus Claudiu Dumitru <r.dumitru@jacobs-university.de>
  * @date Wed Apr 11 CEST 2012
  */

#ifndef CMD_H_
#define CMD_H_

//==============================================================================
// Includes
//==============================================================================
#include <set>
#include <string>
#include <sstream>

//==============================================================================
// Declaration
//==============================================================================
namespace ptu {

/**
 * The two possible axis of the PTU.
 */
enum Axis {
    PAN, TILT
};

/**
 * The three possible actions on presets.
 */
enum PresetAction {
    SET, GOTO, CLEAR
};

/**
 * Possible axis limits.
 */
enum AxisLimit {
    UPPER, LOWER
};

/**
 * The possible control modes.
 */
enum CtrllMode {
    INDEP, PURE
};

class Cmd {
private:
    /**
     * Appends to a string stream the proper character PAN / TILT.
     */
    static void appendAxis(const Axis& axis, std::string currName, std::stringstream& ss);

protected:
    // do not allow any instance of this class
    Cmd();

public:
    // static const fields
    static const std::string DELIM_CR;          //!< The CR delimiter.
    static const std::string DELIM_SP;          //!< The space delimiter.

    static const std::string SUCC_BEG;          //!< The beginning of any successful command.
    static const std::string ERR_BEG;           //!< The beginning of any error.

    static const std::string SUCC_CMD;          //!< The return message for a successful command.

    // static methods
    /**
     * Get current pan-tilt position.
     * @param axis the axis to be used
     * @param offset if true, the relative value is used
     * @return the properly formated message
     */
    static std::string getPos(const Axis& axis, const bool& offset = false);

    /**
     * Set current pan-tilt position.
     * @param val the value to which to set the position
     * @param axis the axis to be used
     * @param offset if true, the relative value is used
     * @return the properly formated message
     */
    static std::string setPos(const int& val, const Axis& axis, const bool& offset = false);

    /**
     * Get resolution in arc seconds.
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string getResoultion(const Axis& axis);

    /**
     * Get minimum position possible.
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string getMinPos(const Axis& axis);

    /**
     * Get maximum position possible.
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string getMaxPos(const Axis& axis);

    /**
     * Get the pan position limit mode.
     * @return the properly formated message
     */
    static std::string getPanLimitMode();

    /**
     * Enable or disable pan position limits.
     * @return the properly formated message
     */
    static std::string setPanLimitMode(const bool& val);

    /**
     * Enables immediate position execution mode.
     * @return the properly formated message
     */
    static std::string enableImmediatePosExec();

    /**
     * Enables slaved position execution mode. Combine with await commands
     * to execute multiple coordinated position commands.
     * @return the properly formated message
     */
    static std::string enableSlavedPosExec();

    /**
     * Await position command completion. Also useful for executing multiple
     * coordinated position commands.
     * @return the properly formated message
     */
    static std::string awaitPosCmdCompletion();

    /**
     * Halts position commands on specific axis.
     * @param pan if true, halts position command on pan axis
     * @param tilt if true, halts position command on tilt axis
     * @return the properly formated message
     */
    static std::string haltPosCmd(const bool& pan, const bool& tilt);

    /**
     * Command defines and initiates repetitive monitoring (scanning) of the pan-tilt.
     * Autoscanning is immediately terminated upon receipt of a character from the host
     * computer, and the pan-tilt is sent to its home position.
     * @param panPos1 the pan position from where to start
     * @param panPos2 the pan position where to stop
     * @return the properly formated message
     */
    static std::string autoScan(const int& panPos1, const int& panPos2);

    /**
     * Command defines and initiates repetitive monitoring (scanning) of the pan-tilt.
     * Autoscanning is immediately terminated upon receipt of a character from the host
     * computer, and the pan-tilt is sent to its home position.
     * @param panPos1 the pan position from where to start
     * @param panPos2 the pan position where to stop
     * @param tiltPos1 the tilt position from where to start
     * @param tiltPos2 the tilt position where to stop
     * @return the properly formated message
     */
    static std::string autoScan(const int& panPos1, const int& panPos2,
            const int& tiltPos1, const int& tiltPos2);

    /**
     * Initiate last defined monitor (autoscan) command (the default at power up is pan axis
     * only autoscan between the pan limit positions).
     * @return the properly formated message
     */
    static std::string lastAutoScan();

    /**
     * Get autoscan status at power up.
     * @return the properly formated message
     */
    static std::string getAutoScanAtPowerUp();

    /**
     * Enable / disable autoscan at power up.
     * @return the properly formated message
     */
    static std::string setAutoScanAtPowerUp(const bool& val);

    /**
     * To stop the auto scan. 
     */
    static std::string stopAutoScan();

    /**
     * Sets, goes to or clear a preset position command.
     * @param index the index of the preset to act upon
     * @param action the action to perform on preset
     * @return the properly formated message
     */
    static std::string preset(const int& index, const PresetAction& action);

    /**
     * Get desired pan / tilt speed in positions / sec.
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string getDesiredSpeed(const Axis& axis);

    /**
     * Set desired pan / tilt speed in positions / sec.
     * @param val the desired speed
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string setDesiredSpeed(const int& val, const Axis& axis);

    /**
     * Get current pan / tilt speed in positions / sec.
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string getCurrentSpeed(const Axis& axis);

    /**
     * Set desired pan / tilt speed in position / sec as an offset from the current speed.
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string setDesiredDeltaSpeed(const int& val, const Axis& axis);

    /**
     * Get desired pan / tilt accelearation in positions / sec^2.
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string getDesiredAccel(const Axis& axis);

    /**
     * Set desired pan / tilt acceleration in positions / sec^2.
     * @param val the desired acceleration
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string setDesiredAccel(const int& val, const Axis& axis);

    /**
     * Get desired pan / tilt base speed in positions / sec.
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string getDesiredBaseSpeed(const Axis& axis);

    /**
     * Set desired pan / tilt base speed in positions / sec.
     * @param val the desired speed
     * @param axis the axis to be used
     * @return the properly formated message
     */
    static std::string setDesiredBaseSpeed(const int& val, const Axis& axis);

    /**
     * Get the upper and lower speed bounds for desired speed commands.
     * @param axis the axis to be used
     * @param limit the limit to be queried (upper or lower)
     * @return the properly formated message
     */
    static std::string getSpeedLimit(const Axis& axis, const AxisLimit& limit);

    /**
     * Set the upper and lower speed bounds for desired speed commands.
     * @param val the desired speed bound
     * @param axis the axis to be used
     * @param limit the limit to be queried (upper or lower)
     * @return the properly formated message
     */
    static std::string setSpeedLimit(const int& val, const Axis& axis, const AxisLimit& limit);

    /**
     * Query the current speed control mode.
     * @return the properly formated message
     */
    static std::string getCtrlMode();

    /**
     * Set the current speed control mode (independent or pure speed)
     */
    static std::string setCtrlMode(const CtrllMode& mode);
};

} /* namespace ptu */

#endif /* CMD_H_ */
