#include "mi_include_scene.h"

mCScene::mCScene( mCScene const & a_sceneSource ) :
    m_strName( a_sceneSource.m_strName ),
    m_arrMaterials( a_sceneSource.m_arrMaterials ),
    m_arrNodes( a_sceneSource.m_arrNodes )
{
    m_mapNodeIndices.Reserve( m_arrNodes.GetCount() );
    for ( MIUInt u = m_arrMaterials.GetCount(); u--; m_arrMaterials[ u ] = m_arrMaterials[ u ]->Clone() );
    for ( MIUInt u = m_arrNodes.GetCount(); u--; m_mapNodeIndices[ m_arrNodes[ u ]->GetID() ] = u )
        m_arrNodes[ u ] = new mCNode( *m_arrNodes[ u ] );
    for ( MIUInt u = m_arrNodes.GetCount(); u--; )
    {
        if ( m_arrNodes[ u ]->GetParentID() )
            m_arrNodes[ u ]->AccessParentID() = GetNodeAt( a_sceneSource.GetNodeIndexByID( m_arrNodes[ u ]->GetParentID() ) )->GetID();
        if ( m_arrNodes[ u ]->HasSkin() )
            m_arrNodes[ u ]->AccessSkin()->MigrateBoneIDs( *this, a_sceneSource );
    }
}

mCScene::mCScene( void )
{
}

mCScene::~mCScene( void )
{
    Clear();
}

mCScene & mCScene::operator = ( mCScene const & a_sceneSource )
{
    if ( this == &a_sceneSource )
        return *this;
    mCScene sceneCopy( a_sceneSource );
    Swap( sceneCopy );
    return *this;
}

mCMaterialBase * mCScene::AccessMaterialAt( MIUInt a_uIndex )
{
    return m_arrMaterials[ a_uIndex ];
}

mCMaterialBase * mCScene::AccessMaterialByNodeIndex( MIUInt a_uNodeIndex )
{
    return const_cast< mCMaterialBase * >( GetMaterialByNodeIndex( a_uNodeIndex ) );
}

mCNode * mCScene::AccessNodeAt( MIUInt a_uIndex )
{
    return m_arrNodes[ a_uIndex ];
}

void mCScene::AddMaterial( mCMaterialBase const & a_mtlSource )
{
    m_arrMaterials.Add( a_mtlSource.Clone() );
}

mCMaterial & mCScene::AddNewMaterial( void )
{
    return *dynamic_cast< mCMaterial * >( m_arrMaterials.AddNew() = new mCMaterial );
}

mCMultiMaterial & mCScene::AddNewMultiMaterial( void )
{
    return *dynamic_cast< mCMultiMaterial * >( m_arrMaterials.AddNew() = new mCMultiMaterial );
}

mCNode & mCScene::AddNewNode( void )
{
    mCNode & nodeNew = *( m_arrNodes.AddNew() = new mCNode );
    m_mapNodeIndices[ nodeNew.GetID() ] = m_arrNodes.GetCount() - 1;
    return nodeNew;
}

void mCScene::AddNode( mCNode const & a_nodeSource )
{
    m_arrNodes.Add( new mCNode( a_nodeSource ) );
    m_mapNodeIndices[ m_arrNodes.Back()->GetID() ] = m_arrNodes.GetCount() - 1;
}

void mCScene::Clear( void )
{
    for ( MIUInt u = m_arrMaterials.GetCount(); u--; delete m_arrMaterials[ u ] );
    for ( MIUInt u = m_arrNodes.GetCount(); u--; delete m_arrNodes[ u ] );
    m_arrMaterials.Clear();
    m_arrNodes.Clear();
    m_mapNodeIndices.Clear();
    m_strName.Clear();
}

void mCScene::ClearNodeParent( MIUInt a_uNodeIndex )
{
    AccessNodeAt( a_uNodeIndex )->AccessParentID() = 0;
}

mCMaterialBase const * mCScene::GetMaterialAt( MIUInt a_uIndex ) const
{
    if ( a_uIndex >= GetNumMaterials() )
        return 0;
    return m_arrMaterials[ a_uIndex ];
}

mCMaterialBase const * mCScene::GetMaterialByNodeIndex( MIUInt a_uNodeIndex ) const
{
    if ( a_uNodeIndex >= GetNumNodes() )
        return 0;
    MIUInt uMatIndex = GetMaterialIndexByName( GetNodeAt( a_uNodeIndex )->GetMaterialName() );
    return uMatIndex == MI_DW_INVALID ? 0 : GetMaterialAt( uMatIndex );
}

MIUInt mCScene::GetMaterialIndexByName( mCString const & a_strMaterialName ) const
{
    for ( MIUInt u = m_arrMaterials.GetCount(); u--; )
        if ( m_arrMaterials[ u ]->GetName() == a_strMaterialName )
            return u;
    return MI_DW_INVALID;
}

