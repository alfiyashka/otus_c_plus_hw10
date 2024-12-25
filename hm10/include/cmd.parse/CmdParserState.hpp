#ifndef _CMD_PARSER_STATE_HPP__
#define _CMD_PARSER_STATE_HPP__

#include <string>

enum class ParserModeState
{
    STATIC,
    DYNAMIC_START,
    DYNAMIC_RUNNING,
    DYNAMIC_END
};

/**
 * defines command parser's mode state
 */

class CmdParserState
{
private:
    
    ParserModeState m_state; // current command parser's mode state
    int m_dynamicModeCounter; // used to count '{' or '}'

public:
    CmdParserState() : m_state(ParserModeState::STATIC), m_dynamicModeCounter(0)
    {
    }

    bool isDynamicModeCompleted() const { return m_state == ParserModeState::DYNAMIC_END; }
    
    bool isDynamicModeStarting() const { return m_state == ParserModeState::DYNAMIC_START; }

    bool isStaticMode() const { return m_state == ParserModeState::STATIC; }

    /**
     * tries to modify current state by input parameter
     * @param tag - some input value what can be '{' of '}' to allow modify a current state from static to dynamic
     * @return true if state was changed, else false 
     */
    bool modifyState(const std::string &tag);

    /**
     * tries to end dynamic mode
     * @return true if state was changed, else false 
     */
    bool tryEndDynamicMode();
};

#endif
