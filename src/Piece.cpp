#include "Piece.hpp"

CameraHandler::CameraHandler(int screenWidth, int screenHeight) {
    camera.target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void CameraHandler::UpdateCamera() {
    float minZoom = 0.5f; // Minimum zoom level 
    float maxZoom = 1.0f; // Maximum zoom level
    float zoomSpeed = 0.02f; // Speed of zoom

    if (IsKeyDown(KEY_UP)) camera.zoom += zoomSpeed;
    if (IsKeyDown(KEY_DOWN)) camera.zoom -= zoomSpeed;

    // Clamp zoom
    if (camera.zoom < minZoom) camera.zoom = minZoom;
    if (camera.zoom > maxZoom) camera.zoom = maxZoom;

    if(IsKeyPressed(KEY_R)) camera.zoom = 1.0f;
}

void CameraHandler::BeginCamera() {
    BeginMode2D(camera);
}

void CameraHandler::EndCamera() {
    EndMode2D();
}
