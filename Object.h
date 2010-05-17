#pragma once
#include "StdAfx.h"
#include "Util.h"

namespace Sarona
{
	class World;
	class Object
	{
	private:

		// Global obj refs
		IrrlichtDevice*						m_device;
		btDynamicsWorld*					m_world;


		// Irrlicht node
		scene::ISceneNode*					m_scenenode;

		// Bullet fields
		scoped_ptr<btCollisionShape>		m_shape;
		scoped_ptr<btMotionState>			m_motionstate;
		scoped_ptr<btRigidBody>				m_rigidbody;

	friend class World;
		Object(IrrlichtDevice* dev, btDynamicsWorld* world, scene::ISceneNode* node, btCollisionShape* shape, const btTransform& initialpos);

	public:
		void setMass(btScalar kilos = -1);
		~Object(void);
	};

	class ObjectMotionState : public btMotionState
	{
	public:
		ObjectMotionState(const btTransform &initialpos, scene::ISceneNode *node) {
			m_node = node;
			mPos1 = initialpos;
		}

		virtual ~ObjectMotionState() {
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
