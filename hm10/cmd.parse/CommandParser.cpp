#include "../include/cmd.parse/CommandParser.hpp"


#include <stdexcept>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <fstream>
#include <future>


std::string CommandParser::generateFileNameWithCurrentTime(const std::string& postfix) const
{
    const auto current_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return "bulk" + std::to_string(current_time) + postfix + ".log";
}

void CommandParser::writeOnFile(const std::string &data)
{
    const auto filename = generateFileNameWithCurrentTime();
    m_fileWriter.write(filename, data);
}

void CommandParser::log_work()
{
    while(!m_stopped || m_bulkFilled)
    {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_condV.wait(lk, [this]() ->bool { return m_bulkFilled || m_stopped; } );
            
            if (!m_bulkFilled)
            {
                continue;
            }
            const auto threadIdStr = getThreadId();
            std::cout << "Thread log work. Thread Id " << threadIdStr 
                << ". CmdParser object address is " << std::hex << this << std::endl;

            bool needDelimiter = false;
            for (auto iCmd : m_currentBulkCmds)
            {
                if (needDelimiter)
                {
                    std::cout << ", ";
                }
                std::cout << iCmd;
                needDelimiter = true;
            }
            
            std::cout << std::endl << std::endl;
            m_bulkFilled = false;
            m_currentBulkCmds.clear();
            lk.unlock();
            m_condV.notify_all();
        }

    }   

}

std::string CommandParser::getThreadId() const
{
    std::ostringstream idStream;
    idStream << "Thread_" << std::this_thread::get_id();

    return idStream.str();
}

void CommandParser::file_work()
{
    while (!m_stopped || m_bulkInQueue)
    {
        cmds_t cmds;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condVFile.wait(lock, [this]
                         { return m_bulkInQueue || m_stopped; });
            if (m_bulkCmds.empty())
            {
                continue;
            }

            cmds = m_bulkCmds.back();
            m_bulkCmds.pop_back();
            if (m_bulkCmds.empty())
            {
                m_bulkInQueue = false;
            }
            m_condVFile.notify_all();
        }
        if (!cmds.empty())
        {
            const auto threadIdStr = getThreadId();

            std::cout << "Thread file work. Thread Id " << threadIdStr
                      << ". CmdParser object address is " << std::hex << this << std::endl;

            const auto filename = "Bulk_info_" + generateFileNameWithCurrentTime(threadIdStr);

            std::ofstream file;
            file.open(filename, std::fstream::out);

            for (const auto &data : cmds)
            {
                file << data << std::endl;
            }
            file.close();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
}

void CommandParser::completePackage()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condV.wait(lock, [this ] { return !m_bulkFilled; });        
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_currentBulkCmds.empty())
    {
        m_bulkCmds.push_back(m_currentBulkCmds);
        m_bulkFilled = true;
        m_bulkInQueue = true;
        m_condV.notify_all();
        m_condVFile.notify_all();           
    }
      
}

void CommandParser::init()
{
    if (m_packageSize <= 0)
    {
        throw std::runtime_error("Incorrect package size");
    }
    m_currentBulkCmds.reserve(m_packageSize);
    m_fileWriter.init();
}

void CommandParser::initThreads()
{
    m_logThread = std::thread(&CommandParser::log_work, this);
    m_file1Thread = std::thread(&CommandParser::file_work, this);
    m_file2Thread = std::thread(&CommandParser::file_work, this); 
}

bool CommandParser::cmdIsOnlySpace(const std::string &cmd) const 
{
    auto truncSpace = cmd;
    std::erase(truncSpace, ' ');
    return truncSpace.empty();
}

std::tuple<bool, CmdRange> CommandParser::tryToParseRangeCmd(const std::string &input) const
{
    const std::string commandName("seq");
    const auto foundCmdName = input.find(commandName);
    if (foundCmdName == std::string::npos)
    {
        return std::make_tuple(false, CmdRange());
    }
    auto endCmdName = input.find("\n");
    if (endCmdName == std::string::npos)
    {
        endCmdName = input.length() - 1;
    }
    const std::size_t offset = foundCmdName + commandName.length() + 1;
    std::stringstream rangesStream(std::string(input.c_str() + offset, endCmdName - offset));
    int leftEdge = 0, rightEdge = 0;
    rangesStream >> leftEdge;
    rangesStream >> rightEdge;
    if (leftEdge == 0 && rightEdge == 0)
    {
        throw std::runtime_error("Error. Incorrect input: range's edge numbers are zero" );
    }

    if (leftEdge > rightEdge)
    {
        throw std::runtime_error("Error. Incorrect input: left edge number is bigger than right edge number");
    }
    std::cout << "Input cmd sequence params " << std::to_string(leftEdge) << "  " << std::to_string(rightEdge) << std::endl;
    return std::make_tuple(true, CmdRange(leftEdge, rightEdge));
}

void CommandParser::generateCmdsFromRange(const CmdRange& cmdRange)
{
    for (int i = cmdRange.left(); i <= cmdRange.right(); i++)
    {
        const std::string cmd = std::to_string(i);
        parseCmd(cmd);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void CommandParser::parse(const std::string &cmd)
{
    const auto [parseResult, rangeCmd] = tryToParseRangeCmd(cmd);
    if (parseResult)
    {
        m_cmdRanges.push_back(rangeCmd);

        std::this_thread::sleep_for(std::chrono::seconds(3));   

        if (m_state.isStaticMode())
        {
            {
                std::unique_lock<std::mutex> lock(m_mutexCmdRange);
                std::vector<std::thread> threads;
                for (auto cmdRange : m_cmdRanges)
                {
                    threads.emplace_back(std::thread(
                        &CommandParser::generateCmdsFromRange, this, cmdRange));
                }
                for (auto &t : threads)
                {
                    if (t.joinable())
                        t.join();
                }
                m_cmdRanges.clear(); 
                endJob();
            }
        }
    }
    else
    {
        parseCmd(cmd);
    } 
}

void CommandParser::parseCmd(const std::string &cmd)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condV.wait(lock, [this ] { return !m_bulkFilled || m_currentBulkCmds.empty(); });        
    }

    
    if (cmd.empty())
    {
        //std::cerr << "Encountered empty command";
        return;
    }
    if (cmdIsOnlySpace(cmd))
    {
        return;
    }

    if (m_state.modifyState(cmd))
    {
        if (m_state.isDynamicModeStarting()
            || m_state.isDynamicModeCompleted())
        {
            completePackage();
        }
        return;
    }
    writeOnFile(cmd);
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentBulkCmds.emplace_back(cmd);
    }
    

    if (isStaticModeComplete())
    {
        completePackage();
    }
}

void CommandParser::endJob()
{
    if (m_state.isStaticMode() || m_state.isDynamicModeCompleted())
    {
        completePackage();
    }
}

void CommandParser::stop()
{
    m_stopped = true;
    m_condV.notify_all();
    m_condVFile.notify_all();
    if (m_logThread.joinable())
        m_logThread.join();
    if (m_file1Thread.joinable())
        m_file1Thread.join();
    if (m_file2Thread.joinable()) 
        m_file2Thread.join();
}


CommandParser::~CommandParser()
{
    endJob();
    stop();
}