// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "gtest/gtest.h"

int main(int argc, char** argv)
{
    //--gtest_filter=*Special*  in Properties->CommandArguments

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

