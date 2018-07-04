// Barebones main for gtest
#include <gtest/gtest.h>
#include <stdio.h>

GTEST_API_ int main(int argc, char** argv) {
    printf("Running tests...");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
