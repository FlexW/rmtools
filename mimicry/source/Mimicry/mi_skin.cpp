#include "mi_include_scene.h"

mCSkin::mCSkin( mCSkin const & a_skinSource ) :
    m_arrBoneIDs( a_skinSource.m_arrBoneIDs ),
    m_arrVertexIndices( a_skinSource.m_arrVertexIndices ),
    m_arrBoneIndices( a_skinSource.m_arrBoneIndices ),
    m_arrWeights( a_skinSource.m_arrWeights ),
    m_arrFirstWeightIndexPerVertex( a_skinSource.m_arrFirstWeightIndexPerVertex )
{
}

mCSkin::mCSkin( void )
{
}

mCSkin::~mCSkin( void )
{
}

mCSkin & mCSkin::operator = ( mCSkin const & a_skinSource )
{
    if ( this == &a_skinSource )
        return *this;
    m_arrBoneIDs = a_skinSource.m_arrBoneIDs;
    m_arrVertexIndices = a_skinSource.m_arrVertexIndices;
    m_arrBoneIndices = a_skinSource.m_arrBoneIndices;
    m_arrWeights = a_skinSource.m_arrWeights;
    m_arrFirstWeightIndexPerVertex = a_skinSource.m_arrFirstWeightIndexPerVertex;
    return *this;
}

void mCSkin::Clear( void )
{
    m_arrBoneIDs.Clear();
    m_arrVertexIndices.Clear();
    m_arrBoneIndices.Clear();
    m_arrWeights.Clear();
    m_arrFirstWeightIndexPerVertex.Clear();
}

mCUnique::ID mCSkin::GetBoneIDByIndex( MIUInt uBoneIndex ) const
{
    return m_arrBoneIDs[ uBoneIndex ];
}

MIUInt mCSkin::GetBoneIndex( MIUInt a_uVertexIndex, MIUInt a_uInfluencingBoneIndex ) const
{
    return m_arrBoneIndices[ m_arrFirstWeightIndexPerVertex[ a_uVertexIndex ] + a_uInfluencingBoneIndex ];
}

MIUInt mCSkin::GetBoneIndex( MIUInt a_uWeightIndex ) const
{
    return m_arrBoneIndices[ a_uWeightIndex ];
}

MIUInt mCSkin::GetNumBones( void ) const
{
    return m_arrBoneIDs.GetCount();
}

MIUInt mCSkin::GetNumInfluencingBones( MIUInt a_uVertexIndex ) const
{
    MIUInt const uNextFirstIndex = ( a_uVertexIndex + 1 < GetNumVerts() ) ? m_arrFirstWeightIndexPerVertex[ a_uVertexIndex + 1 ] : GetNumWeights();
    return uNextFirstIndex - m_arrFirstWeightIndexPerVertex[ a_uVertexIndex ];
}

MIUInt mCSkin::GetNumVerts( void ) const
{
    return m_arrFirstWeightIndexPerVertex.GetCount();
}

MIUInt mCSkin::GetNumWeights( void ) const
{
    return m_arrWeights.GetCount();
}

MIFloat mCSkin::GetWeight( MIUInt a_uVertexIndex, MIUInt a_uInfluencingBoneIndex ) const
{
    return m_arrWeights[ m_arrFirstWeightIndexPerVertex[ a_uVertexIndex ] + a_uInfluencingBoneIndex ];
}

MIFloat mCSkin::GetWeight( MIUInt a_uWeightIndex ) const
{
    return m_arrWeights[ a_uWeightIndex ];
}

mEResult mCSkin::InitSwapping( MIUInt a_uVertCount, 
                       mTArray< mCUnique::ID > & a_arrBoneIDs, 
                       mTArray< MIUInt > & a_arrVertexIndices, 
                       mTArray< MIUInt > & a_arrBoneIndices, 
                       mTArray< MIFloat > & a_arrWeights )
{
    MIUInt const uBoneCount = a_arrBoneIDs.GetCount();
    MIUInt const uWeightCount = a_arrWeights.GetCount();
    if ( ( a_arrVertexIndices.GetCount() != uWeightCount ) || 
         ( a_arrBoneIndices.GetCount() != uWeightCount ) )
    {
        MI_ERROR( mCSkinError, EInvalidInitArguments, "Array sizes differ." );
        return mEResult_False;
    }
    for ( MIUInt u = uWeightCount; u--; )
        if ( a_arrBoneIndices[ u ] >= uBoneCount )
        {
            MI_ERROR( mCSkinError, EInvalidInitArguments, "Bone ID array size exceeded." );
            return mEResult_False;
        }
    MIUInt uLastVertexIndex = a_uVertCount - 1;
    for ( MIUInt u = uWeightCount; u--; )
    {
        if ( a_arrVertexIndices[ u ] > uLastVertexIndex )
        {
            MI_ERROR( mCSkinError, EInvalidInitArguments, "Vertex indices unordered or invalid." );
            return mEResult_False;
        }
        uLastVertexIndex = a_arrVertexIndices[ u ];
    }
    MIUInt uNewBoneCount = 0;
    mTArray< MIUInt > arrNewBoneIndices( MI_DW_INVALID, a_arrBoneIndices.GetCount() );
    for ( MIUInt u = 0, uBoneIndex; u != uWeightCount; ++u )
        if ( ( a_arrBoneIndices[ u ] = arrNewBoneIndices[ uBoneIndex = a_arrBoneIndices[ u ] ] ) == MI_DW_INVALID )
            a_arrBoneIndices[ u ] = arrNewBoneIndices[ uBoneIndex ] = uNewBoneCount++;
    m_arrBoneIDs.Resize( uNewBoneCount );
    for ( MIUInt u = uBoneCount; u--; )
        if ( arrNewBoneIndices[ u ] != MI_DW_INVALID )
            m_arrBoneIDs[ arrNewBoneIndices[ u ] ] = a_arrBoneIDs[ u ];
    m_arrVertexIndices.Swap( a_arrVertexIndices );
    m_arrBoneIndices.Swap( a_arrBoneIndices );
    m_arrWeights.Swap( a_arrWeights );
    m_arrFirstWeightIndexPerVertex.Resize( a_uVertCount );
    MIUInt uNextVertexIndex = 0;
    for ( MIUInt u = 0, ue = uWeightCount; u != ue; ++u )
        while ( m_arrVertexIndices[ u ] >= uNextVertexIndex )
            m_arrFirstWeightIndexPerVertex[ uNextVertexIndex++ ] = u;
    while ( uNextVertexIndex != a_uVertCount )
        m_arrFirstWeightIndexPerVertex[ uNextVertexIndex++ ] = uWeightCount;
    return mEResult_Ok;
}

