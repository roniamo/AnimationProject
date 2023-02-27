#include "SimpleScene.h"
#include "Model.h"
#include "Renderer.h"
#include "ObjLoader.h"
#include "CamModel.h"
#include "IglMeshLoader.h"
#include "Movable.h"
#include <boundary_loop.h>
#include <map_vertices_to_circle.h>
#include "../../igl/harmonic.h"
#include "../../igl/per_vertex_normals.h"
#include "../../igl/read_triangle_mesh.h"
#include <imgui.h>
#include <stb_image.h>
#include <iostream>
#include <cstdlib>
#include <random>
#include "GameObject.h"
#include "AppleObject.h"
#include "BombObject.h"
#include "CoinObject.h"
#include "BallObject.h"

using namespace cg3d;

#pragma region ImGui


void SimpleScene::BuildImGui()
{
    switch (currentState) {
        case GameState::MainMenu:
            BuildMainMenu();
            break;
        case GameState::Level1:
            BuildLevel();
            break;
        case GameState::Level2:
            BuildLevel();
            break;
        case GameState::Level3:
            BuildLevel();
            break;
        case GameState::BetweenLevels:
            BuildBetweenLevels();
            break;
    }
}

void SimpleScene::BuildMainMenu() 
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    ImVec2 displaySize = ImGui::GetIO().DisplaySize; // get display size of our game size

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // set windows start position (0,0) here
    ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always); // set windows size to our game size
    ImFont* font = ImGui::GetFont(); // get font of text (like text in buttons)


    //ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.6f, 0.6f, 0.9f, 1.0f)); // Set the window background color to dark gray
    //ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 1.0f, 1.0f)); // Set the button background color to a pale blue
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set the button background color to a pale blue


    // highscore
    std::ifstream inputFile("./data/highscore.txt");
    int highscore = 0;
    if (inputFile.good()) {
        inputFile >> highscore;
        inputFile.close();
    }
    else {
        std::ofstream outputFile("./data/highscore.txt");
        if (outputFile.good()) {
            outputFile << "0";
            outputFile.close();
        }
        else {
            std::cerr << "Failed to create a file: data/highscore.txt" << std::endl;
        }

    }

    /*int width, height, numComponents;
    unsigned char* imageData = stbi_load("textures/bricks.jpg", &width, &height, &numComponents, 4);
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(imageData);

    std::cout << textureId << std::endl;

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::GetWindowDrawList()->AddImage((ImTextureID)textureId, windowPos, ImVec2(windowPos.x + displaySize.x, windowPos.y + displaySize.y));
*/

    ImGui::Begin("Menu", pOpen, flags); // start our gui scene

    ImVec2 windowSize = ImGui::GetWindowSize(); // get current window size

    font->Scale = 3.0f;
    ImGui::PushFont(font);

    // write the score
    char buffer[100];
    std::sprintf(buffer, "Highscore: %d", highscore);
    ImVec2 textSize = ImGui::CalcTextSize(buffer);
    ImVec2 textPos = ImVec2((windowSize.x - textSize.x) / 2.0f, (windowSize.y - textSize.y) / 3.35f);
    ImGui::SetCursorPos(textPos);

    ImGui::Text(buffer);

    font->Scale = 1.0f;
    ImGui::PopFont();


    // calculate center position and size for button
    ImVec2 buttonSize = ImVec2(windowSize.x / 4, windowSize.y / 8);
    ImVec2 buttonPos = ImVec2((windowSize.x - buttonSize.x) / 2, (windowSize.y - buttonSize.y) / 2);
    ImGui::SetCursorPos(buttonPos); // button position

    // change font size and push it for use
    font->Scale = 2.2f; 
    ImGui::PushFont(font);

    // create button
    if (ImGui::Button("Start Game", buttonSize)) 
    {
		SoundHandler::getInstance().PlayOurSound("button");								   
        currentState = GameState::Level1;
        LoadLevel1();
    }

    // calculate center position and size for button
    buttonSize = ImVec2(windowSize.x / 6, windowSize.y / 8);
    buttonPos = ImVec2((windowSize.x - buttonSize.x) / 2, (windowSize.y - buttonSize.y) / 1.35f);
    ImGui::SetCursorPos(buttonPos); // button position
    
    // create button
    if (ImGui::Button("Exit", buttonSize))
    {
        std::exit(0);
    }

    // change the font back to the original state
    font->Scale = 1.0f;
    ImGui::PopFont();

    ImGui::End(); // end menu gui scene
    ImGui::PopStyleColor(); // Reset the style color back to its default value - for window
    //ImGui::PopStyleColor(); // Reset the style color back to its default value - for button

}

