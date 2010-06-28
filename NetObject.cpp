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
		, ZCom_ReplicatorBasic(new ZCom_ReplicatorSetup( // Replicator setup goes here
		ZCOM_REPFLAG_RARELYCHANGED|ZCOM_REPFLAG_MOSTRECENT|ZCOM_REPFLAG_SETUPAUTODELETE, ZCOM_REPRULE_AUTH_2_ALL
		))
	{
		m_mesh[0] = m_texture[0] = NULL;
		m_flags |= ZCOM_REPLICATOR_INITIALIZED;

		m_zcomNode.reset(new ZCom_Node());
		m_zcomNode->setUserData(this);

		// BEGIN REPLICATION SETUP
		m_zcomNode->beginReplicationSetup(2);
			ZCom_ReplicatorSetup setup(/*flags*/ ZCOM_REPFLAG_UNRELIABLE|ZCOM_REPFLAG_MOSTRECENT, /*rules*/ ZCOM_REPRULE_AUTH_2_ALL, /*intercept_id*/ 0, /*min_delay*/ 10, /*max_delay*/ 100);
			m_zcomNode -> addReplicator(new ObjectReplicator(setup.Duplicate(), this), true);
			m_zcomNode -> addReplicator(this, false); // autodelete = false, pretty important when passing 'this' ;)
		m_zcomNode->endReplicationSetup();
		// END REPLICATION SETUP

		m_zcomNode -> registerNodeDynamic(Sarona::TypeRegistry::m_objectId, control);
		m_zcomNode -> setEventInterceptor(this);

		reloadNode();
	}

	NetObject::~NetObject(void)
	{
	}

	void NetObject::UpdateLastKnown(
		const core::vector3df& position, const core::quaternion& rotation,
		const core::vector3df& velocity, const float omega[4],
		const boost::posix_time::ptime& when)
	{
		m_position = position;
		m_rotation = rotation;
		m_velocity = velocity;
		m_omega[0] = omega[0];
		m_omega[1] = omega[1];
		m_omega[2] = omega[2];
		m_omega[3] = omega[3];
		m_when = when;
	}

	void NetObject::RefreshPosRot()
	{
		// manage extrapolation and all here

		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

		boost::posix_time::time_duration dur = boost::posix_time::time_period(m_when, now).length();

		float dt = dur.total_seconds() + float(dur.fractional_seconds()) / 1000;


		// smoothen out
		core::vector3df currentpos = m_sceneNode->getPosition();
		m_sceneNode->setPosition(
			currentpos * 0.5 + 
			(m_position + m_velocity * dt / 1000) * 0.5 );

		core::quaternion toeuler(m_rotation);
		toeuler *= core::quaternion(core::vector3df(
			0,0,90 // Rotation offset
			)*core::DEGTORAD);
		core::vector3df rota;
		toeuler.toEuler(rota);
		m_sceneNode->setRotation(rota * core::RADTODEG);

		//std::cout << "CLIENT:	" << currentpos.X << ",	" << currentpos.Y << ",		" << currentpos.Z << std::endl;
	}

	void NetObject::reloadNode()
	{
		if(m_sceneNode)
			m_sceneNode->remove();

		// Create an irrlicht node..

		scene::IMesh * mesh = m_mesh[0] ? m_device->getSceneManager()->getMesh(m_mesh) : NULL;
		m_sceneNode = m_device->getSceneManager()->addMeshSceneNode(
			mesh,
			0, // parent
			-1, // id
			core::vector3df(0,0,0), // position
			core::vector3df(0,0,0), // rotation
			core::vector3df(m_meshScale, m_meshScale, m_meshScale), // scale
			true					// alsoAddIfMeshPointerZero, pretty important here.
			);
		m_sceneNode->setMaterialFlag(video::EMF_LIGHTING, false);

//		m_device->getSceneManager()->getMeshManipulator()->recalculateNormals(m_sceneNode->getMesh(), true, true);

		if(m_sceneNode->getMaterialCount() > 0)
		{
			// set texture..
			video::ITexture * texture = m_device->getVideoDriver()->getTexture(m_texture);
			m_sceneNode->setMaterialTexture(0, texture);
		}
	}

	core::vector3df NetObject::getPosition()
	{
		return m_sceneNode->getPosition();
	}

	bool NetObject::IsZombie()
	{
		return m_deleteme;
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

	bool NetObject::checkState ()
	{
		// We don't send...
		return false;
	}
	void NetObject::packData (ZCom_BitStream *_stream)
	{
		// We don't send anything.
	}
	void NetObject::unpackData (ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent)
	{
		char ctemp[65]; 
		float ftemp;
		bool dirty = false;

		_stream->getString(ctemp, 64);
		if(strcmp(m_mesh, ctemp) != 0)
		{
			dirty = true;
			strncpy_s(m_mesh, ctemp, 64);
		}
		_stream->getString(ctemp, 64);
		if(strcmp(m_texture, ctemp) != 0)
		{
			dirty = true;
			strncpy_s(m_texture, ctemp, 64);
		}
		ftemp = _stream->getFloat(23);
		if(ftemp != m_meshScale) // TODO: compare with epsilon
		{
			m_meshScale = ftemp;
			dirty = true;
		}

		if(dirty)
			reloadNode();
	}
	void* NetObject::peekData(){return NULL;};
	void NetObject::clearPeekData(){};
	void NetObject::Process(eZCom_NodeRole _localrole, zU32 _simulation_time_passed){};
}
