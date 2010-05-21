#include "StdAfx.h"
#include "BaseWorld.h"
#include "NetObject.h"
#include "ObjectReplicator.h"

namespace Sarona
{
	NetObject::NetObject(ZCom_Control* control, IrrlichtDevice* dev) 
		: m_deleteme(false)
		, m_device(dev)
		, m_sceneNode(NULL)
	{
		m_zcomNode.reset(new ZCom_Node());

		// BEGIN REPLICATION SETUP
		m_zcomNode->beginReplicationSetup(1);
			ZCom_ReplicatorSetup setup(/*flags*/ ZCOM_REPFLAG_UNRELIABLE   , /*rules*/ ZCOM_REPRULE_AUTH_2_ALL, /*intercept_id*/ 0, /*min_delay*/ 0, /*max_delay*/ 100);
			m_zcomNode -> addReplicator(new ObjectReplicator(setup.Duplicate(), this), true);
		m_zcomNode->endReplicationSetup();
		// END REPLICATION SETUP

		m_zcomNode -> registerNodeDynamic(BaseWorld::m_objectId, control);
		m_zcomNode -> setEventInterceptor(this);

		// Create an irrlicht node..

		scene::IMeshSceneNode* node;

		node = m_device->getSceneManager()->addCubeSceneNode(5);
		node->setMaterialFlag(video::EMF_LIGHTING, true);

		m_device->getSceneManager()->getMeshManipulator()->recalculateNormals(node->getMesh(), true, true);

		if(node->getMaterialCount() > 0)
		{
			// set texture..
			node->setMaterialTexture( 0,m_device->getVideoDriver()->getTexture("testgraphics.png"));
		}

		m_sceneNode = node;
	}

	NetObject::~NetObject(void)
	{
	}

	void NetObject::SetPosition(const core::vector3df& position)
	{
		m_sceneNode->setPosition(position);
	}

	bool NetObject::recUserEvent(ZCom_Node *_node, ZCom_ConnID _from, 
					eZCom_NodeRole _remoterole, ZCom_BitStream &_data, 
					zU32 _estimated_time_sent)
	{
		return false;
	}
	                          
	bool NetObject::recInit(ZCom_Node *_node, ZCom_ConnID _from,
			   eZCom_NodeRole _remoterole)
	{
		return false;
	}
	bool NetObject::recSyncRequest(ZCom_Node *_node, ZCom_ConnID _from, 
					  eZCom_NodeRole _remoterole)
	{
		return false;
	}
	bool NetObject::recRemoved(ZCom_Node *_node, ZCom_ConnID _from,
				  eZCom_NodeRole _remoterole)
	{
		// We were asked to remove ourselves from the simulation.
		m_deleteme = true;

		return false;
	}
	                        
	bool NetObject::recFileIncoming(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid, 
					   ZCom_BitStream &_request)
	{
		return false;
	}
	                             
	bool NetObject::recFileData(ZCom_Node *_node, ZCom_ConnID _from,
				   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}
	                     
	bool NetObject::recFileAborted(ZCom_Node *_node, ZCom_ConnID _from,
					  eZCom_NodeRole _remoterole, ZCom_FileTransID _fid) 
	{
		return false;
	}
	                           
	bool NetObject::recFileComplete(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid)
	{
		return false;
	}
}
