// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/spdy/spdy_alt_svc_wire_format.h"

#include "base/logging.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/platform_test.h"

using ::testing::_;

namespace net {

namespace test {

// Expose all private methods of class SpdyAltSvcWireFormat.
class SpdyAltSvcWireFormatPeer {
 public:
  static void SkipWhiteSpace(StringPiece::const_iterator* c,
                             StringPiece::const_iterator end) {
    SpdyAltSvcWireFormat::SkipWhiteSpace(c, end);
  }
  static bool PercentDecode(StringPiece::const_iterator c,
                            StringPiece::const_iterator end,
                            std::string* output) {
    return SpdyAltSvcWireFormat::PercentDecode(c, end, output);
  }
  static bool ParseAltAuthority(StringPiece::const_iterator c,
                                StringPiece::const_iterator end,
                                std::string* host,
                                uint16* port) {
    return SpdyAltSvcWireFormat::ParseAltAuthority(c, end, host, port);
  }
  static bool ParsePositiveInteger16(StringPiece::const_iterator c,
                                     StringPiece::const_iterator end,
                                     uint16* max_age) {
    return SpdyAltSvcWireFormat::ParsePositiveInteger16(c, end, max_age);
  }
  static bool ParsePositiveInteger32(StringPiece::const_iterator c,
                                     StringPiece::const_iterator end,
                                     uint32* max_age) {
    return SpdyAltSvcWireFormat::ParsePositiveInteger32(c, end, max_age);
  }
  static bool ParseProbability(StringPiece::const_iterator c,
                               StringPiece::const_iterator end,
                               double* p) {
    return SpdyAltSvcWireFormat::ParseProbability(c, end, p);
  }
};

}  // namespace test

namespace {

class SpdyAltSvcWireFormatTest : public ::testing::Test {};

// Tests of public API.

// Fuzz test of ParseHeaderFieldValue() with optional whitespaces, ignored
// parameters, duplicate parameters, trailing space, trailing alternate service
// separator, etc.
TEST(SpdyAltSvcWireFormatTest, ParseHeaderFieldValue) {
  for (int i = 0; i < 1 << 13; ++i) {
    std::string header_field_value = "a%3Db%25c=\"";
    std::string expected_host;
    if (i & 1 << 0) {
      expected_host = "foo\"bar\\baz";
      header_field_value.append("foo\\\"bar\\\\baz");
    }
    header_field_value.append(":42\"");
    if (i & 1 << 1) {
      header_field_value.append(" ");
    }
    uint32 expected_max_age = 86400;
    if (i & 3 << 2) {
      expected_max_age = 1111;
      header_field_value.append(";");
      if (i & 1 << 2) {
        header_field_value.append(" ");
      }
      header_field_value.append("mA=1111");
      if (i & 2 << 2) {
        header_field_value.append(" ");
      }
    }
    if (i & 1 << 4) {
      header_field_value.append("; J=s");
    }
    double expected_p = 1.0;
    if (i & 1 << 5) {
      expected_p = 0.33;
      header_field_value.append("; P=.33");
    }
    if (i & 1 << 6) {
      expected_p = 0.0;
      header_field_value.append("; p=0");
    }
    if (i & 1 << 7) {
      expected_max_age = 999999999;
      header_field_value.append("; Ma=999999999");
    }
    if (i & 1 << 8) {
      expected_p = 0.0;
      header_field_value.append("; P=0.");
    }
    if (i & 1 << 9) {
      header_field_value.append(";");
    }
    if (i & 1 << 10) {
      header_field_value.append(" ");
    }
    if (i & 1 << 11) {
      header_field_value.append(",");
    }
    if (i & 1 << 12) {
      header_field_value.append(" ");
    }

    std::string protocol_id;
    std::string host;
    uint16 port = 0;
    uint32 max_age = 0;
    double p = 0.0;

    ASSERT_TRUE(SpdyAltSvcWireFormat::ParseHeaderFieldValue(
        header_field_value, &protocol_id, &host, &port, &max_age, &p));
    EXPECT_EQ("a=b%c", protocol_id);
    EXPECT_EQ(expected_host, host);
    EXPECT_EQ(42, port);
    EXPECT_EQ(expected_max_age, max_age);
    EXPECT_DOUBLE_EQ(expected_p, p);
  }
}

// Test SerializeHeaderFieldValue() with and without hostname and each
// parameter.
TEST(SpdyAltSvcWireFormatTest, SerializeHeaderFieldValue) {
  for (int i = 0; i < 1 << 3; ++i) {
    std::string expected_header_field_value = "a%3Db%25c=\"";
    std::string host;
    if (i & 1 << 0) {
      host = "foo\"bar\\baz";
      expected_header_field_value.append("foo\\\"bar\\\\baz");
    }
    expected_header_field_value.append(":42\"");
    int max_age = 86400;
    if (i & 1 << 1) {
      max_age = 1111;
      expected_header_field_value.append("; ma=1111");
    }
    double p = 1.0;
    if (i & 1 << 2) {
      p = 0.33;
      expected_header_field_value.append("; p=0.33");
    }
    EXPECT_EQ(expected_header_field_value,
              SpdyAltSvcWireFormat::SerializeHeaderFieldValue("a=b%c", host, 42,
                                                              max_age, p));
  }
}

// ParseHeaderFieldValue() should return false on malformed field values:
// invalid percent encoding, unmatched quotation mark, empty port, non-numeric
// characters in numeric fields, negative or larger than 1.0 probability.
TEST(SpdyAltSvcWireFormatTest, ParseHeaderFieldValueInvalid) {
  std::string protocol_id;
  std::string host;
  uint16 port;
  uint32 max_age;
  double p;
  const char* invalid_field_value_array[] = {"", "a%", "a%x", "a%b", "a%9z",
      "a=", "a=\"", "a=\"b\"", "a=\":\"", "a=\"c:\"", "a=\"c:foo\"",
      "a=\"c:42foo\"", "a=\"b:42\"bar", "a=\"b:42\" ; m",
      "a=\"b:42\" ; min-age", "a=\"b:42\" ; ma", "a=\"b:42\" ; ma=",
      "a=\"b:42\" ; ma=ma", "a=\"b:42\" ; ma=123bar", "a=\"b:42\" ; p=-2",
      "a=\"b:42\" ; p=..", "a=\"b:42\" ; p=1.05"};
  for (const char* invalid_field_value : invalid_field_value_array) {
    EXPECT_FALSE(SpdyAltSvcWireFormat::ParseHeaderFieldValue(
        invalid_field_value, &protocol_id, &host, &port, &max_age, &p))
        << invalid_field_value;
  }
}

// ParseHeaderFieldValue() should return false on a field values truncated
// before closing quotation mark, without trying to access memory beyond the end
// of the input.
TEST(SpdyAltSvcWireFormatTest, ParseTruncatedHeaderFieldValue) {
  std::string protocol_id;
  std::string host;
  uint16 port;
  uint32 max_age;
  double p;
  const char* field_value_array[] = {
      "p=\":137\"", "p=\"foo:137\"", "p%25=\"foo\\\"bar\\\\baz:137\""};
  for (std::string field_value : field_value_array) {
    for (size_t len = 1; len < field_value.size(); ++len) {
      EXPECT_FALSE(SpdyAltSvcWireFormat::ParseHeaderFieldValue(
          field_value.substr(0, len), &protocol_id, &host, &port, &max_age, &p))
          << len;
    }
  }
}

// Tests of private methods.

// Test SkipWhiteSpace().
TEST(SpdyAltSvcWireFormatTest, SkipWhiteSpace) {
  StringPiece input("a \tb  ");
  StringPiece::const_iterator c = input.begin();
  test::SpdyAltSvcWireFormatPeer::SkipWhiteSpace(&c, input.end());
  ASSERT_EQ(input.begin(), c);
  ++c;
  test::SpdyAltSvcWireFormatPeer::SkipWhiteSpace(&c, input.end());
  ASSERT_EQ(input.begin() + 3, c);
  ++c;
  test::SpdyAltSvcWireFormatPeer::SkipWhiteSpace(&c, input.end());
  ASSERT_EQ(input.end(), c);
}

// Test PercentDecode() on valid input.
TEST(SpdyAltSvcWireFormatTest, PercentDecodeValid) {
  StringPiece input("");
  std::string output;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::PercentDecode(
      input.begin(), input.end(), &output));
  EXPECT_EQ("", output);

