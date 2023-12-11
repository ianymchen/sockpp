// tcp6echo.cpp
//
// Simple TCP echo client
//
// --------------------------------------------------------------------------
// This file is part of the "sockpp" C++ socket library.
//
// Copyright (c) 2014-2023 Frank Pagliughi
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// --------------------------------------------------------------------------

#include <iostream>
#include <string>

#include "sockpp/tcp6_connector.h"
#include "sockpp/version.h"

using namespace std;
using namespace std::chrono;

// --------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    cout << "Sample IPv6 TCP echo client for 'sockpp' " << sockpp::SOCKPP_VERSION << '\n'
         << endl;

    std::string host = (argc > 1) ? argv[1] : "::1";
    in_port_t port = (argc > 2) ? atoi(argv[2]) : 12345;

    sockpp::initialize();

    // Try to resolve the address

    // Note that this works if the library was compiled with or without exceptions.
    // Applications normally only handles the exception or the return code.

    auto res = sockpp::inet6_address::create(host, port);

    if (!res) {
        cerr << "Error resolving address for '" << host << "': " << res << endl;
        return 1;
    }

    sockpp::tcp6_connector conn(res.value());

    if (!conn) {
        cerr << "Error connecting to server at " << res.value() << "\n\t"
             << conn.last_error_str() << endl;
        return 1;
    }

    cout << "Created a connection from " << conn.address() << endl;

    // Set a timeout for the responses
    if (!conn.read_timeout(5s)) {
        cerr << "Error setting timeout on TCP stream: " << conn.last_error_str() << endl;
    }

    string s, sret;
    while (getline(cin, s) && !s.empty()) {
        if (conn.write(s) != ssize_t(s.length())) {
            cerr << "Error writing to the TCP stream: " << conn.last_error_str() << endl;
            break;
        }

        sret.resize(s.length());
        ssize_t n = conn.read_n(&sret[0], s.length());

        if (n != ssize_t(s.length())) {
            cerr << "Error reading from TCP stream: " << conn.last_error_str() << endl;
            break;
        }

        cout << sret << endl;
    }

    return (!conn) ? 1 : 0;
}
