// Copyright 2019 Plotnikov Aleksey <alex280201@gmail.com>

#include <gtest/gtest.h>

#include <header.hpp>

TEST(Json, Construct) {
  std::string text =
      "{\"orderID\": 12345,\"shopperName\": \"Ivan\",\"shopperEmail\" : "
      "\"ivanov@example.com\",\"contents\" : [{\"productID\": "
      "34,\"productName\": \"super\",\"quantity\": 1},{\"productID\": "
      "56,\"productName\" : \"super-duper\",\"quantity\" : 3}, "
      "true],\"orderCompleted\": true}";
  Json obj(text);

  EXPECT_EQ(obj.is_array(), false);
  EXPECT_EQ(obj.is_object(), true);
  EXPECT_EQ(std::any_cast<double>(obj["orderID"]), 12345.);
  EXPECT_EQ(std::any_cast<std::string>(obj["shopperName"]), "Ivan");
  EXPECT_EQ(std::any_cast<std::string>(obj["shopperEmail"]),
            "ivanov@example.com");

  auto cont = std::any_cast<Json>(obj["contents"]);

  EXPECT_EQ(cont.is_array(), true);
  EXPECT_EQ(cont.is_object(), false);
  EXPECT_EQ(std::any_cast<bool>(cont[2]), true);

  auto prod1 = std::any_cast<Json>(cont[0]);
  auto prod2 = std::any_cast<Json>(cont[1]);

  EXPECT_EQ(prod1.is_array(), false);
  EXPECT_EQ(prod1.is_object(), true);
  EXPECT_EQ(prod2.is_array(), false);
  EXPECT_EQ(prod2.is_object(), true);
  EXPECT_EQ(std::any_cast<double>(prod1["productID"]), 34.);
  EXPECT_EQ(std::any_cast<std::string>(prod1["productName"]), "super");
  EXPECT_EQ(std::any_cast<double>(prod1["quantity"]), 1.);
  EXPECT_EQ(std::any_cast<double>(prod2["productID"]), 56.);
  EXPECT_EQ(std::any_cast<std::string>(prod2["productName"]), "super-duper");
  EXPECT_EQ(std::any_cast<double>(prod2["quantity"]), 3.);
}

TEST(Json, parse) {
  std::string text =
      "[ 1.4567, 50.67, null,{ \"Having fun?\":\"Defenetly yes\" }]";
  Json obj = Json::parse(text);

  EXPECT_EQ(obj.is_array(), true);
  EXPECT_EQ(obj.is_object(), false);
  EXPECT_EQ(std::any_cast<double>(obj[0]), 1.4567);
  EXPECT_EQ(std::any_cast<double>(obj[1]), 50.67);
  EXPECT_EQ(obj[2].has_value(), false);

  auto t1 = std::any_cast<Json>(obj[3]);
  EXPECT_EQ(t1.is_array(), false);
  EXPECT_EQ(t1.is_object(), true);
  EXPECT_EQ(std::any_cast<std::string>(t1["Having fun?"]), "Defenetly yes");
}

TEST(Json, exceptions) {
  std::string wrong_text1 =
      "[ 1.4567 50.67, null,{ \"Having fun?\":\"Defenetly yes\" }]";
  std::string wrong_text2 =
      " 1.4567, 50.67, null,{ \"Having fun?\":\"Defenetly yes\" }]";
  std::string wrong_text3 =
      "{\"orderID\" 12345,\"shopperName\": \"Ivan\",\"shopperEmail\" : "
      "\"ivanov@example.com\",\"orderCompleted\": true}";
  std::string wrong_text4 =
      "{\"orderID\": 12345,\"shopperName\": \"Ivan\",\"shopperEmail\" : "
      "ivanov@example.com,\"orderCompleted\": true}";

  EXPECT_THROW(auto a = Json(wrong_text1), std::invalid_argument);
  EXPECT_THROW(auto b = Json(wrong_text2), std::invalid_argument);
  EXPECT_THROW(auto c = (wrong_text3), std::invalid_argument);
  EXPECT_THROW(auto d = (wrong_text4), std::invalid_argument);
}
