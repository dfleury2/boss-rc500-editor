#pragma once

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace BossRC {

constexpr uint8_t kDeviceId = 0x10;
constexpr uint8_t kModelId[4] = {0x00, 0x00, 0x00, 0x77};   // RC-500 (RC-5 = 0x76)
constexpr uint8_t kCmdRQ1 = 0x11;
constexpr uint8_t kCmdDT1 = 0x12;
constexpr uint32_t kAddrEditMode = 0x7F700000;
constexpr auto kMidiPortName = "BOSS_RC-500";   // RC-5 : "BOSS_RC-5"
constexpr auto kVolumeLabel = L"BOSS RC-500";   // RC-5 : L"BOSS RC-5"

enum class EditMode : int { Off = 0, On = 1, Busy = 2, Unknown = -1 };

// --------------------------------------------------------------------------
inline uint8_t checksum(const std::vector<uint8_t>& addr_and_data)
{
    unsigned sum = 0;
    for (uint8_t b : addr_and_data)
        sum += b;
    return static_cast<uint8_t>((128 - (sum & 0x7F)) & 0x7F);
}

// --------------------------------------------------------------------------
inline void push_addr(std::vector<uint8_t>& v, uint32_t a)
{
    for (int s = 24; s >= 0; s -= 8)
        v.push_back(static_cast<uint8_t>(a >> s));
}

// --------------------------------------------------------------------------
inline std::vector<uint8_t> header(uint8_t cmd)
{
    std::vector<uint8_t> m{0xF0, 0x41, kDeviceId};
    m.insert(m.end(), std::begin(kModelId), std::end(kModelId));
    m.push_back(cmd);
    return m;
}

// --------------------------------------------------------------------------
// DT1 : écriture. Ex. dt1(kAddrEditMode, {1}) => edit mode ON
// --------------------------------------------------------------------------
inline std::vector<uint8_t> dt1(uint32_t addr, const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> body;
    push_addr(body, addr);
    body.insert(body.end(), data.begin(), data.end());
    auto m = header(kCmdDT1);
    m.insert(m.end(), body.begin(), body.end());
    m.push_back(checksum(body));
    m.push_back(0xF7);
    return m;
}

// --------------------------------------------------------------------------
// RQ1 : demande de lecture de `size` octets (taille codée sur 4 octets)
// --------------------------------------------------------------------------
inline std::vector<uint8_t> rq1(uint32_t addr, uint32_t size)
{
    std::vector<uint8_t> body;
    push_addr(body, addr);
    push_addr(body, size);
    auto m = header(kCmdRQ1);
    m.insert(m.end(), body.begin(), body.end());
    m.push_back(checksum(body));
    m.push_back(0xF7);
    return m;
}

// --------------------------------------------------------------------------
inline std::vector<uint8_t> edit_mode_on()
{
    return dt1(kAddrEditMode, {0x01});
}
inline std::vector<uint8_t> edit_mode_off()
{
    return dt1(kAddrEditMode, {0x00});
}
inline std::vector<uint8_t> edit_mode_query()
{
    return rq1(kAddrEditMode, 1);
}

// --------------------------------------------------------------------------
// Réponse DT1 de l'appareil pour l'adresse edit mode ?
// --------------------------------------------------------------------------
inline EditMode parse_edit_mode(const std::vector<uint8_t>& msg)
{
    auto h = header(kCmdDT1);
    if (msg.size() != h.size() + 4 + 1 + 1 + 1)
        return EditMode::Unknown;
    if (!std::equal(h.begin(), h.end(), msg.begin()))
        return EditMode::Unknown;
    size_t o = h.size();
    uint32_t a = (uint32_t(msg[o]) << 24) | (msg[o + 1] << 16) | (msg[o + 2] << 8) | msg[o + 3];
    if (a != kAddrEditMode)
        return EditMode::Unknown;
    switch (msg[o + 4]) {
        case 0:
            return EditMode::Off;
        case 1:
            return EditMode::On;
        case 2:
            return EditMode::Busy;
        default:;
    }
    return EditMode::Unknown;
}

}   // namespace BossRC

