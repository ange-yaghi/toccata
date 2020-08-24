#ifndef TOCCATA_CORE_MIDI_DEVICE_SYSTEM_H
#define TOCCATA_CORE_MIDI_DEVICE_SYSTEM_H

#include <Windows.h>

#include <string>
#include <vector>

#pragma comment(lib, "winmm.lib")

namespace toccata {

    class MidiDeviceSystem {
    public:
        struct MidiDevice {
            std::wstring Name;
            int Index;
        };

        enum class ErrorCode {
            None,
            CouldNotEnumerateDevices,
            CouldNotDisconnect,
            CouldNotFindPreferredDevice,
            CouldNotConnectToDevice,
            CouldNotStartMidiInput,
            NotConnected,
            DeviceListChangedDuringUpdate
        };

    public:
        MidiDeviceSystem();
        ~MidiDeviceSystem();

        void Reset();
        bool Refresh();
        int GetDeviceCount() const { return (int)m_devices.size(); }
        const MidiDevice &GetDevice(int index) { return m_devices[index]; }
        int FindDevice(const std::wstring &deviceName) const;

        std::wstring GetLastDeviceName() const { return m_deviceName; }

        bool IsConnected() const { return m_connected; }
        bool Connect(int deviceIndex);
        bool Reconnect();
        bool Disconnect();

        void ResetError();
        std::wstring GetLastErrorMessage() const { return m_lastErrorMessage; }
        ErrorCode GetLastErrorCode() const { return m_lastErrorCode; }

    protected:
        void RaiseError(ErrorCode error, const std::wstring errorMessage);

    protected:
        bool m_connected;
        std::wstring m_deviceName;
        HMIDIIN m_device;

        std::vector<MidiDevice> m_devices;

    protected:
        ErrorCode m_lastErrorCode;
        std::wstring m_lastErrorMessage;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MIDI_DEVICE_SYSTEM_H */
