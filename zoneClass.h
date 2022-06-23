//Class defining all the set listening points
class Point {
  private:
    

  public:
    float x;
    float y;
    float rayon;
    float hyst;
    const char* filename;
    const char* looping;
    bool isLoopTrigger;
    bool mainStopped = false;

    /*
     * rayon in meters
     * y = latitude
     * x = longitude
     */
    Point(float y, float x, float rayon, float hyst, const char* filename, bool isLoopTrigger, const char* looping) {
      this->rayon = rayon;
      this-> hyst = hyst;
      this->x = x;
      this->y = y;
      this->filename = filename;
      this->isLoopTrigger = isLoopTrigger;
      this->looping = looping;
    }
};


//List of all created points
LinkedList<Point*> listePoints = LinkedList<Point*>();
