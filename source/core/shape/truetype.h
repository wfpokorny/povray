//******************************************************************************
///
/// @file core/shape/truetype.h
///
/// Declarations related to the TrueType-based text geometric primitive.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifndef POVRAY_CORE_TRUETYPE_H
#define POVRAY_CORE_TRUETYPE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
#include "base/fileinputoutput_fwd.h"

// POV-Ray header files (core module)
#include "core/scene/object.h"
#include "core/shape/csg_fwd.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreShape
///
/// @{

//******************************************************************************
///
/// @name Object Types
///
/// @{

#define TTF_OBJECT (BASIC_OBJECT)

/// @}
///
//******************************************************************************

/// @note(1)
/// The existing code creates one large glyph from the input string.
///
/// @note(2)
/// The existing code doesn't support the winding test method for inside and
/// outside determination, but rather segment/curve crossing tests which may
/// render some fonts incorrectly.  An outline with overlapping contours for
/// example.
///
/// @note(3)
/// Most of the code dates from the early 1990s and is in need of update.
/// Support for right to left, vertical fonts and other unsupported
/// extensions.
///
/// @note(4)
/// The basic intersection and inside_glyph testing as coded relies on the
/// first & first/last coordinate being on curve which is strictly not
/// required - though many fonts examined appear to follow this convention.
/// Related, the wrap of the final x2 point to the first point looks to be
/// redundant for most fonts as they already repeat the first and last point.
///
/// @note(5)
/// For better performance - at the expense of a little storage - the
/// generated on curve points could be created on the original read of the
/// font's glyphs also fixing the exposure to the first, first and last or
/// all points being control points.  Should our built in fonts be processed
/// to have all the on curve points?
///

/*****************************************************************************
* Global typedefs
******************************************************************************/

struct GlyphStruct;
using GlyphPtr = GlyphStruct*;

struct TrueTypeInfo;

struct TrueTypeFont final
{
    static constexpr POV_UINT32 kAnyCMAP = 0xFFFFFFFFu;

    TrueTypeFont(const UCS2String& fn, const std::shared_ptr<pov_base::IStream>& f, POV_UINT32 cm, CharsetID cs, LegacyCharset scs);
    ~TrueTypeFont();

    UCS2String          filename;
    std::shared_ptr<pov_base::IStream> file;
    POV_UINT32          cmap;
    CharsetID           charset;
    LegacyCharset       legacyCharset;
    TrueTypeInfo*       info;
};

class TrueType final : public ObjectBase
{
    public:
        GlyphPtr glyph;      /* (GlyphPtr) Pointer to the glyph */
        DBL depth;        /* Amount of extrusion */

        TrueType();
        virtual ~TrueType() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        static void ProcessNewTTF(CSG *Object, TrueTypeFont* font, const UCS2 *text_string,
                                  DBL depth, const Vector3d& offset);
    protected:
        bool Inside_Glyph(DBL x, DBL y, const GlyphStruct* glyph) const;
        int solve_quad(DBL *x, DBL *y, DBL mindist, DBL maxdist) const;
        void GetZeroOneHits(const GlyphStruct* glyph, const Vector3d& P, const Vector3d& D,
                            DBL glyph_depth, DBL *t0, DBL *t1) const;
        bool GlyphIntersect(const Vector3d& P, const Vector3d& D, const GlyphStruct* glyph,
                            DBL glyph_depth, const BasicRay &ray, IStack& Depth_Stack, TraceThreadData *Thread);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_TRUETYPE_H