mCString const & mCScene::GetName( void ) const
{
    return m_strName;
}

mCNode const * mCScene::GetNodeAt( MIUInt a_uIndex ) const
{
    if ( a_uIndex >= GetNumNodes() )
        return 0;
    return m_arrNodes[ a_uIndex ];
}

MIUInt mCScene::GetNodeIndexByID( mCUnique::ID a_ID ) const
{
    MIUInt uResult = MI_DW_INVALID;
    m_mapNodeIndices.GetAt( a_ID, uResult );
    return uResult;
}

MIUInt mCScene::GetNodeIndexByName( mCString const & a_strNodeName ) const
{
    for ( MIUInt u = m_arrNodes.GetCount(); u--; )
        if ( m_arrNodes[ u ]->GetName() == a_strNodeName )
            return u;
    return MI_DW_INVALID;
}

MIUInt mCScene::GetNodeParentIndex( MIUInt a_uNodeIndex ) const
{
    return GetNodeIndexByID( GetNodeAt( a_uNodeIndex )->GetParentID() );
}

namespace
{
    void AddNodeIndex( mCScene const & a_sceneScene, MIUInt a_uNodeIndex, mTArray< MIUInt > & a_arrNodeIndices, mTArray< MIBool > & a_arrNodeStates )
    {
        if ( a_arrNodeStates[ a_uNodeIndex ] )
            return;
        MIUInt const uParentIndex = a_sceneScene.GetNodeParentIndex( a_uNodeIndex );
        if ( uParentIndex != MI_DW_INVALID )
            AddNodeIndex( a_sceneScene, uParentIndex, a_arrNodeIndices, a_arrNodeStates );
        a_arrNodeIndices.Add( a_uNodeIndex );
        a_arrNodeStates[ a_uNodeIndex ] = MITrue;
    }

    void GetNodeReorderPattern( mCScene const & a_sceneScene, mTArray< MIUInt > & a_arrDest )
    {
        MIUInt const uNodeCount = a_sceneScene.GetNumNodes();
        mTArray< MIBool > arrNodeStates( MIFalse, uNodeCount );
        a_arrDest.Reserve( uNodeCount );
        for ( MIUInt u = uNodeCount; u--; )
            AddNodeIndex( a_sceneScene, u, a_arrDest, arrNodeStates );
    }
}

void mCScene::GetNodesSortedByLinks( mTArray< mCNode const * > & a_arrDest ) const
{
    mTArray< MIUInt > arrPattern;
    GetNodeReorderPattern( *this, arrPattern );
    a_arrDest.SetAt( 0, m_arrNodes.GetBuffer(), GetNumNodes() );
    g_reorder( a_arrDest.AccessBuffer(), arrPattern.GetBuffer(), GetNumNodes() );
}

void mCScene::GetNodesSortedByLinks( mTArray< mCNode * > & a_arrDest )
{
    mTArray< MIUInt > arrPattern;
    GetNodeReorderPattern( *this, arrPattern );
    a_arrDest = m_arrNodes;
    g_reorder( a_arrDest.AccessBuffer(), arrPattern.GetBuffer(), GetNumNodes() );
}
 
MIUInt mCScene::GetNumMaterials( void ) const
{
    return m_arrMaterials.GetCount();
}

MIUInt mCScene::GetNumNodes( void ) const
{
    return m_arrNodes.GetCount();
}

void mCScene::IdentifyBones( void )
{
    for ( MIUInt u = GetNumNodes(); u--; )
        if ( GetNodeAt( u )->HasSkin() )
            for ( MIUInt v = GetNodeAt( u )->GetSkin()->GetNumBones(); v--; )
                AccessNodeAt( GetNodeIndexByID( GetNodeAt( u )->GetSkin()->GetBoneIDByIndex( v ) ) )->AccessIsBone() = MITrue;
    for ( MIUInt u = GetNumNodes(); u--; )
        if ( GetNodeAt( u )->GetIsBone() )
            for ( mCNode * pNode = AccessNodeAt( u ); pNode->AccessIsBone() = MITrue, pNode->GetParentID(); )
                pNode = AccessNodeAt( GetNodeIndexByID( pNode->GetParentID() ) );
}

