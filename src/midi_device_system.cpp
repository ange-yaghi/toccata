#include "../include/midi_device_system.h"

#include "../include/midi_callback.h"

#include <sstream>

toccata::MidiDeviceSystem::MidiDeviceSystem() {
    m_connected = false;
    m_deviceName = L"";
}

toccata::MidiDeviceSystem::~MidiDeviceSystem() {
    /* void */
}

void toccata::MidiDeviceSystem::Reset() {
    m_devices.clear();

    midiInStop(m_device);
    midiInClose(m_device);

    m_device = nullptr;
}

bool toccata::MidiDeviceSystem::Refresh() {
    m_devices.clear();

    const unsigned int deviceCount = midiInGetNumDevs();
    for (unsigned int i = 0; i < deviceCount; ++i) {
        MIDIINCAPS deviceInfo;
        MMRESULT result = midiInGetDevCaps(i, &deviceInfo, sizeof(MIDIINCAPS));

        if (result != MMSYSERR_NOERROR) {
            m_devices.clear();

            if (result == MMSYSERR_BADDEVICEID) {
                RaiseError(ErrorCode::DeviceListChangedDuringUpdate, L"Device list changed during update");
                return false;
            }
            else {
                RaiseError(ErrorCode::CouldNotEnumerateDevices, L"Could not enumerate devices");
                return false;
            }
        }

        MidiDevice device;
        device.Index = i;
        device.Name = deviceInfo.szPname;

        m_devices.push_back(device);
    }

    if (IsConnected()) {
        const bool found = (FindDevice(m_deviceName) != -1);

        if (!found) {
            midiInStop(m_device);
            midiInClose(m_device);

            m_connected = false;
        }
    }

    return true;
}

int toccata::MidiDeviceSystem::FindDevice(const std::wstring &deviceName) const {
    const int deviceCount = GetDeviceCount();
    for (int i = 0; i < deviceCount; ++i) {
        if (m_devices[i].Name == m_deviceName) {
            return i;
        }
    }

    return -1;
}

bool toccata::MidiDeviceSystem::Connect(int deviceIndex) {
    if (!Refresh()) {
        return false;
    }

    if (deviceIndex < 0 || deviceIndex >= GetDeviceCount()) {
        std::wstringstream ss;
        ss << "Device index [" << deviceIndex << "] out of range";
        RaiseError(ErrorCode::CouldNotFindPreferredDevice, ss.str());

        return false;
    }

    m_deviceName = m_devices[deviceIndex].Name;

    if (IsConnected()) {
        if (!Disconnect()) {
            return false;
        }
    }

    MMRESULT result = midiInOpen(
        &m_device, 
        deviceIndex,
        (DWORD_PTR)&MidiCallback, 
        NULL, 
        CALLBACK_FUNCTION | MIDI_IO_STATUS
    );

    if (result != MMSYSERR_NOERROR) {
        std::wstringstream ss;
        ss << "Could not connect to: " << m_deviceName;
        RaiseError(ErrorCode::CouldNotConnectToDevice, ss.str());

        m_connected = false;
        return false;
    }

    result = midiInStart(m_device);

    if (result != MMSYSERR_NOERROR) {
        RaiseError(ErrorCode::CouldNotStartMidiInput, L"Could not start midi input");

        midiInClose(m_device);
        m_connected = false;

        return false;
    }

    m_connected = true;
    return true;
}

bool toccata::MidiDeviceSystem::Reconnect() {
    if (!Refresh()) {
        return false;
    }

    if (IsConnected()) {
        if (!Disconnect()) {
            return false;
        }
    }

    const int deviceIndex = FindDevice(m_deviceName);
    if (deviceIndex == -1) {
        std::wstringstream ss;
        ss << "Could not find preferred device: " << m_deviceName;
        RaiseError(ErrorCode::CouldNotFindPreferredDevice, ss.str());

        return false;
    }

    return Connect(deviceIndex);
}

bool toccata::MidiDeviceSystem::Disconnect() {
    if (!IsConnected()) {
        RaiseError(ErrorCode::CouldNotDisconnect, L"Not connected to a device");

        return false;
    }

    MMRESULT result = midiInStop(m_device);

    if (result != MMSYSERR_NOERROR) {
        std::wstringstream ss;
        ss << "Could not stop midi input from device: " << m_deviceName;
        RaiseError(ErrorCode::CouldNotDisconnect, ss.str());

        return false;
    }

    result = midiInClose(m_device);

    if (result != MMSYSERR_NOERROR) {
        std::wstringstream ss;
        ss << "Could not disconnect from device: " << m_deviceName;
        RaiseError(ErrorCode::CouldNotDisconnect, ss.str());

        return false;
    }
    else {
        m_connected = false;
        m_device = nullptr;
        return true;
    }
}

void toccata::MidiDeviceSystem::ResetError() {
    m_lastErrorCode = ErrorCode::None;
    m_lastErrorMessage = L"";
}

void toccata::MidiDeviceSystem::RaiseError(ErrorCode error, const std::wstring errorMessage) {
    m_lastErrorCode = error;
    m_lastErrorMessage = errorMessage;
}