void SimpleScene::BuildLevel() {
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // set windows start position (0,0) here
    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always); // set windows size to our game size (dynamic size)

    ImFont* font = ImGui::GetFont(); // get font of text (like text in buttons)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.8f)); // Set the button background color to a transparent light gray.

    ImGui::Begin("Level", pOpen, flags); // start our gui scene


    // change font size and push it for use
    font->Scale = 2.2f;
    ImGui::PushFont(font);
    ImGui::Text("Current Level: %d", currentLevel);
    ImGui::Text("Your score is: %d", score);
    ImGui::Text("Camera: ");
    ImGui::PopFont();

    for (int i = 0; i < camList.size(); i++) {
        ImGui::SameLine(0);
        bool selectedCamera = camList[i] == camera;
        if (selectedCamera)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        if (ImGui::Button(std::to_string(i + 1).c_str())) {
            SoundHandler::getInstance().PlayOurSound("button");
            SetCamera(i);
        }
        if (selectedCamera)
            ImGui::PopStyleColor();
    }

    // Get the size of the ImGui window
    ImVec2 windowSize = ImGui::GetWindowSize();

    // Set the X position of the cursor to the center of the window
    ImGui::SetCursorPosX((windowSize.x / 2) - 100); // Assuming the button is 200 pixels wide
    if (!paused && ImGui::Button("Pause", ImVec2(200, 35))) {
        SoundHandler::getInstance().PlayOurSound("button");
        paused = true;
    }
    else if (paused && ImGui::Button("Unpause", ImVec2(200, 35))) {
        SoundHandler::getInstance().PlayOurSound("button");
        paused = false;
    }

    ImGui::End();
    ImGui::PopStyleColor();
}

void SimpleScene::BuildBetweenLevels() {
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    ImVec2 displaySize = ImGui::GetIO().DisplaySize; // get display size of our game size

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // set windows start position (0,0) here
    ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always); // set windows size to the entire display

    ImFont* font = ImGui::GetFont(); // get font of text (like text in buttons)

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.75f)); // Set the button background color to a pale blue

    ImGui::Begin("Between Levels", pOpen, flags); // start our gui scene

    ImVec2 windowSize = ImGui::GetWindowSize(); // get current window size


    // write you win or you beat stage x
    ImVec2 textWinSize;
    ImVec2 textWinPos;
    font->Scale = 3.0f;
    ImGui::PushFont(font);
    
    char buffer[100];
    if (currentLevel < 3 && currentLevel != -1) std::sprintf(buffer, "You beat stage %d!", currentLevel);
    else if (currentLevel != -1) {
        std::sprintf(buffer, "You Win!");
        // save highscore to file
        std::ofstream outputFile("./data/highscore.txt");
        if (outputFile.good()) {
            outputFile << score;
            outputFile.close();
        }
    }
    else {
        std::sprintf(buffer, "You've Lost!");
        // save highscore to file
        std::ofstream outputFile("./data/highscore.txt");
        if (outputFile.good()) {
            outputFile << score;
            outputFile.close();
        }
    }

    textWinSize = ImGui::CalcTextSize(buffer);
    textWinPos = ImVec2((windowSize.x - textWinSize.x) / 2.0f, (windowSize.y - textWinSize.y) / 2.5f);
    ImGui::SetCursorPos(textWinPos);

    ImGui::Text(buffer);


    // write the score
    char buffer2[100];
    std::sprintf(buffer2, "Score: %d", score);
    ImVec2 textSize = ImGui::CalcTextSize(buffer2);
    ImVec2 textPos = ImVec2((windowSize.x - textSize.x) / 2.0f, (windowSize.y - textSize.y) / 2.0f);
    ImGui::SetCursorPos(textPos);

    ImGui::Text("Score: %d", score);
    
    font->Scale = 1.0f;
    ImGui::PopFont();
    font->Scale = 2.2f;
    ImGui::PushFont(font);
    
    // calculate center position and size for button
    ImVec2 buttonSize = ImVec2(windowSize.x / 4, windowSize.y / 8);
    ImVec2 buttonPos = ImVec2((windowSize.x - buttonSize.x) / 2, (windowSize.y - buttonSize.y) / 1.5f);
    ImGui::SetCursorPos(buttonPos); // button position

    // create button
    if (currentLevel < 3 && currentLevel != -1 && ImGui::Button("Next Level", buttonSize))
    {
		SoundHandler::getInstance().PlayOurSound("button");
        currentLevel++;
        if (currentLevel == 2) {
            currentState = GameState::Level2;
            LoadLevel2();
        }
        else if (currentLevel == 3) {
            currentState = GameState::Level3;
            LoadLevel3();
        }
        paused = false;
    }
    else if ((currentLevel == 3 || currentLevel == -1) && ImGui::Button("Exit Game", buttonSize))
    {
        std::exit(0);
    }

    // change the font back to the original state
    font->Scale = 1.0f;
    ImGui::PopFont();

    ImGui::End();
    ImGui::PopStyleColor();
}

void SimpleScene::GoToEndGamePanel() {
    currentLevel = -1;
    currentState = GameState::BetweenLevels;
    paused = true;
}

#pragma endregion

#pragma region Levels
//list of objects
std::list<GameObject*> gameObjectsLst;

void SimpleScene::EndLevel() {
    currentState = GameState::BetweenLevels;
    paused = true;
}

