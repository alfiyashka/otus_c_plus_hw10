#ifndef _COMMAND_PARSER_HPP__
#define _COMMAND_PARSER_HPP__

#include <string>
#include <vector>

#include "CmdFileWriter.hpp"
#include "CmdParserState.hpp"
#include "CmdRange.hpp"

#include <deque>
#include <mutex>
#include <condition_variable>


#include <tuple>

#include <thread>
/**
 * parses commands
 */
class CommandParser
{
public:
    using cmds_t = std::vector<std::string>;
    using bulkCmds_t = std::deque<cmds_t>;

    using CmdRanges_t = std::deque<CmdRange>;

private:
    const int m_packageSize; // defines package size    
    cmds_t m_currentBulkCmds; // current bulk of parsed commands
    bulkCmds_t m_bulkCmds; //queue of list commands to handle

    CmdRanges_t m_cmdRanges; //queue of static mode cmd ranges

    CmdFileWriter m_fileWriter; // writes command data into a file
    CmdParserState m_state; // current state

    std::mutex m_mutex;
    std::mutex m_mutexCmdRange;  // used only for parse static cmds in range
    std::condition_variable m_condV;
    std::condition_variable m_condVFile;

    std::atomic<bool> m_bulkFilled;
    std::atomic<bool> m_bulkInQueue;

    std::thread m_logThread;  // thread for print information into a console
    std::thread m_file1Thread, m_file2Thread; // threads for print information into a file

    std::atomic<bool> m_stopped;

    std::atomic<bool> m_switcher;

   
    /**
     * generates filename with current time
     */
    std::string generateFileNameWithCurrentTime(const std::string& postfix = "") const;

    /**
     * writes data on file
     * @param data - data to write
     */
    void writeOnFile(const std::string& data);

    bool isStaticModeComplete() { return m_state.isStaticMode() && m_packageSize == m_currentBulkCmds.size(); }

    /**
     * performs complete package operations
     */
    void completePackage();

    bool cmdIsOnlySpace(const std::string &cmd) const;

    /**
     * writes completed bulk data into console
     */
    void log_work();

    /**
     * writes completed bulk data into file
     */
    void file_work();

    /**
     * get current thread id
     */
    std::string getThreadId() const;

    void initThreads();

    void parseCmd(const std::string &cmd);

    /**
     * checks input data is range cmd or not and return range parameters to generate cmd in static mode
     */
    std::tuple<bool, CmdRange> tryToParseRangeCmd(const std::string &input) const;

    /**
     * generates cmds from range
     */
    void generateCmdsFromRange(const CmdRange& cmdRange);

public:
    CommandParser(const int packageSize)
     : m_packageSize(packageSize)
      , m_bulkFilled(false)
      , m_stopped(false)
      , m_bulkInQueue(false)
      , m_switcher(false)
    {
        initThreads();
    }

    CommandParser(const int packageSize, const std::string& resultDir)
     : m_packageSize(packageSize)
       , m_fileWriter(resultDir)
       , m_bulkFilled(false)
       , m_stopped(false)
       , m_bulkInQueue(false)
       , m_switcher(false)
    {
        initThreads();
    }

    

    ~CommandParser();
   
    /**
     * performs complete parse job
     */
    void endJob();

    /**
     * performs initialization actions
     */
    void init();    

    /**
     * parses commands
     * @param cmd - command
     */
    void parse(const std::string& cmd);

    void stop(); 
};



#endif