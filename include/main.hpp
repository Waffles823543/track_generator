// Visuals
#define TRACKPOINT_SIZE 10.0f
#define CONE_SIZE 10.0f

// Track
#define TRACK_WIDTH 100.0f
#define CONE_SPACING 100.0f

// scaling
#define PIXEL_TO_M 0.01f

class TrackPoint {
    public:
        TrackPoint(int x, int y);
        
        int xPos;
        int yPos;

        double dX;
        double dY;

        sf::CircleShape shape;

        void Draw(sf::RenderWindow &window);

};

class ConePair {
    public:
        ConePair(int leftX, int leftY, int rightX, int rightY, bool BigOrange);

        int LeftXPos;
        int LeftYPos;
        int RightXPos;
        int RightYPos;

        bool isOrange;

        sf::CircleShape leftConeShape;
        sf::CircleShape rightConeShape;

        void Draw(sf::RenderWindow &window);
};

class Track {
    public:
        Track(int tL);

        std::vector<TrackPoint> trackPoints;
        std::vector<ConePair> conePairs;

        int carStartX = 0;
        int carStartY = 0;
        double carStartDir = 0;

        bool hasOrange;

        double currdX;
        double currdY;

        double trackLength;

        void AddTrackPoint(int x, int y, bool preview, sf::RenderWindow &window);

        void Draw(sf::RenderWindow &window);

        void outputToCSV (std::string& filePath);
};
