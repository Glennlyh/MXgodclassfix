#include "Time.h"
#include <cctype>
#include <string>
#include <cstdio>
#include <algorithm>

using namespace std;

static inline string trimSpaces(const string& s) {
    size_t i = 0, j = s.size();
    while (i < j && isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && isspace(static_cast<unsigned char>(s[j - 1]))) --j;
    return s.substr(i, j - i);
}

Time Time::parse(const string& raw) {
    string s = trimSpaces(raw);
    if (s.empty()) return Time(-1);

    bool pm = false, am = false;
    {
        string lower; lower.reserve(s.size());
        for (char ch : s) lower.push_back((char)tolower((unsigned char)ch));

        auto ends_with = [&](const string& suf) {
            return lower.size() >= suf.size()
                && lower.substr(lower.size() - suf.size()) == suf;
            };

        if (ends_with("a.m.")) { am = true; s = s.substr(0, s.size() - 4); }
        else if (ends_with("am")) { am = true; s = s.substr(0, s.size() - 2); }
        else if (ends_with("p.m.")) { pm = true; s = s.substr(0, s.size() - 4); }
        else if (ends_with("pm")) { pm = true; s = s.substr(0, s.size() - 2); }

        s = trimSpaces(s);
    }

    auto colon = s.find(':');
    if (colon != string::npos) {
        string hh = s.substr(0, colon);
        string mm = s.substr(colon + 1);
        if (hh.empty() || mm.empty()) return Time(-1);
        for (char c : hh) if (!isdigit((unsigned char)c)) return Time(-1);
        for (char c : mm) if (!isdigit((unsigned char)c)) return Time(-1);
        int H = stoi(hh);
        int M = stoi(mm);
        if (am || pm) {
            if (H < 1 || H > 12) return Time(-1);
            if (pm && H != 12) H += 12;
            if (am && H == 12) H = 0;
        }
        if (H < 0 || H > 23 || M < 0 || M > 59) return Time(-1);
        return Time(H * 60 + M);
    }

    for (char c : s) if (!isdigit((unsigned char)c)) return Time(-1);
    int val = stoi(s);

    int H = 0, M = 0;
    if (s.size() <= 2) {
        H = val; M = 0;
    }
    else if (s.size() == 3) {
        H = val / 100;
        M = val % 100;
    }
    else {
        H = val / 100;
        M = val % 100;
    }

    if (am || pm) {
        if (H < 1 || H > 12) return Time(-1);
        if (pm && H != 12) H += 12;
        if (am && H == 12) H = 0;
    }
    if (H < 0 || H > 23 || M < 0 || M > 59) return Time(-1);
    return Time(H * 60 + M);
}

string Time::hhmm12() const {
    if (minutes_ < 0) return "----";
    int H24 = minutes_ / 60;
    int M = minutes_ % 60;
    bool isPM = (H24 >= 12);
    int H12 = H24 % 12;
    if (H12 == 0) H12 = 12;

    char buf[16];
    snprintf(buf, sizeof(buf), "%d:%02d%s", H12, M, isPM ? "pm" : "am");
    return string(buf);
}

Time Time::fromHHMM(int hhmm) {
    if (hhmm < 0) return Time(-1);
    int H = hhmm / 100;
    int M = hhmm % 100;
    if (H < 0 || H > 23 || M < 0 || M > 59) return Time(-1);
    return Time(H * 60 + M);
}

Time Time::addMinutes(int delta) const {
    if (!isKnown()) return Time(-1);
    int m = minutes_ + delta;
    if (m < 0) m = 0;
    if (m > 24 * 60 - 1) m = 24 * 60 - 1;
    return Time(m);
}

int Time::compare(Time a, Time b) {
    if (!a.isKnown() && !b.isKnown()) return 0;
    if (!a.isKnown()) return 1;
    if (!b.isKnown()) return -1;
    if (a.minutes_ < b.minutes_) return -1;
    if (a.minutes_ > b.minutes_) return 1;
    return 0;
}

bool Time::inArrivalWindow(Time freightDepart, Time cargoDeadline, int earlyAllowanceMins) {
    if (!freightDepart.isKnown() || !cargoDeadline.isKnown()) return false;
    int lower = cargoDeadline.minutes_ - earlyAllowanceMins;
    if (lower < 0) lower = 0;
    return (freightDepart.minutes_ >= lower) && (freightDepart.minutes_ <= cargoDeadline.minutes_);
}