  input = StringPiece("foo");
  output.clear();
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::PercentDecode(
      input.begin(), input.end(), &output));
  EXPECT_EQ("foo", output);

  input = StringPiece("%2ca%5Cb");
  output.clear();
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::PercentDecode(
      input.begin(), input.end(), &output));
  EXPECT_EQ(",a\\b", output);
}

// Test PercentDecode() on invalid input.
TEST(SpdyAltSvcWireFormatTest, PercentDecodeInvalid) {
  const char* invalid_input_array[] = {"a%", "a%x", "a%b", "%J22", "%9z"};
  for (const char* invalid_input : invalid_input_array) {
    StringPiece input(invalid_input);
    std::string output;
    EXPECT_FALSE(test::SpdyAltSvcWireFormatPeer::PercentDecode(
        input.begin(), input.end(), &output))
        << input;
  }
}

// Test ParseAltAuthority() on valid input.
TEST(SpdyAltSvcWireFormatTest, ParseAltAuthorityValid) {
  StringPiece input(":42");
  std::string host;
  uint16 port;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseAltAuthority(
      input.begin(), input.end(), &host, &port));
  EXPECT_TRUE(host.empty());
  EXPECT_EQ(42, port);

  input = StringPiece("foo:137");
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseAltAuthority(
      input.begin(), input.end(), &host, &port));
  EXPECT_EQ("foo", host);
  EXPECT_EQ(137, port);
}