void SimpleScene::LoadLevel1() {
    currentLevel = 1;
    amountOfApples = 4;
    // setup snake position
    
    // load objects
    auto appleMaterial{ std::make_shared<Material>("blank", program) };
    appleMaterial->AddTexture(0, "textures/appleTexture2.jpg", 2);
    GameObject* apple1 = new AppleObject(root, appleMaterial, this);
    GameObject* apple2 = new AppleObject(root, appleMaterial, this);
    GameObject* apple3 = new AppleObject(root, appleMaterial, this);
    GameObject* apple4 = new AppleObject(root, appleMaterial, this);
    gameObjectsLst.push_back(apple1);
    gameObjectsLst.push_back(apple2);
    gameObjectsLst.push_back(apple3);
    gameObjectsLst.push_back(apple4);

    auto bombMaterial{ std::make_shared<Material>("blank", program) };
    bombMaterial->AddTexture(0, "textures/carbon.jpg", 2);
    GameObject* bomb1 = new BombObject(root, bombMaterial, this);   
    GameObject* bomb2 = new BombObject(root, bombMaterial, this);
    gameObjectsLst.push_back(bomb1);
    gameObjectsLst.push_back(bomb2);

    auto ballMaterial{ std::make_shared<Material>("blank", program) };
    ballMaterial->AddTexture(0, "textures/ballTexture.jpg", 2);
    GameObject* ball1 = new BallObject(root, ballMaterial, this);
    gameObjectsLst.push_back(ball1);
    SetupTexturesBall();

    auto coinMaterial{ std::make_shared<Material>("blank", program) };
    coinMaterial->AddTexture(0, "textures/gold.jpg", 2);
    for (int i = 0; i < 10; i++) {
        GameObject* coin = new CoinObject(root, coinMaterial, this);
        gameObjectsLst.push_back(coin);
    }

    // reset camera
    SetCamera(0);

    allowMove = false;
}

void SimpleScene::LoadLevel2() {
    
    for (GameObject* obj : gameObjectsLst) {
        obj->model->isHidden = true;
    }

    // load objects
    amountOfApples = 5;

    auto appleMaterial{ std::make_shared<Material>("blank", program) };
    appleMaterial->AddTexture(0, "textures/appleTexture2.jpg", 2);
    GameObject* apple1 = new AppleObject(root, appleMaterial, this);
    GameObject* apple2 = new AppleObject(root, appleMaterial, this);
    GameObject* apple3 = new AppleObject(root, appleMaterial, this);
    GameObject* apple4 = new AppleObject(root, appleMaterial, this);
    GameObject* apple5 = new AppleObject(root, appleMaterial, this);
    gameObjectsLst.push_back(apple1);
    gameObjectsLst.push_back(apple2);
    gameObjectsLst.push_back(apple3);
    gameObjectsLst.push_back(apple4);
    gameObjectsLst.push_back(apple5);

    auto bombMaterial{ std::make_shared<Material>("blank", program) };
    bombMaterial->AddTexture(0, "textures/carbon.jpg", 2);
    GameObject* bomb1 = new BombObject(root, bombMaterial, this);
    GameObject* bomb2 = new BombObject(root, bombMaterial, this);
    GameObject* bomb3 = new BombObject(root, bombMaterial, this);
    GameObject* bomb4 = new BombObject(root, bombMaterial, this);
    gameObjectsLst.push_back(bomb1);
    gameObjectsLst.push_back(bomb2);
    gameObjectsLst.push_back(bomb3);
    gameObjectsLst.push_back(bomb4);

    auto ballMaterial{ std::make_shared<Material>("blank", program) };
    ballMaterial->AddTexture(0, "textures/ballTexture.jpg", 2);
    GameObject* ball1 = new BallObject(root, ballMaterial, this);
    GameObject* ball2 = new BallObject(root, ballMaterial, this);
    gameObjectsLst.push_back(ball1);
    gameObjectsLst.push_back(ball2);
    SetupTexturesBall();

    auto coinMaterial{ std::make_shared<Material>("blank", program) };
    coinMaterial->AddTexture(0, "textures/gold.jpg", 2);
    for (int i = 0; i < 10; i++) {
        GameObject* coin = new CoinObject(root, coinMaterial, this);
        gameObjectsLst.push_back(coin);
    }

	daylight->AddTexture(0, "textures/cubemaps/Daylight Box2_", 3);
    allowMove = false;
}

void SimpleScene::LoadLevel3() {
    for (GameObject* obj : gameObjectsLst) {
        obj->model->isHidden = true;
    }

    // load objects
    amountOfApples = 6;

    auto appleMaterial{ std::make_shared<Material>("blank", program) };
    appleMaterial->AddTexture(0, "textures/appleTexture2.jpg", 2);
    GameObject* apple1 = new AppleObject(root, appleMaterial, this);
    GameObject* apple2 = new AppleObject(root, appleMaterial, this);
    GameObject* apple3 = new AppleObject(root, appleMaterial, this);
    GameObject* apple4 = new AppleObject(root, appleMaterial, this);
    GameObject* apple5 = new AppleObject(root, appleMaterial, this);
    GameObject* apple6 = new AppleObject(root, appleMaterial, this);
    gameObjectsLst.push_back(apple1);
    gameObjectsLst.push_back(apple2);
    gameObjectsLst.push_back(apple3);
    gameObjectsLst.push_back(apple4);
    gameObjectsLst.push_back(apple5);
    gameObjectsLst.push_back(apple6);

    auto bombMaterial{ std::make_shared<Material>("blank", program) };
    bombMaterial->AddTexture(0, "textures/carbon.jpg", 2);
    GameObject* bomb1 = new BombObject(root, bombMaterial, this);
    GameObject* bomb2 = new BombObject(root, bombMaterial, this);
    GameObject* bomb3 = new BombObject(root, bombMaterial, this);
    GameObject* bomb4 = new BombObject(root, bombMaterial, this);
    GameObject* bomb5 = new BombObject(root, bombMaterial, this);
    GameObject* bomb6 = new BombObject(root, bombMaterial, this);
    GameObject* bomb7 = new BombObject(root, bombMaterial, this);
    gameObjectsLst.push_back(bomb1);
    gameObjectsLst.push_back(bomb2);
    gameObjectsLst.push_back(bomb3);
    gameObjectsLst.push_back(bomb4);
    gameObjectsLst.push_back(bomb5);
    gameObjectsLst.push_back(bomb6);
    gameObjectsLst.push_back(bomb7);

    auto ballMaterial{ std::make_shared<Material>("blank", program) };
    ballMaterial->AddTexture(0, "textures/ballTexture.jpg", 2);
    GameObject* ball1 = new BallObject(root, ballMaterial, this);
    GameObject* ball2 = new BallObject(root, ballMaterial, this);
    GameObject* ball3 = new BallObject(root, ballMaterial, this);
    gameObjectsLst.push_back(ball1);
    gameObjectsLst.push_back(ball2);
    gameObjectsLst.push_back(ball3);
    SetupTexturesBall();

    auto coinMaterial{ std::make_shared<Material>("blank", program) };
    coinMaterial->AddTexture(0, "textures/gold.jpg", 2);
    for (int i = 0; i < 10; i++) {
        GameObject* coin = new CoinObject(root, coinMaterial, this);
        gameObjectsLst.push_back(coin);
    }

	daylight->AddTexture(0, "textures/cubemaps/Daylight Box3_", 3);
    allowMove = false;
}

