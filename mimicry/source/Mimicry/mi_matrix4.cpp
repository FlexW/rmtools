#include "mi_include_standard.h"
#include "mi_include_3d.h"

mCMatrix4::mCMatrix4( mCQuaternion const & a_quatSource )
{
    ModifyRotation( a_quatSource );
    m_arrElements[ 3 ]  = m_arrElements[ 7 ] = m_arrElements[ 11 ] = m_arrElements[ 12 ] = m_arrElements[ 13 ] = m_arrElements[ 14 ] = 0.0f;
    m_arrElements[ 15 ] = 1.0f;
}

mCMatrix4::mCMatrix4( mCMatrix4 const & a_matSource )
{
    *this = a_matSource;
}

mCMatrix4::mCMatrix4( void )
{
    SetToIdentity();
}

mCMatrix4::~mCMatrix4( void )
{
}

mCMatrix4 & mCMatrix4::operator = ( mCMatrix4 const & a_matSource )
{
    if ( this == &a_matSource )
        return *this;
    g_memcpy( m_arrElements, a_matSource.m_arrElements, sizeof( MIFloat ) * EElement_Count );
    return *this;
}

MIBool mCMatrix4::operator == ( mCMatrix4 const & a_matOther ) const
{
    return memcmp( this, &a_matOther, sizeof( mCMatrix4 ) ) == 0;
}

MIFloat & mCMatrix4::operator [] ( MIUInt a_uElementIndex )
{
    return AccessElement( a_uElementIndex );
}

MIFloat mCMatrix4::operator [] ( MIUInt a_uElementIndex ) const
{
    return GetElement( a_uElementIndex );
}

mCMatrix4 mCMatrix4::operator * ( mCMatrix4 const & a_matRight ) const
{
    mCMatrix4 matResult;
    matResult.SetToProduct( *this, a_matRight );
    return matResult;
}

mCMatrix4 mCMatrix4::operator * ( mCQuaternion const & a_quatRotation ) const
{
    return *this * mCMatrix4( a_quatRotation );
}

mCMatrix4 & mCMatrix4::operator *= ( mCMatrix4 const & a_matRight )
{
    SetToProduct( *this, a_matRight );
    return *this;
}

mCMatrix4 & mCMatrix4::operator *= ( mCQuaternion const & a_quatRotation )
{
    return ( *this *= mCMatrix4( a_quatRotation ) );
}

MIBool mCMatrix4::operator != ( mCMatrix4 const & a_matOther ) const
{
    return !( *this ==a_matOther );
}

MIFloat & mCMatrix4::AccessElement( MIUInt a_uElementIndex )
{
    return m_arrElements[ a_uElementIndex ];
}

MIFloat & mCMatrix4::AccessElement( MIUInt a_uRowIndex, MIUInt a_uColumnIndex )
{
    return m_arrRowColumn[ a_uRowIndex ][ a_uColumnIndex ];
}

MIFloat * mCMatrix4::AccessElements( void )
{
    return m_arrElements;
}

MIFloat * mCMatrix4::AccessRow( MIUInt a_uRowIndex )
{
    return m_arrRowColumn[ a_uRowIndex ];
}

mCVec3 & mCMatrix4::AccessTranslation( void )
{
    return *reinterpret_cast< mCVec3 * >( m_arrRowColumn[ 3 ] );
}

MIFloat mCMatrix4::GetElement( MIUInt a_uElementIndex ) const
{
    return m_arrElements[ a_uElementIndex ];
}

MIFloat mCMatrix4::GetElement( MIUInt a_uRowIndex, MIUInt a_uColumnIndex ) const
{
    return m_arrRowColumn[ a_uRowIndex ][ a_uColumnIndex ];
}

MIFloat const * mCMatrix4::GetElements( void ) const
{
    return m_arrElements;
}

