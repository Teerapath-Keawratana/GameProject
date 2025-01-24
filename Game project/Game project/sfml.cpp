////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  Assignment:      
//  Instructor:     David Burchill
//  Year / Term:    Fall 2024
//  File name:      Source.cpp
// 
//  Student name:   Teerapath
//  Student email:  Teerapath@hotmail.com
// 
//     I certify that this work is my work only, any work copied from Stack Overflow, textbooks, 
//     or elsewhere is properly cited. 
// 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  BUG 
//  list any and all bugs in your code 
//  1. I didnt get abc to work because of xyz, I tried ijk...
//

#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1080, 640), "GEX works!");
    sf::CircleShape shape(30.f);
    sf::CircleShape shape2(20.f);
    shape2.setPosition(200.f, 300.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        // get input
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // handle input


        // redraw word

        window.clear();
        window.draw(shape);
        window.draw(shape2);
        window.display();
    }

    return 0;
}


