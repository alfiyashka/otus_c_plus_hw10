#ifndef _COMMAND_RANGE_HPP__
#define _COMMAND_RANGE_HPP__

/**
 * defines range data from that cmds will be generated
 */
class CmdRange
{
    int m_leftEdge;
    int m_rightEdge;

public:
    CmdRange() noexcept : m_leftEdge(0), m_rightEdge(0)
    {
    }

    CmdRange(const int leftEdge, const int rightEdge) noexcept : m_leftEdge(leftEdge), m_rightEdge(rightEdge)
    {
    }

    bool isEmpty() const noexcept { return m_leftEdge == 0 && m_rightEdge == 0; }
    void set(const int leftEdge, const int rightEdge) noexcept
    {
        m_leftEdge = leftEdge;
        m_rightEdge = rightEdge;
    }

    int left() const noexcept { return m_leftEdge; }
    int right() const noexcept { return m_rightEdge; }

    void clear() noexcept
    {
        m_leftEdge = 0;
        m_rightEdge = 0;
    }
};

#endif