// ---------------------------------------------------------------------------
#ifdef _WIN32
// Les projets (Raylib, etc.) définissent souvent des macros d'exclusion qui
// retirent l'API MIDI de <mmsystem.h> : on les neutralise pour ce fichier.
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <windows.h>

#include <mmsystem.h>   // doit venir APRES <windows.h> (types UINT, DWORD_PTR, HMIDIOUT...)

#pragma comment(lib, "winmm.lib")

namespace BossRC {

// ---------------------------------------------------------------------------
class Connection
{
  public:
    // Ouvre les ports MIDI in/out dont le nom contient "BOSS_RC-500"
    // (Windows peut préfixer : "MIDIIN2 (BOSS_RC-500)").
    bool open()
    {
        close();

        for (UINT i = 0, n = (UINT)midiOutGetNumDevs(); i < n; ++i) {
            MIDIOUTCAPSW c{};
            if (midiOutGetDevCapsW(i, &c, sizeof c) == MMSYSERR_NOERROR && matches(c.szPname)) {
                if (midiOutOpen(&out_, i, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
                    break;
                out_ = nullptr;
            }
        }
        for (UINT i = 0, n = midiInGetNumDevs(); i < n; ++i) {
            MIDIINCAPSW c{};
            if (midiInGetDevCapsW(i, &c, sizeof c) == MMSYSERR_NOERROR && matches(c.szPname)) {
                if (midiInOpen(&in_, i, (DWORD_PTR)&Connection::inProc, (DWORD_PTR)this, CALLBACK_FUNCTION) == MMSYSERR_NOERROR) {
                    inHdr_ = MIDIHDR{};
                    inHdr_.lpData = (LPSTR)inBuf_;
                    inHdr_.dwBufferLength = sizeof inBuf_;
                    midiInPrepareHeader(in_, &inHdr_, sizeof inHdr_);
                    midiInAddBuffer(in_, &inHdr_, sizeof inHdr_);
                    midiInStart(in_);
                    break;
                }
                in_ = nullptr;
            }
        }
        if (!out_ || !in_) {
            close();
            return false;
        }

        return true;
    }

    void close()
    {
        if (in_) {
            midiInStop(in_);
            midiInReset(in_);
            midiInUnprepareHeader(in_, &inHdr_, sizeof inHdr_);
            midiInClose(in_);
            in_ = nullptr;
        }
        if (out_) {
            midiOutReset(out_);
            midiOutClose(out_);
            out_ = nullptr;
        }
    }

    bool send(std::vector<uint8_t> msg)
    {
        if (!out_)
            return false;
        MIDIHDR h{};
        h.lpData = (LPSTR)msg.data();
        h.dwBufferLength = (DWORD)msg.size();
        if (midiOutPrepareHeader(out_, &h, sizeof h) != MMSYSERR_NOERROR)
            return false;
        bool ok = midiOutLongMsg(out_, &h, sizeof h) == MMSYSERR_NOERROR;
        while (ok && !(h.dwFlags & MHDR_DONE))
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        midiOutUnprepareHeader(out_, &h, sizeof h);
        return ok;
    }

    EditMode queryEditMode(int timeoutMs = 500)
    {
        {
            std::lock_guard<std::mutex> l(m_);
            last_ = EditMode::Unknown;
        }
        if (!send(edit_mode_query()))
            return EditMode::Unknown;
        std::unique_lock<std::mutex> l(m_);
        cv_.wait_for(l, std::chrono::milliseconds(timeoutMs), [&] { return last_ != EditMode::Unknown; });
        return last_;
    }

    // Équivalent de la machine à états de connect_manager.js :
    // envoie edit mode ON jusqu'à confirmation (l'appli renvoie ON si l'appareil répond OFF).
    bool enterEditMode(int timeoutMs = 10000)
    {
        auto t0 = std::chrono::steady_clock::now();
        while (elapsed(t0) < timeoutMs) {
            EditMode m = queryEditMode();
            if (m == EditMode::On)
                return true;
            if (m != EditMode::Busy)
                send(edit_mode_on());
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        return false;
    }

    // Attend l'apparition du volume USB (label "BOSS RC-500", ou à défaut un
    // lecteur contenant un dossier "ROLAND", comme le fait instrument.js).
    static bool waitForVolume(std::string& drive, int timeoutMs = 15000)
    {
        auto t0 = std::chrono::steady_clock::now();
        while (elapsed(t0) < timeoutMs) {
            if (findVolume(drive))
                return true;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        return false;
    }

    static bool findVolume(std::string& drive)
    {
        wchar_t buf[512];
        DWORD n = GetLogicalDriveStringsW(512, buf);
        if (!n || n > 512)
            return false;
        for (wchar_t* p = buf; *p; p += wcslen(p) + 1) {
            wchar_t label[MAX_PATH + 1] = L"";
            if (GetVolumeInformationW(p, label, MAX_PATH, nullptr, nullptr, nullptr, nullptr, 0) && wcscmp(label, kVolumeLabel) == 0) {
                drive = WStringToString(p);
                return true;
            }
        }
        return false;
    }

    // Démontage du volume (équivalent $native.fs.unmount), à faire AVANT edit mode OFF.
    static bool unmountVolume(const std::string& drive /* "E:\\" */)
    {
        const auto path = StringToWString(R"(\\.\)" + drive.substr(0, 2));
        HANDLE h =
            CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (h == INVALID_HANDLE_VALUE)
            return false;
        DWORD r;
        bool ok = DeviceIoControl(h, FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &r, nullptr) &&
                  DeviceIoControl(h, FSCTL_DISMOUNT_VOLUME, nullptr, 0, nullptr, 0, &r, nullptr);
        PREVENT_MEDIA_REMOVAL pmr{FALSE};
        DeviceIoControl(h, IOCTL_STORAGE_MEDIA_REMOVAL, &pmr, sizeof pmr, nullptr, 0, &r, nullptr);
        DeviceIoControl(h, IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0, nullptr, 0, &r, nullptr);
        CloseHandle(h);
        return ok;
    }

    bool leaveEditMode() { return send(edit_mode_off()); }

  private:
    static bool matches(const wchar_t* name)
    {
        std::wstring n = name, k(kMidiPortName, kMidiPortName + strlen(kMidiPortName));
        return n.find(k) != std::wstring::npos;
    }
    // void closeHandle()
    // {
    //     if (!handle_)
    //         return;
    //     if (claimed_)
    //         libusb_release_interface(handle_, interface_);
    //     claimed_ = false;
    //     if (detachedKernelDriver_)
    //         libusb_attach_kernel_driver(handle_, interface_);
    //     detachedKernelDriver_ = false;
    //     libusb_close(handle_);
    //     handle_ = nullptr;
    //     interface_ = -1;
    //     altSetting_ = -1;
    //     epIn_ = epOut_ = 0;
    // }

    static int elapsed(std::chrono::steady_clock::time_point t0)
    {
        return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count());
    }
    static void CALLBACK inProc(HMIDIIN h, UINT msg, DWORD_PTR inst, DWORD_PTR p1, DWORD_PTR)
    {
        auto* self = reinterpret_cast<Connection*>(inst);
        if (msg != MIM_LONGDATA)
            return;
        auto* hdr = reinterpret_cast<MIDIHDR*>(p1);
        if (hdr->dwBytesRecorded) {
            std::vector<uint8_t> v((uint8_t*)hdr->lpData, (uint8_t*)hdr->lpData + hdr->dwBytesRecorded);
            EditMode m = parse_edit_mode(v);
            if (m != EditMode::Unknown) {
                std::lock_guard<std::mutex> l(self->m_);
                self->last_ = m;
                self->cv_.notify_all();
            }
        }
        if (self->in_)
            midiInAddBuffer(h, hdr, sizeof *hdr);   // ré-arme le buffer
    }

    HMIDIOUT out_ = nullptr;
    HMIDIIN in_ = nullptr;
    MIDIHDR inHdr_{};
    uint8_t inBuf_[256] = {};
    std::mutex m_;
    std::condition_variable cv_;
    EditMode last_ = EditMode::Unknown;

    // Fonction utilitaire pour convertir un std::wstring en std::string (UTF-8)
    static std::string WStringToString(const std::wstring& wstr)
    {
        if (wstr.empty())
            return {};

        // Obtenir la taille nécessaire pour le buffer UTF-8
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);

        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), strTo.data(), size_needed, nullptr, nullptr);

        return strTo;
    }

