#pragma once
#include "StdAfx.h"
#include "Util.h"
#include "ObjectReplicator.h"

namespace Sarona
{
	class PhysWorld;
	class PhysObjectMotionState;
	class PhysObject
		: public ZCom_NodeEventInterceptor // Receive regular events
		, public ZCom_ReplicatorBasic // Act as a simple replicator ourselves
	{
	private:
		
		// ZCom stuff
		scoped_ptr<ZCom_Node>				m_zcomNode;

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

		// Replicator methods:
		bool checkState ();
		void packData (ZCom_BitStream *_stream);
		void unpackData (ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent);
		void* peekData();
		void clearPeekData();
		void Process(eZCom_NodeRole _localrole, zU32 _simulation_time_passed);

	friend class PhysWorld;
		PhysObject(PhysWorld* world, btDynamicsWorld* dynworld, const btTransform& initialpos);

		// Data to be shared with peers through replicator interface:
		char m_mesh[65];
		char m_texture[65];

		// Local data
		btScalar m_mass;
		string m_body;

		bool m_dirty; // whether an update is required.

		void recreateBody(); // Used to update physics simulation parameters such as mass or mesh type
		
		btCollisionShape* getShape(const string& shapetext);

	public:
		void setMass(btScalar kilos = -1);
		//void setPosition(const btVector3& pos);
		void setTexture(const std::string&);
		void setBody(const std::string&);
		void setMesh(const std::string&);
		~PhysObject(void);
	};

	class PhysObjectMotionState : public btMotionState
	{
	public:
		PhysObjectMotionState(const btTransform &pos, ObjectReplicator *obj)
			: m_body(NULL)
			, m_pos(pos)
			, m_obj(obj)
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
			
			//std::cout << "Pos : " << m_node->getPosition().X << ", " << m_node->getPosition().Y << ", " << m_node->getPosition().Z << std::endl;
		}

	protected:
		ObjectReplicator *m_obj;
		btRigidBody* m_body;
		btTransform m_pos;
	};

}