mCMatrix4 mCMatrix4::GetInverted( void ) const
{
    mCMatrix4 matInverse;
    MIFloat * pInv = matInverse.AccessElements();
    MIFloat const * pEnt = GetElements();
    MIFloat fDeterminant;

    pInv[ 0 ] = pEnt[ 5 ]  * pEnt[ 10 ] * pEnt[ 15 ] - 
                pEnt[ 5 ]  * pEnt[ 11 ] * pEnt[ 14 ] - 
                pEnt[ 9 ]  * pEnt[ 6 ]  * pEnt[ 15 ] + 
                pEnt[ 9 ]  * pEnt[ 7 ]  * pEnt[ 14 ] +
                pEnt[ 13 ] * pEnt[ 6 ]  * pEnt[ 11 ] - 
                pEnt[ 13 ] * pEnt[ 7 ]  * pEnt[ 10 ];

    pInv[ 4 ] = -pEnt[ 4 ]  * pEnt[ 10 ] * pEnt[ 15 ] + 
                 pEnt[ 4 ]  * pEnt[ 11 ] * pEnt[ 14 ] + 
                 pEnt[ 8 ]  * pEnt[ 6 ]  * pEnt[ 15 ] - 
                 pEnt[ 8 ]  * pEnt[ 7 ]  * pEnt[ 14 ] - 
                 pEnt[ 12 ] * pEnt[ 6 ]  * pEnt[ 11 ] + 
                 pEnt[ 12 ] * pEnt[ 7 ]  * pEnt[ 10 ];

    pInv[ 8 ] = pEnt[ 4 ]  * pEnt[ 9 ] * pEnt[ 15 ] - 
                pEnt[ 4 ]  * pEnt[ 11 ] * pEnt[ 13 ] - 
                pEnt[ 8 ]  * pEnt[ 5 ] * pEnt[ 15 ] + 
                pEnt[ 8 ]  * pEnt[ 7 ] * pEnt[ 13 ] + 
                pEnt[ 12 ] * pEnt[ 5 ] * pEnt[ 11 ] - 
                pEnt[ 12 ] * pEnt[ 7 ] * pEnt[ 9 ];

    pInv[ 12 ] = -pEnt[ 4 ]  * pEnt[ 9 ] * pEnt[ 14 ] + 
                  pEnt[ 4 ]  * pEnt[ 10 ] * pEnt[ 13 ] +
                  pEnt[ 8 ]  * pEnt[ 5 ] * pEnt[ 14 ] - 
                  pEnt[ 8 ]  * pEnt[ 6 ] * pEnt[ 13 ] - 
                  pEnt[ 12 ] * pEnt[ 5 ] * pEnt[ 10 ] + 
                  pEnt[ 12 ] * pEnt[ 6 ] * pEnt[ 9 ];

    pInv[ 1 ] = -pEnt[ 1 ]  * pEnt[ 10 ] * pEnt[ 15 ] + 
                 pEnt[ 1 ]  * pEnt[ 11 ] * pEnt[ 14 ] + 
                 pEnt[ 9 ]  * pEnt[ 2 ] * pEnt[ 15 ] - 
                 pEnt[ 9 ]  * pEnt[ 3 ] * pEnt[ 14 ] - 
                 pEnt[ 13 ] * pEnt[ 2 ] * pEnt[ 11 ] + 
                 pEnt[ 13 ] * pEnt[ 3 ] * pEnt[ 10 ];

    pInv[ 5 ] = pEnt[ 0 ]  * pEnt[ 10 ] * pEnt[ 15 ] - 
                pEnt[ 0 ]  * pEnt[ 11 ] * pEnt[ 14 ] - 
                pEnt[ 8 ]  * pEnt[ 2 ] * pEnt[ 15 ] + 
                pEnt[ 8 ]  * pEnt[ 3 ] * pEnt[ 14 ] + 
                pEnt[ 12 ] * pEnt[ 2 ] * pEnt[ 11 ] - 
                pEnt[ 12 ] * pEnt[ 3 ] * pEnt[ 10 ];

    pInv[ 9 ] = -pEnt[ 0 ]  * pEnt[ 9 ] * pEnt[ 15 ] + 
                 pEnt[ 0 ]  * pEnt[ 11 ] * pEnt[ 13 ] + 
                 pEnt[ 8 ]  * pEnt[ 1 ] * pEnt[ 15 ] - 
                 pEnt[ 8 ]  * pEnt[ 3 ] * pEnt[ 13 ] - 
                 pEnt[ 12 ] * pEnt[ 1 ] * pEnt[ 11 ] + 
                 pEnt[ 12 ] * pEnt[ 3 ] * pEnt[ 9 ];

    pInv[ 13 ] = pEnt[ 0 ]  * pEnt[ 9 ] * pEnt[ 14 ] - 
                 pEnt[ 0 ]  * pEnt[ 10 ] * pEnt[ 13 ] - 
                 pEnt[ 8 ]  * pEnt[ 1 ] * pEnt[ 14 ] + 
                 pEnt[ 8 ]  * pEnt[ 2 ] * pEnt[ 13 ] + 
                 pEnt[ 12 ] * pEnt[ 1 ] * pEnt[ 10 ] - 
                 pEnt[ 12 ] * pEnt[ 2 ] * pEnt[ 9 ];

    pInv[ 2 ] = pEnt[ 1 ]  * pEnt[ 6 ] * pEnt[ 15 ] - 
                pEnt[ 1 ]  * pEnt[ 7 ] * pEnt[ 14 ] - 
                pEnt[ 5 ]  * pEnt[ 2 ] * pEnt[ 15 ] + 
                pEnt[ 5 ]  * pEnt[ 3 ] * pEnt[ 14 ] + 
                pEnt[ 13 ] * pEnt[ 2 ] * pEnt[ 7 ] - 
                pEnt[ 13 ] * pEnt[ 3 ] * pEnt[ 6 ];

    pInv[ 6 ] = -pEnt[ 0 ]  * pEnt[ 6 ] * pEnt[ 15 ] + 
                 pEnt[ 0 ]  * pEnt[ 7 ] * pEnt[ 14 ] + 
                 pEnt[ 4 ]  * pEnt[ 2 ] * pEnt[ 15 ] - 
                 pEnt[ 4 ]  * pEnt[ 3 ] * pEnt[ 14 ] - 
                 pEnt[ 12 ] * pEnt[ 2 ] * pEnt[ 7 ] + 
                 pEnt[ 12 ] * pEnt[ 3 ] * pEnt[ 6 ];

    pInv[ 10 ] = pEnt[ 0 ]  * pEnt[ 5 ] * pEnt[ 15 ] - 
                 pEnt[ 0 ]  * pEnt[ 7 ] * pEnt[ 13 ] - 
                 pEnt[ 4 ]  * pEnt[ 1 ] * pEnt[ 15 ] + 
                 pEnt[ 4 ]  * pEnt[ 3 ] * pEnt[ 13 ] + 
                 pEnt[ 12 ] * pEnt[ 1 ] * pEnt[ 7 ] - 
                 pEnt[ 12 ] * pEnt[ 3 ] * pEnt[ 5 ];

    pInv[ 14 ] = -pEnt[ 0 ]  * pEnt[ 5 ] * pEnt[ 14 ] + 
                  pEnt[ 0 ]  * pEnt[ 6 ] * pEnt[ 13 ] + 
                  pEnt[ 4 ]  * pEnt[ 1 ] * pEnt[ 14 ] - 
                  pEnt[ 4 ]  * pEnt[ 2 ] * pEnt[ 13 ] - 
                  pEnt[ 12 ] * pEnt[ 1 ] * pEnt[ 6 ] + 
                  pEnt[ 12 ] * pEnt[ 2 ] * pEnt[ 5 ];

    pInv[ 3 ] = -pEnt[ 1 ] * pEnt[ 6 ] * pEnt[ 11 ] + 
                 pEnt[ 1 ] * pEnt[ 7 ] * pEnt[ 10 ] + 
                 pEnt[ 5 ] * pEnt[ 2 ] * pEnt[ 11 ] - 
                 pEnt[ 5 ] * pEnt[ 3 ] * pEnt[ 10 ] - 
                 pEnt[ 9 ] * pEnt[ 2 ] * pEnt[ 7 ] + 
                 pEnt[ 9 ] * pEnt[ 3 ] * pEnt[ 6 ];

    pInv[ 7 ] = pEnt[ 0 ] * pEnt[ 6 ] * pEnt[ 11 ] - 
                pEnt[ 0 ] * pEnt[ 7 ] * pEnt[ 10 ] - 
                pEnt[ 4 ] * pEnt[ 2 ] * pEnt[ 11 ] + 
                pEnt[ 4 ] * pEnt[ 3 ] * pEnt[ 10 ] + 
                pEnt[ 8 ] * pEnt[ 2 ] * pEnt[ 7 ] - 
                pEnt[ 8 ] * pEnt[ 3 ] * pEnt[ 6 ];

    pInv[ 11 ] = -pEnt[ 0 ] * pEnt[ 5 ] * pEnt[ 11 ] + 
                  pEnt[ 0 ] * pEnt[ 7 ] * pEnt[ 9 ] + 
                  pEnt[ 4 ] * pEnt[ 1 ] * pEnt[ 11 ] - 
                  pEnt[ 4 ] * pEnt[ 3 ] * pEnt[ 9 ] - 
                  pEnt[ 8 ] * pEnt[ 1 ] * pEnt[ 7 ] + 
                  pEnt[ 8 ] * pEnt[ 3 ] * pEnt[ 5 ];

    pInv[ 15 ] = pEnt[ 0 ] * pEnt[ 5 ] * pEnt[ 10 ] - 
                 pEnt[ 0 ] * pEnt[ 6 ] * pEnt[ 9 ] - 
                 pEnt[ 4 ] * pEnt[ 1 ] * pEnt[ 10 ] + 
                 pEnt[ 4 ] * pEnt[ 2 ] * pEnt[ 9 ] + 
                 pEnt[ 8 ] * pEnt[ 1 ] * pEnt[ 6 ] - 
                 pEnt[ 8 ] * pEnt[ 2 ] * pEnt[ 5 ];

    fDeterminant = pEnt[ 0 ] * pInv[ 0 ] + pEnt[ 1 ] * pInv[ 4 ] + pEnt[ 2 ] * pInv[ 8 ] + pEnt[ 3 ] * pInv[ 12 ];
    if ( fDeterminant != 0 )
        for ( MIUInt u = 0; u != 16; ++u )
            pInv[ u ] /= fDeterminant;
        
    return matInverse;
}

