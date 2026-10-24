float func_80006DCC(float angle) {
    angle -= 90.0;
    if (angle < 0.0f) {
        angle += 360.0;
    } else if (angle >= 360.0) {
        angle -= 360.0;
    }
    return angle;
}
