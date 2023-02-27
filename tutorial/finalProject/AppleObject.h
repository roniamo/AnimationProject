#include "GameObject.h"

class AppleObject : public GameObject {
public:

	AppleObject(std::shared_ptr<cg3d::Movable> root, std::shared_ptr<cg3d::Material> material, SimpleScene* simpleScene);
	void moveObject() override;
	void CollidedWithSnake() override;

private:
	double distance;
	double speed;
	int randomAxis;
};