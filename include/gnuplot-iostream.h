/*
Copyright (c) 2013 Daniel Stahlke

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef GNUPLOT_IOSTREAM_H
#define GNUPLOT_IOSTREAM_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

class Gnuplot {
public:
    Gnuplot(const std::string &gnuplot_cmd = "gnuplot -persist") {
#ifdef _WIN32
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        HANDLE hChildStdInRd, hChildStdInWr;
        if (!CreatePipe(&hChildStdInRd, &hChildStdInWr, &sa, 0)) {
            throw std::runtime_error("CreatePipe failed");
        }
        if (!SetHandleInformation(hChildStdInWr, HANDLE_FLAG_INHERIT, 0)) {
            throw std::runtime_error("SetHandleInformation failed");
        }

        STARTUPINFOW si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdInput = hChildStdInRd;
        si.dwFlags |= STARTF_USESTDHANDLES;

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        std::wstring wcmd(gnuplot_cmd.begin(), gnuplot_cmd.end());
        if (!CreateProcessW(NULL, &wcmd[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            throw std::runtime_error("Could not start gnuplot. Make sure gnuplot is installed and in PATH.");
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hChildStdInRd);

        int fd = _open_osfhandle(reinterpret_cast<intptr_t>(hChildStdInWr), 0);
        gnuplot_pipe = _fdopen(fd, "w");
#else
        gnuplot_pipe = popen(gnuplot_cmd.c_str(), "w");
#endif
        if (!gnuplot_pipe) {
            throw std::runtime_error("Could not start gnuplot. Make sure gnuplot is installed and in PATH.");
        }
    }

    ~Gnuplot() {
        if (gnuplot_pipe) {
#ifdef _WIN32
            fclose(gnuplot_pipe);
#else
            pclose(gnuplot_pipe);
#endif
        }
    }

    Gnuplot& operator<<(const std::string &cmd) {
        if (!gnuplot_pipe) {
            throw std::runtime_error("Gnuplot is not running");
        }
        fputs((cmd + "\n").c_str(), gnuplot_pipe);
        fflush(gnuplot_pipe);
        return *this;
    }

    template<typename T>
    void send1d(const std::vector<T> &data) {
        *this << "plot '-' with lines";
        for (const auto &val : data) {
            fprintf(gnuplot_pipe, "%f\n", static_cast<double>(val));
        }
        fprintf(gnuplot_pipe, "e\n");
        fflush(gnuplot_pipe);
    }
    
    // Overload per vector<pair<double,double>> - NO plot command interno
    void send1d(const std::vector<std::pair<double, double>>& data) {
        for (const auto& point : data) {
            fprintf(gnuplot_pipe, "%f %f\n", point.first, point.second);
        }
        fprintf(gnuplot_pipe, "e\n");
        fflush(gnuplot_pipe);
    }

    // template<typename T1, typename T2>
    // void send1d(const std::vector<std::pair<T1, T2>> &data) {
    //     *this << "plot '-' with lines";
    //     for (const auto &point : data) {
    //         fprintf(gnuplot_pipe, "%f %f\n", 
    //             static_cast<double>(point.first), 
    //             static_cast<double>(point.second));
    //     }
    //     fprintf(gnuplot_pipe, "e\n");
    //     fflush(gnuplot_pipe);
    // }

    // Modifica necessaria per emulare l'hold on di MATLAB
    template <typename T1, typename T2>
    void send1d_raw(const std::vector<std::pair<T1, T2>> &data)
    {
        for (const auto &point : data)
        {
            fprintf(gnuplot_pipe, "%f %f\n",
                    static_cast<double>(point.first),
                    static_cast<double>(point.second));
        }
        fprintf(gnuplot_pipe, "e\n");
        fflush(gnuplot_pipe);
    }

    void flush() {
        if (gnuplot_pipe) {
            fflush(gnuplot_pipe);
        }
    }

private:
    FILE *gnuplot_pipe;

    Gnuplot(const Gnuplot &) = delete;
    Gnuplot& operator=(const Gnuplot &) = delete;
};

#endif // GNUPLOT_IOSTREAM_H
