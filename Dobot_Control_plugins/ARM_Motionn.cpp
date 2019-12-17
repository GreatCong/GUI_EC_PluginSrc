#include "ARM_Motion.h"

ARM_Motion::ARM_Motion()
{

}

///
/// \brief ARM_Motion::return_angle
/// \param a
/// \param b
/// \param c
/// \return
///
float ARM_Motion::return_angle(float a, float b, float c) {
  // cosine rule for angle between c and a
  return std::acos((a * a + c * c - b * b) / (2 * a * c));
}

///
/// \brief ARM_Motion::check_angle
/// \param a
/// \param b
/// \param c
/// \return
///
uint8_t ARM_Motion::check_angle(float a, float b, float c){
  float data;
    data = (a * a + c * c - b * b) - (2 * a * c);

    if(data > 0){
      return 1;
    }
    else{
      return 0;
    }
}

