cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME "fragments-of-the-sky")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Main.cpp
        Source/Utils/Math.cpp
        Source/Utils/Random.cpp
        Source/Actors/Actor.cpp
        Source/Components/Component.cpp
        Source/Game.cpp
        Source/Components/DrawComponents/DrawComponent.cpp
        Source/Components/DrawComponents/DrawComponent.h
        Source/Components/RigidBodyComponent.cpp
        Source/Components/RigidBodyComponent.h
        Source/Utils/Json.h
        Source/Utils/CSV.h
        Source/Components/DrawComponents/DrawSpriteComponent.cpp
        Source/Components/DrawComponents/DrawSpriteComponent.h
        Source/Components/DrawComponents/DrawPolygonComponent.cpp
        Source/Components/DrawComponents/DrawPolygonComponent.h
        Source/Actors/Aeris.cpp
        Source/Actors/Aeris.h
        Source/Components/ColliderComponents/AABBColliderComponent.cpp
        Source/Components/ColliderComponents/AABBColliderComponent.h
        Source/Components/DrawComponents/DrawAnimatedComponent.cpp
        Source/Components/DrawComponents/DrawAnimatedComponent.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/Systems/AudioSystem.cpp
        Source/Systems/AudioSystem.h
        Source/UI/Elements/UIScreen.h
        Source/UI/Elements/UIScreen.cpp
        Source/UI/Elements/UIFont.h
        Source/UI/Elements/UIFont.cpp
        Source/UI/Elements/UIButton.cpp
        Source/UI/Elements/UIButton.h
        Source/UI/Elements/UIText.cpp
        Source/UI/Elements/UIText.h
        Source/UI/Elements/UIElement.cpp
        Source/UI/Elements/UIElement.h
        Source/UI/HUD.cpp
        Source/UI/Elements/UIImage.cpp
        Source/UI/Elements/UIImage.h
        Source/Systems/SpatialHashing.cpp
        Source/Systems/SpatialHashing.h
        Source/Actors/Fragment.cpp
        Source/Actors/Fragment.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)
