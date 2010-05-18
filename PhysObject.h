#pragma once
#include "StdAfx.h"
#include "BaseObject.h"
#include "Util.h"

namespace Sarona
{
	class PhysWorld;
	class PhysObject : public BaseObject
	{
	private:

		// Global obj refs
		btDynamicsWorld*					m_world;

		// Bullet fields
		scoped_ptr<btCollisionShape>		m_shape;
		scoped_ptr<btMotionState>			m_motionstate;
		scoped_ptr<btRigidBody>				m_rigidbody;

	friend class PhysWorld;
		PhysObject(IrrlichtDevice* dev, scene::ISceneNode* node, btDynamicsWorld* world, btCollisionShape* shape, const btTransform& initialpos);

	public:
		void setMass(btScalar kilos = -1);
		~PhysObject(void);
	};

	class PhysObjectMotionState : public btMotionState
	{
	public:
		PhysObjectMotionState(const btTransform &initialpos, scene::ISceneNode *node) {
			m_node = node;
			mPos1 = initialpos;
		}

		virtual ~PhysObjectMotionState() {
		}

		void setNode(scene::ISceneNode *node) {
			m_node = node;
		}

		virtual void getWorldTransform(btTransform &worldTrans) const {
			worldTrans = mPos1;
		}

		virtual void setWorldTransform(const btTransform &worldTrans) {
			if(!m_node) return; // silently return before we set a node

			irr::core::matrix4 matr;
			btTransformToIrrlichtMatrix(worldTrans, matr);

			m_node->setRotation(matr.getRotationDegrees());
			m_node->setPosition(matr.getTranslation());

			mPos1 = worldTrans;
			
			//std::cout << "Pos : " << m_node->getPosition().X << ", " << m_node->getPosition().Y << ", " << m_node->getPosition().Z << std::endl;
		}

	protected:
		scene::ISceneNode *m_node;
		btTransform mPos1;
	};

}
