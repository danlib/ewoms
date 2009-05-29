/*****************************************************************************
 *   Copyright (C) 2008-2009 by Melanie Darcis                               *
 *   Copyright (C) 2008-2009 by Klaus Mosthaf                                *
 *   Copyright (C) 2008-2009 by Andreas Lauser                               *
 *   Copyright (C) 2008-2009 by Bernd Flemisch                               *
 *   Institute of Hydraulic Engineering                                      *
 *   University of Stuttgart, Germany                                        *
 *   email: melanie.darcis _at_ iws.uni-stuttgart.de                         *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version, as long as this copyright notice    *
 *   is included in its original form.                                       *
 *                                                                           *
 *   This program is distributed WITHOUT ANY WARRANTY.                       *
 *****************************************************************************/
/*!
 * \file 
 *
 * \brief This file contains the data which is required to calculate
 *        all fluxes of components over a face of a finite volume.
 *
 * This means pressure, concentration and temperature gradients, phase
 * densities at the intergration point, etc.
 */
#ifndef DUMUX_2P2CNI_FLUX_DATA_HH
#define DUMUX_2P2CNI_FLUX_DATA_HH

#include <dumux/auxiliary/math.hh>

namespace Dune
{

/*!
 * \brief This template class contains the data which is required to
 *        calculate all fluxes of components over a face of a finite
 *        volume for the two-phase, two-component model.
 *
 * This means pressure and concentration gradients, phase densities at
 * the intergration point, etc.
 */
template <class TypeTag>
class TwoPTwoCNIFluxData : public TwoPTwoCFluxData<TypeTag>
{
    typedef TwoPTwoCFluxData<TypeTag>                       ParentType;
    typedef typename GET_PROP_TYPE(TypeTag, PTAG(Scalar))   Scalar;
    typedef typename GET_PROP_TYPE(TypeTag, PTAG(GridView)) GridView;

    typedef typename GET_PROP_TYPE(TypeTag, PTAG(Problem))    Problem;
    typedef typename GET_PROP_TYPE(TypeTag, PTAG(VertexData)) VertexData;

    typedef typename GridView::template Codim<0>::Entity Element;
    typedef std::vector<VertexData>                      VertexDataArray;
    
    enum {
        dim           = GridView::dimension,
        dimWorld      = GridView::dimensionworld,

        numPhases     = GET_PROP_VALUE(TypeTag, PTAG(NumPhases)),
    };
    
    typedef Dune::FieldVector<Scalar, dimWorld>  GlobalPosition;
    typedef Dune::FieldVector<Scalar, dim>       LocalPosition;

    typedef typename GET_PROP_TYPE(TypeTag, PTAG(FVElementGeometry)) FVElementGeometry;
    typedef typename FVElementGeometry::SubControlVolume             SCV;
    typedef typename FVElementGeometry::SubControlVolumeFace         SCVFace;

    typedef Dune::FieldVector<Scalar, numPhases>      PhasesVector;

    typedef typename GET_PROP_TYPE(TypeTag, PTAG(TwoPTwoCIndices)) Indices;

public:
    TwoPTwoCNIFluxData(const Problem &problem,
                       const Element &element,
                       const FVElementGeometry &elemGeom,
                       int faceIdx,
                       const VertexDataArray &elemDat)
        : ParentType(problem, element, elemGeom, faceIdx, elemDat)
    {
        temperatureGrad = 0;

        // Harmonic mean of the heat conducitivities of the
        // sub control volumes adjacent to the face
        heatCondAtIp = harmonicMean(elemDat[this->face->i].heatCond,
                                    elemDat[this->face->j].heatCond);
        
        // calculate temperature gradient
        GlobalPosition tmp(0.0);
        for (int idx = 0; idx < this->fvElemGeom.numVertices; idx++) 
        {
            tmp = this->face->grad[idx];
            tmp *= elemDat[idx].temperature;
            temperatureGrad += tmp;
        }
    }
      
    //! temperature gradient
    GlobalPosition temperatureGrad;
    
    //! heat conductivity at integration point
    Scalar heatCondAtIp;
};

} // end namepace

#endif
