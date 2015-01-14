#ifndef OSMIUM_GEOM_PROJECTION_HPP
#define OSMIUM_GEOM_PROJECTION_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013-2015 Jochen Topf <jochen@topf.org> and others (see README).

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

#include <memory>
#include <string>

#include <proj_api.h>

#include <osmium/geom/coordinates.hpp>
#include <osmium/geom/util.hpp>
#include <osmium/osm/location.hpp>

namespace osmium {

    namespace geom {

        /**
         * C++ wrapper for a Coordinate Reference System of the proj library.
         */
        class CRS {

            struct ProjCRSDeleter {
                void operator()(void* crs) {
                    pj_free(crs);
                }
            }; // struct ProjCRSDeleter

            std::unique_ptr<void, ProjCRSDeleter> m_crs;

            projPJ get() const {
                return m_crs.get();
            }

        public:

            CRS(const std::string& crs) :
                m_crs(pj_init_plus(crs.c_str()), ProjCRSDeleter()) {
                if (!m_crs) {
                    throw osmium::projection_error(std::string("creation of CRS failed: ") + pj_strerrno(*pj_get_errno_ref()));
                }
            }

            CRS(int epsg) :
                CRS(std::string("+init=epsg:") + std::to_string(epsg)) {
            }

            bool is_latlong() const {
                return pj_is_latlong(m_crs.get()) != 0;
            }

            bool is_geocent() const {
                return pj_is_geocent(m_crs.get()) != 0;
            }

            /**
             * Transform coordinates from one CRS into another. Wraps the same function
             * of the proj library.
             *
             * Coordinates have to be in radians and are produced in radians.
             *
             * @throws osmmium::projection_error if the projection fails
             */
            friend Coordinates transform(const CRS& src, const CRS& dest, Coordinates c) {
                int result = pj_transform(src.get(), dest.get(), 1, 1, &c.x, &c.y, nullptr);
                if (result != 0) {
                    throw osmium::projection_error(std::string("projection failed: ") + pj_strerrno(result));
                }
                return c;
            }

        }; // class CRS

        /**
         * Functor that does projection from WGS84 (EPSG:4326) to the given
         * CRS.
         */
        class Projection {

            int m_epsg;
            std::string m_proj_string;
            CRS m_crs_wgs84 {4326};
            CRS m_crs_user;

        public:

            Projection(const std::string& proj_string) :
                m_epsg(-1),
                m_proj_string(proj_string),
                m_crs_user(proj_string) {
            }

            Projection(int epsg) :
                m_epsg(epsg),
                m_proj_string(std::string("+init=epsg:") + std::to_string(epsg)),
                m_crs_user(epsg) {
            }

            Coordinates operator()(osmium::Location location) const {
                Coordinates c {location.lon(), location.lat()};

                if (m_epsg != 4326) {
                    c = transform(m_crs_wgs84, m_crs_user, Coordinates(deg_to_rad(location.lon()), deg_to_rad(location.lat())));
                    if (m_crs_user.is_latlong()) {
                        c.x = rad_to_deg(c.x);
                        c.y = rad_to_deg(c.y);
                    }
                }

                return c;
            }

            int epsg() const noexcept {
                return m_epsg;
            }

            std::string proj_string() const {
                return m_proj_string;
            }

        }; // class Projection

    } // namespace geom

} // namespace osmium

#endif // OSMIUM_GEOM_PROJECTION_HPP
