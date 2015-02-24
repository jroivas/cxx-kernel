#include "storage.hh"

void Storage::addDevice(Device *dev)
{
    if (dev == NULL) return;
    if (m_devices == NULL) {
        m_devices = (Device*)dev;
        return;
    }

    Device *tmp_dev = m_devices;
    while (tmp_dev->next != NULL) {
        tmp_dev = tmp_dev->next;
    }

    tmp_dev->next = dev;
}

uint32_t Storage::numDevices()
{
    Device *dev = m_devices;
    if (dev == NULL) {
        return 0;
    }

    uint32_t num = 1;

    while (dev->next != NULL) {
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
    if (dev == NULL) {
        return NULL;
    }

    return dev->next;
}
