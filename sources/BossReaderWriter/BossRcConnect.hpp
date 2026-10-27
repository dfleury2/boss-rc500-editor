#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace rc500 {

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

}   // namespace rc500

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

namespace rc500 {

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

}   // namespace rc500
#elif defined(__linux__)
// ---------------------------------------------------------------------------
// Linux : ALSA rawmidi (link: -lasound) + udev/udisks pour le volume USB.
// Droits : accès à /dev/snd/midiC*D* (groupe "audio"), montage via udisksctl
// (sans root) ou volume déjà monté automatiquement par le bureau.
#include <alsa/asoundlib.h>
#include <chrono>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <thread>
#include <unistd.h>

namespace rc500 {

// ---------------------------------------------------------------------------
struct Volume
{
    std::string device;       // ex. /dev/sdb1
    std::string mountpoint;   // vide si présent mais non monté
};

// ---------------------------------------------------------------------------
class Connection
{
  public:
    ~Connection() { close(); }

    // Cherche une carte ALSA dont le nom contient "RC-500" et ouvre son 1er port rawmidi.
    bool open()
    {
        close();
        for (int card = -1; snd_card_next(&card) >= 0 && card >= 0;) {
            char* name = nullptr;
            if (snd_card_get_name(card, &name) < 0 || !name)
                continue;
            std::string n = name;
            free(name);
            if (n.find("RC-500") == std::string::npos)
                continue;

            snd_ctl_t* ctl = nullptr;
            char hw[32];
            snprintf(hw, sizeof hw, "hw:%d", card);
            if (snd_ctl_open(&ctl, hw, 0) < 0)
                continue;
            int dev = -1;
            bool ok = false;
            while (snd_ctl_rawmidi_next_device(ctl, &dev) >= 0 && dev >= 0) {
                char port[48];
                snprintf(port, sizeof port, "hw:%d,%d", card, dev);
                if (snd_rawmidi_open(&in_, &out_, port, SND_RAWMIDI_NONBLOCK) >= 0) {
                    ok = true;
                    break;
                }
                in_ = nullptr;
                out_ = nullptr;
            }
            snd_ctl_close(ctl);
            if (ok)
                return true;
        }
        return false;
    }

    void close()
    {
        if (in_) {
            snd_rawmidi_close(in_);
            in_ = nullptr;
        }
        if (out_) {
            snd_rawmidi_close(out_);
            out_ = nullptr;
        }
        rx_.clear();
    }

    bool send(const std::vector<uint8_t>& msg)
    {
        if (!out_)
            return false;
        size_t done = 0;
        auto t0 = std::chrono::steady_clock::now();
        while (done < msg.size()) {
            ssize_t w = snd_rawmidi_write(out_, msg.data() + done, msg.size() - done);
            if (w > 0)
                done += (size_t)w;
            else if (w == -EAGAIN || w == 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            else
                return false;
            if (elapsed(t0) > 1000)
                return false;
        }
        snd_rawmidi_drain(out_);
        return true;
    }

    EditMode queryEditMode(int timeoutMs = 500)
    {
        if (!in_)
            return EditMode::Unknown;
        flushInput();
        if (!send(edit_mode_query()))
            return EditMode::Unknown;
        auto t0 = std::chrono::steady_clock::now();
        while (elapsed(t0) < timeoutMs) {
            std::vector<uint8_t> m;
            while (nextSysex(m)) {
                EditMode e = parse_edit_mode(m);
                if (e != EditMode::Unknown)
                    return e;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return EditMode::Unknown;
    }

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

    bool leaveEditMode() { return send(edit_mode_off()); }

    // Volume USB : /dev/disk/by-label/BOSS\x20RC-500 (udev), puis /proc/mounts.
    static bool findVolume(Volume& v)
    {
        char real[PATH_MAX];
        if (!realpath("/dev/disk/by-label/BOSS\\x20RC-500", real))
            return false;
        v.device = real;
        v.mountpoint = mountpointOf(v.device);
        return true;
    }

    // Attend l'apparition du volume, le monte via udisks si besoin.
    static bool waitForVolume(Volume& v, int timeoutMs = 15000, bool autoMount = true)
    {
        auto t0 = std::chrono::steady_clock::now();
        while (elapsed(t0) < timeoutMs) {
            if (findVolume(v)) {
                if (v.mountpoint.empty() && autoMount) {
                    // laisse une chance à l'automount du bureau avant de monter soi-même
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

    // À faire AVANT leaveEditMode().
    static bool unmountVolume(const Volume& v)
    {
        sync();
        return runUdisks("unmount", v.device);
    }

  private:
    static int elapsed(std::chrono::steady_clock::time_point t0)
    {
        return (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
    }

    static bool runUdisks(const char* verb, const std::string& dev)
    {
        if (dev.rfind("/dev/", 0) != 0 || dev.find_first_of("'\"\\$` ;&|\n") != std::string::npos)
            return false;
        std::string cmd = std::string("udisksctl ") + verb + " -b " + dev + " >/dev/null 2>&1";
        return std::system(cmd.c_str()) == 0;
    }

    static std::string mountpointOf(const std::string& dev)
    {
        std::ifstream f("/proc/mounts");
        std::string d, mp, rest;
        while (f >> d >> mp && std::getline(f, rest)) {
            if (d != dev)
                continue;
            std::string out;   // décode les échappements octaux (\040 = espace)
            for (size_t i = 0; i < mp.size(); ++i) {
                if (mp[i] == '\\' && i + 3 < mp.size() + 0 && isdigit((unsigned char)mp[i + 1])) {
                    out += (char)std::stoi(mp.substr(i + 1, 3), nullptr, 8);
                    i += 3;
                }
                else
                    out += mp[i];
            }
            return out;
        }
        return {};
    }

    void flushInput()
    {
        uint8_t b[256];
        while (in_ && snd_rawmidi_read(in_, b, sizeof b) > 0) {
        }
        rx_.clear();
    }

    // Lit les octets disponibles et extrait un message SysEx complet F0..F7.
    bool nextSysex(std::vector<uint8_t>& out)
    {
        uint8_t b[256];
        ssize_t n;
        while (in_ && (n = snd_rawmidi_read(in_, b, sizeof b)) > 0)
            rx_.insert(rx_.end(), b, b + n);
        auto s = std::find(rx_.begin(), rx_.end(), 0xF0);
        if (s == rx_.end()) {
            rx_.clear();
            return false;
        }
        auto e = std::find(s, rx_.end(), 0xF7);
        if (e == rx_.end()) {
            rx_.erase(rx_.begin(), s);
            return false;
        }
        out.assign(s, e + 1);
        rx_.erase(rx_.begin(), e + 1);
        return true;
    }

    snd_rawmidi_t* in_ = nullptr;
    snd_rawmidi_t* out_ = nullptr;
    std::vector<uint8_t> rx_;
};

}   // namespace rc500
#endif   // _WIN32 / __linux__