MIFloat const * mCMatrix4::GetRow( MIUInt a_uRowIndex ) const
{
    return m_arrRowColumn[ a_uRowIndex ];
}

mCVec3 const & mCMatrix4::GetTranslation( void ) const
{
    return *reinterpret_cast< mCVec3 const * >( m_arrRowColumn[ 3 ] );
}

void mCMatrix4::ModifyRotation( mCQuaternion const & a_quatSource )
{
    MIFloat const fX = a_quatSource.GetX();
    MIFloat const fY = a_quatSource.GetY();
    MIFloat const fZ = a_quatSource.GetZ();
    MIFloat const fW = a_quatSource.GetW();
    MIFloat const fX2 = 2 * fX;
    MIFloat const fY2 = 2 * fY;
    MIFloat const fZ2 = 2 * fZ;
    MIFloat const fXX2 = fX2 * fX;
    MIFloat const fXY2 = fX2 * fY;
    MIFloat const fXZ2 = fX2 * fZ;
    MIFloat const fXW2 = fX2 * fW;
    MIFloat const fYY2 = fY2 * fY;
    MIFloat const fYZ2 = fY2 * fZ;
    MIFloat const fYW2 = fY2 * fW;
    MIFloat const fZZ2 = fZ2 * fZ;
    MIFloat const fZW2 = fZ2 * fW;
    m_arrElements[ 0 ]  = 1.0f - ( fYY2 + fZZ2 );
    m_arrElements[ 1 ]  =        ( fXY2 - fZW2 );
    m_arrElements[ 2 ]  =        ( fXZ2 + fYW2 );
    m_arrElements[ 4 ]  =        ( fXY2 + fZW2 );
    m_arrElements[ 5 ]  = 1.0f - ( fXX2 + fZZ2 );
    m_arrElements[ 6 ]  =        ( fYZ2 - fXW2 );
    m_arrElements[ 8 ]  =        ( fXZ2 - fYW2 );
    m_arrElements[ 9 ]  =        ( fYZ2 + fXW2 );
    m_arrElements[ 10 ] = 1.0f - ( fXX2 + fYY2 );
}

