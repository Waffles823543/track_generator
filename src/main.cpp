#include <SFML/Graphics.hpp>
#include <main.hpp>

#include <iostream>
#include <memory>
#include <cmath>
using namespace std;

int main() {

    auto window = sf::RenderWindow{ { 1920u, 1080u }, "Track Generator" };
    window.setFramerateLimit(144);

    Track myTrack;

    SegmentType selectedTool = STRAIGHT;

    while (window.isOpen()) {
        window.clear();

        for (auto event = sf::Event{}; window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    myTrack.AddTrackPoint(event.mouseButton.x, event.mouseButton.y, selectedTool, false, window);
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            selectedTool = STRAIGHT;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) {
            selectedTool = CIRCLE;
        }

        // preview line:
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        myTrack.AddTrackPoint(mousePos.x, mousePos.y, selectedTool, true, window);

        myTrack.Draw(window);

        window.display();
    }
}



void Track::AddTrackPoint (int x, int y, SegmentType tool, bool preview, sf::RenderWindow &window) {
    TrackPoint newPoint(x, y);
    if (preview) {
        if (trackPoints.size() > 0) {
            trackPoints.pop_back();
            if (trackSegs.size() > 0) {
                trackSegs.pop_back();
            }
        } else {
            return;
        }
    }

    if (trackPoints.size() > 0) {
        TrackPoint lastPoint = trackPoints.back();

        double prevdX, prevdY = 0;
        if (trackPoints.size() > 1) {
            prevdX = (double)(lastPoint.xPos - trackPoints[trackPoints.size() - 2].xPos);
            prevdY = (double)(lastPoint.yPos - trackPoints[trackPoints.size() - 2].yPos);
        }
        TrackSegment newSeg(lastPoint.xPos, lastPoint.yPos, newPoint.xPos, newPoint.yPos, prevdX, prevdY, tool);

        if (tool == STRAIGHT) {
            newSeg.PopulateStraightTrack();
        } //else if (tool == CIRCLE) {              // BROKEY THIS SHIT DON'T WORK
        //     newSeg.PopulateCircleTrack(window);
        // }

        trackSegs.push_back(newSeg);
    }

    trackPoints.push_back(newPoint);
}

void Track::Draw(sf::RenderWindow &window) {
    for (auto point : trackPoints) {
        point.Draw(window);
    }

    for (auto seg : trackSegs) {
        seg.Draw(window);
    }
}



TrackSegment::TrackSegment(int x1, int y1, int x2, int y2, double prevdX, double prevdY, SegmentType tool) {
    startX = x1;
    startY = y1;
    endX = x2;
    endY = y2;
    startdX = prevdX;
    startdY = prevdY;
    segmentType = tool;
}

void TrackSegment::PopulateStraightTrack() {
    double dist = sqrt(pow((double)(endX - startX), 2) + pow((double)(endY - startY), 2));

    double cosHeading = (double)(endY - startY) / dist;
    double sinHeading = (double)(endX - startX) / dist;

    double dX = CONE_SPACING * sinHeading;
    double dY = CONE_SPACING * cosHeading;

    double sinPerpend = cosHeading;
    double cosPerpend = -sinHeading;
    
    double leftXOff = (TRACK_WIDTH / 2) * sinPerpend;
    double leftYOff = (TRACK_WIDTH / 2) * cosPerpend;
    double rightXOff = -leftXOff;
    double rightYOff = -leftYOff;

    int amtConePairs = floor(dist / CONE_SPACING);

    double i = 0;
    for (i = 0; i <= amtConePairs; i++) {
        int thisPairX = startX + i*dX;
        int thisPairY = startY + i*dY;

        ConePair newConePair(thisPairX + leftXOff, thisPairY + leftYOff, thisPairX + rightXOff, thisPairY + rightYOff);

        conePairs.push_back(newConePair);
    }

    endX = startX + i*dX;
    endY = startY + i*dY;
}

// BROKEY THIS SHIT DON"T WORK
void TrackSegment::PopulateCircleTrack(sf::RenderWindow &window) {
    
    // double prevSegM = startdX / startdY;
    // double prevSegC = -(startY - prevSegM * startX);
    // double prevSegA = -prevSegM;

    // double diameter = abs(prevSegA*endX + endY + prevSegC) / sqrt(pow(prevSegA, 2) + 1);
    double diameter = sqrt (pow((endX - startX), 2) + pow((endY - startY), 2));

    // double circCenterX = endX - xFlip * (prevSegA * diameter) / (2 * sqrt(pow(prevSegA, 2) + 1));
    // double circCenterY = endY - yFlip * (diameter) / (2 * sqrt(pow(prevSegA, 2) + 1));
    double circCenterX = (startX + endX) / 2;
    double circCenterY = (startY + endY) / 2;

    double half_circum = 0.05 * M_PI_2 * pow((diameter / 2), 2);
    int amtConePairs = half_circum / CONE_SPACING;

    double t, tmax;
    if ((endX - startX) * startdX + (endY - startY) * startdY > 0) {
        t = acos((startX - circCenterX) * 2 / diameter);
        tmax = -acos((endX - circCenterX) * 2 / diameter);
    } else {
        t = -acos((startX - circCenterX) * 2 / diameter);
        tmax = acos((endX - circCenterX) * 2 / diameter);
    }
    double dt = (tmax - t) / amtConePairs;

    // sf::CircleShape debugCircle(diameter/2);
    // debugCircle.setFillColor(sf::Color(255, 255, 255));
    // debugCircle.setPosition(circCenterX, circCenterY);
    // debugCircle.setOrigin(diameter/2, diameter/2);
    // window.draw(debugCircle);

    for (int i = 0; i < amtConePairs; i++) {
        t += dt;
        double coneX = (diameter / 2) * cos(t) + circCenterX;
        double coneY = (diameter / 2) * sin(t) + circCenterY;

        // sf::CircleShape debugCircle(10);
        // debugCircle.setFillColor(sf::Color(255, 255, 255));
        // debugCircle.setPosition(coneX, coneY);
        // debugCircle.setOrigin(10, 10);
        // window.draw(debugCircle);

        ConePair newConePair(coneX, coneY, coneX, coneY);
        conePairs.push_back(newConePair);
    }

}

void TrackSegment::Draw(sf::RenderWindow &window) {
    for (auto conePair : conePairs) {
        conePair.Draw(window);
    }


    sf::Vertex debug_line[] = {
        sf::Vertex(sf::Vector2f(startX, startY)),
        sf::Vertex(sf::Vector2f(endX, endY))
    };
    window.draw(debug_line, 2, sf::Lines);

}



ConePair::ConePair(int leftX, int leftY, int rightX, int rightY) {
    LeftXPos = leftX;
    LeftYPos = leftY;
    RightXPos = rightX;
    RightYPos = rightY;

    leftConeShape = sf::CircleShape(CONE_SIZE);
    rightConeShape = sf::CircleShape(CONE_SIZE);

    leftConeShape.setFillColor(sf::Color(0, 0, 255));
    rightConeShape.setFillColor(sf::Color(255, 255, 0));

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
