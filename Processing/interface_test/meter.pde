class Meter
{
  PVector pos, size;
  int dist = 0, strenght = 0;
  float mapDist = 0.0;
  float mapStrenght = 0.0;
  float minStrenght = 12000;
  float maxStrenght = 0.0;
  int MIN, MAX;
  
  Meter(float _x, float _y, float _w, float _h, int _min, int _max)
  {
    pos  = new PVector(_x, _y);
    size = new PVector(_w, _h);
    MIN = _min;
    MAX = _max;
  }
  
  // DISPLAY ///////////////////////////////////////////////////////////////////////
  void display()
  {
    pushMatrix();
    
    // display some text
    pushStyle();
    textAlign(LEFT);
    textFont(font, 18);
    fill(255);
    text( "maxS:" + maxStrenght, 0, 20);
    text( "minS:" + minStrenght, 0, 40);
    popStyle();
    
    // display the cursor
    pushMatrix();
    pushStyle();
    translate(0.0, pos.y);
    translate(mapDist, 0);
    fill(255, 0,0, mapStrenght);
    stroke(255, 0, 0);
    rect(-size.x*0.5, -size.y*0.5, size.x, size.y);
    popStyle();
    
    // display the distance number and strenght number
    pushStyle();
    textAlign(CENTER);
    textFont(font, 32);
    fill(255);
    text( dist, 0, -100);
    text( strenght, 0, 100);
    popStyle();

    popMatrix();
    popMatrix();
  }
  
  // SET DIST //////////////////////////////////////////////////////////////////////
  void setDist(int _value)
  {
    dist = _value;
    mapDist = width - (map( dist, MIN, MAX, 0.0, 1.0)* width);
    mapDist = constrain( mapDist, 0.0, width);
    println( "dist: " + dist + ", [" + mapDist + "];");
  }
  
  // SET STRENGHT //////////////////////////////////////////////////////////////////
  void setStrenght(int _value)
  {
    strenght = _value;
    if( strenght > maxStrenght)
      maxStrenght = strenght;
    if( strenght < minStrenght)
      minStrenght = strenght;
      
    mapStrenght = map(strenght, minStrenght, maxStrenght, 0.0, 1.0) * 255;
    mapStrenght = constrain(mapStrenght, 0, 255);
    
    //println( "strenght: " + strenght + ", [" + mapStrenght + "];");
  }
  
  void reset() {
    minStrenght = 12000;
    maxStrenght = 0.0;
  }
}
