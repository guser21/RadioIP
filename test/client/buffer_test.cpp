//
// Created by guser on 5/29/18.
//

#include <gtest/gtest.h> // googletest header file

#include <string>
#include <client/buffer.h>

using std::string;

TEST(Simple, ClientBuffer) {
    const char *ch1 = "ci";
    const char *ch2 = "na";
    const char *ch3 = "mon";
    Buffer buffer(4);
    buffer.push(ch1, 2, 2);

    auto ci = buffer.read();
    buffer.commit_read(1);


    buffer.push(ch2, 2, 4);
    auto na = buffer.read();

    buffer.push(ch3, 3, 6);

    auto mon = buffer.read();
    EXPECT_EQ(mon.first, 0);
}

TEST(Edge_Case, ClientBuffer) {
    const char *ch1 = "do";
    const char *ch2 = "re";
    const char *ch3 = "mi";

    Buffer buffer(4);

    buffer.push(ch1, 2, 2);
    buffer.push(ch2, 2, 4);

    auto r = buffer.read();
    EXPECT_EQ(r.first, 4);
    buffer.commit_read(3);

    buffer.push(ch3, 2, 6);

    auto r1 = buffer.read();
    EXPECT_EQ(r1.first, 1);
    buffer.commit_read(1);


    auto r2 = buffer.read();
    EXPECT_EQ(r2.first, 2);
    buffer.commit_read(2);


}