namespace
{
    MIFloat * s_pWeights;

    MIInt CompareVretWeightIndices( MILPCVoid a_pFirst, MILPCVoid a_pSecond )
    {
        if ( s_pWeights[ *static_cast< MIUInt const * >( a_pFirst ) ] == s_pWeights[ *static_cast< MIUInt const * >( a_pSecond ) ] )
            return 0;
        return s_pWeights[ *static_cast< MIUInt const * >( a_pFirst ) ] > s_pWeights[ *static_cast< MIUInt const * >( a_pSecond ) ] ? -1 : 1;
    }
}

void mCSkin::LimitNumInfluencingBonesPerVert( MIUInt const a_uMax )
{
    mTArray< MIUInt >  arrVertexIndices( m_arrVertexIndices.GetCount() );
    mTArray< MIUInt >  arrBoneIndices( m_arrBoneIndices.GetCount() );
    mTArray< MIFloat > arrWeights( m_arrWeights.GetCount() );
    mTArray< MIUInt >  arrVertWeightIndices( 0, 100 );
    MIUInt * pVertWeightIndices = arrVertWeightIndices.AccessBuffer();
    for ( MIUInt u = 0, ue = GetNumVerts(); u != ue; ++u )
    {
        MIUInt uWeightCount = GetNumInfluencingBones( u );
        MIFloat fWeightSum = 0.0f;
        s_pWeights = &m_arrWeights[ m_arrFirstWeightIndexPerVertex[ u ] ];
        for ( MIUInt v = uWeightCount; v--; pVertWeightIndices[ v ] = v );
        qsort( pVertWeightIndices, uWeightCount, sizeof( *pVertWeightIndices ), &CompareVretWeightIndices );
        uWeightCount = g_min( uWeightCount, a_uMax );
        for ( MIUInt v = 0; v != uWeightCount; ++v )
        {
            arrVertexIndices.Add( u );
            arrBoneIndices.Add( GetBoneIndex( u, pVertWeightIndices[ v ] ) );
            arrWeights.Add( s_pWeights[ pVertWeightIndices[ v ] ] );
            fWeightSum += s_pWeights[ pVertWeightIndices[ v ] ];
        }
        for ( MIFloat * pWeights = &arrWeights.Back(), * pEnd = pWeights - uWeightCount; pWeights != pEnd; *pWeights-- /= fWeightSum );
    }
    arrVertexIndices.UnReserve();
    arrBoneIndices.UnReserve();
    arrWeights.UnReserve();
    mCSkin skinResult;
    skinResult.InitSwapping( GetNumVerts(), m_arrBoneIDs, arrVertexIndices, arrBoneIndices, arrWeights );
    Swap( skinResult );
}

void mCSkin::MigrateBoneIDs( mCScene const & a_sceneNew, mCScene const & a_sceneCurrent )
{
    for ( MIUInt u = 0, ue = m_arrBoneIDs.GetCount(); u != ue; ++u )
        m_arrBoneIDs[ u ] = a_sceneNew.GetNodeAt( a_sceneCurrent.GetNodeIndexByID( m_arrBoneIDs[ u ] ) )->GetID();
}

void mCSkin::Swap( mCSkin & a_skinOther )
{
    if ( this == &a_skinOther )
        return;
    m_arrBoneIDs.Swap( a_skinOther.m_arrBoneIDs );
    m_arrVertexIndices.Swap( a_skinOther.m_arrVertexIndices );
    m_arrBoneIndices.Swap( a_skinOther.m_arrBoneIndices );
    m_arrWeights.Swap( a_skinOther.m_arrWeights );
    m_arrFirstWeightIndexPerVertex.Swap( a_skinOther.m_arrFirstWeightIndexPerVertex );
}
