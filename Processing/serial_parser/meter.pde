class Meter
{
  PVector pos;
  PVector size;
  float value = 0.0;
  int MIN, MAX;
  
  Meter(float _x, float _y, float _w, float _h, int _min, int _max)
  {
    pos  = new PVector(_x, _y);
    size = new PVector(_w, _h);
    MIN = _min;
    MAX = _max;
  }
  
  void display()
  {
    pushMatrix();
    noFill();
    stroke(255, 0, 0);
    rect(pos.x, pos.y, size.x, size.y);

    fill(255, 0,0);
    rect(pos.x + (size.x-value), pos.y, value, size.y);
    
    popMatrix();
  }
  
  void setValue(float _value)
  {
    value = map( _value, MIN, MAX, 0.0, 1.0)*size.x;
    value = constrain( value, 0.0, size.x);
  }
  
  /*
  float getValue()
  {
  }
  */
}
