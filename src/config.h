#pragma once

class Config
{
public:
    void clear();
    int getCalibrationMin();
    void setCalibrationMin(int value);
    int getCalibrationMax();
    void setCalibrationMax(int value);
    int getLedCount();
    void setLedCount(int value);
};