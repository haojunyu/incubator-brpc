// Copyright (c) 2014 baidu-rpc authors.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BRPC_DESCRIBABLE_H
#define BRPC_DESCRIBABLE_H

#include <ostream>
#include "base/macros.h"
#include "base/class_name.h"

namespace brpc {

struct DescribeOptions {
    DescribeOptions()
        : verbose(true)
        , use_html(false)
    {}

    bool verbose;
    bool use_html;
};

class Describable {
public:
    virtual ~Describable() {}
    virtual void Describe(std::ostream& os, const DescribeOptions&) const {
        os << base::class_name_str(*this);
    }
};

class NonConstDescribable {
public:
    virtual ~NonConstDescribable() {}
    virtual void Describe(std::ostream& os, const DescribeOptions&) {
        os << base::class_name_str(*this);
    }
};

inline std::ostream& operator<<(std::ostream& os, const Describable& obj) {
    DescribeOptions options;
    options.verbose = false;
    obj.Describe(os, options);
    return os;
}

inline std::ostream& operator<<(std::ostream& os,
                                NonConstDescribable& obj) {
    DescribeOptions options;
    options.verbose = false;
    obj.Describe(os, options);
    return os;
}

// Append `indent' spaces after each newline.
// Example:
//   IndentingOStream os1(std::cout, 2);
//   IndentingOStream os2(os1, 2);
//   std::cout << "begin1\nhello" << std::endl << "world\nend1" << std::endl;
//   os1 << "begin2\nhello" << std::endl << "world\nend2" << std::endl;
//   os2 << "begin3\nhello" << std::endl << "world\nend3" << std::endl;
// Output:
// begin1
// hello
// world
// end1
// begin2
//   hello
//   world
//   end2
//   begin3
//     hello
//     world
//     end3
class IndentingOStream : virtual private std::streambuf, public std::ostream {
public:
    IndentingOStream(std::ostream& dest, int indent)
        : std::ostream(this)
        , _dest(dest.rdbuf())
        , _is_at_start_of_line(false)
        , _indent(indent, ' ')
    {}
protected:
    virtual int overflow(int ch) {
        if (_is_at_start_of_line && ch != '\n' ) {
            _dest->sputn(_indent.data(), _indent.size());
        }
        _is_at_start_of_line = (ch == '\n');
        return _dest->sputc(ch);
    }
private:
    DISALLOW_COPY_AND_ASSIGN(IndentingOStream);
    std::streambuf* _dest;
    bool _is_at_start_of_line;
    std::string _indent;
};

} // namespace brpc

#endif  // BRPC_DESCRIBABLE_H