// Test ParseAltAuthority() on invalid input: empty string, no port, zero port,
// non-digit characters following port.
TEST(SpdyAltSvcWireFormatTest, ParseAltAuthorityInvalid) {
  const char* invalid_input_array[] = {"", ":", "foo:", ":bar", ":0", "foo:0",
      ":12bar", "foo:23bar", " ", ":12 ", "foo:12 "};
  for (const char* invalid_input : invalid_input_array) {
    StringPiece input(invalid_input);
    std::string host;
    uint16 port;
    EXPECT_FALSE(test::SpdyAltSvcWireFormatPeer::ParseAltAuthority(
        input.begin(), input.end(), &host, &port))
        << input;
  }
}

// Test ParseInteger() on valid input.
TEST(SpdyAltSvcWireFormatTest, ParseIntegerValid) {
  StringPiece input("3");
  uint16 value;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger16(
      input.begin(), input.end(), &value));
  EXPECT_EQ(3, value);

  input = StringPiece("1337");
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger16(
      input.begin(), input.end(), &value));
  EXPECT_EQ(1337, value);
}

// Test ParseIntegerValid() on invalid input: empty, zero, non-numeric, trailing
// non-numeric characters.
TEST(SpdyAltSvcWireFormatTest, ParseIntegerInvalid) {
  const char* invalid_input_array[] = {"", " ", "a", "0", "00", "1 ", "12b"};
  for (const char* invalid_input : invalid_input_array) {
    StringPiece input(invalid_input);
    uint16 value;
    EXPECT_FALSE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger16(
        input.begin(), input.end(), &value))
        << input;
  }
}

// Test ParseIntegerValid() around overflow limit.
TEST(SpdyAltSvcWireFormatTest, ParseIntegerOverflow) {
  // Largest possible uint16 value.
  StringPiece input("65535");
  uint16 value16;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger16(
      input.begin(), input.end(), &value16));
  EXPECT_EQ(65535, value16);

  // Overflow uint16, ParsePositiveInteger16() should return false.
  input = StringPiece("65536");
  ASSERT_FALSE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger16(
      input.begin(), input.end(), &value16));

  // However, even if overflow is not checked for, 65536 overflows to 0, which
  // returns false anyway.  Check for a larger number which overflows to 1.
  input = StringPiece("65537");
  ASSERT_FALSE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger16(
      input.begin(), input.end(), &value16));

  // Largest possible uint32 value.
  input = StringPiece("4294967295");
  uint32 value32;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger32(
      input.begin(), input.end(), &value32));
  EXPECT_EQ(4294967295, value32);

  // Overflow uint32, ParsePositiveInteger32() should return false.
  input = StringPiece("4294967296");
  ASSERT_FALSE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger32(
      input.begin(), input.end(), &value32));

  // However, even if overflow is not checked for, 4294967296 overflows to 0,
  // which returns false anyway.  Check for a larger number which overflows to
  // 1.
  input = StringPiece("4294967297");
  ASSERT_FALSE(test::SpdyAltSvcWireFormatPeer::ParsePositiveInteger32(
      input.begin(), input.end(), &value32));
}

// Test ParseProbability() on valid input.
TEST(SpdyAltSvcWireFormatTest, ParseProbabilityValid) {
  StringPiece input("0");
  double probability = -2.0;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
      input.begin(), input.end(), &probability));
  EXPECT_DOUBLE_EQ(0.0, probability);

  input = StringPiece("0.");
  probability = -2.0;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
      input.begin(), input.end(), &probability));
  EXPECT_DOUBLE_EQ(0.0, probability);

  input = StringPiece("0.0");
  probability = -2.0;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
      input.begin(), input.end(), &probability));
  EXPECT_DOUBLE_EQ(0.0, probability);

  input = StringPiece("1");
  probability = -2.0;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
      input.begin(), input.end(), &probability));
  EXPECT_DOUBLE_EQ(1.0, probability);

  input = StringPiece("1.");
  probability = -2.0;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
      input.begin(), input.end(), &probability));
  EXPECT_DOUBLE_EQ(1.0, probability);

  input = StringPiece("1.0");
  probability = -2.0;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
      input.begin(), input.end(), &probability));
  EXPECT_DOUBLE_EQ(1.0, probability);

  input = StringPiece("0.37");
  probability = -2.0;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
      input.begin(), input.end(), &probability));
  EXPECT_DOUBLE_EQ(0.37, probability);

  input = StringPiece("0.72");
  probability = -2.0;
  ASSERT_TRUE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
      input.begin(), input.end(), &probability));
  EXPECT_DOUBLE_EQ(0.72, probability);
}

// Test ParseProbability() on invalid input: empty string, non-digit characters,
// negative input, larger than 1.0.
TEST(SpdyAltSvcWireFormatTest, ParseProbabilityInvalid) {
  const char* invalid_input_array[] = {"", " ", ".", "a", "-2", "-0", "0a",
      "1b ", "0.9z2", "0.33 ", "0.98x", "2.0", "1.0001", "1.00001",
      "1.000001"};
  for (const char* invalid_input : invalid_input_array) {
    StringPiece input(invalid_input);
    double probability;
    EXPECT_FALSE(test::SpdyAltSvcWireFormatPeer::ParseProbability(
        input.begin(), input.end(), &probability));
  }
}

}  // namespace

}  // namespace net
