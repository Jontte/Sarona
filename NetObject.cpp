#include "StdAfx.h"
#include "BaseWorld.h"
#include "NetObject.h"
#include "ObjectReplicator.h"
#include "PacketDef.h"

namespace Sarona
{
	NetObject::NetObject(ZCom_Control* control, IrrlichtDevice* dev)
		: m_deleteme(false)
		, m_sceneNode(NULL)
		, m_device(dev)
		, m_meshScale(1,1,1)
	{
		m_zcomNode.reset(new ZCom_Node());
		m_zcomNode->setUserData(this);

		// BEGIN REPLICATION SETUP
		m_zcomNode->beginReplicationSetup(1);
			m_zcomNode -> addReplicator(
				new ObjectReplicator(
					new ZCom_ReplicatorSetup(ZCOM_REPFLAG_UNRELIABLE|ZCOM_REPFLAG_MOSTRECENT|ZCOM_REPFLAG_SETUPAUTODELETE, ZCOM_REPRULE_AUTH_2_ALL)
				, this)
			, true);
		m_zcomNode->endReplicationSetup();
		// END REPLICATION SETUP

		m_zcomNode -> registerNodeDynamic(Sarona::TypeRegistry::m_objectId, control);
		m_zcomNode -> setEventInterceptor(this);

		reloadNode();

		// the node is first shown after the first status update packet
		// which shows up pretty soon now that we've notified the endpoint of obj creation
		m_sceneNode->setVisible(false);
	}

	NetObject::~NetObject(void)
	{
		if(m_sceneNode)
			m_sceneNode->remove();

		m_zcomNode->setEventInterceptor(NULL);
//		m_zcomNode->unregisterNode();
		m_zcomNode.reset();
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
		m_timer.restart();
//		std::cout << "CLIENT:	" << position.X << ",	" << position.Y << ",		" << position.Z << std::endl;
	}

	void NetObject::RefreshPosRot(const boost::posix_time::ptime& now)
	{
		// manage extrapolation and all here
		// smoothen out
		core::vector3df currentpos = m_sceneNode->getPosition();
		m_sceneNode->setPosition(
			currentpos * 0.5 +
			(m_position + m_velocity * m_timer.elapsed(now) / 1000) * 0.5 );
//		m_sceneNode->setPosition(
//			currentpos + m_velocity*dt
//			);
//		m_sceneNode->setPosition(m_position);

		core::quaternion toeuler(m_rotation);
		/*toeuler *= core::quaternion(core::vector3df(
			0,0,90 // Rotation offset
			)*core::DEGTORAD);  */
		core::vector3df rota;
		toeuler.toEuler(rota);
		m_sceneNode->setRotation(rota * core::RADTODEG);
	}

	void NetObject::reloadNode()
	{
		if(m_sceneNode)
			m_sceneNode->remove();

		// Create an irrlicht node..
		core::vector3df euler;
		m_rotation.toEuler(euler);

		scene::IMesh * mesh = m_mesh.length() ? m_device->getSceneManager()->getMesh(m_mesh.c_str()) : NULL;

		if(mesh)
		{
			m_sceneNode = m_device->getSceneManager()->addMeshSceneNode(
				mesh,
				0, // parent
				-1, // id
				m_position, // position
				euler, // rotation
				m_meshScale, // scale
				true// alsoAddIfMeshPointerZero
				);
		}
		else
		{
			m_sceneNode = m_device->getSceneManager()->addCubeSceneNode (
				m_meshScale.getLength(),
				0,
				-1,
				m_position,
				euler);
		}
		m_sceneNode->setMaterialFlag(video::EMF_LIGHTING, false);

//		m_device->getSceneManager()->getMeshManipulator()->recalculateNormals(m_sceneNode->getMesh(), true, true);

		if(m_sceneNode->getMaterialCount() > 0 && m_texture != "")
		{
			// set texture..
			video::ITexture * texture = m_device->getVideoDriver()->getTexture(m_texture.c_str());
			m_sceneNode->setMaterialTexture(0, texture);
		}
	}

	core::vector3df NetObject::getPosition()
	{
		return m_sceneNode->getPosition();
	}

	core::vector3df NetObject::getRotation()
	{
		return m_sceneNode->getRotation();
	}

	bool NetObject::IsZombie()
	{
		return m_deleteme;
	}

	bool NetObject::recUserEvent(ZCom_Node *_node, ZCom_ConnID _from,
					eZCom_NodeRole _remoterole, ZCom_BitStream &_data,
					zU32 _estimated_time_sent)
	{
		// Received event from remote node
		unsigned int Id = _data.getInt(16);

		if(Id == Protocol::NodeStateUpdate::Id)
		{
			Protocol::NodeStateUpdate state;
			state.read(_data);

			bool dirty = false;

			if(m_mesh != state.mesh)
			{
				dirty = true;
				m_mesh = state.mesh;
			}
			if(m_texture != state.texture)
			{
				dirty = true;
				m_texture = state.texture;
			}
			core::vector3df scale(
				state.mesh_scale.x(),
				state.mesh_scale.y(),
				state.mesh_scale.z()
			);
			if(m_meshScale.getDistanceFromSQ(scale)>0.0001) // TODO: compare with epsilon
			{
				dirty = true;
				m_meshScale = scale;
			}
//			std::cout << "Updated: " << m_texture << ", " << m_mesh << ", " << m_meshScale << std::endl;

			if(dirty)
				reloadNode();
		}
		else
		{
			std::cerr << "FATAL: Unhandled node message with id: " << Id << std::endl;
		}

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
		// Let's turn zombie.
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