#pragma endregion


SimpleScene::SimpleScene(std::string name, Display* display) : SceneWithImGui(std::move(name), display)
{
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}

void SimpleScene::Init(float fov, int width, int height, float near2, float far2)
{
    // create the root object that will contain all visible models
    root = Movable::Create("root");
    AddChild(root);

    daylight = std::make_shared<Material>("daylight", "shaders/cubemapShader");
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    root->AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

    meshDataIndex = 0;
    
    // add snake here ////
    SetupSnake();
    //////////////////////

    SetupCameras(fov, width, height, near2, far2);

}

#pragma region Snake Setup


void SimpleScene::SetupSnake()
{
    program = std::make_shared<Program>("shaders/basicShader");
    auto snakeMesh{ IglLoader::MeshFromFiles("snakeMesh", "data/snake1.obj")};
    auto snakeMaterial{ std::make_shared<Material>("blank", program) };
    snakeMaterial->AddTexture(0, "textures/snake.jpg", 2);
    snake = Model::Create("snake", snakeMesh, snakeMaterial);

    auto morphFunc = [&](Model* model, cg3d::Visitor* visitor) {
        return meshDataIndex;
    };
    /*autoSnake = AutoMorphingModel::Create(*snake, morphFunc);
    autoSnake->showWireframe = true;
    root->AddChild(autoSnake);*/


    // Joints
    // create first cylinder and add as child to root
    snakes.push_back(AutoMorphingModel::Create(*snake, morphFunc));
    snakes[0]->Scale(scaleFactor, Axis::Z);
    snakes[0]->SetCenter(Eigen::Vector3f(0, 0, -0.8f * scaleFactor));
    snakes[0]->RotateByDegree(90, Eigen::Vector3f(0, 1, 0));
    root->AddChild(snakes[0]);

    treeSnake.init(snakes[0]->GetMesh()->data[0].vertices, snakes[0]->GetMesh()->data[0].faces);
	
    // Textures
    SetupTextures(0);
    SetupJoins();

}

void SimpleScene::SetupJoins() {
    auto morphFunc = [&](Model* model, cg3d::Visitor* visitor) {
        return meshDataIndex;
    };
    auto program = std::make_shared<Program>("shaders/basicShader");
    auto snakeMesh{ IglLoader::MeshFromFiles("snakeMesh", "data/snake1.obj") };
    auto snakeMaterial{ std::make_shared<Material>("blank", program) };
    snakeMaterial->AddTexture(0, "textures/snake.jpg", 2);
    for (int i = 1; i < 16; i++) {
        snake = Model::Create("snake", snakeMesh, snakeMaterial);
        snakes.push_back(AutoMorphingModel::Create(*snake, morphFunc));
        snakes[i]->Scale(scaleFactor, Axis::Z);
        snakes[i]->Translate(1.6f * scaleFactor, Axis::Z);
        snakes[i]->SetCenter(Eigen::Vector3f(0, 0, -0.8f * scaleFactor));
        snakes[i - 1]->AddChild(snakes[i]);

        SetupTextures(i);

        snakes[i]->isHidden = true;
    }
}

void SimpleScene::AddPart() {
    if (snakeLength > 15) return;

    snakes[snakeLength]->isHidden = false;
    snakeLength++;
}

void SimpleScene::SetupTextures(int index) 
{
    Eigen::MatrixXd textureCoords; //texture map

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;

    cg3d::MeshData snakeMeshData = snakes[index]->GetMesh()->data[0];

    int sizeOfVRows = snakeMeshData.vertices.rows();
    int sizeOfFRows = snakeMeshData.faces.rows();

    V.resize(snakeMeshData.vertices.rows(), snakeMeshData.vertices.cols());
    F.resize(snakeMeshData.faces.rows(), snakeMeshData.faces.cols());

    for (int i = 0; i < sizeOfVRows; i++) {
        V.row(i) = snakeMeshData.vertices.row(i);
        // need to do some changes here
    }

    for (int i = 0; i < sizeOfFRows; i++) {
        F.row(i) = snakeMeshData.faces.row(i);
        // need to do some changes here
    }

    Eigen::MatrixXd TC(V.rows(), 2);
    Eigen::VectorXd vec(V.rows());
    vec = V.col(0) + V.col(1) + V.col(2);
    vec /= 3;
    TC.col(0) = vec;
    TC.col(1) = V.col(0);
	TC *= 2;

    auto mesh = snakes[index]->GetMeshList();
    mesh[0]->data.push_back({ V, F, snakeMeshData.vertexNormals, TC });
    snakes[index]->SetMeshList(mesh);
    snakes[index]->meshIndex = snakes[index]->meshIndex + 1;
    meshDataIndex = 1;

}

