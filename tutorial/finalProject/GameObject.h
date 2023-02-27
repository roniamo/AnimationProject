#pragma once

#include <memory>
#include <Model.h>
#include <AABB.h>
#include "SimpleScene.h"
#include "SoundHandler.h"
#include "IglMeshLoader.h"

class GameObject {
public:

	virtual void moveObject() = 0;
	virtual void CollidedWithSnake() = 0;
	std::shared_ptr<cg3d::Model> model;
	igl::AABB<Eigen::MatrixXd, 3> tree;
	SimpleScene* scene;
	int meshDataIndex = 0;
	int id;
};