void mCMatrix4::RemoveScale( void )
{
    mCVec3 vecCol1( m_arrRowColumn[ 0 ][ 0 ], m_arrRowColumn[ 1 ][ 0 ], m_arrRowColumn[ 2 ][ 0 ] );
    mCVec3 vecCol2( m_arrRowColumn[ 0 ][ 1 ], m_arrRowColumn[ 1 ][ 1 ], m_arrRowColumn[ 2 ][ 1 ] );
    mCVec3 vecCol3( m_arrRowColumn[ 0 ][ 2 ], m_arrRowColumn[ 1 ][ 2 ], m_arrRowColumn[ 2 ][ 2 ] );
    vecCol1.Normalize();
    vecCol2 = ( vecCol2 - vecCol1 * vecCol1.CalcDotProduct( vecCol2 ) ).Normalize();
    vecCol3 = ( vecCol3 - vecCol1 * vecCol1.CalcDotProduct( vecCol3 ) - vecCol2 * vecCol2.CalcDotProduct( vecCol3 ) ).Normalize();
    m_arrRowColumn[ 0 ][ 0 ] = vecCol1[ 0 ]; m_arrRowColumn[ 1 ][ 0 ] = vecCol1[ 1 ]; m_arrRowColumn[ 2 ][ 0 ] = vecCol1[ 2 ];
    m_arrRowColumn[ 0 ][ 1 ] = vecCol2[ 0 ]; m_arrRowColumn[ 1 ][ 1 ] = vecCol2[ 1 ]; m_arrRowColumn[ 2 ][ 1 ] = vecCol2[ 2 ];
    m_arrRowColumn[ 0 ][ 2 ] = vecCol3[ 0 ]; m_arrRowColumn[ 1 ][ 2 ] = vecCol3[ 1 ]; m_arrRowColumn[ 2 ][ 2 ] = vecCol3[ 2 ];
    m_arrElements[ 3 ]  = m_arrElements[ 7 ] = m_arrElements[ 11 ] = 0.0f;
    m_arrElements[ 15 ] = 1.0f;
}

