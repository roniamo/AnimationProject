#include "BallObject.h"
#include <random>
using namespace cg3d;

BallObject::BallObject(std::shared_ptr<cg3d::Movable> root, std::shared_ptr<cg3d::Material> material, SimpleScene* simpleScene) {
    
    scene = simpleScene;

    id = 2;
    distance = 0;
    speed = 0.001;
    auto ballMesh{ IglLoader::MeshFromFiles("ballMesh", "data/sphere.obj") };

    auto ball = Model::Create("ball", ballMesh, material);
    auto morphFunc = [&](Model* model, cg3d::Visitor* visitor) {
        return meshDataIndex;
    };
    auto autoBall = AutoMorphingModel::Create(*ball, morphFunc);

    root->AddChild(autoBall);


    std::random_device rd;
    std::mt19937 gen(rd());

    int min_num = -18;
    int max_num = 18;

    std::uniform_int_distribution<> distr(min_num, max_num);

    autoBall->Translate(distr(gen), Movable::Axis::X);
    autoBall->Translate(distr(gen), Movable::Axis::Z);

    model = autoBall;
    tree.init(model->GetMesh()->data[0].vertices, model->GetMesh()->data[0].faces);

    std::uniform_int_distribution<> dist(0, 1);

    randomAxis = dist(gen);
}

void BallObject::moveObject()
{
    if (randomAxis == 0) {
        model->Translate(speed, Movable::Axis::Z);
    }
    else
    {
        model->Translate(speed, Movable::Axis::X);
    }
    distance = distance + speed;
    if (distance > 5 || distance < 0)
    {
        speed = -1 * speed;
    }
}

void BallObject::CollidedWithSnake() {

    // do something
    SoundHandler::getInstance().PlayOurSound("pickup");
    scene->speedy = true;
    scene->oldTime = std::chrono::system_clock::now();

    // setting isHidden will delete the object and remove from list next frame
    model->isHidden = true;

}