void SimpleScene::SetupTexturesBall() {

    for (GameObject* ballObj : gameObjectsLst) {
        if (ballObj->id != 2) continue;
        
        Eigen::MatrixXd textureCoords; //texture map

        Eigen::MatrixXd V;
        Eigen::MatrixXi F;

        cg3d::MeshData snakeMeshData = ballObj->model->GetMesh()->data[0];

        int sizeOfVRows = snakeMeshData.vertices.rows();
        int sizeOfFRows = snakeMeshData.faces.rows();

        V.resize(snakeMeshData.vertices.rows(), snakeMeshData.vertices.cols());
        F.resize(snakeMeshData.faces.rows(), snakeMeshData.faces.cols());

        for (int i = 0; i < sizeOfVRows; i++) {
            V.row(i) = snakeMeshData.vertices.row(i);
        }

        for (int i = 0; i < sizeOfFRows; i++) {
            F.row(i) = snakeMeshData.faces.row(i);
        }

        Eigen::MatrixXd TC(V.rows(), 2);
        Eigen::VectorXd vec(V.rows());
        vec = V.col(0) + V.col(1) + V.col(2);
        vec /= 3;
        TC.col(0) = V.col(1);
        TC.col(1) = vec;
        TC *= 2;

        auto mesh = ballObj->model->GetMeshList();
        mesh[0]->data.push_back({ V, F, snakeMeshData.vertexNormals, TC });
        ballObj->model->SetMeshList(mesh);
        ballObj->model->meshIndex = ballObj->model->meshIndex + 1;
        ballObj->meshDataIndex = 1;
    }
}

#pragma endregion

#pragma region Collision

bool SimpleScene::IsColliding() {
    for (GameObject* curObj : gameObjectsLst) {
        // check only if close
        Eigen::Vector3f distance = snakes[0]->GetTranslation() - curObj->model->GetTranslation();
        // the smaller the number after the <    the less accurate the collision check will be but faster.
        if (sqrt(pow(distance[0], 2) + pow(distance[1], 2) + pow(distance[2], 2)) < 3) {
            if (AreTreesColliding(treeSnake, curObj->tree, snakes[0], curObj->model)) {
                // collided with curObj
                curObj->CollidedWithSnake();
                return true;
            }
        }

    }
    return false;
}


bool SimpleScene::AreTreesColliding(igl::AABB<Eigen::MatrixXd, 3>& firstTree, igl::AABB<Eigen::MatrixXd, 3>& secondTree, std::shared_ptr<cg3d::Model> firstModel, std::shared_ptr<cg3d::Model> secondModel)
{
    if (!AreBoxesColliding(firstTree, secondTree, firstModel, secondModel)) {
        return false;
    }
    else {
        // there is box collision, check if leaves or need to go deeper.
        // check if they are leaves
        if (firstTree.is_leaf() && secondTree.is_leaf()) return true;
        else if (firstTree.is_leaf()) {
            if (secondTree.m_left != nullptr) return AreTreesColliding(firstTree, *secondTree.m_left, firstModel, secondModel);
            if (secondTree.m_right != nullptr) return AreTreesColliding(firstTree, *secondTree.m_right, firstModel, secondModel);
        }
        else if (secondTree.is_leaf()) {
            if (firstTree.m_left != nullptr) return AreTreesColliding(*firstTree.m_left, secondTree, firstModel, secondModel);
            if (firstTree.m_right != nullptr) return AreTreesColliding(*firstTree.m_right, secondTree, firstModel, secondModel);
        }
        else {
            // both are not leaves, check all
            return (AreTreesColliding(*firstTree.m_left, *secondTree.m_left, firstModel, secondModel) || AreTreesColliding(*firstTree.m_left, *secondTree.m_right, firstModel, secondModel)
                || AreTreesColliding(*firstTree.m_right, *secondTree.m_right, firstModel, secondModel) || AreTreesColliding(*firstTree.m_right, *secondTree.m_left, firstModel, secondModel));
        }
    }

    return false;
}

