#include <SFML/Graphics.hpp>
#include <main.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <math.h>
using namespace std;

int main() {

    auto window = sf::RenderWindow{ { 1920u, 1080u }, "Track Generator" };
    window.setFramerateLimit(144);

    Track myTrack(0);

    bool mousePressed = false;

    while (window.isOpen()) {
        window.clear();

        for (auto event = sf::Event{}; window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                    std::string filePath = "outputCSV/output.csv";
                    myTrack.outputToCSV(filePath);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (myTrack.carStartX == 0 && myTrack.carStartY == 0) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        myTrack.carStartX = mousePos.x;
                        myTrack.carStartY = mousePos.y;
                    }

                    mousePressed = true;
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mousePressed = false;

                    myTrack.currdX = 0;
                    myTrack.currdY = 0;
                    myTrack.trackLength = 0;
                }
            }
        }

        if (mousePressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            myTrack.AddTrackPoint(mousePos.x, mousePos.y, false, window);
        }

        myTrack.Draw(window);

        window.display();
    }
}

Track::Track(int tL) {
    trackLength = 0;

    currdX = 0;
    currdY = 0;

    hasOrange = false;
}

void Track::outputToCSV (std::string& filePath) {
    std::ofstream outFile(filePath);

    if (!outFile) {
        std::cerr << "Error: Could not open the file for writing." << std::endl;
        return;
    }

    outFile << "car_start," << carStartX*PIXEL_TO_M << "," << carStartY*PIXEL_TO_M << "," << carStartDir << ",0.0,0.0,0.0,0.0,0.0" << endl;

    for(auto conePair: conePairs) {
        if (conePair.isOrange) {
            outFile << "big_orange," << conePair.LeftXPos*PIXEL_TO_M << "," << conePair.LeftYPos*PIXEL_TO_M;
            outFile << ",0.0,0.000196,0.000196,0.0" << endl;
            outFile << "big_orange," << conePair.RightXPos*PIXEL_TO_M << "," << conePair.RightYPos*PIXEL_TO_M;
            outFile << ",0.0,0.000196,0.000196,0.0" << endl;
        } else {
            outFile << "blue," << conePair.LeftXPos*PIXEL_TO_M << "," << conePair.LeftYPos*PIXEL_TO_M;
            outFile << ",0.0,0.000196,0.000196,0.0" << endl;
            outFile << "yellow," << conePair.RightXPos*PIXEL_TO_M << "," << conePair.RightYPos*PIXEL_TO_M;
            outFile << ",0.0,0.000196,0.000196,0.0" << endl;
        }
    }

    outFile.close();
}

void Track::AddTrackPoint (int x, int y, bool preview, sf::RenderWindow &window) {
    TrackPoint newPoint(x, y);
    if (preview) {
        if (trackPoints.size() > 0) {
            trackPoints.pop_back();
        } else {
            return;
        }
    }

    if (trackPoints.size() > 0) {
        TrackPoint lastPoint = trackPoints.back();

        double dX = 0;
        double dY = 0;
        if (trackPoints.size() > 1) {
            dX = (double)(x - lastPoint.xPos);
            dY = (double)(y - lastPoint.yPos);
        }

        newPoint.dX = dX;
        newPoint.dY = dY;

        currdX += dX;
        currdY += dY;

        double dTrackL = sqrt(dX*dX + dY*dY);

        if (trackLength < CONE_SPACING && trackLength + dTrackL > CONE_SPACING) {
            double m = currdY/currdX;
            double mPerp = -1/m;
            double dXPerp = currdX;
            double dYPerp = currdX*mPerp;

            double dXPerpNorm = TRACK_WIDTH * dXPerp / sqrt(dXPerp*dXPerp + dYPerp*dYPerp);
            double dYPerpNorm = TRACK_WIDTH * dYPerp / sqrt(dXPerp*dXPerp + dYPerp*dYPerp);

            if (hasOrange) {
                if (dXPerpNorm / dYPerpNorm > 0) {
                    ConePair newConePair(x - dXPerpNorm, y - dYPerpNorm, x + dXPerpNorm, y + dYPerpNorm, false);
                    conePairs.push_back(newConePair);
                } else {
                    ConePair newConePair(x + dXPerpNorm, y + dYPerpNorm, x - dXPerpNorm, y - dYPerpNorm, false);
                    conePairs.push_back(newConePair);
                }
            } else {
                if (conePairs.size() > 0) {
                    hasOrange = true;
                } else {
                    double carStartdX = x - carStartX;
                    double carStartdY = y - carStartY;

                    carStartDir = atan2(carStartdY, carStartdX);
                }

                if (dXPerpNorm / dYPerpNorm > 0) {
                    ConePair newConePair(x - dXPerpNorm, y - dYPerpNorm, x + dXPerpNorm, y + dYPerpNorm, true);
                    conePairs.push_back(newConePair);
                } else {
                    ConePair newConePair(x + dXPerpNorm, y + dYPerpNorm, x - dXPerpNorm, y - dYPerpNorm, true);
                    conePairs.push_back(newConePair);
                }
            }

            currdX = 0;
            currdY = 0;
        }

        trackLength += dTrackL;
        trackLength = fmod(trackLength, CONE_SPACING);
    }

    trackPoints.push_back(newPoint);
}

void Track::Draw(sf::RenderWindow &window) {
    for (auto point : trackPoints) {
        point.Draw(window);
    }

    for (auto pair : conePairs) {
        pair.Draw(window);
    }
}

ConePair::ConePair(int leftX, int leftY, int rightX, int rightY, bool BigOrange) {
    LeftXPos = leftX;
    LeftYPos = leftY;
    RightXPos = rightX;
    RightYPos = rightY;

    isOrange = BigOrange;

    leftConeShape = sf::CircleShape(CONE_SIZE);
    rightConeShape = sf::CircleShape(CONE_SIZE);

    if (BigOrange) {
        leftConeShape.setFillColor(sf::Color(255, 150, 0));
        rightConeShape.setFillColor(sf::Color(255, 150, 0));
    } else {
        leftConeShape.setFillColor(sf::Color(0, 0, 255));
        rightConeShape.setFillColor(sf::Color(255, 255, 0));
    }

    leftConeShape.setPosition(LeftXPos, LeftYPos);
    rightConeShape.setPosition(RightXPos, RightYPos);

    leftConeShape.setOrigin(CONE_SIZE, CONE_SIZE);
    rightConeShape.setOrigin(CONE_SIZE, CONE_SIZE);
}

void ConePair::Draw(sf::RenderWindow &window) {
    window.draw(leftConeShape);
    window.draw(rightConeShape);
}


TrackPoint::TrackPoint (int x, int y) {
    xPos = x;
    yPos = y;

    shape = sf::CircleShape(TRACKPOINT_SIZE);

    shape.setFillColor(sf::Color(150, 0, 150));

    shape.setPosition(xPos, yPos);

    shape.setOrigin(TRACKPOINT_SIZE, TRACKPOINT_SIZE);
}

void TrackPoint::Draw(sf::RenderWindow &window) {
    window.draw(shape);
}
