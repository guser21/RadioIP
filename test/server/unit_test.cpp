//
// Created by guser on 5/29/18.
//

#include <gtest/gtest.h>
#include <server/server_parser.h>
#include <common/safe_structures.h>

TEST(Parser, Retransmission) {
    ServerParser parser;
    auto res = parser.parse_requests("LOUDER_PLEASE 123,134,134");
    ASSERT_EQ(res.size(), 3);

    auto b_res = parser.parse_requests("LOUDER_PLEASE 123,jd3,123");
    ASSERT_EQ(b_res.size(), 0);
}

TEST(SafeBuffer, Simple) {
    SafeBuffer safeBuffer(3, 4);
    safeBuffer.push(2, "abcd");
    safeBuffer.push(6, "efgh");
    safeBuffer.push(10, "ijkl");
    ASSERT_EQ("efgh", safeBuffer.get(6));

    safeBuffer.push(14, "mnop");
    ASSERT_EQ("", safeBuffer.get(2));
}