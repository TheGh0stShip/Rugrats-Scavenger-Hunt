void func_80008340(
    float *destination,
    const float *position,
    const float *axis_x,
    const float *axis_y,
    const float *axis_z
) {
    destination[0] = -axis_z[0];
    destination[1] = -axis_z[1];
    destination[2] = -axis_z[2];
    destination[3] = 0.0f;
    destination[4] = axis_y[0];
    destination[5] = axis_y[1];
    destination[6] = axis_y[2];
    destination[7] = 0.0f;
    destination[8] = axis_x[0];
    destination[9] = axis_x[1];
    destination[10] = axis_x[2];
    destination[11] = 0.0f;
    destination[12] = position[0];
    destination[13] = position[1];
    destination[14] = position[2];
    destination[15] = 1.0f;
}
