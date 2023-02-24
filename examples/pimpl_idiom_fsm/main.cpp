#include <cassert>
#include "pimpl.h"


int main(int argc, char *argv[])
{
    pimpl m_pimpl;

    m_pimpl.start(); // print: started
    assert(m_pimpl.is_started());

    m_pimpl.stop(); // print: stoped
    assert(!m_pimpl.is_started());

    return 0;
}
