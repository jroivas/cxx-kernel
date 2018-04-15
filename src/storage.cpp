#include "storage.hh"

void Storage::addDevice(Device *dev)
{
    if (dev == nullptr) return;
    if (m_devices == nullptr) {
        m_devices = (Device*)dev;
        return;
    }

    Device *tmp_dev = m_devices;
    while (tmp_dev->next != nullptr) {
        tmp_dev = tmp_dev->next;
    }

    tmp_dev->next = dev;
}

uint32_t Storage::numDevices()
{
    Device *dev = m_devices;
    if (dev == nullptr) {
        return 0;
    }

    uint32_t num = 1;

    while (dev->next != nullptr) {
        dev = dev->next;
        ++num;
    }
    return num;
}

Storage::Device *Storage::getDevice()
{
    return m_devices;
}

Storage::Device *Storage::nextDevice(Device *dev)
{
    if (dev == nullptr) {
        return nullptr;
    }

    return dev->next;
}
