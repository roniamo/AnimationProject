#pragma once

#include "Scene.h"
#include "Texture.h"
#include <memory>
#include <utility>
#include <AutoMorphingModel.h>
#include "SceneWithImGui.h"
#include <AABB.h>
#include <chrono>


class SimpleScene : public cg3d::SceneWithImGui
{

enum class GameState {
    MainMenu,
    Level1,
    Level2,
    Level3,
    BetweenLevels
};

public:
    SimpleScene(std::string name, cg3d::Display* display);
    void Init(float fov, int width, int height, float near, float far);
    void KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods) override;
    void SetupSnake();
    void AddPart();
    void SetupJoins();
    void SetupTextures(int index);
    void SetupTexturesBall();
    void SetupCameras(float fov, int width, int height, float near, float far);
    void SetCamera(int index);
    void AddViewportCallback(cg3d::Viewport* _viewport) override;
    void FollowSnake();
    Eigen::Vector3f getTipOfCyl(int cyl_index);
    Eigen::Vector3f getBaseOfCyl(int cyl_index);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void BuildImGui() override;
    void BuildMainMenu();
    void BuildLevel();
    void BuildBetweenLevels();
    void LoadLevel1();
    void LoadLevel2();
    void LoadLevel3();
	void appearingObject();
    void AddScore(int amount);
    bool IsColliding();
    bool AreTreesColliding(igl::AABB<Eigen::MatrixXd, 3>& firstTree, igl::AABB<Eigen::MatrixXd, 3>& secondTree, std::shared_ptr<cg3d::Model> firstModel, std::shared_ptr<cg3d::Model> secondModel);
    bool AreBoxesColliding(igl::AABB<Eigen::MatrixXd, 3>& firstTree, igl::AABB<Eigen::MatrixXd, 3>& secondTree, std::shared_ptr<cg3d::Model> firstModel, std::shared_ptr<cg3d::Model> secondModel);
    void RemoveAndDestroyObjects();		
    int amountOfApples = 0;
    bool speedy = false;
    std::chrono::system_clock::time_point oldTime;
    void EndLevel();
    void GoToEndGamePanel();
	std::vector<std::shared_ptr<cg3d::AutoMorphingModel>> snakes;

private:
	igl::AABB<Eigen::MatrixXd, 3> treeSnake;
    std::shared_ptr<cg3d::Program> program;						
    std::shared_ptr<cg3d::Model> snake;
	std::shared_ptr<cg3d::Material> daylight;		 
    std::shared_ptr<cg3d::AutoMorphingModel> autoSnake;
    std::shared_ptr<cg3d::Movable> root;
    int meshDataIndex = 0;
    cg3d::Viewport* viewport = nullptr;
    std::vector<std::shared_ptr<cg3d::Camera>> camList{ 3 };
    GameState currentState = GameState::MainMenu;
    int currentLevel = 1;
    int score = 0;
    bool paused = false;
    int snakeLength = 1;
    float scaleFactor = 1;
    float CYL_LENGTH = 1.6f;
    int timer = 0;
	int checkEveryXUpdates = 5;
    int currentUpdateAmount = 0;
    std::chrono::steady_clock::time_point prevTime;
    bool firstPrevTime = false;
    bool allowMove = false;
};
