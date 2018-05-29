//
// Created by guser on 5/29/18.
//

#include <gtest/gtest.h> // googletest header file

#include <string>
#include <client/buffer.h>

using std::string;

TEST(BufferTest, CStrEqual) {
    Buffer buffer(4);
    buffer.push(("heey"), 4, 2);
    EXPECT_EQ(buffer.load(),1);
}