    // Fonction utilitaire pour convertir un std::string (UTF-8) en std::wstring (WCHAR)
    static std::wstring StringToWString(const std::string& str)
    {
        if (str.empty())
            return {};

        // 1. Obtenir la taille nécessaire pour le buffer wchar_t
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);

        // 2. Allouer le wstring avec la taille requise et effectuer la conversion
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), wstrTo.data(), size_needed);

        return wstrTo;
    }
};

}   // namespace BossRC
#elif defined(__linux__)
// Linux : MIDI USB direct via libusb (aucune dépendance ALSA/PipeWire).
// Compilation : $(pkg-config --cflags --libs libusb-1.0)
// Attention : l'interface MIDI USB est détachée temporairement du pilote noyau
// pendant la connexion, ce qui peut rendre le MIDI indisponible aux autres applis.
#include <cctype>
#include <chrono>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <libusb-1.0/libusb.h>
#include <thread>
#include <unistd.h>

namespace BossRC {

struct Volume
{
    std::string device;
    std::string mountpoint;
};

class Connection
{
  public:
    ~Connection() { close(); }
    Connection() = default;
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    // Cherche le BOSS RC-500 par ses descripteurs USB, puis l'interface USB MIDI
    // Streaming (classe 1, sous-classe 3) et ses endpoints bulk IN/OUT.
    bool open()
    {
        close();
        int rc = libusb_init(&ctx_);
        if (rc < 0) {
            ctx_ = nullptr;
            return false;
        }

        libusb_device** list = nullptr;
        const ssize_t count = libusb_get_device_list(ctx_, &list);
        if (count < 0) {
            close();
            return false;
        }

        bool found = false;
        for (ssize_t i = 0; i < count && !found; ++i) {
            libusb_device* dev = list[i];
            libusb_device_descriptor dd{};
            if (libusb_get_device_descriptor(dev, &dd) != 0)
                continue;

            libusb_device_handle* candidate = nullptr;
            int rc = libusb_open(dev, &candidate);

            if (rc != LIBUSB_SUCCESS) {
                fprintf(stderr, "libusb_open a échoué : %s (%d)\n", libusb_error_name(rc), rc);

                fprintf(stderr, "Bus=%03u, périphérique=%03u\n", libusb_get_bus_number(dev), libusb_get_device_address(dev));

                continue;
            }

            if (candidate == nullptr) {
                fprintf(stderr, "libusb_open a réussi mais le handle est nul !\n");
                continue;
            }

            const std::string product = getString(candidate, dd.iProduct);
            const std::string manufacturer = getString(candidate, dd.iManufacturer);
            if (!isTargetDevice(product, manufacturer)) {
                libusb_close(candidate);
                continue;
            }

            libusb_config_descriptor* config = nullptr;
            int cr = libusb_get_active_config_descriptor(dev, &config);
            if (cr != 0) {
                // Certains périphériques n'ont pas encore de configuration active.
                if (libusb_get_config_descriptor(dev, 0, &config) != 0 || !config) {
                    libusb_close(candidate);
                    continue;
                }
            }

            int iface = -1, alt = -1;
            uint8_t epIn = 0, epOut = 0;
            for (uint8_t j = 0; j < config->bNumInterfaces && iface < 0; ++j) {
                const libusb_interface& in = config->interface[j];
                for (int a = 0; a < in.num_altsetting && iface < 0; ++a) {
                    const libusb_interface_descriptor& id = in.altsetting[a];
                    if (id.bInterfaceClass != LIBUSB_CLASS_AUDIO || id.bInterfaceSubClass != 3)
                        continue;   // USB MIDIStreaming
                    uint8_t inEp = 0, outEp = 0;
                    for (uint8_t e = 0; e < id.bNumEndpoints; ++e) {
                        const libusb_endpoint_descriptor& ed = id.endpoint[e];
                        if ((ed.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) != LIBUSB_TRANSFER_TYPE_BULK)
                            continue;
                        if ((ed.bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN)
                            inEp = ed.bEndpointAddress;
                        else
                            outEp = ed.bEndpointAddress;
                    }
                    if (inEp && outEp) {
                        iface = id.bInterfaceNumber;
                        alt = id.bAlternateSetting;
                        epIn = inEp;
                        epOut = outEp;
                    }
                }
            }
            libusb_free_config_descriptor(config);

            if (iface < 0) {
                libusb_close(candidate);
                continue;
            }

            handle_ = candidate;
            interface_ = iface;
            altSetting_ = alt;
            epIn_ = epIn;
            epOut_ = epOut;

            // Le pilote snd-usb-audio peut déjà posséder cette interface.
            // Détacher uniquement l'interface MIDI, pas l'interface stockage.
            const int active = libusb_kernel_driver_active(handle_, interface_);
            if (active == 1) {
                if (libusb_detach_kernel_driver(handle_, interface_) != 0) {
                    closeHandle();
                    continue;
                }
                detachedKernelDriver_ = true;
            }

            rc = libusb_claim_interface(handle_, interface_);
            if (rc != 0) {
                closeHandle();
                continue;
            }
            claimed_ = true;
            if (altSetting_ != 0 && libusb_set_interface_alt_setting(handle_, interface_, altSetting_) != 0) {
                closeHandle();
                continue;
            }
            found = true;
        }

        libusb_free_device_list(list, 1);
        if (!found) {
            close();
            return false;
        }
        usbRxBytes_.clear();
        sysexAssembly_.clear();
        completedSysex_.clear();
        return true;
    }

    void close()
    {
        closeHandle();
        if (ctx_) {
            libusb_exit(ctx_);
            ctx_ = nullptr;
        }
        interface_ = -1;
        altSetting_ = -1;
        epIn_ = epOut_ = 0;
        usbRxBytes_.clear();
        sysexAssembly_.clear();
        completedSysex_.clear();
    }

    // Encode un SysEx MIDI 1.0 en paquets USB-MIDI Event de 4 octets.
    bool send(const std::vector<uint8_t>& msg)
    {
        if (!handle_ || !claimed_ || msg.size() < 2 || msg.front() != 0xF0 || msg.back() != 0xF7)
            return false;

        std::vector<uint8_t> packets;
        packets.reserve(((msg.size() + 2) / 3) * 4);
        size_t pos = 0;
        while (pos < msg.size()) {
            const size_t remain = msg.size() - pos;
            const size_t n = remain > 3 ? 3 : remain;
            uint8_t cin = 0x04;   // SysEx start/continue, 3 octets
            if (remain <= 3)
                cin = static_cast<uint8_t>(0x04 + n);   // 1=>5, 2=>6, 3=>7 (end)
            packets.push_back(cin);                     // câble USB MIDI 0, CIN dans les 4 bits bas
            for (size_t j = 0; j < 3; ++j)
                packets.push_back(j < n ? msg[pos + j] : 0);
            pos += n;
        }

        int transferred = 0;
        const int rc = libusb_bulk_transfer(handle_, epOut_, packets.data(), static_cast<int>(packets.size()), &transferred, 1500);
        return rc == 0 && transferred == static_cast<int>(packets.size());
    }

    EditMode queryEditMode(int timeoutMs = 500)
    {
        if (!handle_ || !send(edit_mode_query()))
            return EditMode::Unknown;
        const auto t0 = std::chrono::steady_clock::now();
        while (elapsed(t0) < timeoutMs) {
            std::vector<uint8_t> m;
            if (nextSysex(m, 25)) {
                const EditMode e = parse_edit_mode(m);
                if (e != EditMode::Unknown)
                    return e;
            }
        }
        return EditMode::Unknown;
    }

    bool enterEditMode(int timeoutMs = 10000)
    {
        const auto t0 = std::chrono::steady_clock::now();
        while (elapsed(t0) < timeoutMs) {
            const EditMode m = queryEditMode();
            if (m == EditMode::On)
                return true;
            if (m != EditMode::Busy)
                send(edit_mode_on());
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        return false;
    }

    bool leaveEditMode() { return send(edit_mode_off()); }

    static bool findVolume(Volume& v)
    {
        char real[PATH_MAX];
        if (!realpath("/dev/disk/by-label/BOSS RC-500", real) && !realpath("/dev/disk/by-label/BOSS\\x20RC-500", real))
            return false;
        v.device = real;
        v.mountpoint = mountpointOf(v.device);
        return true;
    }

    static bool waitForVolume(Volume& v, int timeoutMs = 15000, bool autoMount = true)
    {
        const auto t0 = std::chrono::steady_clock::now();
        while (elapsed(t0) < timeoutMs) {
            if (findVolume(v)) {
                if (v.mountpoint.empty() && autoMount) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                    v.mountpoint = mountpointOf(v.device);
                    if (v.mountpoint.empty() && runUdisks("mount", v.device))
                        v.mountpoint = mountpointOf(v.device);
                }
                if (!v.mountpoint.empty() || !autoMount)
                    return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        return false;
    }

    static bool unmountVolume(const Volume& v)
    {
        if (v.device.empty())
            return false;
        ::sync();
        return runUdisks("unmount", v.device);
    }

  private:
    void closeHandle()
    {
        if (!handle_)
            return;
        if (claimed_)
            libusb_release_interface(handle_, interface_);
        claimed_ = false;
        if (detachedKernelDriver_)
            libusb_attach_kernel_driver(handle_, interface_);
        detachedKernelDriver_ = false;
        libusb_close(handle_);
        handle_ = nullptr;
        interface_ = -1;
        altSetting_ = -1;
        epIn_ = epOut_ = 0;
    }

    static int elapsed(std::chrono::steady_clock::time_point t0)
    {
        return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count());
    }

    static std::string getString(libusb_device_handle* h, uint8_t index)
    {
        if (!h || index == 0)
            return {};
        unsigned char buf[256]{};
        const int n = libusb_get_string_descriptor_ascii(h, index, buf, sizeof(buf));
        return n > 0 ? std::string(reinterpret_cast<char*>(buf), static_cast<size_t>(n)) : std::string{};
    }

    static bool isTargetDevice(const std::string& product, const std::string& manufacturer)
    {
        // Les descripteurs Roland affichent généralement « RC-500 » et « BOSS ».
        // Ne pas utiliser uniquement le VID/PID : cela varie selon les révisions.
        return product.find("RC-500") != std::string::npos &&
               (manufacturer.empty() || manufacturer.find("BOSS") != std::string::npos ||
                manufacturer.find("Roland") != std::string::npos || manufacturer.find("ROLAND") != std::string::npos);
    }

    static bool runUdisks(const char* verb, const std::string& dev)
    {
        if (dev.rfind("/dev/", 0) != 0 || dev.find_first_of("'\"\\$` ;&|\n") != std::string::npos)
            return false;
        const std::string cmd = std::string("udisksctl ") + verb + " -b " + dev + " >/dev/null 2>&1";
        return std::system(cmd.c_str()) == 0;
    }

    static std::string mountpointOf(const std::string& dev)
    {
        std::ifstream f("/proc/mounts");
        std::string d, mp, rest;
        while (f >> d >> mp && std::getline(f, rest)) {
            if (d != dev)
                continue;
            std::string out;
            for (size_t i = 0; i < mp.size(); ++i) {
                if (mp[i] == '\\' && i + 3 < mp.size() && std::isdigit(static_cast<unsigned char>(mp[i + 1])) &&
                    std::isdigit(static_cast<unsigned char>(mp[i + 2])) && std::isdigit(static_cast<unsigned char>(mp[i + 3]))) {
                    out += static_cast<char>(std::stoi(mp.substr(i + 1, 3), nullptr, 8));
                    i += 3;
                }
                else {
                    out += mp[i];
                }
            }
            return out;
        }
        return {};
    }

    // Lit une transaction USB MIDI, découpe les paquets de 4 octets et
    // reconstitue les SysEx à partir des CIN 4/5/6/7.
    bool nextSysex(std::vector<uint8_t>& out, unsigned timeoutMs)
    {
        if (!completedSysex_.empty()) {
            out = std::move(completedSysex_.front());
            completedSysex_.pop_front();
            return true;
        }
        if (!handle_ || !claimed_)
            return false;

        unsigned char buf[512];
        int transferred = 0;
        const int rc = libusb_bulk_transfer(handle_, epIn_, buf, sizeof(buf), &transferred, timeoutMs);
        if (rc != 0 && rc != LIBUSB_ERROR_TIMEOUT)
            return false;
        if (transferred > 0)
            usbRxBytes_.insert(usbRxBytes_.end(), buf, buf + transferred);

        size_t pos = 0;
        while (usbRxBytes_.size() - pos >= 4) {
            const uint8_t cin = usbRxBytes_[pos] & 0x0F;
            const uint8_t* p = usbRxBytes_.data() + pos + 1;
            switch (cin) {
                case 0x04:   // SysEx continue : trois octets
                    if (sysexAssembly_.empty() && p[0] != 0xF0)
                        break;
                    sysexAssembly_.insert(sysexAssembly_.end(), p, p + 3);
                    break;
                case 0x05:   // SysEx terminé, un octet
                case 0x06:   // SysEx terminé, deux octets
                case 0x07:   // SysEx terminé, trois octets
                    if (sysexAssembly_.empty() && p[0] != 0xF0)
                        break;
                    sysexAssembly_.insert(sysexAssembly_.end(), p, p + (cin - 0x04));
                    if (!sysexAssembly_.empty() && sysexAssembly_.front() == 0xF0 && sysexAssembly_.back() == 0xF7)
                        completedSysex_.push_back(std::move(sysexAssembly_));
                    sysexAssembly_.clear();
                    break;
                default:
                    break;   // ignorer les autres messages MIDI
            }
            pos += 4;
        }
        if (pos)
            usbRxBytes_.erase(usbRxBytes_.begin(), usbRxBytes_.begin() + static_cast<std::ptrdiff_t>(pos));

        if (completedSysex_.empty())
            return false;
        out = std::move(completedSysex_.front());
        completedSysex_.pop_front();
        return true;
    }

    libusb_context* ctx_ = nullptr;
    libusb_device_handle* handle_ = nullptr;
    int interface_ = -1;
    int altSetting_ = -1;
    uint8_t epIn_ = 0;
    uint8_t epOut_ = 0;
    bool claimed_ = false;
    bool detachedKernelDriver_ = false;
    std::vector<uint8_t> usbRxBytes_;
    std::vector<uint8_t> sysexAssembly_;
    std::deque<std::vector<uint8_t>> completedSysex_;
};

}   // namespace BossRC
#endif   // _WIN32 / __linux__