void mCScene::MakeMatMeshScene( void )
{
    MakeOneMeshScene();
    mCMesh meshSource;
    meshSource.Swap( *AccessNodeAt( 0 )->AccessMesh() );
    RemoveNode( AccessNodeAt( 0 ) );
    mCMultiMaterial matMerged;
    matMerged.Swap( *dynamic_cast< mCMultiMaterial * >( AccessMaterialAt( 0 ) ) );
    RemoveMaterial( AccessMaterialAt( 0 ) );
    for ( MIUInt u = 0, ue = matMerged.GetSubMaterials().GetCount(); u != ue; ++u )
    {
        AddNewMaterial().Swap( matMerged.AccessSubMaterials()[ u ] );
        mCNode & nodeDest = AddNewNode();
        nodeDest.AccessMaterialName() = nodeDest.AccessName() = GetMaterialAt( u )->GetName();
        mCMesh meshDest;
        meshSource.ExtractByID( u, meshDest );
        for ( MIUInt v = meshDest.GetNumFaces(); v--; meshDest.AccessFaces()[ v ].AccessMatID() = 0 );
        nodeDest.SwapMesh( meshDest );
    }
}

void mCScene::MakeOneMeshScene( void )
{
    mCScene sceneNew;
    sceneNew.SetName( GetName() );
    mCMultiMaterial & matMerged = sceneNew.AddNewMultiMaterial();
    mCNode & nodeMerged = sceneNew.AddNewNode();
    nodeMerged.AccessName() = GetName() == "" ? "Merged" : GetName();
    matMerged.AccessName() = GetNumMaterials() == 1 ? GetMaterialAt( 0 )->GetName() : ( "Mtls_" + GetName() );
    mCMesh meshMerged;
    for ( MIUInt u = 0, ue = GetNumNodes(); u != ue; ++u )
        if ( GetNodeAt( u )->HasMesh() )
            meshMerged.Merge( *GetNodeAt( u )->GetMesh(), &matMerged, GetMaterialByNodeIndex( u ), &matMerged );
    nodeMerged.SwapMesh( meshMerged );
    nodeMerged.AccessMaterialName() = matMerged.GetName();
    Swap( sceneNew );
}

void mCScene::Merge( mCScene & a_sceneSource )
{
    mCScene sceneSource;
    sceneSource.Swap( a_sceneSource );
    m_arrMaterials.SetAt( m_arrMaterials.GetCount(), sceneSource.m_arrMaterials.GetBuffer(), sceneSource.m_arrMaterials.GetCount() );
    sceneSource.m_arrMaterials.Clear();
    for ( MIUInt u = 0, ue = sceneSource.GetNumNodes(); u != ue; ++u )
        AddNewNode().Swap( *sceneSource.AccessNodeAt( u ) );
}

void mCScene::RemoveMaterial( mCMaterialBase * a_pMaterial )
{
    MIUInt const uIndex = m_arrMaterials.IndexOf( a_pMaterial );
    if ( uIndex == MI_DW_INVALID )
        return;
    delete m_arrMaterials[ uIndex ];
    m_arrMaterials.RemoveAt( uIndex );
}

void mCScene::RemoveNode( mCNode * a_pNode )
{
    MIUInt const uIndex = m_arrNodes.IndexOf( a_pNode );
    if ( uIndex == MI_DW_INVALID )
        return;
    mCUnique::ID ID = a_pNode->GetID();
    for ( MIUInt u = GetNumNodes(); u--; )
        if ( m_arrNodes[ u ]->GetParentID() == ID )
            SetNodeParent( u, MI_DW_INVALID );
    if ( m_arrNodes.Back() != a_pNode )
    {
        g_swap( m_arrNodes[ uIndex ], m_arrNodes.Back() );
        m_mapNodeIndices[ m_arrNodes[ uIndex ]->GetID() ] = uIndex;
    }
    m_mapNodeIndices.RemoveAt( ID );
    m_arrNodes.RemoveAt( m_arrNodes.GetCount() - 1 );
    delete a_pNode;
}

void mCScene::SetName( mCString const & a_strName )
{
    m_strName = a_strName;
}

void mCScene::SetNodeParent( MIUInt a_uNodeIndex, MIUInt a_uParentNodeIndex )
{
    AccessNodeAt( a_uNodeIndex )->AccessParentID() = ( a_uParentNodeIndex == MI_DW_INVALID ) ? 0 : GetNodeAt( a_uParentNodeIndex )->GetID();
}

void mCScene::SortNodesByLinks( void )
{
    GetNodesSortedByLinks( m_arrNodes );
    for ( MIUInt u = GetNumNodes(); u--; m_mapNodeIndices[ m_arrNodes[ u ]->GetID() ] = u );
}

void mCScene::Swap( mCScene & a_sceneOther )
{
    if ( this == &a_sceneOther )
        return;
    m_strName.Swap( a_sceneOther.m_strName );
    m_arrMaterials.Swap( a_sceneOther.m_arrMaterials );
    m_arrNodes.Swap( a_sceneOther.m_arrNodes );
    m_mapNodeIndices.Swap( a_sceneOther.m_mapNodeIndices );
}
