#include <fs/filesystem.hh>

void Dir::reset()
{
    m_index = 0;
}

void Dir::update()
{
}

Dirent *Dir::next()
{
    if (m_index >= m_items.size()) {
        return NULL;
    }

    Dirent *res = (Dirent *)m_items.at(m_index);
    ++m_index;

    return res;
}
