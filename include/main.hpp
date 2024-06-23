// Visuals
#define TRACKPOINT_SIZE 10.0f
#define CONE_SIZE 10.0f

// Track
#define TRACK_WIDTH 100.0f
#define CONE_SPACING 100.0f

enum SegmentType {
    STRAIGHT, CIRCLE, BEZIER
};

class TrackPoint {
    public:
        TrackPoint(int x, int y);
        
        int xPos;
        int yPos;

        sf::CircleShape shape;

        void Draw(sf::RenderWindow &window);

};

class ConePair {
    public:
        ConePair(int leftX, int leftY, int rightX, int rightY);

        int LeftXPos;
        int LeftYPos;
        int RightXPos;
        int RightYPos;

        sf::CircleShape leftConeShape;
        sf::CircleShape rightConeShape;

        void Draw(sf::RenderWindow &window);
};

class TrackSegment {
    public:
        TrackSegment(int x1, int y1, int x2, int y2, double prevdX, double prevdY, SegmentType tool);

        int startX;
        int startY;
        int endX;
        int endY;

        double startdX = 0;
        double startdY = 0;

        std::vector<ConePair> conePairs;

        SegmentType segmentType;

        void PopulateStraightTrack();
        void PopulateCircleTrack(sf::RenderWindow &window);

        void Draw(sf::RenderWindow &window);
};

class Track {
    public:
        std::vector<TrackPoint> trackPoints;
        std::vector<TrackSegment> trackSegs;

        void AddTrackPoint(int x, int y, SegmentType tool, bool preview, sf::RenderWindow &window);

        void Draw(sf::RenderWindow &window);

};