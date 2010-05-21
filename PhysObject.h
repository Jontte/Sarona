#pragma once
#include "StdAfx.h"
#include "Util.h"
#include "ObjectReplicator.h"

namespace Sarona
{
	class PhysWorld;
	class PhysObjectMotionState;
	class PhysObject : public ZCom_NodeEventInterceptor
	{
	private:
		
		// ZCom stuff
		scoped_ptr<ZCom_Node>							m_zcomNode;

		// Global obj refs
		btDynamicsWorld*					m_world;

		// Bullet fields
		scoped_ptr<btCollisionShape>		m_shape;
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


	friend class PhysWorld;
		PhysObject(ZCom_Control* control, btDynamicsWorld* world, btCollisionShape* shape, const btTransform& initialpos);

	public:
		void setMass(btScalar kilos = -1);
		~PhysObject(void);
	};

	class PhysObjectMotionState : public btMotionState
	{
	public:
		PhysObjectMotionState(const btTransform &initialpos, ObjectReplicator *obj) {
			m_obj = obj;
			mPos1 = initialpos;
		}

		void reset()
		{
			m_obj = NULL;
		}

		virtual ~PhysObjectMotionState() {
		}

		virtual void getWorldTransform(btTransform &worldTrans) const {
			worldTrans = mPos1;
		}

		virtual void setWorldTransform(const btTransform &worldTrans) {
			if(!m_obj) return; // silently return before we set a node

			btVector3 btPos = worldTrans.getOrigin();//matr.getTranslation();
			float pos[3];

			pos[0] = btPos[0];
			pos[1] = btPos[1];
			pos[2] = btPos[2];

			m_obj->Input(pos);

//			irr::core::matrix4 matr;
//			btTransformToIrrlichtMatrix(worldTrans, matr);

//			m_obj->setRotation(matr.getRotationDegrees());
//			m_obj->setPosition(matr.getTranslation());

			mPos1 = worldTrans;
			
			//std::cout << "Pos : " << m_node->getPosition().X << ", " << m_node->getPosition().Y << ", " << m_node->getPosition().Z << std::endl;
		}

	protected:
		ObjectReplicator *m_obj;
		btTransform mPos1;
	};

}