void mCMatrix4::SetToIdentity( void )
{
    SetToZero();
    m_arrRowColumn[ 0 ][ 0 ] = 1.0f;
    m_arrRowColumn[ 1 ][ 1 ] = 1.0f;
    m_arrRowColumn[ 2 ][ 2 ] = 1.0f;
    m_arrRowColumn[ 3 ][ 3 ] = 1.0f;
}

void mCMatrix4::SetToRotation( mCQuaternion const & a_quatRotation )
{
    mCMatrix4 matTemp( a_quatRotation );
    m_arrRowColumn[ 0 ][ 0 ] = matTemp.m_arrRowColumn[ 0 ][ 0 ];
    m_arrRowColumn[ 0 ][ 1 ] = matTemp.m_arrRowColumn[ 0 ][ 1 ];
    m_arrRowColumn[ 0 ][ 2 ] = matTemp.m_arrRowColumn[ 0 ][ 2 ];
    m_arrRowColumn[ 1 ][ 0 ] = matTemp.m_arrRowColumn[ 1 ][ 0 ];
    m_arrRowColumn[ 1 ][ 1 ] = matTemp.m_arrRowColumn[ 1 ][ 1 ];
    m_arrRowColumn[ 1 ][ 2 ] = matTemp.m_arrRowColumn[ 1 ][ 2 ];
    m_arrRowColumn[ 2 ][ 0 ] = matTemp.m_arrRowColumn[ 2 ][ 0 ];
    m_arrRowColumn[ 2 ][ 1 ] = matTemp.m_arrRowColumn[ 2 ][ 1 ];
    m_arrRowColumn[ 2 ][ 2 ] = matTemp.m_arrRowColumn[ 2 ][ 2 ];
}

void mCMatrix4::SetToProduct( mCMatrix4 const & a_matLeft, mCMatrix4 const & a_matRight )
{
    static mCMatrix4 matTemp;
    mCMatrix4 & matDest = ( ( this != &a_matLeft ) && ( this != &a_matRight ) ) ? *this : matTemp;
    matDest.SetToZero();
    for ( MIUInt u = 4; u--; )
    {
        MIFloat const * pRow = a_matLeft.GetRow( u );
        for ( MIUInt v = 4; v--; )
        {
            MIFloat const * pColumn = a_matRight.GetElements() + v;
            MIFloat & fElement = matDest.AccessElement( u, v );
            for ( MIUInt w = 4; w--; )
                fElement += pRow[ w ] * pColumn[ w * 4 ];
        }
    }
    matDest.Swap( *this );
}

void mCMatrix4::SetToZero( void )
{
    g_memset( m_arrElements, 0, sizeof( MIFloat ) * EElement_Count );
}

void mCMatrix4::Swap( mCMatrix4 & a_matOther )
{
    if ( this != &a_matOther )
        g_memswap( *this, a_matOther );
}

void mCMatrix4::Transpose( void )
{
    for ( MIUInt u = 4; u--; )
        for ( MIUInt v = 4; v--; )
            if ( v != u )
                g_swap( m_arrRowColumn[ u ][ v ], m_arrRowColumn[ v ][ u ] );
}
