#pragma once
#include<raylib.h>

class Button
{
    private:
    
    Texture2D texture;

    Vector2 position;
    
    public:

    Button(const char* imagePath, Vector2 imagePosition , float scale);

    ~Button();

    void Draw();

    bool isPressed(Vector2 mousePos,bool mousePressed);
};
class figure
{
    private:
    Texture2D figimg;

    public:
    figure(const char* imagepath,float scale);
    ~figure();
    void Draw();
};