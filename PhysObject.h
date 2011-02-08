#pragma once
#ifndef PHYSOBJECT_H_
#define PHYSOBJECT_H_
#include "StdAfx.h"
#include "Util.h"
#include "ObjectReplicator.h"

namespace Sarona
{
	class PhysWorld;
	class PhysObjectMotionState;
	class JSObject;
	class PhysObject
		: public ZCom_NodeEventInterceptor // Receive regular events
	{
		friend class PhysWorld;
		friend class JSObject;
	private:

		// ZCom stuff
		scoped_ptr<ZCom_Node>				m_zcomNode;

		// Replicator handle. Weak ref; it is managed by zoidcom and ought to outlive this object
		ObjectReplicator*					m_replicator;

		// Global obj refs
		btDynamicsWorld*					m_dynamicsWorld;
		PhysWorld*							m_world;

		// Bullet fields
		btCollisionShape*					m_shape; // ATN: No ownership!
		scoped_ptr<PhysObjectMotionState>	m_motionstate;
		scoped_ptr<btRigidBody>				m_rigidbody;

		bool recUserEvent(ZCom_Node *_node, ZCom_ConnID _from,
						eZCom_NodeRole _remoterole, ZCom_BitStream &_data,
						zU32 _estimated_time_sent);

		bool recInit(ZCom_Node *_node, ZCom_ConnID _from,
				   eZCom_NodeRole _remoterole);
		bool recSyncRequest(ZCom_Node *_node, ZCom_ConnID _from,
						  eZCom_NodeRole _remoterole);

		bool recRemoved(ZCom_Node *_node, ZCom_ConnID _from,
					  eZCom_NodeRole _remoterole);

		bool recFileIncoming(ZCom_Node *_node, ZCom_ConnID _from,
						   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid,
						   ZCom_BitStream &_request);

		bool recFileData(ZCom_Node *_node, ZCom_ConnID _from,
					   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid) ;

		bool recFileAborted(ZCom_Node *_node, ZCom_ConnID _from,
						  eZCom_NodeRole _remoterole, ZCom_FileTransID _fid) ;

		bool recFileComplete(ZCom_Node *_node, ZCom_ConnID _from,
						   eZCom_NodeRole _remoterole, ZCom_FileTransID _fid);

		// Waiting for deletion
		bool m_deleteme;

		PhysObject(PhysWorld* world, btDynamicsWorld* dynworld, const btTransform& initialpos);

		// Data to be shared with peers through replicator interface:
		string m_mesh;
		string m_texture;
		float m_meshScale;

		// Local data
		btScalar m_mass;
		string m_body;
		float m_bodyScale;

		// whether an update is required.
		bool m_dirty;

		void recreateBody(); // Used to update physics simulation parameters such as mass or mesh type

		btCollisionShape* getShape(const string& shapetext);

	public:
		typedef ZCom_NodeID Id; // Used to implement weak refs to these objects

		void setMass(btScalar kilos = -1);
		//void setPosition(const btVector3& pos);
		void setTexture(const std::string&);
		void setBody(const std::string&);
		void setMesh(const std::string&);
		void setMeshScale(float in);
		void setBodyScale(float in);

		void push(const btVector3&);

		// Death handling
		void kill();
		bool isZombie();

		// If dirty, send status update to all peers.
		void sendUpdate();

		// Used to reference this object in the network
		ZCom_NodeID	getNetworkId();

		~PhysObject(void);
	};

	class PhysObjectMotionState : public btMotionState
	{
	public:
		PhysObjectMotionState(const btTransform &pos, ObjectReplicator *obj)
			: m_body(NULL)
			, m_obj(obj)
			, m_pos(pos)
		{}

		void setBody(btRigidBody* body)
		{
			m_body = body;
		}

		void reset()
		{
			m_obj = NULL;
		}

		virtual ~PhysObjectMotionState() {
		}

		virtual void getWorldTransform(btTransform &pos) const {
			pos = m_pos;
		}

		virtual void setWorldTransform(const btTransform &worldTrans) {
			if(!m_obj || !m_body) return; // silently return before we set a node

			btVector3 pos = worldTrans.getOrigin();//matr.getTranslation();
			btQuaternion rot = worldTrans.getRotation();

			// read velocity and omega from rigidbody

			btVector3 vel;

			if(m_body)
			{
				vel = m_body->getLinearVelocity();
			}

			m_obj->Input(pos,rot, vel, btVector4(0,0,0,0));

//			irr::core::matrix4 matr;
//			btTransformToIrrlichtMatrix(worldTrans, matr);

//			m_obj->setRotation(matr.getRotationDegrees());
//			m_obj->setPosition(matr.getTranslation());

			m_pos = worldTrans;

			//std::cout << "SERVER:	" << pos.x() << ",	" << pos.y() << ",	" << pos.z() << std::endl;
		}

	protected:
		btRigidBody* m_body;
		ObjectReplicator *m_obj;
		btTransform m_pos;
	};

}
#endif