bool SimpleScene::AreBoxesColliding(igl::AABB<Eigen::MatrixXd, 3>& firstTree, igl::AABB<Eigen::MatrixXd, 3>& secondTree, std::shared_ptr<cg3d::Model> firstModel, std::shared_ptr<cg3d::Model> secondModel)
{
    Eigen::AlignedBox<double, 3>& firstBox = firstTree.m_box;
    Eigen::AlignedBox<double, 3>& secondBox = secondTree.m_box;

    Eigen::Matrix3d firstRotation = firstModel->GetRotation().cast<double>();
    Eigen::Matrix3d secondRotation = secondModel->GetRotation().cast<double>();

    // Setting up our variables...

    double a_0 = firstBox.sizes()[0] / 2;
    double a_1 = firstBox.sizes()[1] / 2;
    double a_2 = firstBox.sizes()[2] / 2;
    double b_0 = secondBox.sizes()[0] / 2;
    double b_1 = secondBox.sizes()[1] / 2;
    double b_2 = secondBox.sizes()[2] / 2;

    // all rotations
    Eigen::RowVector3d A_0 = firstRotation * Eigen::Vector3d(1, 0, 0);
    Eigen::RowVector3d A_1 = firstRotation * Eigen::Vector3d(0, 1, 0);
    Eigen::RowVector3d A_2 = firstRotation * Eigen::Vector3d(0, 0, 1);
    Eigen::RowVector3d B_0 = secondRotation * Eigen::Vector3d(1, 0, 0);
    Eigen::RowVector3d B_1 = secondRotation * Eigen::Vector3d(0, 1, 0);
    Eigen::RowVector3d B_2 = secondRotation * Eigen::Vector3d(0, 0, 1);

    //// all rotation to matrix
    Eigen::Matrix3d A;
    A << Eigen::RowVector3d(A_0[0], A_1[0], A_2[0]), Eigen::RowVector3d(A_0[1], A_1[1], A_2[1]), Eigen::RowVector3d(A_0[2], A_1[2], A_2[2]);
    Eigen::Matrix3d B;
    B << Eigen::RowVector3d(B_0[0], B_1[0], B_2[0]), Eigen::RowVector3d(B_0[1], B_1[1], B_2[1]), Eigen::RowVector3d(B_0[2], B_1[2], B_2[2]);

    // from equation
    Eigen::Matrix3d C = B * A.transpose();

    Eigen::Matrix4f firstTransform = firstModel->GetTransform().cast<float>();
    Eigen::Matrix4f secondTransform = secondModel->GetTransform().cast<float>();

    //// maybe we can do it without this calcs
    Eigen::Vector4f C_0_temp = firstTransform * Eigen::Vector4f(firstBox.center()[0], firstBox.center()[1], firstBox.center()[2], 1);
    Eigen::Vector4f C_1_temp = secondTransform * Eigen::Vector4f(secondBox.center()[0], secondBox.center()[1], secondBox.center()[2], 1);

    //// for equations
    Eigen::Vector3d D = Eigen::Vector3d(C_1_temp[0], C_1_temp[1], C_1_temp[2]) - Eigen::Vector3d(C_0_temp[0], C_0_temp[1], C_0_temp[2]);


    double first, second, sum;
    /// All 15 cases from the lectures
    first = (a_1 * abs(C(2, 0))) + (a_2 * abs(C(1, 0)));
    second = (b_1 * abs(C(0, 2))) + (b_2 * abs(C(1, 0)));
    sum = abs((C(1, 0) * A_2).dot(D) - (C(2, 0) * A_1).dot(D));
    if (sum > first + second) return false;

    first = (a_1 * abs(C(2, 1))) + (a_2 * abs(C(1, 1)));
    second = (b_0 * abs(C(0, 2))) + (b_2 * abs(C(0, 0)));
    sum = abs((C(1, 1) * A_2).dot(D) - (C(2, 1) * A_1).dot(D));
    if (sum > first + second) return false;

    first = (a_1 * abs(C(2, 2))) + (a_2 * abs(C(1, 2)));
    second = (b_0 * abs(C(0, 1))) + (b_1 * abs(C(0, 0)));
    sum = abs((C(1, 2) * A_2).dot(D) - (C(2, 2) * A_1).dot(D));
    if (sum > first + second) return false;

    first = (a_0 * abs(C(2, 0))) + (a_2 * abs(C(0, 0)));
    second = (b_1 * abs(C(1, 2))) + (b_2 * abs(C(1, 1)));
    sum = abs((C(2, 0) * A_0).dot(D) - (C(0, 0) * A_2).dot(D));
    if (sum > first + second) return false;

    first = (a_0 * abs(C(2, 1))) + (a_2 * abs(C(0, 1)));
    second = (b_0 * abs(C(1, 2))) + (b_2 * abs(C(1, 0)));
    sum = abs((C(2, 1) * A_0).dot(D) - (C(0, 1) * A_2).dot(D));
    if (sum > first + second) return false;

    first = (a_0 * abs(C(2, 2))) + (a_2 * abs(C(0, 2)));
    second = (b_0 * abs(C(1, 1))) + (b_1 * abs(C(1, 0)));
    sum = abs((C(2, 2) * A_0).dot(D) - (C(0, 2) * A_2).dot(D));
    if (sum > first + second) return false;

    first = (a_0 * abs(C(1, 0))) + (a_1 * abs(C(0, 0)));
    second = (b_1 * abs(C(2, 2))) + (b_2 * abs(C(2, 1)));
    sum = abs((C(0, 0) * A_1).dot(D) - (C(1, 0) * A_0).dot(D));
    if (sum > first + second) return false;

    first = (a_0 * abs(C(1, 1))) + (a_1 * abs(C(0, 1)));
    second = (b_0 * abs(C(2, 2))) + (b_2 * abs(C(2, 0)));
    sum = abs((C(0, 1) * A_1).dot(D) - (C(1, 1) * A_0).dot(D));
    if (sum > first + second) return false;

    first = (a_0 * abs(C(1, 2))) + (a_1 * abs(C(0, 2)));
    second = (b_0 * abs(C(2, 1))) + (b_1 * abs(C(2, 0)));
    sum = abs((C(0, 2) * A_1).dot(D) - (C(1, 2) * A_0).dot(D));
    if (sum > first + second) return false;

    first = (b_0 * abs(C(0, 0))) + (b_1 * abs(C(0, 1))) + (b_2 * abs(C(0, 2)));
    sum = abs(A_0.dot(D));
    if (sum > first + a_0) return false;

    first = (b_0 * abs(C(1, 0))) + (b_1 * abs(C(1, 1))) + (b_2 * abs(C(1, 2)));
    sum = abs(A_1.dot(D));
    if (sum > first + a_1) return false;

    first = (b_0 * abs(C(2, 0))) + (b_1 * abs(C(2, 1))) + (b_2 * abs(C(2, 2)));
    sum = abs(A_2.dot(D));
    if (sum > first + a_2) return false;

    first = (a_0 * abs(C(0, 0))) + (a_1 * abs(C(1, 0))) + (a_2 * abs(C(2, 0)));
    sum = abs(B_0.dot(D));
    if (sum > first + b_0) return false;

    first = (a_0 * abs(C(0, 1))) + (a_1 * abs(C(1, 1))) + (a_2 * abs(C(2, 1)));
    sum = abs(B_1.dot(D));
    if (sum > first + b_1) return false;

    first = (a_0 * abs(C(0, 2))) + (a_1 * abs(C(1, 2))) + (a_2 * abs(C(2, 2)));
    sum = abs(B_2.dot(D));
    if (sum > first + b_2) return false;

    return true;
}

