#ifndef OSMIUM_IO_DETAIL_METADATA_OPTIONS_HPP
#define OSMIUM_IO_DETAIL_METADATA_OPTIONS_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013-2017 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <osmium/util/string.hpp>

#include <stdexcept>
#include <string>

namespace osmium {

    namespace io {

        namespace detail {

            class metadata_options {

                enum options {
                    md_none      = 0x00,
                    md_version   = 0x01,
                    md_timestamp = 0x02,
                    md_changeset = 0x04,
                    md_uid       = 0x08,
                    md_user      = 0x10,
                    md_all       = 0x1f
                } m_options = md_all;

            public:

                metadata_options() noexcept = default;

                metadata_options(const std::string& attributes) {
                    if (attributes.empty() || attributes == "all" || attributes == "true") {
                        return;
                    }

                    if (attributes == "none" || attributes == "false") {
                        m_options = options::md_none;
                        return;
                    }

                    const auto attrs = osmium::split_string(attributes, '+', true);
                    int opts = 0;
                    for (const auto& attr : attrs) {
                        if (attr == "version") {
                            opts |= options::md_version;
                        } else if (attr == "timestamp") {
                            opts |= options::md_timestamp;
                        } else if (attr == "changeset") {
                            opts |= options::md_changeset;
                        } else if (attr == "uid") {
                            opts |= options::md_uid;
                        } else if (attr == "user") {
                            opts |= options::md_user;
                        } else {
                            throw std::invalid_argument{std::string{"Unknown OSM object metadata attribute: '"} + attr + "'"};
                        }
                    }
                    m_options = static_cast<options>(opts);
                }

                bool any() const noexcept {
                    return m_options != 0;
                }

                bool all() const noexcept {
                    return m_options == options::md_all;
                }

                bool none() const noexcept {
                    return m_options == 0;
                }

                bool version() const noexcept {
                    return (m_options & options::md_version) != 0;
                }

                bool timestamp() const noexcept {
                    return (m_options & options::md_timestamp) != 0;
                }

                bool changeset() const noexcept {
                    return (m_options & options::md_changeset) != 0;
                }

                bool uid() const noexcept {
                    return (m_options & options::md_uid) != 0;
                }

                bool user() const noexcept {
                    return (m_options & options::md_user) != 0;
                }

            }; // class metadata_options

        } // namespace detail

    } // namespace io

} // namespace osmium

#endif // OSMIUM_IO_DETAIL_METADATA_OPTIONS_HPP
