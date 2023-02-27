#include "AppleObject.h"
#include <random>
using namespace cg3d;

AppleObject::AppleObject(std::shared_ptr<cg3d::Movable> root, std::shared_ptr<cg3d::Material> material, SimpleScene* simpleScene) {
    
    scene = simpleScene;

    id = 0;
    distance = 0;
    speed = 0.001;
    auto cube = Model::Create("cube", Mesh::Cube(), material);
    root->AddChild(cube);

    // create a random number generator engine
    std::random_device rd;
    std::mt19937 gen(rd());

    int min_num = -18;
    int max_num = 18;

    // create a distribution object for the range [min, max]
    std::uniform_int_distribution<> distr(min_num, max_num);

    // generate a random number between min_num and max_num
    cube->Translate(distr(gen), Movable::Axis::X);
    cube->Translate(distr(gen), Movable::Axis::Z);

    cube->RotateByDegree(-90.0, Movable::Axis::Z);

    model = cube;
    tree.init(model->GetMesh()->data[0].vertices, model->GetMesh()->data[0].faces);
  
    std::uniform_int_distribution<> dist(0, 1);

    randomAxis = dist(gen);
}

void AppleObject::moveObject()
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

void AppleObject::CollidedWithSnake() {

    // do something
    SoundHandler::getInstance().PlayOurSound("eat");
    scene->AddPart();
    scene->AddScore(1000);

    scene->amountOfApples = scene->amountOfApples - 1;
    if (scene->amountOfApples == 0) scene->EndLevel();


    // destroy object + remove it from list
    // setting isHidden = true will destroy the object next frame in our code.
    model->isHidden = true;
}