#pragma endregion
#pragma region Input Callbacks


void SimpleScene::KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    auto system = camera->GetRotation().transpose();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_UP:
            snakes[0]->RotateByDegree(8, Axis::X);
            if (snakeLength > 1)
                snakes[1]->RotateByDegree(-8, Axis::X);
            for (int i = 2; i < snakeLength; i++) {
                snakes[i]->RotateByDegree(-8 / i, Axis::X);
            }
            break;
        case GLFW_KEY_DOWN:
            snakes[0]->RotateByDegree(-8, Axis::X);
            if (snakeLength > 1)
                snakes[1]->RotateByDegree(8, Axis::X);
            for (int i = 2; i < snakeLength; i++) {
                snakes[i]->RotateByDegree(8 / i, Axis::X);
            }
            break;
        case GLFW_KEY_LEFT:
            snakes[0]->RotateByDegree(8, Axis::Y);
            if (snakeLength > 1)
                snakes[1]->RotateByDegree(-8, Axis::Y);
            for (int i = 2; i < snakeLength; i++) {
                snakes[i]->RotateByDegree(-8 / i, Axis::Y);
            }
            break;
        case GLFW_KEY_RIGHT:
            snakes[0]->RotateByDegree(-8, Axis::Y);
            if (snakeLength > 1)
                snakes[1]->RotateByDegree(8, Axis::Y);
            for (int i = 2; i < snakeLength; i++) {
                snakes[i]->RotateByDegree(8 / i, Axis::Y);
            }
            break;
        case GLFW_KEY_W:
            camera->TranslateInSystem(system, { 0, 0.05f, 0 });
            break;
        case GLFW_KEY_S:
            camera->TranslateInSystem(system, { 0, -0.05f, 0 });
            break;
        case GLFW_KEY_A:
            camera->TranslateInSystem(system, { -0.05f, 0, 0 });
            break;
        case GLFW_KEY_D:
            camera->TranslateInSystem(system, { 0.05f, 0, 0 });
            break;
        case GLFW_KEY_B:
            camera->TranslateInSystem(system, { 0, 0, 0.05f });
            break;
        case GLFW_KEY_F:
            camera->TranslateInSystem(system, { 0, 0, -0.05f });
            break;
        case GLFW_KEY_1:
            SetCamera(0);
            break;
        case GLFW_KEY_2:
            SetCamera(1);
            break;
        case GLFW_KEY_3:
            SetCamera(2);
            break;
        case GLFW_KEY_4:
            score += 1000;
            currentState = GameState::BetweenLevels;
            paused = true;
            break;
        case GLFW_KEY_5:
            AddPart();
            break;
        case GLFW_KEY_SPACE:
            allowMove = true;
            prevTime = std::chrono::high_resolution_clock::now();
            break;
        }

    }
}

#pragma endregion

void SimpleScene::SetupCameras(float fov, int width, int height, float near2, float far2)
{
    // create the camera objects
    camList.resize(camList.capacity());
    camList[0] = Camera::Create("camera0", fov, float(width) / float(height), near2, far2);
    
    auto program = std::make_shared<Program>("shaders/basicShader"); // TODO: TAL: replace with hard-coded basic program
    auto carbon = std::make_shared<Material>("carbon", program); // default material
    carbon->AddTexture(0, "textures/carbon.jpg", 2);
    auto camera1 = Camera::Create("camera1", fov, double(width) / height, near2, far2);
    auto model = ObjLoader::ModelFromObj("camera1", "data/camera.obj", carbon);
    model->isHidden = true;
    snakes[0]->AddChild(camList[1] = CamModel::Create(*camera1, *model));
    auto camera2 = Camera::Create("camera2", fov, double(width) / height, near2, far2);
    auto model2 = ObjLoader::ModelFromObj("camera2", "data/camera.obj", carbon);
    model2->isHidden = true;
    snakes[0]->AddChild(camList[2] = CamModel::Create(*camera2, *model2));

    camList[0]->Translate(50, Axis::Y);
    camList[0]->RotateByDegree(-90, Axis::X);
    camList[1]->Translate(-1, Axis::Z);
    camList[2]->Translate(20, Axis::Y);
    camList[2]->RotateByDegree(-90, Axis::X);

    camera = camList[0];
}

void SimpleScene::SetCamera(int index)
{
    camera = camList[index];
    viewport->camera = camera;
}

void SimpleScene::AddViewportCallback(Viewport* _viewport)
{
    viewport = _viewport;
    Scene::AddViewportCallback(viewport);
}

void SimpleScene::FollowSnake()
{
    Eigen::Vector3f vec1 = Eigen::Vector3f(1, 0, 0);
    Eigen::Vector3f vec2;
    Eigen::Vector3f vec3 = Eigen::Vector3f(0, 0, 1);
    Eigen::Vector3f vec4;
    Eigen::Vector3f vec5 = Eigen::Vector3f(0, 1, 0);
    Eigen::Vector3f vec6;

    for (int i = snakeLength - 1; i > 0; i--)
    {
        // X Axis
        vec2 = snakes[i]->Tout.rotation() * Eigen::Vector3f(1, 0, 0);
        Eigen::Quaternionf quat = Eigen::Quaternionf::FromTwoVectors(vec2, vec1);
        quat = quat.slerp(0.99, Eigen::Quaternionf::Identity());
        snakes[i]->Rotate(Eigen::Matrix3f(quat));
        // Y Axis
        vec4 = snakes[i]->Tout.rotation() * Eigen::Vector3f(0, 0, 1);
        quat = Eigen::Quaternionf::FromTwoVectors(vec4, vec3);
        quat = quat.slerp(0.99, Eigen::Quaternionf::Identity());
        snakes[i]->Rotate(Eigen::Matrix3f(quat));
        // Z Axis
        vec6 = snakes[i]->Tout.rotation() * Eigen::Vector3f(0, 1, 0);
        quat = Eigen::Quaternionf::FromTwoVectors(vec6, vec5);
        quat = quat.slerp(0.99, Eigen::Quaternionf::Identity());
        snakes[i]->Rotate(Eigen::Matrix3f(quat));
    }
}

Eigen::Vector3f SimpleScene::getTipOfCyl(int cyl_index) {
    Eigen::Matrix4f linkTransform = snakes[cyl_index]->GetAggregatedTransform();
    Eigen::Vector3f linkCenter = Eigen::Vector3f(linkTransform.col(3).x(), linkTransform.col(3).y(), linkTransform.col(3).z());
    return linkCenter + snakes[cyl_index]->GetRotation() * Eigen::Vector3f(0, 0, CYL_LENGTH / 2);		  
}

Eigen::Vector3f SimpleScene::getBaseOfCyl(int cyl_index) {
    // get base of cylinder
    Eigen::Matrix4f linkTransform = snakes[cyl_index]->GetAggregatedTransform();
    Eigen::Vector3f linkCenter = Eigen::Vector3f(linkTransform.col(3).x(), linkTransform.col(3).y(), linkTransform.col(3).z());
    return linkCenter - snakes[cyl_index]->GetRotation() * Eigen::Vector3f(0, 0, CYL_LENGTH / 2);
}		  

void SimpleScene::Update(const Program& p, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    if (currentState == GameState::MainMenu) return;
    
    Scene::Update(p, proj, view, model);
	
    if (paused || !allowMove) return;	

    RemoveAndDestroyObjects();
	
    for (GameObject* var : gameObjectsLst)
    {
        var->moveObject();
	}		

    // how much time elapsed between the last frame and this one
    std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(endTime - prevTime).count();
    prevTime = endTime;

    auto system = snakes[0]->GetRotation().transpose();
    if (speedy == false) snakes[0]->TranslateInSystem(system, { 0 , 0, -2.0f * deltaTime});
    else snakes[0]->TranslateInSystem(system, { 0 , 0, -6.0f * deltaTime });
    FollowSnake();
	
    currentUpdateAmount++;
    if (speedy == true) {
        // add timer and check
        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - oldTime);
        if (elapsed.count() >= 3000) speedy = false;
    }

    if (currentUpdateAmount >= checkEveryXUpdates) {
        currentUpdateAmount = 0;
        IsColliding();
    }
}

#pragma region GameObjects
void SimpleScene::appearingObject() {
    auto program = std::make_shared<Program>("shaders/basicShader"); // TODO: TAL: replace with hard-coded basic program
    //auto cubeMesh{ IglLoader::MeshFromFiles("cubeMesh", "data/cube.off") };
    auto cubeMaterial{ std::make_shared<Material>("blank", program) };
    cubeMaterial->AddTexture(0, "textures/grass.bmp", 2);
    auto cube = Model::Create("cube", Mesh::Cube(), cubeMaterial);
    root->AddChild(cube);

    std::random_device rd;
    std::mt19937 gen(rd());

    int min_num = -20;
    int max_num = 20;

    std::uniform_int_distribution<> distr(min_num, max_num);
    // generate a random number between min_num and max_num
    cube->Translate(distr(gen), Axis::X);
    cube->Translate(distr(gen), Axis::Z);
}

void SimpleScene::RemoveAndDestroyObjects() {
													   
    for (auto it = gameObjectsLst.begin(); it != gameObjectsLst.end(); ) {
        if ((*it)->model->isHidden == true) {
            delete *it;
            it = gameObjectsLst.erase(it);
        }
        else {
            ++it;
        }
    }
}

void SimpleScene::AddScore(int amount) {
    score += amount;
}

#pragma endregion
